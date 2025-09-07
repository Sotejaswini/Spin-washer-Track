// src/main.cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <csignal>

#include "Scheduler.h"
#include "utils.h"
#include "httplib.h"   // make sure -I third_party is in your compile line

static std::atomic<bool> g_running(true);

struct Args {
    bool headless = false;
    int machines = -1;
    int port = 8080;
};

static Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "--headless") a.headless = true;
        else if (s == "--machines" && i + 1 < argc) a.machines = std::stoi(argv[++i]);
        else if (s == "--port" && i + 1 < argc) a.port = std::stoi(argv[++i]);
    }
    if (const char* p = std::getenv("PORT")) { try { a.port = std::stoi(p); } catch (...) {} }
    return a;
}

static std::string statusJson(Scheduler& sched) {
    auto ms = sched.snapshotMachines();
    auto qs = sched.snapshotQueue();
    std::ostringstream oss;
    oss << "{ \"machines\": [";
    for (size_t i = 0; i < ms.size(); ++i) {
        const auto& m = ms[i];
        oss << "{"
            << "\"id\":" << m.id << ","
            << "\"busy\":" << (m.busy ? "true" : "false") << ","
            << "\"user\":\"" << m.user << "\","
            << "\"wing\":\"" << m.wing << "\","
            << "\"remaining\":" << m.remaining
            << "}";
        if (i + 1 < ms.size()) oss << ",";
    }
    oss << "], \"queue\": [";
    for (size_t i = 0; i < qs.size(); ++i) {
        const auto& q = qs[i];
        oss << "{"
            << "\"name\":\"" << q.name << "\","
            << "\"wing\":\"" << q.wing << "\","
            << "\"duration\":" << q.durationSec << ","
            << "\"seq\":" << q.seq
            << "}";
        if (i + 1 < qs.size()) oss << ",";
    }
    oss << "] }";
    return oss.str();
}

void restServer(Scheduler& sched, std::shared_ptr<httplib::Server> svr, int port) {
    // CORS
    httplib::Headers cors = {
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET,POST,OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"},
    };
    svr->set_default_headers(cors);
    svr->Options(".*", [](const httplib::Request&, httplib::Response& res){ res.status = 200; });

    // Reasonable network timeouts
    svr->set_read_timeout(5, 0);
    svr->set_write_timeout(5, 0);
    svr->set_idle_interval(0, 100000); // 100ms

    // Serve UI from ./web
    bool mounted = svr->set_mount_point("/", "./web");
    if (!mounted) std::cerr << "[REST] Failed to mount ./web (UI). Check path.\n";

    // Routes
    svr->Get("/health", [](const httplib::Request&, httplib::Response& res){
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr->Post("/book", [&](const httplib::Request& req, httplib::Response& res) {
        auto name = req.get_param_value("name");
        auto wing = req.get_param_value("wing");
        auto dur  = req.get_param_value("duration");
        if (name.empty() || wing.empty() || dur.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"Missing params: name,wing,duration\"}", "application/json");
            return;
        }
        int duration = 0;
        try { duration = std::max(1, std::stoi(dur)); }
        catch (...) { res.status = 400; res.set_content("{\"error\":\"Invalid duration\"}", "application/json"); return; }

        sched.book(name, wing, duration);
        res.set_content("{\"ok\":true,\"message\":\"Booking accepted\",\"name\":\""+name+"\",\"wing\":\""+wing+"\"}", "application/json");
    });

    svr->Get("/status", [&](const httplib::Request&, httplib::Response& res) {
        sched.printStatus();
        res.set_content(statusJson(sched), "application/json");
    });

    svr->Get("/logs", [&](const httplib::Request& req, httplib::Response& res) {
        size_t n = 100;
        if (req.has_param("n")) { try { int v = std::stoi(req.get_param_value("n")); if (v > 0 && v <= 2000) n = (size_t)v; } catch (...) {} }
        res.set_content(Utils::Logger::instance().tail(n), "text/plain");
    });

    svr->Post("/shutdown", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"ok\":true,\"message\":\"Shutting down\"}", "application/json");
        g_running.store(false);
        std::thread([svr]{ std::this_thread::sleep_for(std::chrono::milliseconds(50)); svr->stop(); }).detach();
    });

    std::cout << "[REST] Open UI at:  http://localhost:" << port << "/\n";
    std::cout << "[REST] Health at:   http://localhost:" << port << "/health\n";

    bool ok = svr->listen("0.0.0.0", port);
    if (!ok) std::cerr << "[REST] Failed to bind port " << port << ". Try --port 9000\n";
}

static void consoleLoop(Scheduler& sched) {
    while (g_running.load()) {
        std::cout << "\n1. Book Machine\n2. Show Status\n3. Exit\nChoose: ";
        int ch;
        if (!(std::cin >> ch)) break;
        if (ch == 1) {
            std::string name, wing; int dur;
            std::cout << "Name: "; std::cin >> name;
            std::cout << "Wing: "; std::cin >> wing;
            std::cout << "Duration(sec): "; std::cin >> dur;
            if (dur < 1) dur = 1;
            sched.book(name, wing, dur);
        } else if (ch == 2) {
            sched.printStatus();
        } else if (ch == 3) {
            g_running.store(false);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);

    int machines = args.machines;
    if (!args.headless) {
        std::cout << "Enter number of washing machines: ";
        if (!(std::cin >> machines)) return 0;
    }
    if (machines < 1) machines = 1;

    Scheduler sched(machines);
    Utils::Logger::instance().log("System started with " + std::to_string(machines) + " machines.");

    // Handle Ctrl+C
    std::signal(SIGINT, [](int){ g_running.store(false); });

    auto svr = std::make_shared<httplib::Server>();
    std::thread rest([&]{ restServer(sched, svr, args.port); });

    std::thread ui;
    if (!args.headless) {
        ui = std::thread([&]{ consoleLoop(sched); });
    } else {
        std::cout << "[MODE] Headless (REST only). Use /book and /status.\n";
    }

    if (ui.joinable()) ui.join(); // In headless, no UI; server keeps running

    // If UI exits by user choice, stop server. In headless, server stops via /shutdown or SIGINT.
    if (!args.headless) {
        g_running.store(false);
        if (svr) svr->stop();
    }

    if (rest.joinable()) rest.join();
    sched.shutdown();
    Utils::Logger::instance().log("System shutting down.");
    return 0;
}
