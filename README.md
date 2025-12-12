# 🌀 Spin Washer Track

A **real-time concurrent washing machine booking system** built in **C++11**, simulating hostel/apartment laundry management with concurrency, scheduling, and REST APIs.  

📺 Demo: [Watch here](https://drive.google.com/file/d/14XUmhXVbyr1VlH6rPqpAs5nliOaG83yK/view)
![Screenshot](https://github.com/Sotejaswini/Spin-washer-Track/blob/main/output/cmd.png)
![Screenshot](https://github.com/Sotejaswini/Spin-washer-Track/blob/main/output/booking.png)

---

## ✨ Highlights

- ⏳ **FIFO Scheduling** — First-Come, First-Served booking  
- 🧵 **Concurrency** — Multiple washing machines run in parallel threads  
- 🔒 **Thread-Safe Queue** — Controlled via mutexes & condition variables  
- ⏰ **Live Timers** — Per-second countdown for active cycles  
- 🌐 **REST API** — Powered by [`cpp-httplib`](https://github.com/yhirose/cpp-httplib)  
- 💻 **Web UI** — Book users, view machine status, filter by wing  
- 🔔 **Smart Alerts** — Sound + browser notifications on completion  
- 📝 **Logging** — Full booking & completion logs in `log.txt`  
- 📦 **Deployment Ready** — Backend via Docker, frontend via GitHub Pages  

---

## 🛠️ Core Concepts

- C++11 **threads, mutexes, condition variables, atomics**  
- **Resource allocation & scheduling** with FIFO queues  
- **HTTP/REST server** integration using `httplib.h`  
- **Timer management** with real-time per-second updates  

---

## ⚡ Build & Run

### 1. Compile
```bash
g++ src/main.cpp src/Scheduler.cpp src/WashingMachine.cpp src/User.cpp \
    -I src -I third_party -o WashingMachineSystem -std=c++11 -pthread
```
### 2. Run
```bash
./WashingMachineSystem
```
### 👉 You’ll be prompted with:
```
Enter number of washing machines:
```

### 🌍 How It Works

- Users book machines via the Web UI (enter name, wing, duration)

- Scheduler assigns machines using FIFO + concurrency control

- Countdown timers display live progress for each active machine

- REST API exposes booking & status for the frontend

- Alerts + logs fire when cycles complete

## License

This project is licensed under the MIT License.
