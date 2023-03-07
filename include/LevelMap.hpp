#ifndef LEVEL_MAP_HPP
#define LEVEL_MAP_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <random>

#define ROOM     ('#')
#define START    ('x')
#define EMPTY    (' ')
#define DEPTHMAX (256)
#define NBORDER    (4)
#define NBOX       (9)

// LevelData struct
struct LevelData {
  char value;
  bool occupied, visible;
  int id, type;
  std::vector<int> connections;
  std::vector<int> directions;
  LevelData(): value(EMPTY), occupied(false), visible(false), id(-1) {}
};

class LevelMap {
  std::random_device rdev;
  std::mt19937 rng;
  std::uniform_int_distribution<std::mt19937::result_type> dist;

  bool exitPlaced;
  int width, height, mapID, minCells, maxCells, level;
  int ncells, restartCount;
  std::vector<LevelData> levelData;
  std::deque<int> cellQueue;
  std::deque<int> endCells;

  int occupancy(int offset);
  bool visit(int offset);
  void init();
  void update(bool &started, bool &restart);
  void connect_cells();
  void display();
public:
  LevelMap(int l);
  ~LevelMap();

  void create();
  void save();

  std::unordered_map<int, LevelData> levelHash;
};

#endif