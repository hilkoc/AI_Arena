#include "ChpState.hpp"

bool ChpState::termination_condition() {
    // Ends when there are no more rounds to be played or no more players to play against.
    LOG(DEBUG) << "Rounds: " << this->rounds << ", players left: " << this->player_bets.size();
    return this->rounds == 0 || this->player_bets.size() < 2;  
};

std::map<Agent*, Reward> ChpState::update(std::vector<Action>& actions) {
    this->rounds -= 1;
    std::map<Agent*, Reward> rewards;
    unsigned int max_bet(0);
    Agent* round_winner = nullptr;
    unsigned int wins(0);
    for (Action action : actions) {
        Agent& agent = action.get_agent();
        unsigned int bet = action.get_bet();
        LOG(INFO) << "    Agent " << agent.get_id() << ": " << bet;
        wins += bet;
        player_bets[&agent][bet] = false;
        if (bet > max_bet) {
            max_bet = bet;
            round_winner = &agent;            
        }
        rewards[&agent] = Reward(0);  // Default to 0.
    }
    
    LOG(INFO) << "Round " << bets - rounds << ". Player " << round_winner->get_id() << " wins " << wins;
    rewards[round_winner] = Reward(wins);  // Winner gets it all.
    return rewards;
};


/** Some states need to know about the Agents that are interacting with it. */
bool ChpState::add_agents(std::vector<Agent*>& agents){
    for (Agent* agent : agents) {
        // For convience makethe vector size one bigger than needed. Ignore 0th index.
        player_bets[agent] = std::vector<bool>(this->bets);
    }
        LOG(DEBUG) << "Total agents " << player_bets.size();
};
