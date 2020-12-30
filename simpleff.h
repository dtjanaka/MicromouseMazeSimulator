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
  int heading; // 0:W, 1:N, 2:E, 3:S
  struct Coord {
    int x;
    int y;
    Coord(int x, int y) : x{ x }, y{ y } {}
  };
  int distances[16][16]{ 0 };
  bool walls[16][16][4]{ false }; // [W N E S]
  int minAccNeighVal(unsigned x, unsigned y);
  void calculateDistances();
  bool isAtCenter(unsigned x, unsigned y) const;
};

#endif //SIMPLEFF_H