#pragma once

//#include "core/State.hpp"
class State;

/** Creates the initial state for a task. */
class Factory {
public:
    virtual ~Factory(){};

    virtual State& createState() = 0;
};
