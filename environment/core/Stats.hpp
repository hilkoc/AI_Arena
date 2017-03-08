#pragma once

#include "logging/log.h"
#include "Reward.hpp"

/** Represents the statistics for one episode or session. */
class Stats {
public:

    Stats() {
        nr_steps = 0;
    };

    void add_rewards(std::map<Agent*, Reward> const& rewards) {
        this->nr_steps += 1;
        for (auto const& pair : rewards) {
            this->total_rewards[pair.first] += pair.second;
        }
    };

    std::map<Agent*, Reward> const& get_total_rewards() const {
        return this->total_rewards;
    };

    /** Logs the total stats */
    void log_summary(typelog loglevel=QUIET) const {
        for (auto const& pair : this->total_rewards) {
            unsigned int const agent_id = pair.first->get_id() ;
            Reward total_reward = pair.second;
            LOG(loglevel) << "Agent " << agent_id << ": total " << total_reward << " , average " << total_reward * 1.0 / nr_steps;
        }
    };

private:

    /** The number of steps in the episode until termination_condition was reached. */
    unsigned int nr_steps;

    /** The total reward for this episode. */
    std::map<Agent*, Reward> total_rewards;
};


///** Represents the statistics for one session. A session consists of multiple episodes. */
//using SessionStats = EpisodeStats;
