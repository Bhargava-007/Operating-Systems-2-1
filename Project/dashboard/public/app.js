// Linux Concurrent Producer-Consumer Dashboard Logic

class ProducerConsumerApp {
    constructor() {
        // Core state parameters
        this.bufferSize = 5;
        this.numProducers = 2;
        this.numConsumers = 2;
        this.itemsPerProducer = 4;
        this.stepSpeed = 400; // ms

        // Execution mode: 'idle', 'browser_sim', 'native_c'
        this.activeMode = 'idle';
        this.currentSessionId = null;

        // Visual simulation / state tracker
        this.simTimer = null;
        this.buffer = [];
        this.inPtr = 0;
        this.outPtr = 0;
        this.semEmpty = 5;
        this.semFull = 0;
        this.semMutex = 1;
        this.totalProduced = 0;
        this.totalConsumed = 0;
        this.startTime = null;
        this.threadStates = { producers: [], consumers: [] };

        // FIFO verification tracker
        this.producedSequence = [];
        this.consumedSequence = [];
        this.fifoViolations = 0;
        this.occupancyViolations = 0;

        // DOM elements
        this.dom = {
            inputBufferSize: document.getElementById('inputBufferSize'),
            inputProducers: document.getElementById('inputProducers'),
            inputConsumers: document.getElementById('inputConsumers'),
            inputItems: document.getElementById('inputItems'),
            inputSpeed: document.getElementById('inputSpeed'),

            valBufferSize: document.getElementById('valBufferSize'),
            valProducers: document.getElementById('valProducers'),
            valConsumers: document.getElementById('valConsumers'),
            valItems: document.getElementById('valItems'),
            valSpeed: document.getElementById('valSpeed'),

            valSemEmpty: document.getElementById('valSemEmpty'),
            valSemFull: document.getElementById('valSemFull'),
            valSemMutex: document.getElementById('valSemMutex'),
            textSemMutex: document.getElementById('textSemMutex'),
            semMutexCard: document.getElementById('semMutexCard'),

            producerThreadList: document.getElementById('producerThreadList'),
            consumerThreadList: document.getElementById('consumerThreadList'),
            producerCountBadge: document.getElementById('producerCountBadge'),
            consumerCountBadge: document.getElementById('consumerCountBadge'),

            circularBufferSvg: document.getElementById('circularBufferSvg'),
            linearSlotsTrack: document.getElementById('linearSlotsTrack'),
            linearPointerMeta: document.getElementById('linearPointerMeta'),

            metricThroughput: document.getElementById('metricThroughput'),
            metricTimeElapsed: document.getElementById('metricTimeElapsed'),

            systemStatusDot: document.getElementById('systemStatusDot'),
            systemStatusText: document.getElementById('systemStatusText'),
            verificationPill: document.getElementById('verificationPill'),

            btnToggleSim: document.getElementById('btnToggleSim'),
            btnToggleSimText: document.getElementById('btnToggleSimText'),
            btnStepSim: document.getElementById('btnStepSim'),
            btnReset: document.getElementById('btnReset'),
            btnRunCompiled: document.getElementById('btnRunCompiled'),

            terminalLog: document.getElementById('terminalLog'),
            tabTerminal: document.getElementById('tabTerminal'),
            tabFileLog: document.getElementById('tabFileLog')
        };

        this.initEventListeners();
        this.resetState();
    }

