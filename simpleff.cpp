#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <windows.h>
using namespace std;

#include "simpleff.h"
#include "PathFinder.h"
#include "Maze.h"

void clearScreen();

SimpleFF::SimpleFF(bool shouldPause) : pause(shouldPause) {
  heading = 1;
}

SimpleFF::~SimpleFF() {

}

MouseMovement SimpleFF::nextMovement(unsigned x, unsigned y, const Maze& maze) {
  const bool frontWall = maze.wallInFront();
  const bool leftWall = maze.wallOnLeft();
  const bool rightWall = maze.wallOnRight();
  
  // Pause at each cell if the user requests it.
  // It allows for better viewing on command line.
  if (pause) {
    cout << "Hit enter to continue..." << endl;
    cin.ignore(10000, '\n');
    cin.clear();
  }

  clearScreen();
  cout << maze.draw(3) << endl << endl;
  //for (int i = 15; i >= 0; i--) {
  //    for (int j = 0; j < 16; j++)
  //        cout << distances[j][i] << "\t";
  //    cout << endl;
  //}
  //Sleep(100);
    
  // If we somehow miraculously hit the center
  // of the maze, just terminate and celebrate!
  if (isAtCenter(x, y)) {
    for (int i = 15; i >= 0; i--) {
      for (int j = 0; j < 16; j++)
        cout << distances[j][i] << "\t";
      cout << endl;
    }

    for (int i = 15; i >= 0; i--) {
      for (int j = 0; j < 16; j++)
        cout << "[" << walls[j][i][0] << " " << walls[j][i][1] << " " << walls[j][i][2] << " " << walls[j][i][3] << "]" << " ";
      cout << endl;
    }
    cout << "Success!" << std::endl;
    return Finish;
  }
  
  switch (heading) {
  case 0:
    walls[x][y][0] = frontWall;
    walls[x][y][3] = leftWall;
    walls[x][y][1] = rightWall;
    if (x > 0) walls[x - 1][y][2] = frontWall;
    if (y > 0) walls[x][y - 1][1] = leftWall;
    if (y < 15) walls[x][y + 1][3] = rightWall;
    break;
  case 1:
    walls[x][y][1] = frontWall;
    walls[x][y][0] = leftWall;
    walls[x][y][2] = rightWall;
    if (y < 15) walls[x][y + 1][3] = frontWall;
    if (x > 0) walls[x - 1][y][2] = leftWall;
    if (x < 15) walls[x + 1][y][0] = rightWall;
    break;
  case 2:
    walls[x][y][2] = frontWall;
    walls[x][y][1] = leftWall;
    walls[x][y][3] = rightWall;
    if (x < 15) walls[x + 1][y][0] = frontWall;
    if (y < 15) walls[x][y + 1][3] = leftWall;
    if (y > 0) walls[x][y - 1][1] = rightWall;
    break;
  case 3:
    walls[x][y][3] = frontWall;
    walls[x][y][2] = leftWall;
    walls[x][y][0] = rightWall;
    if (y > 0) walls[x][y - 1][1] = frontWall;
    if (x < 15) walls[x + 1][y][0] = leftWall;
    if (x > 0) walls[x - 1][y][2] = rightWall;
    break;
  }

  calculateDistances();

  vector<int> neigh {
    { (x > 0) ? distances[x - 1][y] : INT_MAX },
    { (y < 15) ? distances[x][y + 1] : INT_MAX },
    { (x < 15) ? distances[x + 1][y] : INT_MAX },
    { (y > 0) ? distances[x][y - 1] : INT_MAX }
  };

  int togo = -1;
  for (int i = 0; i < 4; i++) {
    if (neigh[i] == distances[x][y] - 1 && !walls[x][y][i]) {
      togo = i;
      break;
    }
  }

  if (heading == togo) return MoveForward;
  if (abs(heading - togo) == 2) {
    heading = togo;
    return TurnAround; // turn around instead of move backward
  }
  if (heading - togo == 1 || heading - togo == -3) {
    heading = togo;
    return TurnCounterClockwise;
  }
  heading = togo;
  return TurnClockwise;
}

int SimpleFF::minAccNeighVal(unsigned x, unsigned y) {
  vector<int> neigh{
    { (x > 0) ? ((!walls[x][y][0]) ? distances[x - 1][y] : 256) : 256 },
    { (y < 15) ? ((!walls[x][y][1]) ? distances[x][y + 1] : 256) : 256 },
    { (x < 15) ? ((!walls[x][y][2]) ? distances[x + 1][y] : 256) : 256 },
    { (y > 0) ? ((!walls[x][y][3]) ? distances[x][y - 1] : 256) : 256 }
  };
  
  return *min_element(neigh.begin(), neigh.end());
}

void SimpleFF::calculateDistances() {
  bool visited[16][16]{ false };
  queue<Coord> tocalc;
  
  for (int i = 0; i < 16; i++)
    for (int j = 0; j < 16; j++)
      distances[i][j] = 256;

  tocalc.push(Coord(7, 7));
  tocalc.push(Coord(7, 8));
  tocalc.push(Coord(8, 7));
  tocalc.push(Coord(8, 8));
  visited[7][7] = visited[7][8] = visited[8][7] = visited[8][8] = true;
  distances[7][7] = distances[7][8] = distances[8][7] = distances[8][8] = 0;

  while (!tocalc.empty()) {
    Coord c{ tocalc.front() };
    tocalc.pop();
    visited[c.x][c.y] = true;

    //order: left, up, right, down
    if (c.x > 0 && !walls[c.x][c.y][0] && !visited[c.x - 1][c.y]) {
      tocalc.push(Coord{ c.x - 1, c.y }); 
      distances[c.x - 1][c.y] = minAccNeighVal(c.x - 1, c.y) + 1;  
    }
    if (c.y < 15 && !walls[c.x][c.y][1] && !visited[c.x][c.y + 1]) {
      tocalc.push(Coord{ c.x, c.y + 1 }); 
      distances[c.x][c.y + 1] = minAccNeighVal(c.x, c.y + 1) + 1;
    }
    if (c.x < 15 && !walls[c.x][c.y][2] && !visited[c.x + 1][c.y]) {
      tocalc.push(Coord{ c.x + 1, c.y });
      distances[c.x + 1][c.y] = minAccNeighVal(c.x + 1, c.y) + 1;
    }
    if (c.y > 0 && !walls[c.x][c.y][3] && !visited[c.x][c.y - 1]) {
      tocalc.push(Coord{ c.x, c.y - 1 });
      distances[c.x][c.y - 1] = minAccNeighVal(c.x, c.y - 1) + 1;
    }
  }
}

bool SimpleFF::isAtCenter(unsigned x, unsigned y) const {
  unsigned midpoint = MazeDefinitions::MAZE_LEN / 2;

  if (MazeDefinitions::MAZE_LEN % 2 != 0) {
    return x == midpoint && y == midpoint;
  }

  return (x == midpoint && y == midpoint) ||
         (x == midpoint - 1 && y == midpoint) ||
         (x == midpoint && y == midpoint - 1) ||
         (x == midpoint - 1 && y == midpoint - 1);
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++/g31 UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  COORD upperLeft = { 0, 0 };
  DWORD dwCharsWritten;
  FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
    &dwCharsWritten);
  SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
  static const char* term = getenv("TERM");
  if (term == nullptr || strcmp(term, "dumb") == 0)
    cout << endl;
  else
  {
    static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
    cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
  }
}

#endif