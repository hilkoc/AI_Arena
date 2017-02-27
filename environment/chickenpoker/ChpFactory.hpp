#pragma once

#include "ChpState.hpp"
#include "core/Factory.hpp"


class ChpFactory : public Factory {
public:
    virtual ~ChpFactory() = default;
    ChpFactory() = default;

    ChpFactory(unsigned int cards_in) : cards(cards_in) {};

    virtual State& createState() {
        this->chpState = ChpState(cards);
        return this->chpState;
    }

private:
    //prohibit allocation on the heap
    void * operator new   (size_t);
    void * operator new[] (size_t);
    // void   operator delete   (void *); //gives linking errors
    void   operator delete[] (void*);

    ChpState chpState;
    unsigned int cards;
};
