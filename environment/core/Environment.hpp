#pragma once

#include "Agent.hpp"
#include "Factory.hpp"
#include "State.hpp"
#include "Action.hpp"
#include "Reward.hpp"
#include "Stats.hpp"
#include "logging/log.h"


/** The Environment connects and interacts with Agents. In each iteration It sends the State to the agents.
  It receives an Action from the agents and updates the State dependent on the given Actions.
  Finally it sends a Reward to the Agents.

  In an episode the Environment iterates until the termination condition is reached. */
class Environment {
public:
    virtual ~Environment();
 
    Environment(Factory& factoryImpl): factory(factoryImpl) {};

    /** Initializes a session for multiple episodes. Connects all agents. */
    virtual void connect_agents(std::vector<Agent*>& agents);

    /** Disconnects all agents. Happens automatically when environment is destroyed. */
    virtual void disconnect_agents();


    /** Assumes all Agents are connected. Runs one session of given nr of episodes.
    For each episode run iterations with connected agents and evolves the game state until the termination condition is met. */
    virtual void run_episodes(unsigned int nr =1) {
        Stats session_stats;
        for (int episode_nr = 1; episode_nr <= nr; ++episode_nr) {
            LOG(INFO) << "Starting episode " << episode_nr;
            Stats episode_stats;
            
            this->initialize_episode();
            while (!state->termination_condition()) {
                this->step(episode_stats);
            }
            this->finalize_episode(episode_stats, session_stats);
        }
        LOG(QUIET) << "Session summary";
        session_stats.log_summary();
    }

    /** Removes agent with given id from the environment. For example after a timeout or an invalid action.*/
    virtual void drop_agent(Agent& agent);


protected:

    /** Prepares an episode. Creates the initial State and signals Agents to initialize. */
    virtual void initialize_episode();

    /** Runs one iteration for the current episode */
    virtual void step(Stats& episode_stats) {
        std::vector<Action> actions;
        LOG(DEBUG) << "Sending states, receiving actions ";
        for (Agent* agent : this->agents) {
            LOG(DEBUG) << "  Sending state to agent " << agent->get_id();
            Action action = state->send_to(*agent);
            actions.push_back(action);
        }
        LOG(DEBUG) << "state-update,  calculating rewards";
        std::map<Agent*, Reward> rewards = state->update(actions);
        episode_stats.add_rewards(rewards);
        for (Agent* agent : agents) {
            Reward reward = rewards[agent];
            agent->receive_reward(reward);
        }
    }

    /** Signals all Agents that the episode is finished. After this function returns a new episode may be started. */
    virtual void finalize_episode(Stats& episode_stats, Stats& session_stats);

    Factory& factory;
    State* state;
    std::vector<Agent*> agents;
};

