#include "crow_all.h"
#include "json.hpp"
#include <iostream>
#include <mutex>
#include <unordered_set>

using json = nlohmann::json;

int main() {
    crow::SimpleApp app;
    std::mutex mtx; // Mutex to protect our list of active WebSocket users
    std::unordered_set<crow::websocket::connection*> users;

    std::cout << "==========================================" << std::endl;
    std::cout << "[SYSTEM] Booting Sanrakshan Engine..." << std::endl;
    std::cout << "==========================================" << std::endl;

    // 1. Basic REST Route (To test in your browser)
    CROW_ROUTE(app, "/")([](){
        return "Sanrakshan C++ Engine is Online and ready for traffic!";
    });

    // 2. The WebSocket Route (Where Person 2's UI will connect)
    // 2. The WebSocket Route (Where Person 2's UI will connect)
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection& conn) {
            std::cout << "[WS] UI Dashboard Connected!" << std::endl;
            std::lock_guard<std::mutex> lock(mtx);
            users.insert(&conn);
        })
        // FIX: Added 'uint16_t code' to match the new Crow signature!
        .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            std::cout << "[WS] UI Dashboard Disconnected: " << reason << " (Code: " << code << ")" << std::endl;
            std::lock_guard<std::mutex> lock(mtx);
            users.erase(&conn);
        })
        .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
            std::cout << "[WS INCOMING] " << data << std::endl;
            try {
                auto parsed = json::parse(data);
                std::cout << "Action requested: " << parsed["action"] << std::endl;
            } catch (json::parse_error& e) {
                std::cout << "JSON parse error: " << e.what() << std::endl;
            }
        });

    // 3. Start the server on port 8080 using all available CPU threads
    app.port(8080).multithreaded().run();

    return 0;
}