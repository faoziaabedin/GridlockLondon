// code/adapters/TimerService.cpp
#include "TimerService.h"

void TimerService::start(int intervalMs, std::function<void()> cb) {
    // Stub implementation for Deliverable 2
    // Just call the callback once immediately
    // In full implementation, this would call cb repeatedly every intervalMs
    (void)intervalMs;  // Suppress unused parameter warning
    
    if (cb) {
        cb();
    }
}

void TimerService::stop() {
    // Stub implementation - do nothing
    running_ = false;
}

bool TimerService::isRunning() const {
    // Stub implementation - always return false
    return false;
}
