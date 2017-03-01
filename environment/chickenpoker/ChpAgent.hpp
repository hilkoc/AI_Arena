#pragma once

#include <vector>

#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"
#include "ChpState.hpp"

#include "logging/log.h"

class State;
//class Action;

/** The Agent. */
class ChpAgent : public Agent {
public:
    virtual ~ChpAgent(){};

    ChpAgent(unsigned int const agent_id) : Agent(agent_id) {
        next_bet = 1;
        LOG(DEBUG) << "My id is" << this->get_id();
    };

    virtual Action receive_state(State* state) {
        LOG(DEBUG) << " delegating <receive_state>";
        return this->do_receive_state(state);
    };

    virtual Action do_receive_state(ChpState* state) {
        LOG(DEBUG) << "<ChpState*> receive_state. Agent " << this->get_id() << "received ";
        state->log_summary();
        Action action(*this, this->next_bet);
        next_bet += 1;
        return action;
    };

    virtual void receive_reward(Reward& reward) {
        LOG(DEBUG) << "Received reward: " << reward;
    };


private:

    unsigned int next_bet;
};
