# Smart CPU Scheduler with Dynamic Priority Adjustment

**Course:** Operating Systems and Systems Programming  
**Course Code:** 25CS2104E  
**Section:** S11 | **Team No:** 1  
**Guide/Instructor:** Mrs. Alekhya  

### Team Members
| Roll Number | Name |
|---|---|
| 2520090169 | Bhargava |
| 2520090107 | Hanish |
| 2520090162 | Teja |
| 2520090043 | Pardhu |

**Project Category:** OS Lab Hackathon Project  

---

## Problem Statement

> "Design a CPU scheduler that dynamically adjusts process priorities to reduce starvation and improve response time."

In traditional static priority scheduling, low-priority processes can suffer from indefinite blocking (starvation) if higher-priority processes continuously arrive or consume the CPU. This project implements a **Smart CPU Scheduler** featuring an aging-based dynamic priority adjustment algorithm to guarantee progress, prevent starvation, and achieve balanced turnaround across all processes.

---

## Folder Structure

```
FinalHackathon/
├── core/
│   ├── scheduler.c           # Core scheduling logic (FCFS, Round Robin, Priority, Comparison)
│   ├── dynamic_priority.c    # Aging algorithm / starvation prevention logic
│   ├── process.h             # Process struct and shared definitions
│   └── Makefile              # To compile all C files
├── ui/
│   ├── index.html            # Main UI entry point (zero dependencies)
│   ├── style.css             # Design system (Plus Jakarta Sans, warm-neutral palette)
│   └── app.js                # Gantt chart, process table, UI logic
├── docs/
│   ├── project-report.md     # Full project report
│   └── system-design.md      # Architecture + algorithm explanation
└── README.md                 # Setup, compilation, and run instructions
```

---

## Core Algorithms Implemented

1. **First-Come, First-Served (FCFS)**:
   - Non-preemptive scheduling based strictly on arrival time order.
2. **Round Robin (RR)**:
   - Preemptive scheduling allocating a configurable time quantum per cycle.
3. **Static Priority Scheduling (Non-preemptive)**:
   - Dispatches processes based on original static priority ($1 = \text{Highest Priority}$).
4. **Dynamic Priority with Aging**:
   - Progressive aging rule: for every `AGING_INTERVAL` ($5$ time units) a process waits in the READY state, its priority is boosted by $+1$ (numerical value decreases by $1$).
   - Runaway protection: Priority boost is capped at $+5$ levels above original priority, bounded at highest priority level $1$.
   - Starvation monitoring: Alerts triggered whenever a process waits $> 20$ units.

---

## How to Compile & Run the C Core

Because GCC and Make are installed inside your Linux/WSL environment, execute using `wsl`:

### If you are in `FinalHackathon\core` in PowerShell:
```powershell
wsl make clean
wsl make
wsl ./scheduler
```

### If you are in the workspace root (`Operating-Systems-2-1`):
```powershell
wsl bash -c "cd FinalHackathon/core && make clean && make && ./scheduler"
```

### If you are inside WSL Bash (`wsl`):
```bash
make clean
make
./scheduler
```

The output will display:
1. Step-by-step CPU execution traces for FCFS, Round Robin, Static Priority, and Dynamic Priority.
2. Process metric tables (Arrival, Burst, Priority, Waiting Time, Turnaround Time, Response Time).
3. Comparative analysis table demonstrating waiting time reductions and starvation mitigation.

---

## How to Run the UI

Open the web interface directly in any modern browser without needing a web server or Node runtime:
- Double-click or open `FinalHackathon/ui/index.html` in Chrome, Firefox, Edge, or Safari.
- Alternatively, via command line:
  ```bash
  # Windows PowerShell
  start FinalHackathon/ui/index.html
  ```

### UI Features
- **Visual Process Workload Management**: Add custom processes or load sample workloads.
- **Interactive Algorithm Switcher**: FCFS, Round Robin (with dynamic Quantum input), Static Priority, Dynamic Priority.
- **Gantt Chart Timeline**: Muted pastel color-coded horizontal execution blocks with timestamp tick marks and idle state visualization.
- **Starvation Alert Banner**: Live alert whenever any process waits over 20 time units.
- **Side-by-Side Comparison Engine**: Compares Static Priority vs Dynamic Aging to visually demonstrate starvation elimination.

---

## Screenshots Placeholder

*Place your demonstration screenshots in `FinalHackathon/docs/screenshots/` (e.g. `gantt_timeline.png`, `comparison_view.png`).*
