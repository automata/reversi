#ifndef _MOVE_H_
#define _MOVE_H_

#include "cell.h"

// Represents a move, with player and cell.
class Move
{

    Cell_state _player;
    Cell _position;

public:

    Move(Cell_state p, Cell const &c)
        : _player(p), _position(c)
    {
    }
    
    Cell_state player() const
    {
        return _player;
    }

    Cell position() const
    {
        return _position;
    }
};


#endif /* _MOVE_H_ */
