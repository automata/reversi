#ifndef _CELL_H_
#define _CELL_H_

enum Cell_state { empty, black, white };

// Represents a cell in the board
class Cell
{

    int _row, _column;

public:

    Cell(int r, int c)
        : _row(r), _column(c)
    {
    }

    // Row of the cell.
    int row() const 
    { 
        return _row; 
    }
    
    // Column of the cell.
    int col() const 
    { 
        return _column; 
    }

};


#endif /* _CELL_H_ */
