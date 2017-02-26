#pragma once


/** The action that agents send to the environment. */
class Action {
public:
  virtual ~Action();
  
  // default constructor  
  Action() = default;

};
