#include <iostream>
#include <cctype>
//#include <chrono>
#include <list>
#include <string.h>

#include "core/chickenpoker.hpp"
#include <tclap/CmdLine.h>


bool quiet_output = false; //Need to be passed to a bunch of classes; extern is cleaner.


Networking promptNetworking();
void promptDimensions(unsigned short & w, unsigned short & h);

struct GameParams {
  unsigned int cards;
  unsigned int games;
  bool quiet;
  bool ignore_timeout;
  bool override_names;
  std::vector<std::string> run_commands;

};

int main(int argc, char ** argv) {
  /** Parses the command line parameters, initializes the game and runs the game. */

    unsigned int id =  99; //std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();

    TCLAP::CmdLine cmd("Chicken Poker Game Environment", ' ', "0.0");

    TCLAP::SwitchArg overrideSwitch("o", "override", "Overrides player-sent names using cmd args [SERVER ONLY].", cmd, false);
    TCLAP::SwitchArg timeoutSwitch("t", "timeout", "Ignore timeouts. Gives all bots unlimited time).", cmd, false);
    TCLAP::SwitchArg quietSwitch("q", "quiet", "Don't show intermidate game output.", cmd, false);

    TCLAP::ValueArg<unsigned int> setsArg("s", "sets", "A set consists of p games where p is the number of players. Run g = s*p number of games.", false, -1, "number", cmd);   
    TCLAP::ValueArg<unsigned int> gamesArg("g", "games", "Run g number of games. Player ids are rotated after every game.", false, -1, "number", cmd);   
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
    if (sets > 0 && gameParams.games > 0) {
      std::cout << "Only one of -games or -sets may be specified, but not both." << std::endl;
            exit(1);
    }
    if (sets > 0 && gameParams.games < 0 ) {
      gameParams.games = numberOfPlayers * sets;
    }
    
    // Initialize the game
    Networking networking;
    //Create game. Null parameters will be ignored.
    ChickenPoker chickenPoker = ChickenPoker(gameParams.cards, networking, gameParams.ignore_timeout);
    GameState& gameState = chickenPoker;


    std::list<std::string> unlabeledArgs;
    for(auto a = gameParams.run_commands.begin(); a != gameParams.run_commands.end(); a++) {
        unlabeledArgs.push_back(*a);
    }

    std::vector<std::string> names;
    if(gameParams.override_names) {
        if(unlabeledArgs.size() < 4 || unlabeledArgs.size() % 2 != 0) {
            std::cout << "Invalid number of player parameters with override switch enabled.  Override intended for server use only." << std::endl;
            exit(1);
        }
        else {
            try {
                while(!unlabeledArgs.empty()) {
                    networking.startAndConnectBot(unlabeledArgs.front());
                    unlabeledArgs.pop_front();
                    names.push_back(unlabeledArgs.front());
                    unlabeledArgs.pop_front();
                }
            }
            catch(...) {
                std::cout << "Invalid player parameters with override switch enabled.  Override intended for server use only." << std::endl;
                exit(1);
            }
        }
    }
    else {
        if(unlabeledArgs.size() < 1) {
            std::cout << "Please provide a run command string for at least one bot." << std::endl
            << "Use the --help flag for usage details.\n";
            exit(1);
        }
        try {
            while(!unlabeledArgs.empty()) {
                std::cout << unlabeledArgs.front() << std::endl;
                networking.startAndConnectBot(unlabeledArgs.front());
                unlabeledArgs.pop_front();
            }
        }
        catch(...) {
            std::cout << "One or more of your bot launch command strings failed.  Please check for correctness and try again." << std::endl;
            exit(1);
        }
    }

    numberOfPlayers = networking.numberOfPlayers();
    



    std::string outputFilename = ".";
    GameStatistics stats = gameState.run_game() ; //&names, 5, id, true, outputFilename);

    if(quiet_output) {
        std::cout << stats;
    }
    else {
        for(unsigned int a = 0; a < stats.player_statistics.size(); a++) {
          std::cout << "Player #" << stats.player_statistics[a].tag << ", " << stats.player_statistics[a].tag << ", came in rank #" << stats.player_statistics[a].rank << " and was last alive on frame #" << stats.player_statistics[a].last_frame_alive << "!\n";
        }
    }


    return 0;
}
