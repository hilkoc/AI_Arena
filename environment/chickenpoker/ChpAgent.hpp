#pragma once

#include <vector>

#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"
#include "ChpState.hpp"



class State;
//class Action;

/** The Agent. */
class ChpAgent : public Agent {
public:
    virtual ~ChpAgent(){};

    ChpAgent(unsigned int const agent_id) : Agent(agent_id) {
        next_bet = 1;
    };

    virtual Action receive_state(ChpState& state) {
        Action action(*this, this->next_bet);
        next_bet += 1;
        return action;
    };

    virtual void receive_reward(Reward& reward) {};


private:

    unsigned int next_bet;
};
