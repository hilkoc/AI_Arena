#pragma once

#include "ChpState.hpp"
#include "ChpRandomAgent.hpp"
#include "ChpLinearAgent.hpp"
#include "ChpHumanAgent.hpp"
#include "core/NetworkAgent.hpp"
#include "core/Factory.hpp"
#include <vector>

class ChpFactory : public Factory {
public:
    virtual ~ChpFactory() {
        if (chpState != nullptr) {
            delete chpState;
        }
        while (!agents.empty()) {
            delete agents.back();
            agents.pop_back();
        }
    };

    ChpFactory(unsigned int bets_in) : chpState(nullptr), bets(bets_in) {};

    virtual State& createState() {
        // Only hold one state at a time, so delete previous if any.
        if (chpState != nullptr) {
            delete chpState;
        }
        chpState = new ChpState(bets);
        return *(chpState);
    };

    Agent* createRandomAgent(unsigned int const id) {
        ChpRandomAgent* a = new ChpRandomAgent(id);
        this->agents.push_back(a);
        return a;
    }

    Agent* createLinearAgent(unsigned int const id, int step, unsigned int const start) {
        ChpLinearAgent* a = new ChpLinearAgent(id, step, start);
        this->agents.push_back(a);
        return a;
    }

    Agent* createHumanAgent(unsigned int const id) {
        ChpHumanAgent* a = new ChpHumanAgent(id);
        this->agents.push_back(a);
        return a;
    }

    Agent* createNetworkAgent(unsigned int const id, std::string const& cmd) {
        NetworkAgent* a = new NetworkAgent(id, cmd);
        this->agents.push_back(a);
        return a;
    }

    //prohibit allocation on the heap
    void * operator new   (size_t) = delete;
    void * operator new[] (size_t) = delete;
    //void   operator delete   (void *) = delete; //Don't delete delete
    void   operator delete[] (void*) = delete;
    
    // prohibit copying
    ChpFactory(ChpFactory& o) = delete;
    ChpFactory& operator= (ChpFactory& o) = delete;
private:
    unsigned int bets;
    ChpState* chpState;
    std::vector<Agent*> agents;
};
