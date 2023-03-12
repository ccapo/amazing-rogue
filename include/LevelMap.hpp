#ifndef LEVEL_MAP_HPP
#define LEVEL_MAP_HPP

#include <iostream>
#include <vector>
#include <map>
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
  bool occupied, visible, active;
  int id, type, offset;
  std::vector<int> connections;
  std::vector<int> directions;
  LevelData(): value(EMPTY), occupied(false), visible(false), active(false), id(-1), type(0), offset(-1) {}
};

class LevelMap {
  std::random_device rdev;
  std::mt19937 rng;
  std::uniform_int_distribution<std::mt19937::result_type> dist;

  bool exitPlaced;
  int mapID, minCells, maxCells, level;
  int ncells, restartCount;
  std::deque<int> cellQueue;
  std::deque<int> endCells;

  int occupancy(int offset);
  bool visit(int offset);
  void init();
  void update(bool &started, bool &restart);
  void makeBossLevel();
  void connectCells();
public:
  LevelMap(int l);
  ~LevelMap();

  void create();
  void save();

  int width, height;
  std::map<int,int> offset_lut;
  std::vector<LevelData> levelData;
  std::map<int, LevelData> levelHash;
};

#endif