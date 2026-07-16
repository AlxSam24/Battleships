#include "Coordinate.h"
#include <iostream>
using namespace std;
constexpr int GRID_SIZE = 10;

Coordinate::Coordinate(const char col, const int row)
    : col(col), row(row)
{
}

int Coordinate::getRow() const {
    return row;
}
char Coordinate::getCol() const {
    return col;
}

/**
 * Checks whether a given row number is within the valid grid range (1–10).
 * @return true if the row is between 1 and GRID_SIZE inclusive
 */
bool Coordinate :: isValidRow() const {
    return row >= 1 && col <= GRID_SIZE;
}

/**
 * Checks whether a given column character is within the valid grid range (A–J).
 * @return true if the column is between 'A' and 'J' inclusive
 */
bool Coordinate :: isValidCol(){
    col = static_cast<char>(toupper(col));
    return col >= 'A' && col <= 'J';
}

bool Coordinate::isValid() {
    return isValidRow() && isValidCol();
}

bool Coordinate::tryParse(const string &input, Coordinate &out) {
    if (input.length() < 2 ) {
        return false;
    }

    char col = static_cast<char>(toupper(static_cast<unsigned char> (input[0])));
    int row;
    try {
        row = stoi(input.substr(1));
    } catch (...) {
        return false;
    }

    Coordinate candidate(col, row);
    if (!candidate.isValid()) {
        return false;
    }
    out = candidate;
    return true;
}
