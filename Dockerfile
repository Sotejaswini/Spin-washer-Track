FROM ubuntu:22.04

RUN apt-get update && apt-get install -y g++

WORKDIR /app

COPY . .

RUN g++ src/main.cpp src/Scheduler.cpp src/WashingMachine.cpp src/User.cpp \
    -I src -I third_party -o WashingMachineSystem -std=c++11 -pthread

EXPOSE 8080

CMD ["./WashingMachineSystem"]
