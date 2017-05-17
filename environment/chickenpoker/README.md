# Chicken Poker

Chicken poker is game that can be played with any number of players. The objective for the players is to find out and adapt to each others strategy. To that end, typically multiple games are played before declaring the final winner.

The environment comes with a built-in random bot. Can you find a strategy to defeat the random bot?

## Rules of the game

At the start of the game, each player is dealt n cards with the values 1 to n and is assigned a player id.
Each round every player selects one card from its remaining cards to play.
The player that shows the highest card wins that round
and adds the values of all the cards in that round to its total score.

### Ties
If two or more players show the same card and this happens to be the highest card in that round,
the tie is broken as follows. The value of the highest card is v and this card is shown
by k players. In ascending order of player id, the k players are numbered 0 to k.
The player whos number is equal to the remainder of v divided by k, wins the round.

After each game, player ids are rotated, such that the inital order of the players does not cause statistical advantage or disadvantage.

## How to play

An example of running the game environment could look like this:

```bash
./chickp -n 10 -s 50   "random" "python3 ../ai_chickenpoker/python/chp_bot.py" ```
This will play a session of games with two players. The first is the built-in random bot,
the second is an external bot invoked with the given python command.
This will start 50 sets, which for two players means a 100 games. Each game consists of n rounds.

The -s 50 option is equivalent to -g 100. Using the -s option is a convenient way to ensure that each player has an equal number of games in each position to avoid a (dis)advantage in the long term.

The game environment is called chickp and has the following options and arguments.

```
USAGE:

   ./chickp  {-g <number>|-s <number>} -n <number> [-q] [-v] [-d] [-t] [--]
             [--version] [-h] <cmds in double quotes> ...


Where:

   -g <number>,  --games <number>
     (OR required)  Run g number of games. Player ids are rotated after
     every game.
         -- OR --
   -s <number>,  --sets <number>
     (OR required)  A set consists of p games where p is the number of
     players. Run g = s*p number of games.


   -n <number>,  --bets <number>
     (required)  The number of rounds in the game. This is also the value
     of the highest bet.

   -q,  --quiet
     Don't show intermediate game output.

   -v,  --verbose
     Show output for each round.

   -d,  --debug
     Show all output, including debug.

   -t,  --timeout
     Ignore timeouts. Gives all bots unlimited time.

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <cmds in double quotes>  (accepted multiple times)
     Start commands for bots.


   Chicken Poker Game Environment
```
