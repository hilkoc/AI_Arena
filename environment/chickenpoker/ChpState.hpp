#pragma once

#include <vector>
#include <map>
#include "core/State.hpp"
#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Reward.hpp"


/** The information that an agent receives at the start of an episode. */
class InitialState {
public:
    InitialState(unsigned int const player_id_in, unsigned int const bets_in)
    : player_id(player_id_in), bets(bets_in) {};
    // The player id for breaking ties.
    unsigned int const player_id;
    unsigned int const bets;
};

//class Action; //

/** The state. */
class ChpState : public State {
public:
    virtual ~ChpState() = default;

    ChpState(unsigned int bets_in) : bets(bets_in), rounds(bets_in), episode_count(0) {};

    virtual std::map<Agent*, Reward> update(std::vector<Action>& actions);

    virtual bool termination_condition();

    /** Some states need to know about the Agents that are interacting with it. */
    virtual bool add_agents(std::vector<Agent*>& agents);

    /** Agents may leave (unexpectedly) at any time. */
    virtual bool remove_agent(Agent& agent) {
        player_bets.erase(&agent);
    };


    virtual void reset(){
        rounds = bets;
        episode_count += 1;
        for (auto& pair : player_bets) {
            std::vector<bool>& bets = pair.second;
            std::fill(bets.begin(), bets.end(), true);
            Agent* agent = pair.first;
            InitialState initialState(agent->get_id(), this->bets);
            agent->initialize_episode(initialState);
        }
        // TODO reconnect dropped agents.
    };

    virtual Action send_to(Agent& agent) {
        return agent.receive_state(*this);;    
    };

private:
    /** The total number of rounds in the game, also the maximum bet. */
    unsigned int const bets;

    /** The number of rounds left in the game*/
    unsigned int rounds;

    /** player_bets[player][k] is true indicates that player is allowed to play action k. */
    std::map<Agent*, std::vector<bool> > player_bets;

    /** Used to break ties. This counter incremented at the start of every episode. This way there is not a single agent that has an advantage every episode. */
    unsigned int episode_count;
};

