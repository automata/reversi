#ifndef _BOARD_H_
#define _BOARD_H_

#include "cell.h"
#include "move.h"

// Represent the game board with its current state.
class Board
{

    int _size; // size of the board (_size X _size)
    Cell_state *_board; // board state
    // The 8 possible directions for flipping pieces.
    static int const direction[8][2];

    // Evaluate possible cells to flip.
    // player is the possible player,
    // start_cell is where player could play,
    // direction is the direction vector
    std::vector<Cell> flips_in_direction(Cell const &start_cell,
                                         Cell_state player, 
                                         int const direction[2]);

public:

    Board(int size);

    // Access board at cell c.
    Cell_state &operator[] (Cell const &c); 

    // Access board at cell c (for constant objects)
    Cell_state const &operator[] (Cell const &c) const; 

    // Size of the board
    int size() const 
    { 
        return _size; 
    }

    // Play move. Update board by changing the state of the corresponding
    // cell and flipping the appropriate cells.  Throws
    // Invalid_move_exception if the move is invalid.  
    // m is the move to play.
    void play(Move const &m);

    // Verifu a move is valid.
    // m is the move to verify at the this board.
    bool is_valid(Move const &m);

};

// Show board in an std::ostream.
std::ostream& operator<< (std::ostream &os, Board const &board);

// Exception class for invalid moves.
class Invalid_move_exception
{

    Move _m;
    Board _b;

public:

    Invalid_move_exception(Move m, Board b)
        : _m(m), _b(b)
    {
    }

    Move move() const
    {
        return _m;
    }

    Board board() const
    {
        return _b;
    }

};

#endif /* _BOARD_H_ */
