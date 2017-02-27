#pragma once

#include <vector>

#include "Action.hpp"
#include "State.hpp"
#include "Reward.hpp"


class State;
class Action;

/** The Agent. */
class Agent {
public:
    virtual ~Agent();

    virtual Action receive_state(State& state) {
        Action action(*this, 1);
        return action;
    }

    virtual void receive_reward(Reward& reward);

    bool operator<(Agent const& right) const {
        return agent_id < right.agent_id; // (strcmp(m_or, right.m_or) < 0);
    }

private:
    unsigned int agent_id;
};
