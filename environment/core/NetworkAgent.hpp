#pragma once

#include <vector>

#include "Agent.hpp"
#include "Action.hpp"
#include "State.hpp"
#include "Reward.hpp"
#include "logging/log.h"

class InitialState;


/** The NetworkAgent manages a sub process. */
class NetworkAgent : public virtual Agent {
public:
    virtual ~NetworkAgent() = default;

    NetworkAgent(unsigned int const agent_id, std::string const& cmd);

    /** Receive the initial state for this episode. */
    virtual void initialize_episode(InitialState& initial_state);

    /** React with an action to the observed state. */
    virtual Action receive_state(State& state);

    /** Reward for action made by last call to receive_state. */
    virtual void receive_reward(Reward& reward){};

    /** Send a message to the subprocess */
    virtual void sendString(std::string& msg);

    /** Receive a message from the subprocess */
    virtual std::string getString(const unsigned int timeoutMillis);

    // virtual Action do_receive_state(ChpState& chpState);

private:

#ifdef _WIN32
    struct WinConnection {
        HANDLE write, read;
    };
    
    WinConnection connection;
    HANDLE process;
#else
    struct UniConnection {
        int read, write;
    };

    UniConnection connection;
    int process;
#endif
};

