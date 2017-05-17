# Open AI Arena

The purpose of this project is to research Artificial Intelligence and Reinforcement Learning.

In the AI Arena, multiple agents can interact with a single environment. After sending its action, each each agent will receive a reward. This allows agents to learn, improve their policy and to adapt to each other.

This setting raises interesting questions.

* How do multiple agents interact and adapt to each other?
Will they compete or cooperate?
* How will they deal with a Nash-equilibrium? Do they get stuck in a prisoners dillema or can Artificial Intelligence with enough exploration learn to escape the dillema?


## The Arena

Competition drives progress. That's why the arena is not only for 'bots' to compete with each other. Anyone is free to derive from this code to develop a multi player game and setup an AI programming competition.

The Arena is is C++，but is very flexible. Agents can be implemented in any programming language. They interact with the environment via stdout.

An [example game](./environment/chickenpoker/README.md) is provided within the code.

### Credits

This project is inspired by the [Halite](https://halite.io/index.php) which was a particularly fun AI programming challenge. Indeed some of the code in the NetworkAgent.cpp is derived from [that project](https://github.com/HaliteChallenge/Halite).
