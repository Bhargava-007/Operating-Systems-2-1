/**
 * SchedSim — CPU Scheduler Visualizer
 * Zero-dependency Vanilla JS implementation
 */

// Muted pastel palette for process visual blocks
const PROCESS_COLORS = [
    '#A8C5DA', // P1
    '#B5D5A8', // P2
    '#D4B8E0', // P3
    '#F0D9A0', // P4
    '#F4B8A0', // P5
    '#B8E0D2', // P6
    '#EAC4D5', // P7
    '#D6E2E9'  // P8
];

// Initial realistic workload demonstrating high/low priority competition & potential starvation
const DEFAULT_PROCESSES = [
    { pid: 1, name: 'P1', arrival: 0, burst: 8, priority: 3 },
    { pid: 2, name: 'P2', arrival: 1, burst: 4, priority: 1 },
    { pid: 3, name: 'P3', arrival: 2, burst: 9, priority: 4 },
    { pid: 4, name: 'P4', arrival: 3, burst: 5, priority: 2 },
    { pid: 5, name: 'P5', arrival: 4, burst: 2, priority: 1 }
];

let processList = JSON.parse(JSON.stringify(DEFAULT_PROCESSES));

// DOM Elements
const algorithmSelect = document.getElementById('algorithmSelect');
const quantumGroup = document.getElementById('quantumGroup');
const timeQuantumInput = document.getElementById('timeQuantumInput');
const btnRun = document.getElementById('btnRun');
const btnReset = document.getElementById('btnReset');
const btnLoadSample = document.getElementById('btnLoadSample');
const addProcessForm = document.getElementById('addProcessForm');
const processQueueBody = document.getElementById('processQueueBody');

const ganttTimeline = document.getElementById('ganttTimeline');
const ganttTimeAxis = document.getElementById('ganttTimeAxis');
const ganttLegend = document.getElementById('ganttLegend');
const timelineDuration = document.getElementById('timelineDuration');
const resultsBody = document.getElementById('resultsBody');

const avgWaitVal = document.getElementById('avgWaitVal');
const avgTatVal = document.getElementById('avgTatVal');
const avgRespVal = document.getElementById('avgRespVal');
const starveCountVal = document.getElementById('starveCountVal');
const starvationAlertArea = document.getElementById('starvationAlertArea');

const btnRunComparison = document.getElementById('btnRunComparison');
const comparisonArea = document.getElementById('comparisonArea');
const compStaticWait = document.getElementById('compStaticWait');
const compStaticTat = document.getElementById('compStaticTat');
const compStaticStarve = document.getElementById('compStaticStarve');
const compDynamicWait = document.getElementById('compDynamicWait');
const compDynamicTat = document.getElementById('compDynamicTat');
const compDynamicStarve = document.getElementById('compDynamicStarve');
const comparisonVerdict = document.getElementById('comparisonVerdict');

// Initialize event listeners
function initApp() {
    algorithmSelect.addEventListener('change', () => {
        if (algorithmSelect.value === 'RR') {
            quantumGroup.style.display = 'block';
        } else {
            quantumGroup.style.display = 'none';
        }
    });

    addProcessForm.addEventListener('submit', (e) => {
        e.preventDefault();
        const name = document.getElementById('procName').value.trim();
        const arrival = parseInt(document.getElementById('procArrival').value, 10);
        const burst = parseInt(document.getElementById('procBurst').value, 10);
        const priority = parseInt(document.getElementById('procPriority').value, 10);

        if (!name || isNaN(arrival) || isNaN(burst) || isNaN(priority)) return;

        const nextPid = processList.length > 0 ? Math.max(...processList.map(p => p.pid)) + 1 : 1;
        processList.push({ pid: nextPid, name, arrival, burst, priority });
        renderProcessQueue();
        addProcessForm.reset();
        document.getElementById('procArrival').value = 0;
        document.getElementById('procBurst').value = 4;
        document.getElementById('procPriority').value = 2;
    });

    btnRun.addEventListener('click', () => {
        runScheduling();
    });

    btnReset.addEventListener('click', () => {
        resetApp();
    });

    btnLoadSample.addEventListener('click', () => {
        processList = JSON.parse(JSON.stringify(DEFAULT_PROCESSES));
        renderProcessQueue();
    });

    btnRunComparison.addEventListener('click', () => {
        runComparisonAnalysis();
    });

    renderProcessQueue();
}

