#pragma once


#include <vector>
#include <map>
//#include "Agent.hpp"
#include "Action.hpp"
#include "Reward.hpp"
#include "logging/log.h"

class Agent;
//class Action;

/** The interface for State. */
class State {
public:
    virtual ~State(){};

    virtual std::map<Agent*, Reward> update(std::vector<Action>& actions) = 0;

    virtual bool termination_condition() = 0;

    /** Some states need to know about the Agents that are interacting with it. */
    virtual bool add_agents(std::vector<Agent*>& agents) = 0;

    /** Agents may leave (unexpectedly) at any time. */
    virtual bool remove_agent(Agent& agent) = 0;

    /** Assumes all agents are already added. Prepares internal state to start a new episode. */
    virtual void reset() = 0;

    /** Returns this */
    virtual Action send_to(Agent& agent) = 0;

    /** Returns a string representation of this instance. */
    //virtual std::string serialize() = 0;
};
