#ifndef MAP_GEN_HPP
#define MAP_GEN_HPP

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

// MapData struct
struct MapData {
  char value;
  bool occupied, visible;
  int id;
  std::vector<int> connections;
  std::vector<int> directions;
  MapData(): value(EMPTY), occupied(false), visible(false), id(-1) {}
};

class MapGen {
  std::random_device rdev;
  std::mt19937 rng;
  std::uniform_int_distribution<std::mt19937::result_type> dist;

  bool exitPlaced;
  int width, height, mapID, minCells, maxCells, level;
  int ncells, restartCount;
  std::vector<MapData> mapData;
  std::deque<int> cellQueue;
  std::deque<int> endCells;

  int occupancy(int offset);
  bool visit(int offset);
  void init();
  void update(bool &started, bool &restart);
  void connect_cells();
  void display();
public:
  MapGen(int w, int h, int l);
  ~MapGen();

  void create();
  void save();
};

#endif