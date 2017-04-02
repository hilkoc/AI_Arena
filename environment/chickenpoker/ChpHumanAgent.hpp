#pragma once

#include <iostream>
#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"
#include "ChpState.hpp"

#include "logging/log.h"


/** An Agent that plays random bets. */
class ChpHumanAgent : public Agent {
public:
    virtual ~ChpHumanAgent() = default;

    ChpHumanAgent(unsigned int const agent_id) : Agent(agent_id) {
        LOG(DEBUG) << "Creating human agent " << this->get_id();
    };

    void initialize_episode(InitialState& initial_state) {
        //player_id = initial_state.player_id;
        bets = initial_state.bets;
        allowed_bets.resize(bets + 1);
        std::fill(allowed_bets.begin(), allowed_bets.end(), true);
        allowed_bets[0] = false;
        LOG(INFO) << "   Human Agent " << this->get_id() << ". Starting round with " << bets << " bets.";
    };

    virtual Action receive_state(AgentState& agentState) {
        std::cout << "Allowed bets: ";
        for(int bet = 1; bet <= bets; ++bet) {
            if (allowed_bets[bet]) {
                std::cout << bet << " ";
            }
        }
        std::cout << std::endl;
        unsigned int next_bet;
        std::cout << "Enter bet: ";
        std::cin >> next_bet;
        allowed_bets[next_bet] = false;
        Action action(*this, next_bet);
        return action;
    };

    virtual void receive_reward(Reward& reward) {
        LOG(DEBUG) << "Received reward: " << reward;
    };


private:
    std::vector<bool> allowed_bets;

    // These are set per episode
    unsigned int bets;
};
