import json
import copy  # use it for deepcopy if needed
import math  # for math.inf
import logging

logging.basicConfig(format='%(levelname)s - %(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                    level=logging.INFO)

# Global variables in which you need to store player strategies (this is data structure that'll be used for evaluation)
# Mapping from histories (str) to probability distribution over actions
strategy_dict_x = {}
strategy_dict_o = {}
x_ct = o_ct = 0


class History:
    def __init__(self, history=None):
        """
        # self.history : Eg: [0, 4, 2, 5]
            keeps track of sequence of actions played since the beginning of the game.
            Each action is an integer between 0-8 representing the square in which the move will be played as shown
            below.
              ___ ___ ____
             |_0_|_1_|_2_|
             |_3_|_4_|_5_|
             |_6_|_7_|_8_|

        # self.board
            empty squares are represented using '0' and occupied squares are either 'x' or 'o'.
            Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
            for board
              ___ ___ ____
             |_x_|___|_x_|
             |___|_o_|_o_|
             |___|___|___|

        # self.player: 'x' or 'o'
            Player whose turn it is at the current history/board

        :param history: list keeps track of sequence of actions played since the beginning of the game.
        """
        if history is not None:
            self.history = history
            self.board = self.get_board()
        else:
            self.history = []
            self.board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        self.player = self.current_player()

    def current_player(self):
        """ Player function
        Get player whose turn it is at the current history/board
        :return: 'x' or 'o' or None
        """
        total_num_moves = len(self.history)
        if total_num_moves < 9:
            if total_num_moves % 2 == 0:
                return 'x'
            else:
                return 'o'
        else:
            return None

    def get_board(self):
        """ Play out the current self.history and get the board corresponding to the history in self.board.

        :return: list Eg: ['x', '0', 'x', '0', 'o', 'o', '0', '0', '0']
        """
        board = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
        for i in range(len(self.history)):
            if i % 2 == 0:
                board[self.history[i]] = 'x'
            else:
                board[self.history[i]] = 'o'
        return board

    def is_win(self):
        # check if the board position is a win for either players
        # Feel free to implement this in anyway if needed
        board = self.get_board()
        for i in range(3):
            if board[3*i]=='o' and board[3*i+1]=='o' and board[3*i+2]=='o':
                return 'o'
            if board[3*i]=='x' and board[3*i+1]=='x' and board[3*i+2]=='x':
                return 'x'
            if board[i]=='o' and board[i+3]=='o' and board[i+6] == 'o':
                return 'o'
            if board[i]=='x' and board[i+3]=='x' and board[i+6] == 'x':
                return 'x'
        if board[0]=='o' and board[0+4] == 'o' and board[0+4*2]=='o':
            return 'o'
        if board[2]=='o' and board[2+2] == 'o' and board[2+2*2]=='o':
            return 'o'
        if board[0]=='x' and board[0+4] == 'x' and board[0+4*2]=='x':
            return 'x'
        if board[2]=='x' and board[2+2] == 'x' and board[2+2*2]=='x':
            return 'x'
        # if not ('0' in board):
        #     return '0'
        return False

    def is_draw(self):
        # check if the board position is a draw
        # Feel free to implement this in anyway if needed
        board = self.get_board()
        if not ('0' in board):
            return '0'
        return False

    def get_valid_actions(self):
        # get the empty squares from the board
        # Feel free to implement this in anyway if needed
        valid_actions = []
        for i in range(9):
            if not(i in self.history):
                valid_actions.append(i)
        return valid_actions

    def is_terminal_history(self):
        # check if the history is a terminal history
        # Feel free to implement this in anyway if needed
        if not(self.is_win() or self.is_draw()):
            return False
        else:
            return True

    def get_utility_given_terminal_history(self):
        # Feel free to implement this in anyway if needed
        winner = self.is_win()
        if winner:
            if winner == 'o':
                return -1
            elif winner == 'x':
                return 1
        elif self.is_draw():
            return 0
        pass

    def update_history(self, action):
        # In case you need to create a deepcopy and update the history obj to get the next history object.
        # Feel free to implement this in anyway if needed
        new_history = History((self.history + [action]))
        return new_history


def backward_induction(history_obj):
    """
    :param history_obj: Histroy class object
    :return: best achievable utility (float) for th current history_obj
    """
    global strategy_dict_x, strategy_dict_o, x_ct, o_ct
    # TODO implement
    # (1) Implement backward induction for tictactoe
    # (2) Update the global variables strategy_dict_x or strategy_dict_o which are a mapping from histories to
    # probability distribution over actions.
    # (2a)These are dictionary with keys as string representation of the history list e.g. if the history list of the
    # history_obj is [0, 4, 2, 5], then the key is "0425". Each value is in turn a dictionary with keys as actions 0-8
    # (str "0", "1", ..., "8") and each value of this dictionary is a float (representing the probability of
    # choosing that action). Example: {”0452”: {”0”: 0, ”1”: 0, ”2”: 0, ”3”: 0, ”4”: 0, ”5”: 0, ”6”: 1, ”7”: 0, ”8”:
    # 0}}
    # (2b) Note, the strategy for each history in strategy_dict_x and strategy_dict_o is probability distribution over
    # actions. But since tictactoe is a PIEFG, there always exists an optimal deterministic strategy (SPNE). So your
    # policy will be something like this {"0": 1, "1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0} where
    # "0" was the one of the best actions for the current player/history.
    
    if history_obj.is_terminal_history():
        return history_obj.get_utility_given_terminal_history()
    
    if history_obj.player == "x":
        bestUtil = -math.inf
        bestAction = None
        for i in history_obj.get_valid_actions():
            x_ct+=1
            childUtil = backward_induction(history_obj.update_history(i))
            if childUtil>bestUtil:
                bestUtil = childUtil
                bestAction = i
        key = ''
        for a in history_obj.history:
            key += str(a)
        strategy_dict_x[key] = dict()
        for j in range(9):
            if j == bestAction:
                strategy_dict_x[key][str(j)] = 1
            else:
                strategy_dict_x[key][str(j)] = 0
    
    elif history_obj.player == "o":
        bestUtil = math.inf
        bestAction = None
        for i in history_obj.get_valid_actions():
            o_ct+=1
            childUtil = backward_induction(history_obj.update_history(i))
            if childUtil<bestUtil:
                bestUtil = childUtil
                bestAction = i
        key = ''
        for a in history_obj.history:
            key += str(a)
        strategy_dict_o[key] = dict()
        for j in range(9):
            if j == bestAction:
                strategy_dict_o[key][str(j)] = 1
            else:
                strategy_dict_o[key][str(j)] = 0

    return bestUtil
    # TODO implement
    


def solve_tictactoe():
    backward_induction(History())
    with open('./policy_x.json', 'w') as f:
        json.dump(strategy_dict_x, f)
    with open('./policy_o.json', 'w') as f:
        json.dump(strategy_dict_o, f)
    return strategy_dict_x, strategy_dict_o


if __name__ == "__main__":
    logging.info("Start")
    solve_tictactoe()
    logging.info("End") 
    print(x_ct, o_ct)
