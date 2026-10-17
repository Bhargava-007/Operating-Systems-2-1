# Linux Concurrent Producer-Consumer System Dashboard

This directory contains the systems-engineering UI designed with a refined, minimalist **Sage Green** identity for monitoring, stepping through, and executing the Linux concurrent producer-consumer POSIX threads implementation.

## Features
- **Strict Color Architecture**: Canvas (`#F7F8F5`), Sidebar (`#EDF1EB`), Cards (`#FFFFFF`), Text (`#202B24`), Muted (`#758078`), Sage (`#557A60`), Soft Sage (`#E3EDE4`), Producer (`#62866B`), Consumer (`#B47D5D`), and Error (`#B65D55`).
- **Interactive Circular Buffer Centerpiece**: Crisp SVG geometry mapping circular buffer slots $0$ to $N-1$, dynamic `in` and `out` pointer badges, and an indexed linear memory mirror.
- **Hardware & Concurrency Controls**: Bounded buffer size ($2-16$), producer/consumer thread counts ($1-6$), items-per-producer quota, and simulation animation speed.
- **Native C Compilation Bridge**: Runs the real C codebase through WSL (`make -s && ./pc <buf> <prod> <cons> <items>`) and parses stdout in real-time.
- **Terminal Event Log & File I/O Inspector**: Switch between live terminal output and Unix file-system log (`pc_log.txt`).

## Running the Dashboard
1. Ensure WSL or your Linux environment has `gcc` and `make` installed.
2. Start the dashboard server:
   ```bash
   cd dashboard
   node server.js
   ```
3. Open your browser at:
   ```
   http://localhost:3000
   ```
