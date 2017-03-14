#include "NetworkAgent.hpp"
#include "chickenpoker/ChpState.hpp"
#include "logging/log.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>

// TODO check which includes are necessary
#ifdef _WIN32
    #include <windows.h>
    #include <tchar.h>
    #include <stdio.h>
    #include <strsafe.h>
#else
    #include <signal.h>
    #include <time.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/select.h>
#ifdef __linux__
    #include <sys/prctl.h>
#endif
    #include <unistd.h>
#endif

std::mutex cout_mutex;

// constructor
NetworkAgent::NetworkAgent(unsigned int const agent_id, std::string const& cmd)
    : Agent(agent_id) {
    LOG(INFO) << "Creating NetworkAgent " << cmd;
};


std::string to_string(int n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
};

/** Message the subprocess to reset for a new episode and send the initial state. */
void NetworkAgent::initialize_episode(InitialState& initial_state) {
        std::string player_id, bets;
        player_id = to_string(this->get_id());
        bets = to_string(initial_state.bets);
        // TODO send the above to the subprocess
        sendString(player_id);
        sendString(bets);
        LOG(INFO) << "Network Agent eps init sending id " << player_id << " and bets " << bets << ".";
    };


Action do_receive_state(NetworkAgent* this_agent, ChpState& chpState);

/** React with an action to the observed state. */
Action NetworkAgent::receive_state(State& state) {
    LOG(DEBUG) << "network agent receive_state";
    // Have to cast state down to its known subtype, otherwise all possible subtypes of State
    // will have to appear in the top level inerface of Agent.
    ChpState& chpState = static_cast<ChpState&>(state);
    return do_receive_state(this, chpState);
};


//void NetworkAgent::sendString(std::string& msg); //forward declare

// TODO make this a virtual member function
/** Send the state to the subprocess, wait until the timeout for a response. */
Action do_receive_state(NetworkAgent* this_agent, ChpState& chpState) {
        unsigned int const MAX_TIME_MILLIS(3000); // 3 sec.
        // Convert the state to a string
        std::string state_msg = "state"; // TODO  chpState.serialize();

        LOG(DEBUG) << "network agent sendString: " << state_msg;
        // Send the state_string
        this_agent->sendString(state_msg);
        // Receive the action as string
        std::string action_msg = this_agent->getString(MAX_TIME_MILLIS);
        // Deserialize and return the action
        Action action(*this_agent, 2); // = Action::deserialize(*this_agent, action_msg);
        LOG(DEBUG) << "network agent return action;";
        return action;
};


void NetworkAgent::sendString(std::string& msg) {
    //End message with newline character
    msg += '\n';

#ifdef _WIN32
//    WinConnection connection = this->connection;

    DWORD charsWritten;
    bool success;
    success = WriteFile(connection.write, msg.c_str(), msg.length(), &charsWritten, NULL);
    if(!success || charsWritten == 0) {
        LOG(ERROR) << "Problem writing to pipe\n";
        throw 1;
    }

#else
    //UniConnection connection = this->connection;
    LOG(DEBUG) << "network agent 1 ssize_t charsWritten = write(connection.write,... ";
    ssize_t charsWritten = write(connection.write, msg.c_str(), msg.length());
    LOG(DEBUG) << "network agent 2 ssize_t charsWritten = write(connection.write,... ";
    if(charsWritten < msg.length()) {
        LOG(ERROR) << "Problem writing to pipe\n";
        throw 1;
    }
#endif
};



std::string NetworkAgent::getString(unsigned int const timeoutMillis) {
    bool quiet_output = false;

    std::string newString;
    int timeoutMillisRemaining = timeoutMillis;
    std::chrono::high_resolution_clock::time_point tp = std::chrono::high_resolution_clock::now();
    
#ifdef _WIN32
    // WinConnection connection = connections[playerTag - 1];

    DWORD charsRead;
    bool success;
    char buffer;

    //Keep reading char by char until a newline
    while(true) {
        timeoutMillisRemaining = timeoutMillis - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tp).count();
        if(timeoutMillisRemaining < 0) throw newString;
        //Check to see that there are bytes in the pipe before reading
        //Throw error if no bytes in alloted time
        //Check for bytes before sampling clock, because reduces latency (vast majority the pipe is alread full)
        DWORD bytesAvailable = 0;
        PeekNamedPipe(connection.read, NULL, 0, NULL, &bytesAvailable, NULL);
        if(bytesAvailable < 1) {
            std::chrono::high_resolution_clock::time_point initialTime = std::chrono::high_resolution_clock::now();
            while (bytesAvailable < 1) {
                if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initialTime).count() > timeoutMillisRemaining) {
                    throw newString;
                }
                PeekNamedPipe(connection.read, NULL, 0, NULL, &bytesAvailable, NULL);
            }
        }

        success = ReadFile(connection.read, &buffer, 1, &charsRead, NULL);
        if(!success || charsRead < 1) {
            if(!quiet_output) {
                std::string errorMessage = "Bot #" + std::to_string(playerTag) + " timed out or errored (Windows)\n";
                std::lock_guard<std::mutex> guard(cout_mutex);
                std::cout << errorMessage;
            }
            throw newString;
        }
        if(buffer == '\n') {
            break;
        }
        else {
            newString += buffer;
        }
    }
#else
    // UniConnection connection = connections[playerTag - 1];

    fd_set set;
    FD_ZERO(&set); /* clear the set */
    FD_SET(connection.read, &set); /* add our file descriptor to the set */
    char buffer;

    //Keep reading char by char until a newline
    while(true) {

        //Check if there are bytes in the pipe
        timeoutMillisRemaining = timeoutMillis - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tp).count();
        if(timeoutMillisRemaining < 0) throw newString;
        struct timeval timeout;
        timeout.tv_sec = timeoutMillisRemaining / 1000.0;
        timeout.tv_usec = (timeoutMillisRemaining % 1000)*1000;
        int selectionResult = select(connection.read+1, &set, NULL, NULL, &timeout);

        if(selectionResult > 0) {
            read(connection.read, &buffer, 1);

            if(buffer == '\n') break;
            else newString += buffer;
        } else {
            if(!quiet_output) {
                std::string errorMessage = "Bot #" + std::to_string(this->get_id()) + " timeout or error (Unix) " + std::to_string(selectionResult) + '\n';
                std::lock_guard<std::mutex> guard(cout_mutex);
                std::cout << errorMessage;
            }
            throw newString;
        }
    }
#endif
    //Python turns \n into \r\n
    if(newString.back() == '\r') {
        newString.pop_back();
    }

    return newString;

};
