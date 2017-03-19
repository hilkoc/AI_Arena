#include "logging/log.h"
//Log Config
structlog LOGCFG;

#include "tclap/CmdLine.h"
#include "chickenpoker/ChpFactory.hpp"
#include "chickenpoker/ChpState.hpp"
//#include "chickenpoker/ChpAgent.hpp"
#include "core/Environment.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>


/** Type to hold parsed command line parameters */
struct CmdParams {
  unsigned int bets;
  unsigned int games;
  bool debug;
  bool verbose;
  bool quiet;
  bool ignore_timeout;
  bool override_names;
  std::vector<std::string> run_commands;

};

/** Parses arguments given on the command line and returns an object of type CmdParams. */
CmdParams parse_cmdline_args(int argc, char ** argv) {

    TCLAP::CmdLine cmd("Chicken Poker Game Environment", ' ', "0.1");

    TCLAP::SwitchArg overrideSwitch("o", "override", "Overrides player-sent names using cmd args [SERVER ONLY].", cmd, false);
    TCLAP::SwitchArg timeoutSwitch("t", "timeout", "Ignore timeouts. Gives all bots unlimited time).", cmd, false);
    TCLAP::SwitchArg debugSwitch("d", "debug", "Show all output, including debug.", cmd, false);
    TCLAP::SwitchArg verboseSwitch("v", "verbose", "Show all output for each round.", cmd, false);
    TCLAP::SwitchArg quietSwitch("q", "quiet", "Don't show intermidate game output.", cmd, false);

    TCLAP::ValueArg<unsigned int> setsArg("s", "sets", "A set consists of p games where p is the number of players. Run g = s*p number of games.", false, 0, "number", cmd);
    TCLAP::ValueArg<unsigned int> gamesArg("g", "games", "Run g number of games. Player ids are rotated after every game.", false, 1, "number", cmd);
    TCLAP::ValueArg<unsigned int> betsArg("n", "bets", "The number of rounds in the game. This is also the value of the highest bet.", true, 10, "number", cmd);
    //rm TCLAP::ValueArg<std::string> replayDirectoryArg("i", "replaydirectory", "The path to directory for replay output.", false, ".", "path to directory", cmd);
    TCLAP::UnlabeledMultiArg<std::string> runcmdArgs("runCommands", "Run commands for bots.", false, "cmds in double quotes", cmd);

    cmd.parse(argc, argv);

    CmdParams cmdParams;
    cmdParams.bets = betsArg.getValue();
    cmdParams.games = gamesArg.getValue();
    unsigned int sets = setsArg.getValue();
    cmdParams.debug = debugSwitch.getValue();
    cmdParams.verbose = verboseSwitch.getValue();
    cmdParams.quiet = quietSwitch.getValue();
    cmdParams.ignore_timeout = timeoutSwitch.getValue();
    cmdParams.override_names = overrideSwitch.getValue();
    cmdParams.run_commands = runcmdArgs.getValue();

    unsigned int numberOfPlayers = cmdParams.run_commands.size();

    // Some validation on the parsed arguments
    if (sets > 0 && cmdParams.games > 1) {
      LOG(ERROR) << "Only one of -games or -sets may be specified, but not both.";
            exit(1);
    }
    if (sets > 0 && cmdParams.games == 1) {
      cmdParams.games = numberOfPlayers * sets;
    }

    LOG(DEBUG) << "set, games, nr players is " << sets << ", " << cmdParams.games << ", "<< numberOfPlayers;
    return cmdParams;
}


template<typename Out>
void split(std::string const& s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(std::string const& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

Agent* parse_run_cmd(std::string const& cmd, unsigned int const agent_id, ChpFactory& chpFactory) {
    std::vector<std::string> cmd_parts = split(cmd, ' ');
    if ("random" == cmd_parts[0]) {
        return chpFactory.createRandomAgent(agent_id);
    }
    if ("linear" == cmd_parts[0]) {
        if (3 == cmd_parts.size()) {
            std::string start_s = cmd_parts[1];
            std::string step_s = cmd_parts[2];
            int start = std::stoi(start_s);
            int step = std::stoi(step_s);
            if (start < 1) {
                LOG(WARN) << "First parameter <start> must be positive. Given " << start;
                exit(1);
            }
            return chpFactory.createLinearAgent(agent_id, step, start);
        } else {
            LOG(ERROR) << "linear takes exactly two params <start> and <step>."
             << " For example: " << "\"linear 3 2\". " << "Given " << cmd_parts.size() - 1;
            for (std::string& part : cmd_parts) { LOG(ERROR) << part; }
            exit(1);
        }
    }
    if ("human" == cmd_parts[0]) {
        return chpFactory.createHumanAgent(agent_id);
    }
    return chpFactory.createNetworkAgent(agent_id, cmd);
    exit(1);
    return nullptr;
};

/** Creates and runs the environment with the given parameters. */
int run_main(CmdParams& cmdParams) {
    // Configure logging
    if (cmdParams.debug) {
        LOGCFG.level = DEBUG;
    }
    if (cmdParams.verbose) {
        LOGCFG.level = INFO;
    }
    if (cmdParams.quiet) {
        LOGCFG.level = QUIET;  // -q flag overrides -v flag
    }

    // Initialize the environment and create the agents.
    unsigned int agent_id(1);
    ChpFactory chpFactory(cmdParams.bets);
    std::vector<Agent*> agents;

    for (std::string const& cmd : cmdParams.run_commands) {
        Agent* a = parse_run_cmd(cmd, agent_id, chpFactory);
        LOG(EPSOD) << "Agent " << a->get_id() << ": " << cmd;
        agents.push_back(a);
        agent_id++;
    }

    LOG(EPSOD) << "Starting " << cmdParams.games << " games, each with "
              << cmdParams.bets << " rounds.";
    Environment environment(chpFactory);
    environment.connect_agents(agents);
    environment.run_episodes(cmdParams.games);

    return 0;
}



int main(int argc, char ** argv) {
    LOGCFG.headers = false;
    LOGCFG.level = EPSOD; // EPSOD default log level, only episode and session output.
    LOG(INFO) << "Starting Chicken Poker";

    /** Parses the command line parameters, and runs the environment. */
    CmdParams cmdParams = parse_cmdline_args(argc, argv);
    return run_main(cmdParams);
}
