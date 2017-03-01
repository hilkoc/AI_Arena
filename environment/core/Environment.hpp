#pragma once

#include "Agent.hpp"
#include "Factory.hpp"
#include "State.hpp"
#include "Action.hpp"
#include "Reward.hpp"
#include "logging/log.h"

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
    virtual void connect_agents(std::vector<Agent*>& agents);

    /** Disconnects all players. Happens automatically when environment is destroyed. */
    virtual void disconnect_agents();


    /** Assumes all Agents are connected.
    Initializes a new episode. Runs iterations with connected players and evolves the game state until the termination condition is met.*/
    virtual void run_episodes(unsigned int nr =1) {
        for (int episode_nr = 1; episode_nr <= nr; ++episode_nr) {
            LOG(DEBUG) << "Starting episode " << episode_nr;

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
        std::vector<Action> actions;
        LOG(DEBUG) << "Sending states, receiving actions ";
        for (Agent* agent : this->agents) {
            LOG(DEBUG) << "  Sending state to agent " << agent->get_id();
            Action action = agent->receive_state(state->subtype());
            LOG(DEBUG) << "  actions.push_back(action);";
            actions.push_back(action);
        }
        std::map<Agent*, Reward> rewards = state->update(actions);
        LOG(DEBUG) << "Sending rewards";
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

