#pragma once

//#include "Agent.hpp"
class Agent;

/** The action that agents send to the environment. */
class Action {
public:
    virtual ~Action();

    Action(Agent& agent_in, unsigned int k_in) : agent(agent_in), k(k_in) {};

private:
    Agent& agent;  // The agent that is doing this action.
    unsigned int k; // The card that is played.
};