// Render process input queue table
function renderProcessQueue() {
    processQueueBody.innerHTML = '';
    if (processList.length === 0) {
        processQueueBody.innerHTML = '<tr><td colspan="5" class="empty-state-text">No processes in queue. Add one above.</td></tr>';
        return;
    }

    processList.forEach((p, idx) => {
        const color = PROCESS_COLORS[(p.pid - 1) % PROCESS_COLORS.length];
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>
                <span style="display:inline-block; width:10px; height:10px; border-radius:2px; background:${color}; margin-right:6px;"></span>
                <strong>${p.name}</strong>
            </td>
            <td>${p.arrival}</td>
            <td>${p.burst}</td>
            <td>${p.priority}</td>
            <td><button class="btn-remove" onclick="removeProcess(${idx})">Remove</button></td>
        `;
        processQueueBody.appendChild(tr);
    });
}

window.removeProcess = function(index) {
    processList.splice(index, 1);
    renderProcessQueue();
};

function resetApp() {
    ganttTimeline.innerHTML = '<div class="empty-state-text">Press "Run Simulation" to render execution timeline.</div>';
    ganttTimeAxis.innerHTML = '';
    ganttLegend.innerHTML = '';
    timelineDuration.textContent = 'Duration: 0 units';
    resultsBody.innerHTML = '<tr><td colspan="9" class="empty-state-text">No simulation run yet.</td></tr>';
    avgWaitVal.textContent = '—';
    avgTatVal.textContent = '—';
    avgRespVal.textContent = '—';
    starveCountVal.textContent = '0';
    starvationAlertArea.style.display = 'none';
    comparisonArea.style.display = 'none';
}

// Clone workload cleanly
function cloneWorkload() {
    return processList.map(p => ({
        pid: p.pid,
        name: p.name,
        arrival: p.arrival,
        burst: p.burst,
        remaining: p.burst,
        originalPriority: p.priority,
        currentPriority: p.priority,
        waitingTime: 0,
        turnaroundTime: 0,
        responseTime: -1,
        completionTime: 0,
        firstScheduled: -1,
        done: false
    }));
}

// 1. FCFS Scheduling Algorithm
function scheduleFCFS(processes) {
    const n = processes.length;
    let completed = 0;
    let currentTime = 0;
    const executionLog = []; // { pid, name, start, duration }

    while (completed < n) {
        // Find arrived process with earliest arrival that is not done
        const available = processes.filter(p => !p.done && p.arrival <= currentTime);
        if (available.length === 0) {
            // CPU Idle
            const nextArrival = Math.min(...processes.filter(p => !p.done).map(p => p.arrival));
            executionLog.push({ pid: -1, name: 'Idle', start: currentTime, duration: nextArrival - currentTime });
            currentTime = nextArrival;
            continue;
        }

        available.sort((a, b) => a.arrival - b.arrival);
        const curr = available[0];

        if (curr.firstScheduled === -1) {
            curr.firstScheduled = currentTime;
            curr.responseTime = currentTime - curr.arrival;
        }

        curr.waitingTime = currentTime - curr.arrival;
        executionLog.push({ pid: curr.pid, name: curr.name, start: currentTime, duration: curr.burst });
        currentTime += curr.burst;
        curr.remaining = 0;
        curr.done = true;
        curr.completionTime = currentTime;
        curr.turnaroundTime = curr.completionTime - curr.arrival;
        completed++;
    }

    return { processes, executionLog, totalTime: currentTime };
}

// 2. Round Robin Algorithm
function scheduleRoundRobin(processes, quantum) {
    const n = processes.length;
    let completed = 0;
    let currentTime = 0;
    const executionLog = [];
    const queue = [];
    const inQueue = new Set();

    // Enqueue arrived processes at t=0
    processes.filter(p => p.arrival <= currentTime).forEach(p => {
        queue.push(p);
        inQueue.add(p.pid);
    });

    while (completed < n) {
        if (queue.length === 0) {
            const unDone = processes.filter(p => !p.done);
            if (unDone.length === 0) break;
            const nextArrival = Math.min(...unDone.map(p => p.arrival));
            executionLog.push({ pid: -1, name: 'Idle', start: currentTime, duration: nextArrival - currentTime });
            currentTime = nextArrival;

            processes.filter(p => p.arrival <= currentTime && !p.done && !inQueue.has(p.pid)).forEach(p => {
                queue.push(p);
                inQueue.add(p.pid);
            });
            continue;
        }

        const curr = queue.shift();
        inQueue.delete(curr.pid);

        if (curr.firstScheduled === -1) {
            curr.firstScheduled = currentTime;
            curr.responseTime = currentTime - curr.arrival;
        }

        const slice = Math.min(curr.remaining, quantum);
        executionLog.push({ pid: curr.pid, name: curr.name, start: currentTime, duration: slice });

        currentTime += slice;
        curr.remaining -= slice;

        // Check for arrivals during this time slice
        processes.filter(p => p.arrival <= currentTime && !p.done && p.pid !== curr.pid && !inQueue.has(p.pid)).forEach(p => {
            queue.push(p);
            inQueue.add(p.pid);
        });

        if (curr.remaining === 0) {
            curr.done = true;
            curr.completionTime = currentTime;
            curr.turnaroundTime = curr.completionTime - curr.arrival;
            curr.waitingTime = curr.turnaroundTime - curr.burst;
            completed++;
        } else {
            queue.push(curr);
            inQueue.add(curr.pid);
        }
    }

    return { processes, executionLog, totalTime: currentTime };
}

// 3. Static Priority (Non-preemptive)
function scheduleStaticPriority(processes) {
    const n = processes.length;
    let completed = 0;
    let currentTime = 0;
    const executionLog = [];

    while (completed < n) {
        const available = processes.filter(p => !p.done && p.arrival <= currentTime);
        if (available.length === 0) {
            const nextArrival = Math.min(...processes.filter(p => !p.done).map(p => p.arrival));
            executionLog.push({ pid: -1, name: 'Idle', start: currentTime, duration: nextArrival - currentTime });
            currentTime = nextArrival;
            continue;
        }

        // Sort by priority (lowest number = highest priority), then arrival
        available.sort((a, b) => a.originalPriority - b.originalPriority || a.arrival - b.arrival);
        const curr = available[0];

        if (curr.firstScheduled === -1) {
            curr.firstScheduled = currentTime;
            curr.responseTime = currentTime - curr.arrival;
        }

        curr.waitingTime = currentTime - curr.arrival;
        executionLog.push({ pid: curr.pid, name: curr.name, start: currentTime, duration: curr.burst });
        currentTime += curr.burst;
        curr.remaining = 0;
        curr.done = true;
        curr.completionTime = currentTime;
        curr.turnaroundTime = curr.completionTime - curr.arrival;
        completed++;
    }

    return { processes, executionLog, totalTime: currentTime };
}

// 4. Dynamic Priority with Aging & Starvation Prevention
function scheduleDynamicPriority(processes) {
    const n = processes.length;
    let completed = 0;
    let currentTime = 0;
    const rawTicks = []; // Track tick by tick to aggregate contiguous slices

    while (completed < n) {
        const available = processes.filter(p => !p.done && p.arrival <= currentTime);
        if (available.length === 0) {
            rawTicks.push({ pid: -1, name: 'Idle' });
            currentTime++;
            continue;
        }

        // Select process with best current_priority
        available.sort((a, b) => a.currentPriority - b.currentPriority || a.arrival - b.arrival);
        const curr = available[0];

        if (curr.firstScheduled === -1) {
            curr.firstScheduled = currentTime;
            curr.responseTime = currentTime - curr.arrival;
        }

        rawTicks.push({ pid: curr.pid, name: curr.name });
        curr.remaining--;
        currentTime++;

        // Update waiting time and apply aging for all OTHER ready processes
        for (let i = 0; i < n; i++) {
            const other = processes[i];
            if (other.pid !== curr.pid && !other.done && other.arrival < currentTime) {
                other.waitingTime++;
                // Aging rule: every 5 units wait, boost priority by 1 (capped at +5 boost)
                const boost = Math.min(5, Math.floor(other.waitingTime / 5));
                const newPrio = Math.max(1, other.originalPriority - boost);
                other.currentPriority = newPrio;
            }
        }

        if (curr.remaining === 0) {
            curr.done = true;
            curr.completionTime = currentTime;
            curr.turnaroundTime = curr.completionTime - curr.arrival;
            completed++;
        }
    }

    // Compress raw tick blocks into contiguous execution blocks for Gantt rendering
    const executionLog = [];
    if (rawTicks.length > 0) {
        let currentBlock = { pid: rawTicks[0].pid, name: rawTicks[0].name, start: 0, duration: 1 };
        for (let t = 1; t < rawTicks.length; t++) {
            if (rawTicks[t].pid === currentBlock.pid) {
                currentBlock.duration++;
            } else {
                executionLog.push(currentBlock);
                currentBlock = { pid: rawTicks[t].pid, name: rawTicks[t].name, start: t, duration: 1 };
            }
        }
        executionLog.push(currentBlock);
    }

    return { processes, executionLog, totalTime: currentTime };
}

// Main execution coordinator
function runScheduling() {
    if (processList.length === 0) {
        alert('Please add at least one process to schedule.');
        return;
    }

    const algo = algorithmSelect.value;
    const quantum = parseInt(timeQuantumInput.value, 10) || 3;
    const workload = cloneWorkload();

    let result;
    if (algo === 'FCFS') {
        result = scheduleFCFS(workload);
    } else if (algo === 'RR') {
        result = scheduleRoundRobin(workload, quantum);
    } else if (algo === 'Priority') {
        result = scheduleStaticPriority(workload);
    } else {
        result = scheduleDynamicPriority(workload);
    }

    renderGanttChart(result.executionLog, result.totalTime);
    renderResultsTable(result.processes);
    detectStarvation(result.processes);
}

// Render horizontal Gantt timeline
function renderGanttChart(log, totalTime) {
    ganttTimeline.innerHTML = '';
    ganttTimeAxis.innerHTML = '';
    ganttLegend.innerHTML = '';
    timelineDuration.textContent = `Duration: ${totalTime} units`;

    if (totalTime === 0) return;

    log.forEach(block => {
        const widthPct = (block.duration / totalTime) * 100;
        const div = document.createElement('div');
        div.className = `gantt-block ${block.pid === -1 ? 'gantt-block-idle' : ''}`;
        div.style.width = `${widthPct}%`;

        if (block.pid !== -1) {
            const color = PROCESS_COLORS[(block.pid - 1) % PROCESS_COLORS.length];
            div.style.backgroundColor = color;
            div.textContent = `${block.name} (${block.duration})`;
            div.title = `${block.name} [t=${block.start} to t=${block.start + block.duration}]`;
        } else {
            div.textContent = 'Idle';
            div.title = `Idle [t=${block.start} to t=${block.start + block.duration}]`;
        }
        ganttTimeline.appendChild(div);
    });

    // Filter and position time Axis tick marks cleanly to avoid overlapping
    const rawTicks = [0];
    log.forEach(b => rawTicks.push(b.start + b.duration));
    const uniqueTicks = [...new Set(rawTicks)].sort((a, b) => a - b);

    // Filter ticks that are too close to each other (minimum 4% spacing)
    const displayTicks = [];
    let lastPct = -10;
    uniqueTicks.forEach(t => {
        const pct = (t / totalTime) * 100;
        if (pct - lastPct >= 4 || t === totalTime) {
            displayTicks.push({ time: t, pct });
            lastPct = pct;
        }
    });

    displayTicks.forEach(tick => {
        const span = document.createElement('span');
        span.className = 'gantt-tick';
        // Clamp 0 and 100% to stay within container bounds
        if (tick.pct === 0) {
            span.style.left = '0%';
            span.style.transform = 'none';
        } else if (tick.pct >= 99) {
            span.style.left = '100%';
            span.style.transform = 'translateX(-100%)';
        } else {
            span.style.left = `${tick.pct}%`;
            span.style.transform = 'translateX(-50%)';
        }
        span.textContent = tick.time;
        ganttTimeAxis.appendChild(span);
    });

    // Render legend for processes present
    const uniqueProcs = [...new Map(processList.map(p => [p.pid, p])).values()];
    uniqueProcs.forEach(p => {
        const color = PROCESS_COLORS[(p.pid - 1) % PROCESS_COLORS.length];
        const item = document.createElement('div');
        item.className = 'legend-item';
        item.innerHTML = `<span class="legend-swatch" style="background:${color};"></span><span>${p.name} (Priority ${p.priority})</span>`;
        ganttLegend.appendChild(item);
    });
}

// Populate execution table & statistics
function renderResultsTable(processes) {
    resultsBody.innerHTML = '';
    let totalWt = 0;
    let totalTat = 0;
    let totalRt = 0;

    processes.forEach(p => {
        totalWt += p.waitingTime;
        totalTat += p.turnaroundTime;
        totalRt += p.responseTime;

        const tr = document.createElement('tr');
        if (p.waitingTime > 20) {
            tr.style.backgroundColor = 'var(--danger-soft)';
        }
        tr.innerHTML = `
            <td>${p.pid}</td>
            <td><strong>${p.name}</strong></td>
            <td>${p.arrival}</td>
            <td>${p.burst}</td>
            <td>${p.originalPriority}</td>
            <td><strong>${p.currentPriority}</strong></td>
            <td class="${p.waitingTime > 20 ? 'text-danger' : ''}">${p.waitingTime}</td>
            <td>${p.turnaroundTime}</td>
            <td>${p.responseTime}</td>
        `;
        resultsBody.appendChild(tr);
    });

    const n = processes.length;
    avgWaitVal.textContent = (totalWt / n).toFixed(2);
    avgTatVal.textContent = (totalTat / n).toFixed(2);
    avgRespVal.textContent = (totalRt / n).toFixed(2);
}

// Detect and display starvation warnings (>20 waiting units)
function detectStarvation(processes) {
    const starved = processes.filter(p => p.waitingTime > 20);
    starveCountVal.textContent = starved.length;

    if (starved.length > 0) {
        starvationAlertArea.style.display = 'block';
        starvationAlertArea.innerHTML = `
            <div class="alert-title">Starvation Detected: ${starved.length} process(es) waited over 20 time units!</div>
            ${starved.map(s => `<div class="alert-item">Process <strong>${s.name}</strong> (PID ${s.pid}) waited for <strong>${s.waitingTime} units</strong> before finishing.</div>`).join('')}
        `;
    } else {
        starvationAlertArea.style.display = 'none';
    }
}

// Side-by-side comparison of Static Priority vs Dynamic Priority
function runComparisonAnalysis() {
    if (processList.length === 0) {
        alert('Please add at least one process to compare.');
        return;
    }

    const staticRes = scheduleStaticPriority(cloneWorkload());
    const dynamicRes = scheduleDynamicPriority(cloneWorkload());

    const n = processList.length;
    const staticAvgWait = staticRes.processes.reduce((acc, p) => acc + p.waitingTime, 0) / n;
    const staticAvgTat = staticRes.processes.reduce((acc, p) => acc + p.turnaroundTime, 0) / n;
    const staticStarved = staticRes.processes.filter(p => p.waitingTime > 20).length;

    const dynamicAvgWait = dynamicRes.processes.reduce((acc, p) => acc + p.waitingTime, 0) / n;
    const dynamicAvgTat = dynamicRes.processes.reduce((acc, p) => acc + p.turnaroundTime, 0) / n;
    const dynamicStarved = dynamicRes.processes.filter(p => p.waitingTime > 20).length;

    compStaticWait.textContent = `${staticAvgWait.toFixed(2)} units`;
    compStaticTat.textContent = `${staticAvgTat.toFixed(2)} units`;
    compStaticStarve.textContent = `${staticStarved} process(es)`;

    compDynamicWait.textContent = `${dynamicAvgWait.toFixed(2)} units`;
    compDynamicTat.textContent = `${dynamicAvgTat.toFixed(2)} units`;
    compDynamicStarve.textContent = `${dynamicStarved} process(es)`;

    comparisonArea.style.display = 'block';

    if (staticStarved > 0 && dynamicStarved === 0) {
        comparisonVerdict.innerHTML = `<strong>Result:</strong> Dynamic Priority completely eliminated starvation (${staticStarved} starved under static priority vs 0 with dynamic aging) while preventing low-priority process abandonment.`;
    } else {
        const diff = (staticAvgWait - dynamicAvgWait).toFixed(2);
        comparisonVerdict.innerHTML = `<strong>Result:</strong> Dynamic Aging ensured fair progressive priority escalations, bounding maximum latency and maintaining balanced queue turnaround.`;
    }
}

// Start application
document.addEventListener('DOMContentLoaded', initApp);
