#include "logging/log.h"
//Log Config
structlog LOGCFG;

#include <tclap/CmdLine.h>
#include "chickenpoker/ChpFactory.hpp"
#include "chickenpoker/ChpState.hpp"
#include "chickenpoker/ChpAgent.hpp"
#include "core/Environment.hpp"

#include <iostream>

#include <string>
#include <sstream>
#include <vector>
#include <iterator>


/** Type to hold parsed command line parameters */
struct CmdParams {
  unsigned int bets;
  unsigned int games;
  bool quiet;
  bool ignore_timeout;
  bool override_names;
  std::vector<std::string> run_commands;

};

/** Parses arguments given on the command line and returns an object of type CmdParams. */
CmdParams parse_cmdline_args(int argc, char ** argv) {

    TCLAP::CmdLine cmd("Chicken Poker Game Environment", ' ', "0.0");

    TCLAP::SwitchArg overrideSwitch("o", "override", "Overrides player-sent names using cmd args [SERVER ONLY].", cmd, false);
    TCLAP::SwitchArg timeoutSwitch("t", "timeout", "Ignore timeouts. Gives all bots unlimited time).", cmd, false);
    TCLAP::SwitchArg quietSwitch("q", "quiet", "Don't show intermidate game output.", cmd, false);

    TCLAP::ValueArg<unsigned int> setsArg("s", "sets", "A set consists of p games where p is the number of players. Run g = s*p number of games.", false, 0, "number", cmd);   
    TCLAP::ValueArg<unsigned int> gamesArg("g", "games", "Run g number of games. Player ids are rotated after every game.", false, 1, "number", cmd);   
    TCLAP::ValueArg<unsigned int> betsArg("n", "bets", "The number of rounds in the game. This is also the value of the highest bet.", true, 10, "number", cmd);   
    //rm TCLAP::ValueArg<std::string> replayDirectoryArg("i", "replaydirectory", "The path to directory for replay output.", false, ".", "path to directory", cmd);
    TCLAP::UnlabeledMultiArg<std::string> runcmdArgs("NonspecifiedArgs", "Run commands for bots.", false, "cmds in double quotes", cmd);

    cmd.parse(argc, argv);

    CmdParams cmdParams;
    cmdParams.bets = betsArg.getValue();
    cmdParams.games = gamesArg.getValue();
    unsigned int sets = setsArg.getValue();
    cmdParams.quiet = quietSwitch.getValue();
    cmdParams.ignore_timeout = timeoutSwitch.getValue();
    cmdParams.override_names = overrideSwitch.getValue();
    cmdParams.run_commands = runcmdArgs.getValue();

    unsigned int numberOfPlayers = cmdParams.run_commands.size();

    // Some validation on the parsed arguments
    if (sets > 0 && cmdParams.games > 1) {
      std::cout << "Only one of -games or -sets may be specified, but not both." << std::endl;
            exit(1);
    }
    if (sets > 0 && cmdParams.games == 1) {
      cmdParams.games = numberOfPlayers * sets;
    }

    std::cout << "set, games, nr players is " << sets << ", " << cmdParams.games << ", "<< numberOfPlayers << std::endl;
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
    if ("linear" == cmd_parts[0]) {
        if (3 == cmd_parts.size()) {
            std::string step_s = cmd_parts[1];
            std::string start_s = cmd_parts[2];
            int step = std::stoi(step_s);
            int start = std::stoi(start_s);
            if (start < 1) {
                LOG(WARN) << "Second parameter <start> must be > 1 for linear. Given " << start;
                exit(1);
            }
            return chpFactory.createLinearAgent(agent_id, step, start);
        } else {
            LOG(ERROR) << "linear takes exactly two params <step> and <start>."
             << " For example: " << "\"linear 3 2\". " << "Given " << cmd_parts.size() - 1;
            for (std::string& part : cmd_parts) { LOG(ERROR) << part; }
            exit(1);
        }
    }

    Agent* a = chpFactory.createAgent(agent_id);
    return a;
};

/** Creates and runs the environment with the given parameters. */
int run_main(CmdParams& cmdParams) {

    // Initialize the environment and create the agents.
    unsigned int agent_id(1);
    ChpFactory chpFactory(cmdParams.bets);
    std::vector<Agent*> agents;

    for (std::string const & cmd : cmdParams.run_commands) {
        Agent* a = parse_run_cmd(cmd, agent_id, chpFactory);
        LOG(DEBUG) << "Created agent " << a->get_id();
        agents.push_back(a);
        agent_id++;
    }

    Environment environment(chpFactory);
    environment.connect_agents(agents);
    environment.run_episodes(cmdParams.games);

    return 0;
}



int main(int argc, char ** argv) {
    LOGCFG.headers = false;
    LOGCFG.level = INFO;
    LOG(INFO) << "Starting Chicken Poker";

    // Initializing an unsigned with a negative nr gives garbage.
//    int k = -7;    unsigned int l = k;  LOG(INFO) << "k " << k << " l " << l;

    /** Parses the command line parameters, and runs the environment. */
    CmdParams cmdParams = parse_cmdline_args(argc, argv);
    return run_main(cmdParams);
}

