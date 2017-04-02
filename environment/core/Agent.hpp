#pragma once

#include <vector>

#include "Action.hpp"
#include "State.hpp"
#include "Reward.hpp"
#include "logging/log.h"

class InitialState;
class AgentState;

/** The Agent. */
class Agent {
public:
    virtual ~Agent() = default;

    Agent(unsigned int const agent_id) : id(agent_id) {};

    /** Receive the initial state for this episode. */
    virtual void initialize_episode(InitialState& initial_state) = 0;

    /** React with an action to the observed state. */
    virtual Action receive_state(AgentState& state) = 0;

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
