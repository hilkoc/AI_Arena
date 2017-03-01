#include "ChpState.hpp"

bool ChpState::termination_condition() {
    // Ends when there are no more rounds to be played or no more players to play against.
    return this->rounds == 0 || this->player_bets.size() < 2;  
};
