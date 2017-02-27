#pragma once

#include <vector>
#include <map>
#include "Agent.hpp"
#include "Action.hpp"
#include "Reward.hpp"

class Agent;
//class Action;

/** The interface for State. */
class State {
public:
    virtual ~State(){};

    virtual std::map<Agent, Reward> update(std::vector<Action>& actions) = 0;

    virtual bool termination_condition() = 0;
};
