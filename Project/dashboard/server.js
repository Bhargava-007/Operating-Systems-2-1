const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const PORT = 3000;
const PUBLIC_DIR = path.join(__dirname, 'public');
const C_PROJECT_DIR = path.resolve(__dirname, '..', 'Linux Concurrent Producer-Consumer System Using Semaphores', 'producer_consumer');

// Convert Windows path to WSL path format: C:\path -> /mnt/c/path
function toWslPath(winPath) {
    const resolved = path.resolve(winPath).replace(/\\/g, '/');
    const match = resolved.match(/^([A-Za-z]):\/(.*)$/);
    if (match) {
        return `/mnt/${match[1].toLowerCase()}/${match[2]}`;
    }
    return resolved;
}

const MIME_TYPES = {
    '.html': 'text/html; charset=utf-8',
    '.css': 'text/css; charset=utf-8',
    '.js': 'application/javascript; charset=utf-8',
    '.json': 'application/json',
    '.svg': 'image/svg+xml',
    '.png': 'image/png',
    '.ico': 'image/x-icon'
};

const server = http.createServer((req, res) => {
    const url = new URL(req.url, `http://${req.headers.host}`);
    const pathname = url.pathname;

    // API: Run real C Program via WSL with Timeout-based Deadlock Detection
    if (pathname === '/api/run' && req.method === 'POST') {
        let body = '';
        req.on('data', chunk => { body += chunk; });
        req.on('end', () => {
            try {
                let params = {};
                try {
                    params = JSON.parse(body || '{}');
                } catch (e) {
                    try {
                        params = JSON.parse(body.replace(/\\"/g, '"'));
                    } catch (e2) {
                        params = {};
                    }
                }
                const bufferSize = parseInt(params.bufferSize, 10) || 5;
                const numProducers = parseInt(params.numProducers, 10) || 2;
                const numConsumers = parseInt(params.numConsumers, 10) || 2;
                const itemsPerProducer = parseInt(params.itemsPerProducer, 10) || 4;
                const sessionId = `web_${Date.now().toString(36)}`;

                const wslDir = toWslPath(C_PROJECT_DIR);
                const cmd = `cd "${wslDir}" && make -s && ./pc ${bufferSize} ${numProducers} ${numConsumers} ${itemsPerProducer} ${sessionId}`;

                const child = spawn('wsl', ['bash', '-c', cmd]);

                let stdout = '';
                let stderr = '';
                let isDeadlocked = false;

                // Deadlock detector timeout (15 seconds)
                const watchdogTimer = setTimeout(() => {
                    isDeadlocked = true;
                    child.kill('SIGKILL');
                }, 15000);

                child.stdout.on('data', data => {
                    stdout += data.toString();
                });

                child.stderr.on('data', data => {
                    stderr += data.toString();
                });

                child.on('close', code => {
                    clearTimeout(watchdogTimer);
                    if (isDeadlocked) {
                        res.writeHead(200, { 'Content-Type': 'application/json' });
                        return res.end(JSON.stringify({
                            success: false,
                            deadlockDetected: true,
                            error: 'Deadlock timeout reached (15s). Threads stalled on synchronization primitive.',
                            stdout,
                            sessionId
                        }));
                    }

                    res.writeHead(200, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify({
                        success: code === 0,
                        code,
                        stdout,
                        stderr,
                        sessionId,
                        params: { bufferSize, numProducers, numConsumers, itemsPerProducer }
                    }));
                });

                child.on('error', err => {
                    clearTimeout(watchdogTimer);
                    res.writeHead(500, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify({
                        success: false,
                        error: err.message
                    }));
                });
            } catch (err) {
                res.writeHead(400, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ success: false, error: 'Invalid JSON payload' }));
            }
        });
        return;
    }

    // API: Read Log File pc_log.txt with optional session filtering
    if (pathname === '/api/logs' && req.method === 'GET') {
        const filterSession = url.searchParams.get('session');
        const logPath = path.join(C_PROJECT_DIR, 'pc_log.txt');
        if (fs.existsSync(logPath)) {
            let content = fs.readFileSync(logPath, 'utf8');
            if (filterSession) {
                const lines = content.split('\n');
                content = lines.filter(line => line.includes(`[${filterSession}]`) || line.includes(`session=${filterSession}`)).join('\n');
            }
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end(content);
        } else {
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('');
        }
        return;
    }

    // Static File Serving
    let filePath = path.join(PUBLIC_DIR, pathname === '/' ? 'index.html' : pathname);
    const ext = path.extname(filePath).toLowerCase();
    const contentType = MIME_TYPES[ext] || 'application/octet-stream';

    fs.readFile(filePath, (err, content) => {
        if (err) {
            if (err.code === 'ENOENT') {
                res.writeHead(404, { 'Content-Type': 'text/plain' });
                res.end('404 Not Found');
            } else {
                res.writeHead(500, { 'Content-Type': 'text/plain' });
                res.end('500 Server Error');
            }
        } else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content);
        }
    });
});

server.listen(PORT, () => {
    console.log(`[Producer-Consumer Dashboard] Server running at http://localhost:${PORT}`);
});
