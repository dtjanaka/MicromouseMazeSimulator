#ifndef SIMPLEFF_H
#define SIMPLEFF_H

#include "PathFinder.h"

class SimpleFF : public PathFinder {
public:
	SimpleFF(bool shouldPause = false);
	~SimpleFF();
    MouseMovement nextMovement(unsigned x, unsigned y, const Maze & maze);
private:
    const bool pause;
    bool shouldGoForward;
    bool visitedStart;
    int heading;
    struct Coord {
        int x;
        int y;
    };
    int distances[16][16]{ 0 };
    bool walls[16][16][4]{ false };
    void calculateDistances();
    bool isAtCenter(unsigned x, unsigned y) const;
};

#endif //SIMPLEFF_H