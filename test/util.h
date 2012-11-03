#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <utility>
#include <vector>
#include "move.h"
#include "board.h"

std::string const default_conf_name = "reversi.conf";
std::string const default_game_name = "game.txt";

// Parse command line arguments and read configuration and game files.
// If not given in the command line, appropriate defaults are used.
std::pair<std::string, std::string> parse_arguments(int argc, char *argv[]);

// Print usage and exit.
void usage_and_exit(std::string command);

// Read board size from the configuration file named in conf_name
int get_board_size(std::string conf_name);

// Read all game moves from the game file named game_namd.
std::vector<Move> read_moves(std::string game_name);

// Print a message showing an error in the game file and exit.
// message is the user message, 
// move is the move associated with the error,
// i is the place where move comes in the game (starting from 0),
// board is the current game board
void move_error(std::string message, Move const &move, int n, 
                Board const &board);

#endif /* _UTIL_H_ */
