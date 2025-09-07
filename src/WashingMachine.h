#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

class WashingMachine {
public:
    using FinishCallback = std::function<void(int,const std::string&)>; // id, user

    explicit WashingMachine(int id);
    ~WashingMachine();

    bool startCycle(const std::string& userName, int durationSec, FinishCallback onFinish);

    bool isBusy() const;
    std::string currentUser() const;
    int remainingSeconds() const;
    int id() const { return id_; }

    void stop();

private:
    void runCycle_(std::string userName, int durationSec, FinishCallback onFinish);

    int id_;
    std::thread worker_;
    std::atomic<bool> busy_{false};
    std::atomic<int> remaining_{0};
    std::string currentUser_;
    mutable std::mutex fieldsMtx_;
    std::atomic<bool> stopFlag_{false};
};
