#pragma once
#include <string>
#include <cstdint>

struct User {
    std::string name;
    std::string wing;
    int durationSec;
    uint64_t seq; // FIFO order (monotonic)

    User() = default;
    User(std::string n, std::string w, int d, uint64_t s)
        : name(std::move(n)), wing(std::move(w)), durationSec(d), seq(s) {}
};
