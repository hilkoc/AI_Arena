#include "Environment.hpp"

  Environment::~Environment() {
    this->disconnect_agents();  
  }

  /** Disconnects all players. */
  void Environment::connect_agents(std::vector<Agent> agents){};

  /** Disconnects all players. */
  void Environment::disconnect_agents(){};

  /** Removes player with given id from the game. For example after a timeout or an invalid move.*/
  void Environment::drop_player(unsigned int player_id){};

  /** Prepares an episode. Creates the initial State and signals Agents to initialize. */
  void Environment::initialize_episode(){};

  /** Signals all Agents that the episode is finished. After this function returns a new episode may be started. */
  void Environment::finalize_episode(){};