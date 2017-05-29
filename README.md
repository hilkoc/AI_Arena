# Open AI Arena

The purpose of this project is to research Artificial Intelligence and Reinforcement Learning.

In the AI Arena, multiple agents can interact with a single environment. After sending its action, each each agent will receive a reward. This allows agents to learn, improve their policy and to adapt to each other.

This setting raises interesting questions.

* How do multiple agents interact and adapt to each other?
Will they compete or cooperate?
* How will they deal with a Nash-equilibrium? Do they get stuck in a prisoners dillema or can Artificial Intelligence with enough exploration learn to escape the dillema?


## The Arena

Competition drives progress. That's why the arena is not only for 'bots' to compete with each other. Anyone is free to derive from this code to develop a multi player game and setup an AI tournament for developers and AI researchers to compete with each other.

The Arena is implemented in C++, but is very flexible. Agents can be implemented in any programming language. They interact with the environment via stdout.  
An important feature of the environment is that after each action, every agent or 'bot' will receive a reward. This way bots can learn and improve as they play, just like in a classical reinforcement learning setup.  
Furthermore, instead of playing one episode or 'game' at a time. The environment lets bots play any number of consecutive games. Each bot can remember its state and what it has learned, to allow it to improve game after game.

This adds a new aspect to the challenge. Instead of simply seeing who is best in a single game，the meta-challenge is to see which bot is the fastest learning and fastest to adapt to its opponents.

See if you can defeat the random bot in the [example game](./environment/chickenpoker/README.md) that is already implemented. Or better, derive from the code to create your own multi player AI challenge.


### Credits

This project is inspired by [Halite](https://halite.io/index.php) which was a particularly fun AI programming challenge. Indeed some of the code in the NetworkAgent.cpp is derived from [that project](https://github.com/HaliteChallenge/Halite).
