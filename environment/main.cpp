#include <iostream>
#include <tclap/CmdLine.h>

//#include <cctype>
////#include <chrono>
//#include <list>
//#include <string.h>

#include "chickenpoker/ChpFactory.hpp"
#include "chickenpoker/ChpState.hpp"
#include "core/Environment.hpp"

class Environment;

struct CmdParams {
  unsigned int cards;
  unsigned int games;
  bool quiet;
  bool ignore_timeout;
  bool override_names;
  std::vector<std::string> run_commands;

};

CmdParams parse_cmdline_args(int argc, char ** argv) {
    TCLAP::CmdLine cmd("Chicken Poker Game Environment", ' ', "0.0");

    TCLAP::SwitchArg overrideSwitch("o", "override", "Overrides player-sent names using cmd args [SERVER ONLY].", cmd, false);
    TCLAP::SwitchArg timeoutSwitch("t", "timeout", "Ignore timeouts. Gives all bots unlimited time).", cmd, false);
    TCLAP::SwitchArg quietSwitch("q", "quiet", "Don't show intermidate game output.", cmd, false);

    TCLAP::ValueArg<unsigned int> setsArg("s", "sets", "A set consists of p games where p is the number of players. Run g = s*p number of games.", false, 0, "number", cmd);   
    TCLAP::ValueArg<unsigned int> gamesArg("g", "games", "Run g number of games. Player ids are rotated after every game.", false, 1, "number", cmd);   
    TCLAP::ValueArg<unsigned int> cardsArg("n", "cards", "The number of cards each player is dealt. This is also the value of the highest card.", true, 10, "number", cmd);   
    //rm TCLAP::ValueArg<std::string> replayDirectoryArg("i", "replaydirectory", "The path to directory for replay output.", false, ".", "path to directory", cmd);
    TCLAP::UnlabeledMultiArg<std::string> runcmdArgs("NonspecifiedArgs", "Run commands for bots.", false, "cmds in double quotes", cmd);

    cmd.parse(argc, argv);

    CmdParams cmdParams;
    cmdParams.cards = cardsArg.getValue();
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

int run_main(CmdParams& cmdParams) {

    // Initialize the environment and create the agents.
    std::vector<Agent> agents;
    for (std::string cmd : cmdParams.run_commands) {
         agents.push_back(Agent());
    }
    ChpFactory chpFactory = ChpFactory(cmdParams.cards);
    Environment environment(chpFactory);
    environment.connect_agents(agents);
    environment.run_episodes(cmdParams.games);


    //State& state = chpFactory.createState();
//    Networking networking;
//    //Create game. Null parameters will be ignored.
//    ChickenPoker chickenPoker = ChickenPoker(cmdParams.cards, networking, cmdParams.ignore_timeout);
//    GameState& gameState = chickenPoker;
    return 0;
}


int main(int argc, char ** argv) {
    /** Parses the command line parameters, and runs the environment. */
    CmdParams cmdParams = parse_cmdline_args(argc, argv);
    return run_main(cmdParams);
}

