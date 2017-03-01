#pragma once

#include <vector>
#include <map>
#include "core/State.hpp"
#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"

class Action;

/** The state. */
class ChpState : public State {
public:
    virtual ~ChpState(){};

    ChpState(unsigned int bets_in) : bets(bets_in), rounds(bets_in) {};

    virtual std::map<Agent*, Reward> update(std::vector<Action>& actions);

    virtual bool termination_condition();

    /** Some states need to know about the Agents that are interacting with it. */
    virtual bool add_agents(std::vector<Agent*>& agents);

    /** Agents may leave (unexpectedly) at any time. */
    virtual bool remove_agent(Agent& agent) {
        player_bets.erase(&agent);
    };


    virtual void reset(){
        for (const auto &pair : player_bets) {
            std::vector<bool> bets = pair.second;
            std::fill(bets.begin(), bets.end(), true);
        }
    };

    void log_summary() {
        LOG(INFO) << "Nr agents " << player_bets.size() << " rounds: " << rounds;
    };

    virtual ChpState* subtype() {
        return this;    
    };

private:
    /** The total number of rounds in the game, also the maximum bet. */
    unsigned int const bets;

    /** The number of rounds left in the game*/
    unsigned int rounds;

    /** player_bets[player][k] is true indicates that player is allowed to play action k. */
    std::map<Agent*, std::vector<bool> > player_bets;
};
