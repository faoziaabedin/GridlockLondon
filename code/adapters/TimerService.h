// code/adapters/TimerService.h
#pragma once

#include <functional>

/**
 * TimerService - Calls a callback every intervalMs.
 * Stub implementation for Deliverable 2.
 * Full implementation will use std::this_thread::sleep_for or similar in Deliverable 3.
 */
class TimerService {
public:
    /**
     * Start the timer with given interval.
     * @param intervalMs Interval in milliseconds
     * @param cb Callback function to call
     */
    void start(int intervalMs, std::function<void()> cb);
    
    /**
     * Stop the timer.
     */
    void stop();
    
    /**
     * Check if timer is running.
     * @return true if running, false otherwise
     */
    bool isRunning() const;

private:
    bool running_ = false;
};

