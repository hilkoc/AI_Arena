#include "ChpState.hpp"

bool ChpState::termination_condition() {
    // Ends when no more turns and cannot play alone.
    return this->turns == 0 || this->player_cards.size() < 2;  
};
