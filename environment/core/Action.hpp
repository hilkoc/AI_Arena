#pragma once

// #include "chickenpoker/ChpState.hpp"
#include <string>

//#include "Agent.hpp"
class Agent;

/** The action that agents send to the environment. */
class Action {
public:
    virtual ~Action() = default;

    /** Factory method. Constructs an action from string. */
    static Action deserialize(Agent& agent, std::string const& action_msg);

    /** Returns a string representation of this instance. */
    virtual std::string serialize();

    Action(Agent& agent_in, unsigned int k) : agent(agent_in), bet(k) {};

    Agent& get_agent() const {
        return this->agent;
    };

    unsigned int get_bet() const {
        return this->bet;
    };

private:
    Agent& agent;  // The agent that is doing this action.
    unsigned int bet; // The bet that is placed.
};


