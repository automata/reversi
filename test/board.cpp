#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <vector>
#include "board.h"

// The 8 possible directions for flipping pieces.
int const Board::direction[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},
                                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

Board::Board(int size)
    : _size(size)
{
    // Board size must be even and greater than 4.
    if (size < 4 || size % 2 != 0) {
        std::cerr << "Bad board size " << size << std::endl;
        exit(1);
    }

    // Board represented as a linear array (row major).
    _board = new Cell_state[size * size];
    for (int i = 0; i < size * size; i++) {
            _board[i] = empty;
    }

    // Initial pieces
    _board[size/2*(size - 1) - 1] = black;
    _board[size/2*(size - 1)]     = white;
    _board[size/2*(size + 1) - 1] = white;
    _board[size/2*(size + 1)]     = black;
}

// Access board at cell c.
Cell_state &Board::operator[] (Cell const &c) 
{
    if (c.row() < 0 || c.row() >= _size ||
        c.col() < 0 || c.col() >= _size) {
        throw std::out_of_range("Invalid cell");
    }

    return _board[c.row() * _size + c.col()];
}

// Access board at cell c (for constant objects)
Cell_state const &Board::operator[] (Cell const &c) const 
{
    if (c.row() < 0 || c.row() >= _size ||
        c.col() < 0 || c.col() >= _size) {
        throw std::out_of_range("Invalid cell");
    }

    return _board[c.row() * _size + c.col()];
}

// Evaluate possible cells to flip.
// player is the possible player,
// start_cell is where player could play,
// direction is the direction vector
std::vector<Cell> Board::flips_in_direction(Cell const &start_cell,
                                            Cell_state player, 
                                            int const direction[2])
{
    std::vector<Cell> flip_candidates;

    // We flip cell of the other player
    Cell_state other_player = player == black ? white : black;

    // Start with the first cell from start_cell in the direction
    int current_i = start_cell.row() + direction[0];
    int current_j = start_cell.col() + direction[1];
    Cell current = Cell(current_i, current_j);
    // Add to flip_candidates while cell is valid and of other_player
    while (((0 <= current_i) && (current_i < _size)) &&
           ((0 <= current_j) && (current_j < _size)) &&
           (*this)[current] == other_player) {
        flip_candidates.push_back(current);
        current_i += direction[0];
        current_j += direction[1];
        current = Cell(current_i, current_j);
    }
    // Now we either extrapolated the board or found an empty cell or
    // a cell of player.
    if (!(((0 <= current_i) && (current_i < _size)) &&
          ((0 <= current_j) && (current_j < _size))) ||
        (*this)[current] != player) {
        // If we didn't found a cell of player, erase the candidates found.
        flip_candidates.erase(flip_candidates.begin(), flip_candidates.end());
    }

    return flip_candidates;
}

// Verifu a move is valid.
// m is the move to verify at the this board.
bool Board::is_valid(Move const &m)
{
    // We compute flip candidates in each direction.  If some
    // direction has flip candidates, the move is valid.
    Cell_state &move_position = (*this)[m.position()];
    if (move_position == empty) {
        for (int dir = 0; dir < 8; dir++) {
            std::vector<Cell> flip_candidates;
            flip_candidates = flips_in_direction(m.position(), m.player(),
                                                 direction[dir]);
            if (flip_candidates.size() > 0) {
                return true;
            }
        }
    }

    // Didn't return previously means that no flip candidates found.
    // Move invalid.
    return false;
}

// Play move. Update board by changing the state of the corresponding
// cell and flipping the appropriate cells.  Throws
// Invalid_move_exception if the move is invalid.  
// m is the move to play.
void Board::play(Move const &m)
{
    Cell_state &move_position = (*this)[m.position()];

    // The cell must be empty.
    if (move_position == empty) {
        // Cell is empty.  See if there are cells to flip.
        std::vector<Cell> to_flip;
        // Look all directions.
        for (int dir = 0; dir < 8; dir++) {
            // Compute candidates to flip in this direction.
            std::vector<Cell> flip_candidates;
            flip_candidates = flips_in_direction(m.position(), m.player(),
                                                 direction[dir]);
            // Add them to vector to_flip.
            std::copy(flip_candidates.begin(), flip_candidates.end(),
                      std::back_inserter(to_flip));
        }
        // If flip candidates found, move is valid, else it is invalid.
        if (to_flip.size() > 0) {
            // Valid move, change the boar appropriately.
            // Change position to player.
            move_position = m.player();
            // Flip appropriate cells.
            for (std::vector<Cell>::size_type i = 0; i < to_flip.size(); i++) {
                (*this)[to_flip[i]] = m.player();
            }
            return; // Return if legal move
        }
    }

    // No previous return: illegal move.
    throw Invalid_move_exception(m, *this);
}

// Show board in an std::ostream.
std::ostream& operator<< (std::ostream &os, Board const &board)
{
    for (int i = board.size() - 1; i >= 0; i--) {
        os << std::setw(3) << i << "|";
        for (int j = 0; j < board.size(); j++) {
            os << std::setw(2);
            switch (board[Cell(i,j)]) {
            case empty:
                os << ' ';
                break;
            case black:
                os << 'b';
                break;
            case white:
                os << 'w';
            }
            os << " |";
        }
        os << std::endl;
    }
    os << "   ";
    for (int j = 0; j < board.size(); j++)
        os << std::setw(4) << j;
    os << std::endl;

    return os;
}

