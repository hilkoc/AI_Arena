""" Chicken Poker random bot. """

_myID = -1
_max_bet = -1

#  
# Networking
#
import sys

def deserialize_round(round_string):
    return round_string


def send_string(toBeSent):
    toBeSent += '\n'
    sys.stdout.write(toBeSent)
    sys.stdout.flush()


def get_string():
    return sys.stdin.readline().rstrip('\n')


def get_init():
    global _myID
    global _max_bet
    _myID = int(get_string())
    _max_bet = int(get_string())
    return (_myID, _max_bet)


def send_init(name):
    send_string(name)


def get_frame():
    return deserialize_round(get_string())


def send_frame(bet):
    send_string(bet)


#
# Bot
#
import random

myID, max_bet = get_init()

bets = list(range(max_bet))
random.shuffle(bets)

send_init("MyBot")



def get_bet():
    return 1 + bets.pop()


while True:
    try:
        last_round = get_frame()
        bet = get_bet()
        send_frame(str(bet))
    except Exception as e:
        print(e)
        break
