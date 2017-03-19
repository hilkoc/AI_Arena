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
    LOG(INFO) << "Creating NetworkAgent, running: " << cmd;
    this->start_subprocess(cmd);
};


std::string to_string(int n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
};

/** Message the subprocess to reset for a new episode and send the initial state. */
void NetworkAgent::initialize_episode(InitialState& initial_state) {
        unsigned int const MAX_INIT_TIME_MILLIS(10000); // 10 sec
        std::string reset, player_id, bets;
        reset = "RESET";
        sendString(reset);
        player_id = to_string(this->get_id());
        bets = to_string(initial_state.bets);
        sendString(player_id);
        sendString(bets);
        LOG(INFO) << "Network Agent eps init sending id " << player_id << " and bets " << bets << ".";
        std::string const botname = this->getString(MAX_INIT_TIME_MILLIS);
        LOG(INFO) << "Network Agent bot name: " << botname;
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
        LOG(INFO) << "network agent deserialize action; " << action_msg;
        Action action = Action::deserialize(*this_agent, action_msg);

        return action;
};

// Start the subprocess
void NetworkAgent::start_subprocess(std::string command) {
#ifdef _WIN32

    command = "/C " + command;
    WinConnection parentConnection, childConnection;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    //Child stdout pipe
    if(!CreatePipe(&parentConnection.read, &childConnection.write, &saAttr, 0)) {
        if(!quiet_output) std::cout << "Could not create pipe\n";
        throw 1;
    }
    if(!SetHandleInformation(parentConnection.read, HANDLE_FLAG_INHERIT, 0)) throw 1;

    //Child stdin pipe
    if(!CreatePipe(&childConnection.read, &parentConnection.write, &saAttr, 0)) {
        if(!quiet_output) std::cout << "Could not create pipe\n";
        throw 1;
    }
    if(!SetHandleInformation(parentConnection.write, HANDLE_FLAG_INHERIT, 0)) throw 1;

    //MAKE SURE THIS MEMORY IS ERASED
    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = childConnection.write;
    siStartInfo.hStdOutput = childConnection.write;
    siStartInfo.hStdInput = childConnection.read;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    //C:/xampp/htdocs/Halite/Halite/Debug/ExampleBot.exe
    //C:/Users/Michael/Anaconda3/python.exe
    //C:/Program Files/Java/jre7/bin/java.exe -cp C:/xampp/htdocs/Halite/AIResources/Java MyBot
    bool success = CreateProcess(
        "C:\\windows\\system32\\cmd.exe",
        LPSTR(command.c_str()),     //command line
        NULL,          //process security attributes
        NULL,          //primary thread security attributes
        TRUE,          //handles are inherited
        0,             //creation flags
        NULL,          //use parent's environment
        NULL,          //use parent's current directory
        &siStartInfo,  //STARTUPINFO pointer
        &piProcInfo
    );  //receives PROCESS_INFORMATION
    if(!success) {
        LOG(ERROR) << "Could not start process";
        throw 1;
    }
    else {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        this->process = piProcInfo.hProcess;
        this->connection = parentConnection;
    }

#else

    LOG(DEBUG) << "Starting to run: " << command;

    pid_t pid;
    int writePipe[2];
    int readPipe[2];

    if(pipe(writePipe)) {
        LOG(ERROR) << "Error creating pipe";
        throw 1;
    }
    if(pipe(readPipe)) {
        LOG(ERROR) << "Error creating pipe";
        throw 1;
    }

    pid_t ppid_before_fork = getpid();

    //Fork a child process
    pid = fork();
    if(pid == 0) { //This is the child
        setpgid(getpid(), getpid());

#ifdef __linux__
        // install a parent death signal
        // http://stackoverflow.com/a/36945270
        int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (r == -1)
        {
            LOG(ERROR) << "Error installing parent death signal";
            throw 1;
        }
        if (getppid() != ppid_before_fork)
            exit(1);
#endif

        dup2(writePipe[0], STDIN_FILENO);

        dup2(readPipe[1], STDOUT_FILENO);
        dup2(readPipe[1], STDERR_FILENO);

        execl("/bin/sh", "sh", "-c", command.c_str(), (char*) NULL);

        //Nothing past the execl should be run

        exit(1);
    } else if(pid < 0) {
        LOG(ERROR) << "Fork failed\n";
        throw 1;
    }

    this->connection.read = readPipe[0];
    this->connection.write = writePipe[1];

    this->process = pid;

#endif
}


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
