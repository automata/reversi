#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "util.h"
#include "cell.h"

// Parse command line arguments and read configuration and game files.
// If not given in the command line, appropriate defaults are used.
std::pair<std::string, std::string> parse_arguments(int argc, char *argv[])
{
    std::string conf_name = default_conf_name;
    std::string game_name = default_game_name;
    int current_arg = 1;
    while (current_arg < argc) {
        if (argv[current_arg][0] == '-' 
            && strlen(argv[current_arg]) == 2
            && current_arg + 1 < argc) {
            switch (argv[current_arg][1]) {
            case 'c':
                conf_name = argv[current_arg + 1];
                break;
            case 'g':
                game_name = argv[current_arg + 1];
                break;
            default:
                std::cerr << "Invalid option " << argv[current_arg] 
                          << std::endl;
                usage_and_exit(argv[0]);
            }
            current_arg += 2;
        }
        else {
            std::cerr << "Bad parameter list starting at " 
                      << argv[current_arg] << std::endl;
            usage_and_exit(argv[0]);
        }
    }
    return std::make_pair(conf_name, game_name);
}

// Print usage and exit.
void usage_and_exit(std::string command)
{
    std::cerr << "Usage: "
              << command << " [-c configfile] [-g gamefile]\n";
    exit(1);
}

// Read board size from the configuration file named in conf_name
int get_board_size(std::string conf_name)
{
    std::ifstream conf_file(conf_name.c_str());
    int size;
    conf_file >> size;
    if (!conf_file.good()) {
        std::cerr << "Configuration file " << conf_name << " invalid!\n";
        exit(1);
    }
    return size;
}

// Read all game moves from the game file named game_namd.
std::vector<Move> read_moves(std::string game_name)
{
    std::vector<Move> moves;
    std::ifstream game_file(game_name.c_str());
    if (!game_file.good()) {
        std::cerr << "Bad game file " << game_name << std::endl;
        exit(1);
    }
    int line = 1;
    game_file >> std::ws;
    while (!game_file.eof()) {
        std::string player_str;
        Cell_state player;
        game_file >> player_str;
        std::transform(player_str.begin(), player_str.end(), 
                       player_str.begin(), tolower);
        if (!game_file.good() || 
            (player_str != "black" && player_str != "white")) {
            std::cerr << "Error in file " << game_name
                      << " line " << line << std::endl;
            exit(1);
        }
        if (player_str == "black") {
            player = black;
        }
        else {
            player = white;
        }
        int r, c;
        std::string next_line;
        std::getline(game_file, next_line);
        std::istringstream line_str(next_line);
        line_str >> r >> std::ws;
        if (!line_str.good()) {
            std::cerr << "Error in file " << game_name 
                      << " line " << line << std::endl;
            exit(1);
        }
        line_str >> c >> std::ws;
        if (!line_str.eof()) {
            std::cerr << "Error in file " << game_name 
                      << " line " << line << std::endl;
            exit(1);
        }
        moves.push_back(Move(player, Cell(r, c)));
        line++;
        game_file >> std::ws;
    }
    return moves;
}

// Print a message showing an error in the game file and exit.
// message is the user message, 
// move is the move associated with the error,
// i is the place where move comes in the game (starting from 0),
// board is the current game board
void move_error(std::string message, Move const &move, int n, 
                Board const &board)
{
    std::cerr << message << std::endl;
    std::cerr << "Regarding move number " << n << " (from zero) of player ";
    if (move.player() == black) {
        std::cerr << "black";
    }
    else {
        std::cerr << "white";
    }
    std::cerr << " at position (" 
              << move.position().row() << ", " 
              << move.position().col() 
              << ") in the following board.\n";
    std::cerr << board;
    exit(1);
}