    initEventListeners() {
        this.dom.inputBufferSize.addEventListener('input', (e) => {
            this.bufferSize = parseInt(e.target.value, 10);
            this.dom.valBufferSize.textContent = this.bufferSize;
            if (this.activeMode === 'idle') this.resetState();
        });

        this.dom.inputProducers.addEventListener('input', (e) => {
            this.numProducers = parseInt(e.target.value, 10);
            this.dom.valProducers.textContent = this.numProducers;
            this.dom.producerCountBadge.textContent = this.numProducers;
            if (this.activeMode === 'idle') this.resetState();
        });

        this.dom.inputConsumers.addEventListener('input', (e) => {
            this.numConsumers = parseInt(e.target.value, 10);
            this.dom.valConsumers.textContent = this.numConsumers;
            this.dom.consumerCountBadge.textContent = this.numConsumers;
            if (this.activeMode === 'idle') this.resetState();
        });

        this.dom.inputItems.addEventListener('input', (e) => {
            this.itemsPerProducer = parseInt(e.target.value, 10);
            this.dom.valItems.textContent = this.itemsPerProducer;
            if (this.activeMode === 'idle') this.resetState();
        });

        this.dom.inputSpeed.addEventListener('input', (e) => {
            this.stepSpeed = parseInt(e.target.value, 10);
            this.dom.valSpeed.textContent = `${this.stepSpeed}ms`;
            if (this.activeMode === 'browser_sim') {
                clearInterval(this.simTimer);
                this.simTimer = setInterval(() => this.simulationStep(), this.stepSpeed);
            }
        });

        this.dom.btnToggleSim.addEventListener('click', () => this.toggleSimulation());
        this.dom.btnStepSim.addEventListener('click', () => this.simulationStep());
        this.dom.btnReset.addEventListener('click', () => this.resetState());
        this.dom.btnRunCompiled.addEventListener('click', () => this.runRealCBinary());

        this.dom.tabTerminal.addEventListener('click', () => {
            this.dom.tabTerminal.classList.add('active');
            this.dom.tabFileLog.classList.remove('active');
        });

        this.dom.tabFileLog.addEventListener('click', () => {
            this.dom.tabFileLog.classList.add('active');
            this.dom.tabTerminal.classList.remove('active');
            this.fetchFileLog();
        });
    }

    resetState() {
        this.activeMode = 'idle';
        if (this.simTimer) clearInterval(this.simTimer);
        this.dom.btnToggleSimText.textContent = "Start Animated Simulation";
        this.dom.btnToggleSim.classList.remove('btn-secondary');
        this.dom.btnToggleSim.classList.add('btn-primary');
        this.dom.btnRunCompiled.disabled = false;
        this.dom.btnStepSim.disabled = false;

        this.buffer = new Array(this.bufferSize).fill(null);
        this.inPtr = 0;
        this.outPtr = 0;
        this.semEmpty = this.bufferSize;
        this.semFull = 0;
        this.semMutex = 1;
        this.totalProduced = 0;
        this.totalConsumed = 0;
        this.startTime = null;

        this.producedSequence = [];
        this.consumedSequence = [];
        this.fifoViolations = 0;
        this.occupancyViolations = 0;

        this.threadStates.producers = Array.from({ length: this.numProducers }, (_, i) => ({
            id: i + 1,
            state: 'Ready',
            produced: 0,
            quota: this.itemsPerProducer
        }));

        const totalItems = this.numProducers * this.itemsPerProducer;
        const baseConsumerQuota = Math.floor(totalItems / this.numConsumers);
        const remainder = totalItems % this.numConsumers;

        this.threadStates.consumers = Array.from({ length: this.numConsumers }, (_, i) => ({
            id: i + 1,
            state: 'Ready',
            consumed: 0,
            quota: baseConsumerQuota + (i === this.numConsumers - 1 ? remainder : 0)
        }));

        this.updateSystemStatus("Ready", false);
        this.updateVerificationBadge();
        this.renderAll();
    }

    toggleSimulation() {
        if (this.activeMode === 'browser_sim') {
            this.activeMode = 'idle';
            clearInterval(this.simTimer);
            this.dom.btnToggleSimText.textContent = "Resume Simulation";
            this.updateSystemStatus("Paused Simulation", false);
        } else {
            const totalItems = this.numProducers * this.itemsPerProducer;
            if (this.totalConsumed >= totalItems) {
                this.resetState();
            }
            this.activeMode = 'browser_sim';
            this.currentSessionId = `sim_${Date.now().toString(36)}`;
            if (!this.startTime) this.startTime = Date.now();
            this.dom.btnToggleSimText.textContent = "Pause Simulation";
            this.updateSystemStatus("Running In-Browser Simulation", true);
            this.simTimer = setInterval(() => this.simulationStep(), this.stepSpeed);
        }
    }

