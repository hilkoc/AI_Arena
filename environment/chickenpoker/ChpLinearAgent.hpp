#pragma once


#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"
#include "ChpState.hpp"

#include "logging/log.h"


/** A linear Agent for which the distance between any two consecutive bets is a constant.
    Note: if the step size and number of rounds is not coprime, this agent will make invalid moves. */
class ChpLinearAgent : public Agent {
public:
    virtual ~ChpLinearAgent() = default;

    ChpLinearAgent(unsigned int const agent_id,
        int const step_in,
        unsigned int const start_in)
        : Agent(agent_id), step(step_in), start(start_in)  {
        LOG(DEBUG) << "Linear agent id:" << this->get_id() << " start: " << this->start << " step: " << this->step;
    };

    void initialize_episode(InitialState& initial_state) {
        //InitialState& initial_state = static_cast<InitialState&>(initial_state);
        player_id = initial_state.player_id;
        bets = initial_state.bets;
        next_bet = (start -1) % bets + 1;
        LOG(DEBUG) << "   linear agent " << this->get_id() << " episode initialized";
    };

    virtual Action receive_state(State& state) {
        // Have to cast state down to its known subtype, otherwise all possible subtypes of State
        // will have to appear in the top level inerface of Agent.
        ChpState& chpState = static_cast<ChpState&>(state);
        return this->do_receive_state(chpState);
    };

    virtual Action do_receive_state(ChpState& chpState) {
        Action action(*this, this->next_bet);
        next_bet = (next_bet - 1 + step + bets) % bets + 1;
        return action;
    };

    virtual void receive_reward(Reward& reward) {
        LOG(DEBUG) << "Received reward: " << reward;
    };


private:
    unsigned int const start;
    int const step;
    unsigned int next_bet;

    // These are set per episode
    unsigned int player_id;
    unsigned int bets;
};
