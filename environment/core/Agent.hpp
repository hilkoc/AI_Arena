#pragma once

#include <vector>

#include "Action.hpp"
#include "State.hpp"
#include "Reward.hpp"
#include "logging/log.h"

class State;
//class Action;

/** The Agent. */
class Agent {
public:
    virtual ~Agent(){};

    Agent(unsigned int const agent_id) : id(agent_id) {};

    /** React with an action to the observed state. */
    virtual Action receive_state(State* state) = 0;

    /** Reward for action made by last call to receive_state. */
    virtual void receive_reward(Reward& reward){};

    /** Compare Agents by their id. Required for use as map key. */
    bool operator<(Agent const& right) const {
        return id < right.id;
    }

    unsigned int const get_id() const {
        return id;
    };

private:
    unsigned int const id;
};
