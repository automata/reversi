#include <string>
#include <utility>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>

#include "util.h"
#include "board.h"

// Cell numbering if from the bottom left to the top right

int main(int argc, char *argv[])
{
    // Decide what configure file and game file to use.
    // Either from the defaults or from command line.
    std::string conf_name, game_name;
    std::pair<std::string, std::string> names;
    names = parse_arguments(argc, argv);
    conf_name = names.first;
    game_name = names.second;

    // Feedback to the user.
    std::cout << "Verifying computed reversi game with:" << std::endl;
    std::cout << "  Configure file: " << conf_name << std::endl;
    std::cout << "  Game file: " << game_name << std::endl << std::endl;

    // Read configure file and generate the board.
    int board_size = get_board_size(conf_name);

    Board board(board_size);

    std::vector<Move> moves = read_moves(game_name);

    Cell_state last_player = white; // Last player to verify skipping.
    std::vector<Move>::size_type i;
    try {
        for (i = 0; i < moves.size(); i++) {
            // If a player was skipped, verify validity.
            if (last_player == moves[i].player()) {
                Cell_state other_player = last_player == black ? white : black;
                // Look all cells for a valid move of other_player
                for (int r = 0; r < board_size; r++) {
                    for (int c = 0; c < board_size; c++) {
                        Cell current_cell(r, c);
                        Move other_move(other_player, current_cell);
                        if (board.is_valid(other_move)) {
                            // Found a valid move: error!
                            move_error("A player with valid move was skipped",
                                       other_move, i, board);
                        }
                    }
                }
            }
            // Player is ok. Verify move validity and update board.
            board.play(moves[i]);
            last_player = moves[i].player();
        }
    }
    catch (Invalid_move_exception e) {
        // An invalid move was tried.
        move_error("You tried an invalid move.",
                   e.move(), i, e.board());
    }
    catch (std::out_of_range) {
        // Cell out of the board accessed.
        move_error("Illegal cell.", moves[i], i, board);
    }

    // File ended. Verify if there are any valid moves left.
    for (int r = 0; r < board_size; r++) {
        for (int c = 0; c < board_size; c++) {
            Cell current_cell = Cell(r, c);
            Move possible_move_black(black, current_cell);
            Move possible_move_white(white, current_cell);
            if (board.is_valid(possible_move_black)) {
                move_error("Your game file ended prematurely (valid moves left)",
                          possible_move_black, i, board);
            }
            if (board.is_valid(possible_move_white)) {
                move_error("Your game file ended prematurely (valid moves left)",
                          possible_move_white, i, board);
            }
        }
    }

    std::cout << "Your moves are all legal. Congratulations!\n";
    return 0;
}