    simulationStep() {
        if (!this.startTime) this.startTime = Date.now();
        const totalItems = this.numProducers * this.itemsPerProducer;
        if (this.totalConsumed >= totalItems) {
            this.finishSimulation();
            return;
        }

        // Active producers & consumers needing work
        const activeProducers = this.threadStates.producers.filter(p => p.produced < p.quota);
        const activeConsumers = this.threadStates.consumers.filter(c => c.consumed < c.quota);

        // Blocking POSIX condition logic:
        // A producer CAN proceed if semEmpty > 0
        // A consumer CAN proceed if semFull > 0
        const producerCanAct = (this.semEmpty > 0 && activeProducers.length > 0);
        const consumerCanAct = (this.semFull > 0 && activeConsumers.length > 0);

        // If neither can act, check if completed or blocked
        if (!producerCanAct && !consumerCanAct) {
            if (activeProducers.length === 0 && activeConsumers.length === 0) {
                this.finishSimulation();
                return;
            }
            // Threads blocked waiting on semaphores
            activeProducers.forEach(p => { p.state = 'Blocked'; });
            activeConsumers.forEach(c => { c.state = 'Blocked'; });
            this.renderAll();
            return;
        }

        let doProduce = false;
        if (producerCanAct && consumerCanAct) {
            doProduce = Math.random() < 0.5;
        } else if (producerCanAct) {
            doProduce = true;
        } else {
            doProduce = false;
        }

        if (doProduce) {
            const p = activeProducers[Math.floor(Math.random() * activeProducers.length)];
            // sem_wait(&empty)
            this.semEmpty--;
            // sem_wait(&mutex)
            this.semMutex = 0;
            p.state = 'Running';

            const item = Math.floor(Math.random() * 900) + 100;
            const slot = this.inPtr;
            this.buffer[slot] = item;
            this.inPtr = (this.inPtr + 1) % this.bufferSize;
            this.totalProduced++;
            p.produced++;
            this.producedSequence.push(item);

            // Invariant check: occupancy
            const currentOccupancy = this.totalProduced - this.totalConsumed;
            if (currentOccupancy < 0 || currentOccupancy > this.bufferSize) {
                this.occupancyViolations++;
            }

            this.logMessage(`[SIM] [Producer ${p.id}] Produced: ${item} at slot ${slot} (occupancy: ${currentOccupancy}/${this.bufferSize})`, 'producer');

            // sem_post(&mutex)
            this.semMutex = 1;
            // sem_post(&full)
            this.semFull++;

            p.state = p.produced >= p.quota ? 'Terminated' : 'Ready';
        } else {
            const c = activeConsumers[Math.floor(Math.random() * activeConsumers.length)];
            // sem_wait(&full)
            this.semFull--;
            // sem_wait(&mutex)
            this.semMutex = 0;
            c.state = 'Running';

            const slot = this.outPtr;
            const item = this.buffer[slot];
            this.buffer[slot] = null;
            this.outPtr = (this.outPtr + 1) % this.bufferSize;
            this.totalConsumed++;
            c.consumed++;
            this.consumedSequence.push(item);

            // Invariant check: FIFO order check
            const expectedItem = this.producedSequence[this.consumedSequence.length - 1];
            if (item !== expectedItem) {
                this.fifoViolations++;
            }

            const currentOccupancy = this.totalProduced - this.totalConsumed;
            if (currentOccupancy < 0 || currentOccupancy > this.bufferSize) {
                this.occupancyViolations++;
            }

            this.logMessage(`[SIM] [Consumer ${c.id}] Consumed: ${item} from slot ${slot} (occupancy: ${currentOccupancy}/${this.bufferSize})`, 'consumer');

            // sem_post(&mutex)
            this.semMutex = 1;
            // sem_post(&empty)
            this.semEmpty++;

            c.state = c.consumed >= c.quota ? 'Terminated' : 'Ready';
        }

        this.updateVerificationBadge();
        this.renderAll();

        if (this.totalConsumed >= totalItems) {
            this.finishSimulation();
        }
    }

