#include <iostream>
#include <tclap/CmdLine.h>

//#include <cctype>
////#include <chrono>
//#include <list>
//#include <string.h>

//#include "core/chickenpoker.hpp"
#include "core/Environment.hpp"

class Environment;

struct GameParams {
  unsigned int cards;
  unsigned int games;
  bool quiet;
  bool ignore_timeout;
  bool override_names;
  std::vector<std::string> run_commands;

};

GameParams parse_cmdline_args(int argc, char ** argv) {
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

    GameParams gameParams;
    gameParams.cards = cardsArg.getValue();
    gameParams.games = gamesArg.getValue();
    unsigned int sets = setsArg.getValue();
    gameParams.quiet = quietSwitch.getValue();
    gameParams.ignore_timeout = timeoutSwitch.getValue();
    gameParams.override_names = overrideSwitch.getValue();
    gameParams.run_commands = runcmdArgs.getValue();

    unsigned int numberOfPlayers = gameParams.run_commands.size();

    // Some validation on the parsed arguments
    if (sets > 0 && gameParams.games > 1) {
      std::cout << "Only one of -games or -sets may be specified, but not both." << std::endl;
            exit(1);
    }
    if (sets > 0 && gameParams.games == 1) {
      gameParams.games = numberOfPlayers * sets;
    }

    std::cout << "set, games, nr players is " << sets << ", " << gameParams.games << ", "<< numberOfPlayers << std::endl;
    return gameParams;
}

int run_main(GameParams& gameParams) {

    // Initialize the environment
    Environment environment;
//    Networking networking;
//    //Create game. Null parameters will be ignored.
//    ChickenPoker chickenPoker = ChickenPoker(gameParams.cards, networking, gameParams.ignore_timeout);
//    GameState& gameState = chickenPoker;
    return 0;
}


int main(int argc, char ** argv) {
    /** Parses the command line parameters, and runs the environment. */
    GameParams gameParams = parse_cmdline_args(argc, argv);
    return run_main(gameParams);
}

