//
// Created by alexa on 14/07/2026.
//

#ifndef BATTLESHIPS_COORDINATE_H
#define BATTLESHIPS_COORDINATE_H
#include <string>
#include <ostream>
using namespace std;


class Coordinate {
public:
    Coordinate(char col, int row);
    bool isValidRow() const;
    bool isValidCol();
    bool isValid();
    int getRow() const;
    char getCol() const;
    static bool tryParse(const string& input, Coordinate& out);
    friend ostream& operator<<(ostream&, const Coordinate&);

private:
    char col;
    int row;
};


#endif //BATTLESHIPS_COORDINATE_H