# Spin-washer-Track
Real-time concurrent washing machine booking system with priority scheduling, countdown timers, and REST API integration.
# 💼 Resume Bullets 

- Built a **concurrent booking system** in **C++11** using **threads**, **mutexes**, and **condition variables** to simulate multiple washing machines with per-second timers.
- Implemented **FIFO scheduling** with a thread-safe queue and real-time **resource allocation** across machines.
- Designed a **REST API** (cpp-httplib) exposing **/book**, **/status**, **/logs**, **/health**, enabling remote bookings and monitoring.
- Developed a **web UI** with live machine cards, countdown timers, **wing-based filtering**, and **completion alerts** (sound + notifications).
