#include "Scheduler.h"
#include "utils.h"
#include <iostream>

Scheduler::Scheduler(int machineCount) {
    machines_.reserve(machineCount);
    for (int i = 0; i < machineCount; ++i)
        machines_.emplace_back(new WashingMachine(i));
}
Scheduler::~Scheduler() { shutdown(); }

void Scheduler::book(const std::string& name, const std::string& wing, int durationSec) {
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (shuttingDown_) return;

        int freeIdx = findFreeMachineLocked_();
        if (freeIdx != -1) {
            auto cb = [this](int id, const std::string& u){ this->onMachineFree_(id, u); };
            nameToWing_[name] = wing;
            if (machines_[freeIdx]->startCycle(name, durationSec, cb)) {
                Utils::Logger::instance().log("Booked IMMEDIATE: " + name + " [" + wing + "] on Machine #" + std::to_string(freeIdx));
                std::cout << "[ALLOC] " << name << " [" << wing << "] -> Machine #" << freeIdx
                          << " (" << durationSec << "s)\n";
                return;
            }
        }

        User u{name, wing, durationSec, seqGen_.fetch_add(1)};
        waitQ_.push(u);
        Utils::Logger::instance().log("Queued: " + name + " [wing " + wing + "]");
        std::cout << "[QUEUE] " << name << " [" << wing << "]\n";
    }
    cv_.notify_one();
}

void Scheduler::printStatus() {
    std::lock_guard<std::mutex> lk(mtx_);
    std::cout << "\n=== Washing Machines ===\n";
    for (size_t i = 0; i < machines_.size(); ++i) {
        auto& m = machines_[i];
        bool busy = m->isBusy();
        auto usr = m->currentUser();
        int  rem = m->remainingSeconds();
        std::string wing = (busy && nameToWing_.count(usr)) ? nameToWing_[usr] : "";
        std::cout << "Machine #" << i << " : " << (busy ? "BUSY" : "FREE");
        if (busy) std::cout << " | User: " << usr << " | Wing: " << (wing.empty()?"-":wing) << " | Remaining: " << rem << "s";
        std::cout << "\n";
    }
    std::cout << "--- Waiting Queue (front first) ---\n";
    std::queue<User> qcopy = waitQ_;
    int rank = 1;
    while (!qcopy.empty()) {
        auto it = qcopy.front(); qcopy.pop();
        std::cout << rank++ << ". " << it.name << " [wing " << it.wing << "] (" << it.durationSec << "s)\n";
    }
    std::cout << "===================================\n";
}

void Scheduler::shutdown() {
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (shuttingDown_) return;
        shuttingDown_ = true;
    }
    cv_.notify_all();
    for (auto& m : machines_) m->stop();
}

int Scheduler::findFreeMachineLocked_() {
    for (size_t i = 0; i < machines_.size(); ++i)
        if (!machines_[i]->isBusy()) return (int)i;
    return -1;
}

void Scheduler::tryDispatch_() {
    while (!waitQ_.empty()) {
        int freeIdx = findFreeMachineLocked_();
        if (freeIdx == -1) break;
        auto next = waitQ_.front(); waitQ_.pop();

        auto cb = [this](int id, const std::string& u){ this->onMachineFree_(id, u); };
        nameToWing_[next.name] = next.wing;
        if (machines_[freeIdx]->startCycle(next.name, next.durationSec, cb)) {
            Utils::Logger::instance().log("Booked FROM-QUEUE: " + next.name + " [" + next.wing + "] on Machine #" + std::to_string(freeIdx));
            std::cout << "[DEQUEUE->ALLOC] " << next.name << " [" << next.wing << "] -> Machine #"
                      << freeIdx << " (" << next.durationSec << "s)\n";
        } else {
            // Shouldn't happen; if it does, restore order and stop
            std::queue<User> tmp;
            tmp.push(next);
            while (!waitQ_.empty()) { tmp.push(waitQ_.front()); waitQ_.pop(); }
            waitQ_ = std::move(tmp);
            break;
        }
    }
}

void Scheduler::onMachineFree_(int /*id*/, const std::string& user) {
    std::unique_lock<std::mutex> lk(mtx_);
    // nameToWing_.erase(user); // optional cleanup
    if (shuttingDown_) return;
    tryDispatch_();
}

std::vector<Scheduler::MachineSnapshot> Scheduler::snapshotMachines() {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<MachineSnapshot> out;
    out.reserve(machines_.size());
    for (size_t i = 0; i < machines_.size(); ++i) {
        auto& m = machines_[i];
        auto usr = m->currentUser();
        out.push_back({ (int)i, m->isBusy(), usr,
                        (m->isBusy() && nameToWing_.count(usr)) ? nameToWing_[usr] : "",
                        m->remainingSeconds() });
    }
    return out;
}

std::vector<Scheduler::QueueEntrySnapshot> Scheduler::snapshotQueue() {
    std::lock_guard<std::mutex> lk(mtx_);
    std::queue<User> qcopy = waitQ_;
    std::vector<QueueEntrySnapshot> out;
    while (!qcopy.empty()) {
        auto it = qcopy.front(); qcopy.pop();
        out.push_back({ it.name, it.wing, it.durationSec, it.seq });
    }
    return out;
}
