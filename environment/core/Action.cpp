#include "Action.hpp"

#include <sstream>

/** Returns a string representation of this instance. */
std::string Action::serialize() {
    std::ostringstream oss;
    oss << this->bet;
    return oss.str();
};

/** Static Factory method. Constructs an action from string. */
Action Action::deserialize(Agent& agent, std::string const& action_msg) {
    Action action(agent, 1);
    return action;
};
