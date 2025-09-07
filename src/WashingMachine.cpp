#include "WashingMachine.h"
#include "utils.h"
#include <chrono>
#include <thread>

WashingMachine::WashingMachine(int id) : id_(id) {}
WashingMachine::~WashingMachine() { stop(); if (worker_.joinable()) worker_.join(); }

bool WashingMachine::startCycle(const std::string& userName, int durationSec, FinishCallback onFinish) {
    bool expected = false;
    if (!busy_.compare_exchange_strong(expected, true)) return false;
    {
        std::lock_guard<std::mutex> lk(fieldsMtx_);
        currentUser_ = userName;
        remaining_.store(durationSec);
    }
    stopFlag_.store(false);
    worker_ = std::thread(&WashingMachine::runCycle_, this, userName, durationSec, onFinish);
    return true;
}

void WashingMachine::runCycle_(std::string userName, int durationSec, FinishCallback onFinish) {
    Utils::Logger::instance().log("Machine #" + std::to_string(id_) + " START for " + userName +
                                  " (" + std::to_string(durationSec) + "s)");
    for (int i = durationSec; i > 0; --i) {
        if (stopFlag_.load()) break;
        remaining_.store(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    remaining_.store(0);
    {
        std::lock_guard<std::mutex> lk(fieldsMtx_);
        Utils::Logger::instance().log("Machine #" + std::to_string(id_) + " COMPLETE for " + currentUser_);
    }
    busy_.store(false);
    if (onFinish) onFinish(id_, userName);
    std::lock_guard<std::mutex> lk(fieldsMtx_);
    currentUser_.clear();
}

bool WashingMachine::isBusy() const { return busy_.load(); }
std::string WashingMachine::currentUser() const { std::lock_guard<std::mutex> lk(fieldsMtx_); return currentUser_; }
int  WashingMachine::remainingSeconds() const { return remaining_.load(); }
void WashingMachine::stop() { stopFlag_.store(true); }
