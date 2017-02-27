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

    ChpFactory(unsigned int cards_in) : chpState(nullptr), cards(cards_in) {};

    virtual State& createState() {
        chpState = new ChpState(cards);
        return *(chpState);
    };

private:
    //prohibit allocation on the heap
    void * operator new   (size_t);
    void * operator new[] (size_t);
    // void   operator delete   (void *); //gives linking errors
    void   operator delete[] (void*);

    ChpState* chpState;
    unsigned int cards;
};
