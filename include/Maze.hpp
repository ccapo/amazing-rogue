#ifndef MAZE_HPP
#define MAZE_HPP

#include <iostream>
#include <stack>
#include <vector>
#include <random>

#define WALL     ('#')
#define FLOOR    (' ')
#define PATH     ('o')
#define DEPTHMAX (256)

class Maze {
  int nx, ny;
  int width, height;
public:
  Maze(int n, int m);
  ~Maze();

  void displayMaze();
  void createMaze();
  int getIdx(int y, int x, std::vector < std::pair<int, std::pair<int, int> > > cell_list);
  void saveMaze();

  std::vector<char> maze_tiles;
  std::vector<int> floor_tiles;
};

#endif