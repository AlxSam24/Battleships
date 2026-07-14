#ifndef BATTLESHIPS_PLAYER_H
#define BATTLESHIPS_PLAYER_H


class Player {
public:
    virtual ~Player() = default;
    virtual void placeShips() = 0;
    virtual Coordinate chooseTarget(const Grid& opponentTracking) = 0;
    virtual void notifyResult (Coordinate target, bool hit, Ship* sunkShip) {}
    Grid& ownGrid();
    Grid& trackingGrid();
protected:
    Grid ownGrid_;
    Grid trackingGrid_;
    Fleet fleet_;
};

#endif