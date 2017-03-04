#include <iostream>

#include "logging/log.h"
//Log Config
structlog LOGCFG;

#include <tclap/CmdLine.h>
#include "chickenpoker/ChpFactory.hpp"
#include "chickenpoker/ChpState.hpp"
#include "chickenpoker/ChpAgent.hpp"
#include "core/Environment.hpp"

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

/** Creates and runs the environment with the given parameters. */
int run_main(CmdParams& cmdParams) {

    // Initialize the environment and create the agents.
    unsigned int agent_id(1);
    std::vector<ChpAgent> agents;
    for (std::string cmd : cmdParams.run_commands) {
        ChpAgent chpAgent(agent_id);
        LOG(DEBUG) << "Creating agent " << chpAgent.get_id();
        agents.push_back(chpAgent);
        agent_id++;
    }
    std::vector<Agent*> agents_ptrs; // Convert to pointers
    for (ChpAgent& agent : agents) {
        agents_ptrs.push_back(&agent);
    }

    ChpFactory chpFactory(cmdParams.bets);
    Environment environment(chpFactory);
    environment.connect_agents(agents_ptrs);
    environment.run_episodes(cmdParams.games);


    //State& state = chpFactory.createState();
//    Networking networking;
//    //Create game. Null parameters will be ignored.
//    ChickenPoker chickenPoker = ChickenPoker(cmdParams.bets, networking, cmdParams.ignore_timeout);
//    GameState& gameState = chickenPoker;
    return 0;
}

void test();

int main(int argc, char ** argv) {
    LOGCFG.headers = true;
    LOGCFG.level = DEBUG;
    LOG(INFO) << "Main executed with " << (argc - 1) << " arguments";

    test();
    /** Parses the command line parameters, and runs the environment. */
    CmdParams cmdParams = parse_cmdline_args(argc, argv);
    return run_main(cmdParams);
}


class Observation { // Value type
private:
    int k = int(3);
};

class Stat; //forward declare
class Agen {
public:
    virtual void receive(Stat& stat) = 0;
};


class Agen; //forward declare
class Stat {  //Color
public:
    virtual void send_to(Agen& agen) = 0;
};

    
class SubStat : public Stat {
public:
    virtual void send_to(Agen& agen) {
        LOG(INFO) << "Sending State.";
        agen.receive(*this);
    };
};


class SubAgen : public Agen { // CountVisitor
public:
    virtual void receive(Stat& stat) {
        LOG(INFO) << "SubAgen receiving Stat&";
        SubStat& substat = static_cast<SubStat&>(stat);
        finally_receive(substat);
    };

    void finally_receive(SubStat& stat) {
        LOG(INFO) << "OK! SubAgen receiving SubStat&";
    };
};



void test(){
    SubStat substat;
    SubAgen subagen;

    Stat& stat = substat;
    Agen& agen = subagen;

    // Send the state to the agent.

//    subagen.receive(substat); // OK
//    agen.receive(stat);
    stat.send_to(agen);
//    substat.send_to(agen);
    LOG(INFO) << "\n\n\n";
}


