#pragma once

#include <vector>
#include <map>
#include "core/State.hpp"
//#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"

class Agent;
class Action;

/** The state. */
class ChpState : public State {
public:
    virtual ~ChpState(){};

    ChpState(unsigned int bets_in) : bets(bets_in), rounds(bets_in) {};

    virtual std::map<Agent*, Reward> update(std::vector<Action>& actions) {
        this->rounds -= 1;
        std::map<Agent*, Reward> rewards;
        unsigned int max_bet(0);
        Agent* round_winner = nullptr;
        unsigned int wins(0);
        for (Action action : actions) {
            Agent& agent = action.get_agent();
            unsigned int bet = action.get_bet();
            wins += bet;
            player_bets[&agent][bet] = false;
            if (bet > max_bet) {
                max_bet = bet;
                round_winner = &agent;            
            }
            rewards[&agent] = Reward(0);  // Default to 0.
        }
        
        rewards[round_winner] = Reward(wins);  // Winner gets it all.
        return rewards;
    };

    virtual bool termination_condition();

    /** Some states need to know about the Agents that are interacting with it. */
    virtual bool add_agents(std::vector<Agent*>& agents){
        for (Agent* agent : agents) {
            // For convience makethe vector size one bigger than needed. Ignore 0th index.
            player_bets[agent] = std::vector<bool>(this->bets);
        }
    };

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
private:
    /** The total number of rounds in the game, also the maximum bet. */
    unsigned int const bets;

    /** The number of rounds left in the game*/
    unsigned int rounds;

    /** player_bets[player][k] is true indicates that player is allowed to play action k. */
    std::map<Agent*, std::vector<bool> > player_bets;
};
