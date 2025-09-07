#pragma once
#include "User.h"
#include "WashingMachine.h"
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <unordered_map>

class Scheduler {
public:
    explicit Scheduler(int machineCount);
    ~Scheduler();

    // FIFO booking
    void book(const std::string& name, const std::string& wing, int durationSec);
    void printStatus();
    void shutdown();

    struct MachineSnapshot {
        int id;
        bool busy;
        std::string user;
        std::string wing; // resolved via name->wing cache when possible
        int remaining;
    };
    struct QueueEntrySnapshot {
        std::string name;
        std::string wing;
        int durationSec;
        uint64_t seq;
    };

    std::vector<MachineSnapshot> snapshotMachines();
    std::vector<QueueEntrySnapshot> snapshotQueue();

private:
    void tryDispatch_();                      // assumes lock held
    void onMachineFree_(int id, const std::string& user);
    int  findFreeMachineLocked_();            // assumes lock held

    std::vector<std::unique_ptr<WashingMachine>> machines_;
    std::queue<User> waitQ_;                  // FIFO

    // For displaying wing on active machines
    std::unordered_map<std::string, std::string> nameToWing_;

    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<uint64_t> seqGen_{0};
    bool shuttingDown_{false};
};
