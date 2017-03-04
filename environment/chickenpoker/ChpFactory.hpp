#pragma once

#include "ChpState.hpp"
#include "core/Factory.hpp"


class ChpFactory : public Factory {
public:
    virtual ~ChpFactory() {
        if (chpState != nullptr) {
            delete chpState;
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

    //prohibit allocation on the heap
    void * operator new   (size_t) = delete;
    void * operator new[] (size_t) = delete;
    //void   operator delete   (void *) = delete; //Don't delete delete
    void   operator delete[] (void*) = delete;
    
    // prohibit copying
    ChpFactory(ChpFactory& o) = delete;
    ChpFactory& operator= (ChpFactory& o) = delete;
private:
    ChpState* chpState;
    unsigned int bets;
};
