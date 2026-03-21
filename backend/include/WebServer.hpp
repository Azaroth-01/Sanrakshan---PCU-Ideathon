#pragma once

#include <string>

namespace WebServer {
    // Starts the Crow server and blocks the main thread
    void startServer(int port);

    // Called by the SimulationEngine every 100ms to push JSON to the frontend
    void broadcastState();
}