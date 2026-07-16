//
// Created by alexa on 16/07/2026.
//

#ifndef BATTLESHIPS_SHIP_H
#define BATTLESHIPS_SHIP_H
#include <string>
#include <iostream>
using namespace std;


class Ship {
public:
    Ship(string name, int length, char symbol);
    void registerHit();
    bool isSunk() const;
    int length() const;
    char symbol() const;
    string name() const;
private:
    string name_;
    int length_;
    char symbol_;
    int hitsTaken =0;
};


#endif //BATTLESHIPS_SHIP_H