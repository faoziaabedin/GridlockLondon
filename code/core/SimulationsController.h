#pragma once

#include <memory>
#include <vector>

struct Preset;
class City;
class RoutePlanner;
class Metrics;
class Agent;

class SimulationController {
public:
    static SimulationController& getInstance();

    void loadPreset(const Preset& cfg);
    void start();
    void pause();
    void reset();
    void tick();

    City* getCity() const;
    std::vector<Agent*>& getAgents();
    Metrics* getMetrics() const;

private:
    SimulationController() = default;
    ~SimulationController();   // declare out-of-line
    bool running_ = false;
    std::unique_ptr<City> city_;
    std::unique_ptr<RoutePlanner> planner_;
    std::unique_ptr<Metrics> metrics_;
    std::vector<Agent*> agents_;
};
