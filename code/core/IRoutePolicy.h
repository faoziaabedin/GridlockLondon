#pragma once
class City;
class Agent;
using EdgeId = int;

struct IRoutePolicy {
    virtual ~IRoutePolicy() = default;
    virtual double edgeCost(const City& city, EdgeId e) const = 0;
    virtual bool shouldRerouteOnNode(const Agent& a) const = 0;
};
