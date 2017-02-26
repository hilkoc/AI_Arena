#pragma once

#include <vector>
#include <map>
#include "Agent.hpp"
#include "Action.hpp"
#include "Reward.hpp"

class Agent;
//class Action;

/** The state. */
class State {
public:
    virtual ~State();
    
    // default constructor  
    State() = default;

    virtual std::map<Agent, Reward> update(std::vector<Action>& actions) {
        std::map<Agent, Reward> rewards;        
        return rewards;
    };

    virtual bool termination_condition() {
      return true;  
    };
};
