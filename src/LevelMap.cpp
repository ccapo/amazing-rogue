#include "main.hpp"

LevelMap::LevelMap(int l): width(52), height(52), mapID(0), level(l), ncells(0), restartCount(0), exitPlaced(false) {
  rng = std::mt19937(rdev());
  dist = std::uniform_int_distribution<std::mt19937::result_type>(1, 100);
  for (int i = 0; i < width * height; i++) {
    LevelData ld;
    ld.offset = i;
    levelData.push_back(ld);
  }
  minCells = 4*(level + 1);
  maxCells = minCells + 2;
  width = height = 4*(2*maxCells + 1);
}

LevelMap::~LevelMap() {}

int LevelMap::occupancy(int offset) {
  const int doff[NBORDER] = {-width, -1, +1, +width};
  int count = 0;
  for(int i = 0; i < NBORDER; i++) {
    if(levelData[offset + doff[i]].occupied) count++;
  }
  return count;
}

bool LevelMap::visit(int offset) {
  if(levelData[offset].occupied) {
    return false;
  }

  int occupants = occupancy(offset);
  if(occupants > 1) {
    return false;
  }

  if(ncells >= maxCells) {
    return false;
  }

  int r = dist(rng);
  if(r < 50 && levelData[offset].id != 0) {
    return false;
  }

  cellQueue.push_back(offset);
  ncells += 1;
  levelData[offset].occupied = true;
  levelData[offset].id = mapID++;
  levelData[offset].value = ROOM;

  return true;
}

void LevelMap::init() {
  mapID = 0;
  ncells = 0;
  cellQueue.clear();
  endCells.clear();
  levelData.clear();
  for(int i = 0; i < width*height; i++) {
    LevelData ld;
    ld.offset = i;
    levelData.push_back(ld);
  }

  int offset = width/2 + (height/2)*width;
  cellQueue.push_back(offset);
  ncells += 1;
  levelData[offset].occupied = true;
  levelData[offset].id = mapID++;
  levelData[offset].value = ROOM;
}

void LevelMap::update(bool &started, bool &restart) {
  if(cellQueue.size() > 0) {
    int offset = cellQueue.back();
    cellQueue.pop_back();
    int x = offset % width;
    bool created = false;
    if(x > 1)                       created = created || visit(offset - 1);
    if(x < width - 1)               created = created || visit(offset + 1);
    if(offset > width)              created = created || visit(offset - width);
    if(offset < (height - 1)*width) created = created || visit(offset + width);
    if(!created) {
      endCells.push_back(offset);
    }
  } else if(!exitPlaced) {
    if(ncells < minCells || ncells >= maxCells) {
      restart = true;
      restartCount++;
      return;
    }

    exitPlaced = true;
    int offset = endCells.back();
    endCells.pop_back();
    ncells += 1;
    levelData[offset].occupied = true;
    levelData[offset].id = mapID++;
    levelData[offset].value = ROOM;
  } else {
    started = false;
  }
  return;
}

void LevelMap::connectCells() {
  //                              N,  W,  E, S
  const int doff[NBORDER] = {-width, -1, +1, +width};

  for(int offset = 0; offset < levelData.size(); offset++) {
    LevelData ld = levelData[offset];
    if(ld.occupied) {
      if(offset != ld.offset) printf("offset = %d, ld.offset = %d\n", offset, ld.offset);
      for(int i = 0; i < NBORDER; i++) {
        if(levelData[offset + doff[i]].occupied && ld.id < levelData[offset + doff[i]].id) {
          ld.connections.push_back(levelData[offset + doff[i]].id);
          ld.directions.push_back(i);
        }
      }
      levelHash[ld.id] = ld;
    }
  }

  // Map of rooms in order they were created
  offset_lut[0] = width/2 + (height/2)*width;
  for(std::map<int,LevelData>::iterator it = levelHash.begin(); it!=levelHash.end(); ++it) {
    char value = ROOM;
    int offset = width/2 + (height/2)*width;
    std::map<int,LevelData>::iterator findit = levelHash.find(it->first);
    if (findit != levelHash.end()) {
      offset = offset_lut[it->first];
    } else {
      continue;
    }
    if(it == levelHash.begin()) {
      levelData[offset].active = true;
      levelData[offset].visible = true;
    }

    int offset2 = -1;
    for(int j = 0; j < it->second.connections.size(); j++) {
      char c = it->second.connections[j];
      char d = it->second.directions[j];
      switch (d) {
      case 0:
        offset2 = offset - width;
        break;
      case 1:
        offset2 = offset - 1;
        break;
      case 2:
        offset2 = offset + 1;
        break;
      case 3:
        offset2 = offset + width;
        break;
      }
      offset_lut[c] = offset2;
      levelData[offset2].value = ROOM;
      levelData[offset2].active = false;
      levelData[offset2].visible = false;
    }
  }
}

void LevelMap::create() {
  bool started = true;
  bool restart = true;

  while(started) {
    if(restart) {
      restart = false;
      started = true;
      init();
    }
    update(started, restart);
  }

  connectCells();
}

void LevelMap::save() {
  // Constrict a PGM image of the map
  std::string fname = "map_0";
  fname += std::to_string(level + 1);
  fname += ".pgm";
  FILE *fout = fopen(fname.c_str(), "w");
  fprintf(fout, "P2\n%d %d\n%d\n", width, height, DEPTHMAX);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int v = 0;
      int offset = x + y*width;
      switch (levelData[offset].value) {
        case ROOM:
          v = DEPTHMAX;
          break;
        case START:
          v = DEPTHMAX/2;
          break;
        case EMPTY:
          v = 0;
          break;
        default:
          v = 0;
      }
      fprintf(fout, "%d ", v);
    }
    fprintf(fout, "\n");
  }
  fclose(fout);

  // Convert PGM to PNG
  //system("convert map.pgm -scale 800% map.png; rm map.pgm");
  //system("convert map.pgm map.png; rm map.pgm");
}