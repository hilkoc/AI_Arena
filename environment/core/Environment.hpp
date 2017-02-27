#pragma once

#include <iostream>
#include "Agent.hpp"
#include "Factory.hpp"
#include "State.hpp"
#include "Action.hpp"
#include "Reward.hpp"

class Agent;
class State;


/** The Environment connects and interacts with Agents. In each iteration It sends the State to the agents.
  It receives an Action from the agents and updates the State dependent on the given Actions.
  Finally it sends a Reward to the Agents.

  In an episode the Environment iterates until the termination condition is reached. */
class Environment {
public:
    virtual ~Environment();
 
    Environment(Factory& factoryImpl): factory(factoryImpl){};

    /** Disconnects all players. */
    virtual void connect_agents(std::vector<Agent*> agents);

    /** Disconnects all players. Happens automatically when environment is destroyed. */
    virtual void disconnect_agents();


    /** Assumes all Agents are connected.
    Initializes a new episode. Runs iterations with connected players and evolves the game state until the termination condition is met.*/
    virtual void run_episodes(unsigned int nr =1) {
        for (int episode_nr = 1; episode_nr <= nr; ++episode_nr) {
            std::cout << "Starting episode " << episode_nr << std::endl;

            this->initialize_episode();
            while (!state->termination_condition()) {
                this->step();
            }
            this->finalize_episode();
        }
    }

    /** Removes agent with given id from the environment. For example after a timeout or an invalid action.*/
    virtual void drop_agent(Agent& agent);


protected:

    /** Prepares an episode. Creates the initial State and signals Agents to initialize. */
    virtual void initialize_episode();

    /** Runs one iteration for the current episode */
    virtual void step() {

        std::cout << "Sending states, receiving actions" << std::endl;
        std::vector<Action> actions;
        for (Agent* agent : agents) {
            Action action = agent->receive_state(*state);
            actions.push_back(action);
        }
        std::map<Agent*, Reward> rewards = state->update(actions);
        std::cout << "Sending rewards" << std::endl;
        for (Agent* agent : agents) {
            Reward reward = rewards[agent];
            agent->receive_reward(reward);
        }
    }

    /** Signals all Agents that the episode is finished. After this function returns a new episode may be started. */
    virtual void finalize_episode();

    Factory& factory;
    State* state;
    std::vector<Agent*> agents;
};

