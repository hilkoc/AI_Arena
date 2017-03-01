#pragma once

//#include "Agent.hpp"
class Agent;

/** The action that agents send to the environment. */
class Action {
public:
    virtual ~Action();

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
