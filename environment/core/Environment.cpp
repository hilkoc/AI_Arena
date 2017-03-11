#include "Environment.hpp"


    Environment::~Environment() {
        this->disconnect_agents();  
    }

    /** Initializes a session for multiple episodes. Connects all agents. */
    void Environment::connect_agents(std::vector<Agent*>& agents){
        this->agents = agents;
        this->state = &this->factory.createState();
        // The state needs to know the agents that interact with it.
        this->state->add_agents(this->agents);
    };

    /** Disconnects all agents. */
    void Environment::disconnect_agents(){
        LOG(DEBUG) << "Disconnecting agents " << this->agents.size();
    };

    /** Removes agent with given id from the game. For example after a timeout or an invalid move.*/
    void Environment::drop_agent(Agent& agent){
        this->state->remove_agent(agent);
    };

    /** Prepares an episode. Creates the initial State and signals Agents to initialize. */
    void Environment::initialize_episode(){
        this->state->reset();
        LOG(DEBUG) << "Episode initialized.";
    };

    /** Signals all Agents that the episode is finished. After this function returns a new episode may be started. */
    void Environment::finalize_episode(Stats& episode_stats, Stats& session_stats){
        LOG(EPSOD) << "\nEpisode summary";
        episode_stats.log_summary(EPSOD);
        session_stats.add_rewards(episode_stats.get_total_rewards());
    };
