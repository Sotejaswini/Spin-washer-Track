# Spin-washer-Track
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

Core OS/Concurrency Concepts: threads, mutexes, condition variables, resource allocation, FIFO queues, timer management, HTTP/REST.
# 💼 Resume Bullets 

- Built a **concurrent booking system** in **C++11** using **threads**, **mutexes**, and **condition variables** to simulate multiple washing machines with per-second timers.
- Implemented **FIFO scheduling** with a thread-safe queue and real-time **resource allocation** across machines.
- Designed a **REST API** (cpp-httplib) exposing **/book**, **/status**, **/logs**, **/health**, enabling remote bookings and monitoring.
- Developed a **web UI** with live machine cards, countdown timers, **wing-based filtering**, and **completion alerts** (sound + notifications).
- Get third_party/httplib.h from https://github.com/yhirose/cpp-httplib
 (single header).
