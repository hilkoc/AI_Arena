#pragma once

#include <vector>
#include <map>
#include "core/State.hpp"
#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"

class Agent;
//class Action;

/** The state. */
class ChpState : public State {
public:
    virtual ~ChpState(){};
    
    // default constructor  
    ChpState() = default;

    ChpState(unsigned int cards_in) : cards(cards_in) {};

    virtual std::map<Agent, Reward> update(std::vector<Action>& actions) {
        this->cards -= 1;
        std::map<Agent, Reward> rewards;
        return rewards;
    };

    virtual bool termination_condition() {
      return this->cards == 0;  
    };

private:
    unsigned int cards;
};
