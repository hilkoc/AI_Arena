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

    ChpState(unsigned int cards_in) : cards(cards_in), turns(cards_in) {};

    virtual std::map<Agent*, Reward> update(std::vector<Action>& actions) {
        this->turns -= 1;
        
        std::map<Agent*, Reward> rewards;
        return rewards;
    };

    virtual bool termination_condition();

    /** Some states need to know about the Agents that are interacting with it. */
    virtual bool add_agents(std::vector<Agent*>& agents){
        for (Agent* agent : agents) {
            player_cards[agent] = std::vector<bool>(false, this->cards);
        }
    };

    /** Agents may leave (unexpectedly) at any time. */
    virtual bool remove_agent(Agent& agent) {
        player_cards.erase(&agent);
    };

private:
    unsigned int const cards;
    unsigned int turns;

    // player_cards[player][k] is true indicates that player is allowed to play action k.
    std::map<Agent*, std::vector<bool> > player_cards;
};
