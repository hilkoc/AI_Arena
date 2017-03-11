#include "ChpState.hpp"
#include <algorithm>

bool ChpState::termination_condition() {
    // Ends when there are no more rounds to be played or no more players to play against.
    LOG(DEBUG) << "Rounds: " << this->rounds << ", players left: " << this->player_bets.size();
    return this->rounds == 0 || this->player_bets.size() < 2;  
};

std::map<Agent*, Reward> ChpState::update(std::vector<Action>& actions) {
    this->rounds -= 1;
    std::map<Agent*, Reward> rewards;
    unsigned int max_bet(0);
    std::vector<Agent*> winners;
    unsigned int wins(0);
    for (Action action : actions) {
        Agent& agent = action.get_agent();
        unsigned int bet = action.get_bet();
        LOG(INFO) << "    Agent " << agent.get_id() << ": " << bet;
        LOG(DEBUG) <<  "valid move? " << (player_bets[&agent][bet]? " true " : "false") ;
        if (player_bets[&agent][bet]) {
            player_bets[&agent][bet] = false;
            wins += bet;
            if (bet == max_bet) {
                LOG(DEBUG) << " A Tie needs to be broken.";
                winners.push_back(&agent);
            } else { 
                if (bet > max_bet) {
                    max_bet = bet;
                    winners.clear();
                    winners.push_back(&agent);
                }
            }
        } else {
            LOG(WARN) << "    INVALID ACTION Agent " << agent.get_id() << ": " << bet
                      << ". Dropping Agent.";
            this->remove_agent(agent);
        }
        rewards[&agent] = Reward(0);  // Default to 0.
    }
    Agent* round_winner = nullptr;
    std::sort(winners.begin(), winners.end(), [] (Agent* x, Agent* y) {
         return x->get_id() < y->get_id();} ); // Sort the agents by id.
//    for (Agent* a : winners) {LOG(INFO) << "  sorted a_id " << a->get_id();}
    unsigned int round_winner_idx = (max_bet + this->episode_count) % winners.size();
    round_winner = winners[round_winner_idx];
    LOG(INFO) << "Round " << bets - rounds << ". Player " << round_winner->get_id() << " wins " << wins;
    rewards[round_winner] = Reward(wins);  // Winner gets it all.
    return rewards;
};


/** Some states need to know about the Agents that are interacting with it. */
bool ChpState::add_agents(std::vector<Agent*>& agents){
    for (Agent* agent : agents) {
        // For convience make the vector size one bigger than needed. Ignore 0th index.
        player_bets[agent] = std::vector<bool>(1 + this->bets);
    }
    LOG(DEBUG) << "Total agents " << player_bets.size();
};

