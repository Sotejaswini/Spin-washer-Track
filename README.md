# 🌀 Spin Washer Track

A **real-time concurrent washing machine booking system** built in **C++11**, simulating hostel/apartment laundry management with concurrency and REST APIs.  

---

## ✨ Features

- ⏳ **FIFO Scheduling** — First-Come, First-Served booking  
- 🧵 **Concurrent Execution** — Multiple washing machines run in parallel threads  
- 🔒 **Thread-Safe Queue** — Managed with mutexes and condition variables  
- ⏰ **Per-Second Countdown Timers** for active washing cycles  
- 🌐 **REST API Integration** using [`cpp-httplib`](https://github.com/yhirose/cpp-httplib)  
- 💻 **Web UI** for booking users, viewing live timers, and filtering by wing  
- 🔔 **Alerts** — sound + browser notifications when a machine finishes  
- 📝 **Logging** to `log.txt` for all bookings and completions  
- 📦 **Easy Deployment** — Backend with Docker, frontend via GitHub Pages  

---

## 🛠️ Core Concepts

- C++11 **threads, mutexes, condition variables, atomic operations**  
- **Resource allocation & scheduling** (FIFO queue)  
- **HTTP/REST server** using `httplib.h`  
- **Real-time timer management** with per-second updates  

---

## ⚡ Build & Run

### Compile
```bash
g++ src/main.cpp src/Scheduler.cpp src/WashingMachine.cpp src/User.cpp \
-I src -I third_party -o WashingMachineSystem -std=c++11 -pthread
Run
bash
Copy code
./WashingMachineSystem
👉 You should see the prompt:

typescript
Copy code
Enter number of washing machines:
🌍 Project Flow
Users book washing machines via Web UI (name, wing, duration).

Scheduler assigns machines using FIFO and concurrency control.

Countdown timers display live progress for each machine.

REST API provides booking/status info for frontend.

Alerts + logs generated when cycles complete.

📸 Screenshots / Demo (Optional)
Add architecture diagram, terminal demo, or web UI screenshots here.

📜 License
This project is licensed under the MIT License.

💡 Spin Washer Track demonstrates advanced C++ concurrency and OS concepts applied to a real-world simulation.

yaml
Copy code

---

👉 This README is **resume- and recruiter-friendly**: highlights concurrency, REST, and deployment.  

Do you want me to also design a **small architecture diagram** (machines ↔ scheduler ↔ REST API ↔ w
# Spin-washer-Track
Spin-washer-Track 🌀 | A real-time concurrent washing machine booking system with FIFO scheduling, countdown timers, and REST API integration. Built in C++ (threads, mutexes, condition variables) with a REST + Web UI frontend.
Real-time concurrent washing machine booking system with priority scheduling, countdown timers, and REST API integration.
Concurrent Washing Machine Scheduler (FIFO) — C++ + REST + Web UI

A real-time simulation of a washing machine booking system for hostels/apartments:

FIFO scheduling (First-Come, First-Served) — no priority

Multiple machines run concurrently (each in its own thread)

Thread-safe queue & machine allocation with mutex/condition variable

Per-second countdown timers for active cycles

REST API to book users & fetch status

Web UI to add users (name/wing/duration), filter by wing, view live timers

Alerts (sound + browser notification) when a machine finishes

Logging to log.txt for all bookings and completions

Easy live preview: Frontend deployable on GitHub Pages; backend deployable with Docker

3) Rebuild
g++ src/main.cpp src/Scheduler.cpp src/WashingMachine.cpp src/User.cpp \
   -I src -I third_party -o WashingMachineSystem -std=c++11 -pthread

./WashingMachineSystem


You should now get the prompt: Enter number of washing machines:

Core OS/Concurrency Concepts: threads, mutexes, condition variables, resource allocation, FIFO queues, timer management, HTTP/REST.
# 💼 Resume Bullets 

- Built a **concurrent booking system** in **C++11** using **threads**, **mutexes**, and **condition variables** to simulate multiple washing machines with per-second timers.
- Implemented **FIFO scheduling** with a thread-safe queue and real-time **resource allocation** across machines.
- Designed a **REST API** (cpp-httplib) exposing **/book**, **/status**, **/logs**, **/health**, enabling remote bookings and monitoring.
- Developed a **web UI** with live machine cards, countdown timers, **wing-based filtering**, and **completion alerts** (sound + notifications).
- Get third_party/httplib.h from https://github.com/yhirose/cpp-httplib
 (single header).
