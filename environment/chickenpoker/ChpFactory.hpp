#pragma once

#include "ChpState.hpp"
#include "ChpAgent.hpp"
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

    Agent* createAgent(unsigned int const id) {
        ChpAgent* a = new ChpAgent(id);
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
