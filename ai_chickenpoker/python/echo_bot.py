""" Chicken Poker echo bot. """

import logging
logging.basicConfig(filename='echo_bot.log', level=logging.WARN, filemode='w', format='%(levelname)s %(message)s')
log = logging.getLogger(__name__)
log.info("Starting")


#  
# Networking
#
import sys

def deserialize_round(round_string):
    """ param: round_string is expected to be list of integers separated by spaces.
        The integers are interpreted as pairs (agent_id, bet). """
    log.debug("round_string %s", round_string)
    last_round = dict()
    parts = round_string.strip().split(" ")
    if len(parts) > 1:  # Before the first round we receive an empty string
        for i in range(0, len(parts), 2):
            last_round[int(parts[i])] = int(parts[i+1])
    return last_round


def send_string(toBeSent):
    toBeSent += '\n'
    sys.stdout.write(toBeSent)
    sys.stdout.flush()


def get_string():
    return sys.stdin.readline().rstrip('\n')


def get_init():
    myID = int(get_string())
    max_bet = int(get_string())
    player_ids = [int(pid) for pid in get_string().strip().split(" ")]
    return (myID, max_bet, player_ids)


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
        self.player_ids = None
        self.bets = dict()  # Keep track of available bets that players can make.

    def reset(self):
        """ Starts a new episode """
        self.id, self.max_bet, self.player_ids = get_init()
        log.info("myID %s, max_bet %s, pids %s", str(self.id), str(self.max_bet), str(self.player_ids))

        self.bets.clear()
        for pid in self.player_ids:
            self.bets[pid] = list(range(1, 1 + self.max_bet))
        random.shuffle(self.bets[self.id])

        log.info("send_init(%s)", self.name)
        send_init(self.name)


    def get_bet(self, last_round):
        """ Calculate the next bet to play """
        log.debug("Mybets are %s", str(self.bets[self.id]))
        mybet = None
        for pid, b in last_round.items():
            log.debug("for id %d , removing (%d)", pid, b)
            self.bets[pid].remove(b)
            if (not pid == self.id) and (mybet is None):
                mybet = b

        log.debug("mybet %s, mybets %s, if %s", mybet, self.bets[self.id], (not mybet in self.bets[self.id]))
        if not mybet in self.bets[self.id]:
            log.debug("playing random (%s)", mybet)
            mybet = self.bets[self.id][0]  # Simply play the first

        return mybet

    def play_round(self, frame):
        last_round = get_frame(frame)
        log.debug("last_round %s", str(last_round))
        bet = self.get_bet(last_round)
        log.debug("send_frame %s", str(bet))
        send_frame(str(bet))


def main():
    bot = Bot("EchoBot")
    while True:
        try:
            frame = get_string()
            if "RESET" == frame:
                log.info("Reset")
                bot.reset()
            else:
                bot.play_round(frame)
        except Exception as e:
            print(e)
            log.exception(e)
            break



if __name__ == "__main__":
    main()

