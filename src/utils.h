#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

namespace Utils {

inline std::string timestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

class Logger {
public:
    static Logger& instance() {
        static Logger inst("log.txt");
        return inst;
    }

    void log(const std::string& line) {
        std::lock_guard<std::mutex> lock(mtx_);
        ofs_ << "[" << timestamp() << "] " << line << std::endl;
        ofs_.flush();
    }

    std::string tail(size_t n = 100) {
        std::lock_guard<std::mutex> lock(mtx_);
        ofs_.flush();
        std::ifstream ifs(path_, std::ios::in);
        if (!ifs) return "";
        std::vector<std::string> ring;
        ring.reserve(n);
        std::string s;
        while (std::getline(ifs, s)) {
            if (ring.size() < n) ring.push_back(s);
            else {
                std::rotate(ring.begin(), ring.begin() + 1, ring.end());
                ring.back() = s;
            }
        }
        std::ostringstream out;
        for (auto& line : ring) out << line << "\n";
        return out.str();
    }

private:
    explicit Logger(const std::string& path) : path_(path), ofs_(path, std::ios::app) {}
    std::string path_;
    std::ofstream ofs_;
    std::mutex mtx_;
};

} // namespace Utils
