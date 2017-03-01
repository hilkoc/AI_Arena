#include "Environment.hpp"

    Environment::~Environment() {
        this->disconnect_agents();  
    }

    /** Disconnects all players. */
    void Environment::connect_agents(std::vector<Agent*> agents){
        this->agents = agents;
    };

    /** Disconnects all players. */
    void Environment::disconnect_agents(){
        std::cout << "Disconnecting agents " << this->agents.size() << std::endl;
    };

    /** Removes player with given id from the game. For example after a timeout or an invalid move.*/
    void Environment::drop_agent(Agent& agent){
        this->state->remove_agent(agent);
    };

    /** Prepares an episode. Creates the initial State and signals Agents to initialize. */
    void Environment::initialize_episode(){
        this->state = &this->factory.createState();
        // The state needs to know the agents that interact with it.
        this->state->add_agents(this->agents);
        this->state->reset();
    };

    /** Signals all Agents that the episode is finished. After this function returns a new episode may be started. */
    void Environment::finalize_episode(){};
