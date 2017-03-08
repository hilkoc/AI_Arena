#pragma once


#include <vector>
#include <random>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"
#include "ChpState.hpp"

#include "logging/log.h"


/** An Agent that plays random bets. */
class ChpUniformAgent : public Agent {
public:
    virtual ~ChpUniformAgent() = default;

    ChpUniformAgent(unsigned int const agent_id) : Agent(agent_id) {
        std::srand(unsigned (std::time(0)) ); // Initialize random nr generator.
        LOG(DEBUG) << "Creating uniform agent " << this->get_id();
    };

    void initialize_episode(InitialState& initial_state) {
        //InitialState& initial_state = static_cast<InitialState&>(initial_state);
        player_id = initial_state.player_id;
        bets = initial_state.bets;
        next_bets.clear();
        for(unsigned int i =1; i <= bets; ++i) {
            next_bets.push_back(i);
        }
        std::random_shuffle(next_bets.begin(), next_bets.end());
        LOG(DEBUG) << "   Agent " << this->get_id() << " episode initialized";
    };

    virtual Action receive_state(State& state) {
        // Have to cast state down to its know subtype, otherwise the subtypes of State
        // will have to appear in the top level inerface of Agent.
        ChpState& chpState = static_cast<ChpState&>(state);
        return this->do_receive_state(chpState);
    };

    virtual Action do_receive_state(ChpState& chpState) {
        //state->log_summary();
        Action action(*this, this->next_bets.back());
        next_bets.pop_back();
        return action;
    };

    virtual void receive_reward(Reward& reward) {
        LOG(DEBUG) << "Received reward: " << reward;
    };


private:
    // Get the next move from here
    std::vector<unsigned int> next_bets;

    // These are set per episode
    unsigned int player_id;
    unsigned int bets;
};
