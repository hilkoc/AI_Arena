""" Chicken Poker random bot. """

import logging
logging.basicConfig(filename='chp_bot.log', level=logging.DEBUG, filemode='w')
log = logging.getLogger(__name__)
log.info("Starting")


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
    myID = int(get_string())
    max_bet = int(get_string())
    return (myID, max_bet)


def send_init(name):
    send_string(name)


def get_frame(frame):
    return deserialize_round(frame)


def send_frame(bet):
    send_string(bet)


#
# Bot
#
import random

class Bot(object):
    def __init__(self, name):
        self.name = name
        self.id = None
        self.max_bet = None
        self.bets = None

    def reset(self):
        """ Starts a new episode """
        self.id, self.max_bet = get_init()
        log.info("myID, max_bet = %s %s", str(self.id), str(self.max_bet))

        self.bets = list(range(1, 1 + self.max_bet))
        random.shuffle(self.bets)

        log.info("send_init(%s)", self.name)
        send_init(self.name)


    def get_bet(self):
        """ Calculate the next bet to play """
        return self.bets.pop()

    def play_round(self, frame):
        last_round = get_frame(frame)
        log.debug("last_round %s", last_round)
        bet = self.get_bet()
        log.debug("send_frame %s", str(bet))
        send_frame(str(bet))


def main():
    bot = Bot("MyBot")
    while True:
        try:
            frame = get_string()
            if "RESET" == frame:
                bot.reset()
            else:
                bot.play_round(frame)
        except Exception as e:
            print(e)
            log.exception(e)
            break



if __name__ == "__main__":
    main()