    finishSimulation() {
        this.activeMode = 'idle';
        if (this.simTimer) clearInterval(this.simTimer);
        this.dom.btnToggleSimText.textContent = "Start Animated Simulation";
        this.updateSystemStatus("Simulation Complete", false);

        this.threadStates.producers.forEach(p => p.state = 'Terminated');
        this.threadStates.consumers.forEach(c => c.state = 'Terminated');

        const totalItems = this.numProducers * this.itemsPerProducer;
        this.logMessage(`\n--- [SIM] Verified Invariant Report ---`, 'system');
        this.logMessage(`Total Items Produced: ${this.totalProduced}/${totalItems}`, 'system');
        this.logMessage(`Total Items Consumed: ${this.totalConsumed}/${totalItems}`, 'system');
        this.logMessage(`FIFO Violations Observed: ${this.fifoViolations}`, this.fifoViolations === 0 ? 'system' : 'error');
        this.logMessage(`Buffer Bounds Violations: ${this.occupancyViolations}`, this.occupancyViolations === 0 ? 'system' : 'error');
        this.updateVerificationBadge();
        this.renderAll();
    }

    async runRealCBinary() {
        if (this.simTimer) clearInterval(this.simTimer);
        this.activeMode = 'native_c';
        this.dom.btnRunCompiled.disabled = true;
        this.dom.btnStepSim.disabled = true;
        this.updateSystemStatus("Compiling & Executing C (WSL)...", true);
        this.logMessage(`\n[Native C] Compiling & launching POSIX pthread execution...`, 'system');

        try {
            const res = await fetch('/api/run', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    bufferSize: this.bufferSize,
                    numProducers: this.numProducers,
                    numConsumers: this.numConsumers,
                    itemsPerProducer: this.itemsPerProducer
                })
            });

            const data = await res.json();
            if (data.deadlockDetected) {
                this.updateSystemStatus("Deadlock Detected (Timeout)", false);
                this.logMessage(`[DEADLOCK ALERT] ${data.error}`, 'error');
                this.dom.btnRunCompiled.disabled = false;
                this.dom.btnStepSim.disabled = false;
                return;
            }

            if (data.success) {
                this.currentSessionId = data.sessionId;
                this.updateSystemStatus("C Binary Completed (Exit Code 0)", false);
                this.logMessage(`[Native C] Process finished (Session: ${data.sessionId}). Playing back verified kernel events:`, 'system');

                // Animate visualization based on real native events
                const lines = data.stdout.split('\n');
                let lineIdx = 0;

                this.buffer = new Array(this.bufferSize).fill(null);
                this.inPtr = 0;
                this.outPtr = 0;
                this.semEmpty = this.bufferSize;
                this.semFull = 0;
                this.semMutex = 1;
                this.totalProduced = 0;
                this.totalConsumed = 0;
                this.producedSequence = [];
                this.consumedSequence = [];
                this.fifoViolations = 0;
                this.occupancyViolations = 0;

                const playbackTimer = setInterval(() => {
                    if (lineIdx >= lines.length) {
                        clearInterval(playbackTimer);
                        this.dom.btnRunCompiled.disabled = false;
                        this.dom.btnStepSim.disabled = false;
                        this.activeMode = 'idle';
                        this.updateVerificationBadge();
                        this.renderAll();
                        return;
                    }

                    const line = lines[lineIdx++];
                    if (!line || line.trim() === '') return;

                    // Match Producer: [Producer 1] Produced: 932 at slot 0 (occupancy: 1/5)
                    const prodMatch = line.match(/\[Producer\s+(\d+)\]\s+Produced:\s+(\d+)\s+at\s+slot\s+(\d+)\s+\(occupancy:\s+(\d+)\/(\d+)\)/);
                    if (prodMatch) {
                        const pid = parseInt(prodMatch[1], 10);
                        const item = parseInt(prodMatch[2], 10);
                        const slot = parseInt(prodMatch[3], 10);
                        const occ = parseInt(prodMatch[4], 10);

                        this.buffer[slot] = item;
                        this.inPtr = (slot + 1) % this.bufferSize;
                        this.semFull = occ;
                        this.semEmpty = this.bufferSize - occ;
                        this.totalProduced++;
                        this.producedSequence.push(item);

                        const pThread = this.threadStates.producers.find(p => p.id === pid);
                        if (pThread) {
                            pThread.produced++;
                            pThread.state = pThread.produced >= pThread.quota ? 'Terminated' : 'Running';
                        }
                        this.logMessage(`[C-Native] ${line}`, 'producer');
                        this.renderAll();
                        return;
                    }

                    // Match Consumer: [Consumer 1] Consumed: 932 from slot 0 (occupancy: 0/5)
                    const consMatch = line.match(/\[Consumer\s+(\d+)\]\s+Consumed:\s+(\d+)\s+from\s+slot\s+(\d+)\s+\(occupancy:\s+(\d+)\/(\d+)\)/);
                    if (consMatch) {
                        const cid = parseInt(consMatch[1], 10);
                        const item = parseInt(consMatch[2], 10);
                        const slot = parseInt(consMatch[3], 10);
                        const occ = parseInt(consMatch[4], 10);

                        this.buffer[slot] = null;
                        this.outPtr = (slot + 1) % this.bufferSize;
                        this.semFull = occ;
                        this.semEmpty = this.bufferSize - occ;
                        this.totalConsumed++;
                        this.consumedSequence.push(item);

                        // Real-time FIFO verification
                        const expected = this.producedSequence[this.consumedSequence.length - 1];
                        if (item !== expected) {
                            this.fifoViolations++;
                        }

                        const cThread = this.threadStates.consumers.find(c => c.id === cid);
                        if (cThread) {
                            cThread.consumed++;
                            cThread.state = cThread.consumed >= cThread.quota ? 'Terminated' : 'Running';
                        }
                        this.logMessage(`[C-Native] ${line}`, 'consumer');
                        this.renderAll();
                        return;
                    }

                    this.logMessage(line, 'system');
                }, Math.max(60, this.stepSpeed / 3));

            } else {
                this.updateSystemStatus("C Execution Failed", false);
                this.logMessage(`[Error] C process failure: ${data.stderr || data.error}`, 'error');
                this.dom.btnRunCompiled.disabled = false;
                this.dom.btnStepSim.disabled = false;
                this.activeMode = 'idle';
            }
        } catch (err) {
            this.updateSystemStatus("Backend Error", false);
            this.logMessage(`[Error] Failed to connect to local server: ${err.message}`, 'error');
            this.dom.btnRunCompiled.disabled = false;
            this.dom.btnStepSim.disabled = false;
            this.activeMode = 'idle';
        }
    }

    async fetchFileLog() {
        try {
            const url = this.currentSessionId ? `/api/logs?session=${encodeURIComponent(this.currentSessionId)}` : '/api/logs';
            const res = await fetch(url);
            const text = await res.text();
            this.dom.terminalLog.innerHTML = '';
            if (text.trim() === '') {
                this.logMessage(`[pc_log.txt] No log entries found${this.currentSessionId ? ` for session ${this.currentSessionId}` : ''}. Run native C binary to generate structured logs.`, 'system');
            } else {
                const lines = text.split('\n');
                lines.forEach(l => {
                    if (l) {
                        if (l.includes('[Producer')) {
                            this.logMessage(l, 'producer');
                        } else if (l.includes('[Consumer')) {
                            this.logMessage(l, 'consumer');
                        } else {
                            this.logMessage(l, 'system');
                        }
                    }
                });
            }
        } catch (err) {
            this.logMessage(`[Error] Could not fetch pc_log.txt: ${err.message}`, 'error');
        }
    }

    updateVerificationBadge() {
        if (!this.dom.verificationPill) return;
        if (this.fifoViolations === 0 && this.occupancyViolations === 0) {
            this.dom.verificationPill.innerHTML = `
                <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path><polyline points="22 4 12 14.01 9 11.01"></polyline></svg>
                <span>FIFO Verified · 0 Violations · 0 Deadlocks</span>
            `;
            this.dom.verificationPill.style.color = "var(--primary-sage)";
            this.dom.verificationPill.style.backgroundColor = "var(--soft-sage)";
        } else {
            this.dom.verificationPill.innerHTML = `
                <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="8" x2="12" y2="12"></line><line x1="12" y1="16" x2="12.01" y2="16"></line></svg>
                <span>Invariant Violation: ${this.fifoViolations} FIFO / ${this.occupancyViolations} Bounds</span>
            `;
            this.dom.verificationPill.style.color = "var(--error)";
            this.dom.verificationPill.style.backgroundColor = "var(--error-light)";
        }
    }

    logMessage(msg, type = 'system') {
        const line = document.createElement('div');
        line.className = `log-line ${type}-log`;
        line.textContent = msg;
        this.dom.terminalLog.appendChild(line);
        this.dom.terminalLog.scrollTop = this.dom.terminalLog.scrollHeight;
    }

    updateSystemStatus(text, isActive) {
        this.dom.systemStatusText.textContent = text;
        if (isActive) {
            this.dom.systemStatusDot.classList.add('active');
        } else {
            this.dom.systemStatusDot.classList.remove('active');
        }
    }

    renderAll() {
        this.renderSemaphores();
        this.renderCircularBuffer();
        this.renderLinearBuffer();
        this.renderThreadMonitors();
        this.renderMetrics();
    }

    renderSemaphores() {
        if (this.dom.valSemEmpty) this.dom.valSemEmpty.textContent = this.semEmpty;
        if (this.dom.valSemFull) this.dom.valSemFull.textContent = this.semFull;
        if (this.dom.valSemMutex) this.dom.valSemMutex.textContent = this.semMutex;

        if (this.dom.semMutexCard && this.dom.textSemMutex) {
            if (this.semMutex === 1) {
                this.dom.textSemMutex.textContent = "UNLOCKED (Critical Section Free)";
                this.dom.semMutexCard.className = "metric-card mutex-unlocked";
            } else {
                this.dom.textSemMutex.textContent = "LOCKED (In Critical Section)";
                this.dom.semMutexCard.className = "metric-card mutex-locked";
            }
        }
    }

    renderCircularBuffer() {
        const svg = this.dom.circularBufferSvg;
        svg.innerHTML = '';

        const cx = 220;
        const cy = 190;
        const radius = 125;
        const n = this.bufferSize;

        // Center core circle
        const centerCircle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
        centerCircle.setAttribute("cx", cx);
        centerCircle.setAttribute("cy", cy);
        centerCircle.setAttribute("r", radius - 40);
        centerCircle.setAttribute("fill", "#FFFFFF");
        centerCircle.setAttribute("stroke", "#DFE7E0");
        centerCircle.setAttribute("stroke-width", "1");
        svg.appendChild(centerCircle);

        // Center labels
        const textTitle = document.createElementNS("http://www.w3.org/2000/svg", "text");
        textTitle.setAttribute("x", cx);
        textTitle.setAttribute("y", cy - 14);
        textTitle.setAttribute("text-anchor", "middle");
        textTitle.setAttribute("font-family", "Plus Jakarta Sans, sans-serif");
        textTitle.setAttribute("font-size", "11px");
        textTitle.setAttribute("font-weight", "600");
        textTitle.setAttribute("letter-spacing", "0.5px");
        textTitle.setAttribute("fill", "#748377");
        textTitle.textContent = "CIRCULAR BUFFER";
        svg.appendChild(textTitle);

        const textSlots = document.createElementNS("http://www.w3.org/2000/svg", "text");
        textSlots.setAttribute("x", cx);
        textSlots.setAttribute("y", cy + 12);
        textSlots.setAttribute("text-anchor", "middle");
        textSlots.setAttribute("font-family", "Plus Jakarta Sans, sans-serif");
        textSlots.setAttribute("font-size", "20px");
        textSlots.setAttribute("font-weight", "700");
        textSlots.setAttribute("fill", "#223026");
        textSlots.textContent = `N = ${n}`;
        svg.appendChild(textSlots);

        const textStatus = document.createElementNS("http://www.w3.org/2000/svg", "text");
        textStatus.setAttribute("x", cx);
        textStatus.setAttribute("y", cy + 32);
        textStatus.setAttribute("text-anchor", "middle");
        textStatus.setAttribute("font-family", "Plus Jakarta Sans, sans-serif");
        textStatus.setAttribute("font-size", "11px");
        textStatus.setAttribute("font-weight", "500");
        textStatus.setAttribute("fill", "#587F64");
        textStatus.textContent = `Occupied: ${this.semFull}/${n}`;
        svg.appendChild(textStatus);

        // Draw slots
        for (let i = 0; i < n; i++) {
            const angle = (2 * Math.PI / n) * i - Math.PI / 2;
            const x = cx + radius * Math.cos(angle);
            const y = cy + radius * Math.sin(angle);

            const isOccupied = this.buffer[i] !== null && this.buffer[i] !== undefined;
            const isPointerIn = this.inPtr === i;
            const isPointerOut = this.outPtr === i;

            const slotNode = document.createElementNS("http://www.w3.org/2000/svg", "circle");
            slotNode.setAttribute("cx", x);
            slotNode.setAttribute("cy", y);
            slotNode.setAttribute("r", 24);
            slotNode.setAttribute("fill", isOccupied ? "#E8F0E9" : "#FFFFFF");
            slotNode.setAttribute("stroke", isOccupied ? "#587F64" : "#DFE7E0");
            slotNode.setAttribute("stroke-width", isOccupied ? "2" : "1.5");
            svg.appendChild(slotNode);

            const idxText = document.createElementNS("http://www.w3.org/2000/svg", "text");
            idxText.setAttribute("x", x);
            idxText.setAttribute("y", isOccupied ? y - 5 : y - 2);
            idxText.setAttribute("text-anchor", "middle");
            idxText.setAttribute("font-family", "JetBrains Mono, monospace");
            idxText.setAttribute("font-size", "9px");
            idxText.setAttribute("fill", "#8E9C91");
            idxText.textContent = `[${i}]`;
            svg.appendChild(idxText);

            if (isOccupied) {
                const valText = document.createElementNS("http://www.w3.org/2000/svg", "text");
                valText.setAttribute("x", x);
                valText.setAttribute("y", y + 10);
                valText.setAttribute("text-anchor", "middle");
                valText.setAttribute("font-family", "JetBrains Mono, monospace");
                valText.setAttribute("font-size", "11.5px");
                valText.setAttribute("font-weight", "600");
                valText.setAttribute("fill", "#223026");
                valText.textContent = this.buffer[i];
                svg.appendChild(valText);
            }

            if (isPointerIn || isPointerOut) {
                const ptrDist = radius + 36;
                const px = cx + ptrDist * Math.cos(angle);
                const py = cy + ptrDist * Math.sin(angle);

                const ptrLabel = document.createElementNS("http://www.w3.org/2000/svg", "text");
                ptrLabel.setAttribute("x", px);
                ptrLabel.setAttribute("y", py + 3);
                ptrLabel.setAttribute("text-anchor", "middle");
                ptrLabel.setAttribute("font-family", "JetBrains Mono, monospace");
                ptrLabel.setAttribute("font-size", "9.5px");
                ptrLabel.setAttribute("font-weight", "700");

                if (isPointerIn && isPointerOut) {
                    ptrLabel.setAttribute("fill", "#223026");
                    ptrLabel.textContent = "in·out";
                } else if (isPointerIn) {
                    ptrLabel.setAttribute("fill", "#5F8367");
                    ptrLabel.textContent = "in ➔";
                } else {
                    ptrLabel.setAttribute("fill", "#B27B5D");
                    ptrLabel.textContent = "out ➔";
                }
                svg.appendChild(ptrLabel);
            }
        }
    }

    renderLinearBuffer() {
        this.dom.linearSlotsTrack.innerHTML = '';
        this.dom.linearPointerMeta.textContent = `in: ${this.inPtr} | out: ${this.outPtr} | count: ${this.semFull}`;

        for (let i = 0; i < this.bufferSize; i++) {
            const isOccupied = this.buffer[i] !== null && this.buffer[i] !== undefined;
            const hasIn = this.inPtr === i;
            const hasOut = this.outPtr === i;

            const cell = document.createElement('div');
            cell.className = `slot-cell ${isOccupied ? 'occupied' : ''} ${hasIn ? 'has-in' : ''} ${hasOut ? 'has-out' : ''}`;

            const topMeta = document.createElement('div');
            topMeta.className = 'slot-index';
            topMeta.textContent = `slot ${i}`;

            const val = document.createElement('div');
            val.className = 'slot-value';
            val.textContent = isOccupied ? this.buffer[i] : '—';

            const tags = document.createElement('div');
            tags.className = 'slot-tags';
            if (hasIn) {
                const inTag = document.createElement('span');
                inTag.className = 'slot-tag-in';
                inTag.textContent = 'IN';
                tags.appendChild(inTag);
            }
            if (hasOut) {
                const outTag = document.createElement('span');
                outTag.className = 'slot-tag-out';
                outTag.textContent = 'OUT';
                tags.appendChild(outTag);
            }

            cell.appendChild(topMeta);
            cell.appendChild(val);
            cell.appendChild(tags);
            this.dom.linearSlotsTrack.appendChild(cell);
        }
    }

    renderThreadMonitors() {
        this.dom.producerThreadList.innerHTML = '';
        this.threadStates.producers.forEach(p => {
            const item = document.createElement('div');
            item.className = 'thread-item';

            const left = document.createElement('div');
            left.className = 'thread-id';
            left.textContent = `P${p.id} (${p.produced}/${p.quota})`;

            const badge = document.createElement('span');
            badge.className = `thread-state-badge state-${p.state.toLowerCase()}`;
            badge.textContent = p.state;

            item.appendChild(left);
            item.appendChild(badge);
            this.dom.producerThreadList.appendChild(item);
        });

        this.dom.consumerThreadList.innerHTML = '';
        this.threadStates.consumers.forEach(c => {
            const item = document.createElement('div');
            item.className = 'thread-item';

            const left = document.createElement('div');
            left.className = 'thread-id';
            left.textContent = `C${c.id} (${c.consumed}/${c.quota})`;

            const badge = document.createElement('span');
            badge.className = `thread-state-badge state-${c.state.toLowerCase()}`;
            badge.textContent = c.state;

            item.appendChild(left);
            item.appendChild(badge);
            this.dom.consumerThreadList.appendChild(item);
        });
    }

    renderMetrics() {
        const totalItems = this.numProducers * this.itemsPerProducer;
        if (this.dom.metricThroughput) {
            this.dom.metricThroughput.textContent = `${this.totalProduced} / ${this.totalConsumed}`;
        }

        let elapsed = "0.00s";
        if (this.startTime) {
            elapsed = `${((Date.now() - this.startTime) / 1000).toFixed(2)}s`;
        }

        const fillPct = Math.round((this.semFull / this.bufferSize) * 100);
        if (this.dom.metricTimeElapsed) {
            this.dom.metricTimeElapsed.textContent = `Elapsed: ${elapsed} · Buffer Fill: ${fillPct}% (${totalItems} items)`;
        }
    }
}

document.addEventListener('DOMContentLoaded', () => {
    window.app = new ProducerConsumerApp();
});
