#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <thread>
#include <future>

#include "hlt.hpp"
#include "json.hpp"
#include "../networking/Networking.hpp"

extern bool quiet_output = false;

struct PlayerStatistics {
    int tag;
    int rank;
    int last_frame_alive;
    double average_territory_count;
    double average_strength_count;
    double average_production_count;
    double still_percentage;
    int init_response_time;
    double average_frame_response_time;
};
static std::ostream & operator<<(std::ostream & o, const PlayerStatistics & p) {
    o << p.tag << ' ' << p.rank << ' ' << p.last_frame_alive;// << ' ' << p.average_territory_count << ' ' << p.average_strength_count << ' ' << p.average_production_count << ' ' << p.still_percentage << ' ' << p.average_response_time;
    return o;
}

struct GameStatistics {
    std::vector<PlayerStatistics> player_statistics;
    std::string output_filename;
    std::set<unsigned short> timeout_tags;
    std::vector<std::string> timeout_log_filenames;
};
static std::ostream & operator<<(std::ostream & o, const GameStatistics & g) {
    for(auto a = g.player_statistics.begin(); a != g.player_statistics.end(); a++) o << (*a) << std::endl;
    for(auto a = g.timeout_tags.begin(); a != g.timeout_tags.end(); a++) o << (*a) << ' ';
    if(g.timeout_tags.empty()) o << ' ';
    std::cout << std::endl;
    for(auto a = g.timeout_log_filenames.begin(); a != g.timeout_log_filenames.end(); a++) o << (*a) << ' ';
    if(g.timeout_log_filenames.empty()) o << ' ';
    return o;
}

class Halite {
private:
    //Networking
    Networking networking;

    //Game state
    unsigned short turn_number;
    unsigned short number_of_players;
    unsigned short productive_squares_remaining;
    unsigned short n_players_for_map_creation;
    bool ignore_timeout;
    hlt::Map game_map;
    std::vector<std::string> player_names;
    std::vector< std::map<hlt::Location, unsigned char> > player_moves;

    //Statistics
    std::vector<unsigned short> alive_frame_count;
    std::vector<unsigned int> last_territory_count;
    std::vector<unsigned int> full_territory_count;
    std::vector<unsigned int> full_strength_count;
    std::vector<unsigned int> full_production_count;
    std::vector<unsigned int> full_still_count;
    std::vector<unsigned int> full_cardinal_count;
    std::vector<unsigned int> init_response_times;
    std::vector<unsigned int> total_frame_response_times;
    std::set<unsigned short> timeout_tags;

    //Full game
    std::vector<hlt::Map> full_frames; //All the maps!
    std::vector< std::vector< std::vector<int> > > full_player_moves; //Each inner 2d array represents the moves across the map for the corresponding frame
                                                                      //and is guaranteed to have an outer size of map_height and an inner size of map_width

    std::vector<bool> processNextFrame(std::vector<bool> alive);
    void output(std::string filename);
public:
    Halite(unsigned short width_, unsigned short height_, unsigned int seed_, unsigned short n_players_for_map_creation, Networking networking_, bool shouldIgnoreTimeout);

  GameStatistics runGame(std::vector<std::string> * names_, unsigned int seed, unsigned int id, bool enabledReplay, std::string replayDirectory);
    std::string getName(unsigned char playerTag);

    ~Halite();
};

class GameState {
public:
  virtual ~GameState();
  
  /** Prepares the game. Connects the players and signals them to initialize. */
  virtual void initialize() = 0;

  /** Assumes initialize has been called. Starts the game. */
  virtual void start_game() = 0;

  /** Assumes start_game has been called. Receives the actions from connected players and evolves the game state until the termination conditions are met. */
  virtual void run_game() = 0;

  /** Removes player with given id from the game. For example after a timeout.*/
  virtual void drop_player(unsigned int player_id) = 0;

  /** Resets the game state but keeps players connected. Signals all players that the game is finished. */
  virtual GameStatistics end_game() = 0;

  /** Disconnects all players. */
  virtual void terminate() = 0;

};


class ChickenPoker : public GameState {
public:
  virtual ~ChickenPoker();
  ChickenPoker(unsigned short cards, Networking networking, bool ignore_timeout) {
}
  
  virtual void initialize();
  virtual void start_game();
  virtual void run_game();
  virtual void drop_player(unsigned int player_id);
  virtual GameStatistics end_game();
  virtual void terminate();
};

#endif
