#pragma once
#include <deque>
#include <memory>
class City; class Agent; struct IRoutePolicy; using EdgeId=int;

class RoutePlanner {
public:
    explicit RoutePlanner(std::shared_ptr<IRoutePolicy> p);
    void setPolicy(std::shared_ptr<IRoutePolicy> p);
    std::deque<EdgeId> computePath(const City& city, const Agent& a) const;
private:
    std::deque<EdgeId> dijkstra(const City& city, int start, int end) const;
    std::shared_ptr<IRoutePolicy> policy_;
};
