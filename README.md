# Chicken Poker

Chicken poker is game that can be played with any number of players.

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

## How to play

The game environment is called chickp and has the following options and arguments.

chickp [-q] [-sets s] [-games g] -n n player1_cmd ...
-n n  The number of cards each player is dealt. This is also the value of the highest card.
-q    don't show intermidate game output
The options g and s are mutually exclusive
-games g Run g number of games. Player ids are rotated after every game.
-sets  s A set consists p games where p is the number of players. Run g = s*p number of games.

