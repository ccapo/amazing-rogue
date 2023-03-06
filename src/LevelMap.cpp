#include "main.hpp"

LevelMap::LevelMap(int l): width(52), height(52), mapID(0), level(l), ncells(0), restartCount(0), exitPlaced(false) {
  rng = std::mt19937(rdev());
  dist = std::uniform_int_distribution<std::mt19937::result_type>(1, 100);
  LevelData md;
  for (int i = 0; i < width * height; i++) {
    levelData.push_back(md);
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
  LevelData md;
  for(int i = 0; i < width*height; i++) {
    levelData.push_back(md);
  }

  int offset = width/2 + (height/2)*width;
  cellQueue.push_back(offset);
  ncells += 1;
  levelData[offset].occupied = true;
  levelData[offset].id = mapID++;
  levelData[offset].value = START;
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
    levelData[offset].value = START;
  } else {
    started = false;
  }
  return;
}

void LevelMap::display() {
  // Display the layout
  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      int o = x + y*width;
      std::cout << levelData[o].value;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void LevelMap::connect_cells() {
  //                              N,  W,  E, S
  const int doff[NBORDER] = {-width, -1, +1, +width};
  const int db[NBOX] = {-width - 1, -width + 0, -width + 1, -1,  0, +1, +width - 1, +width + 0, +width + 1};
  std::unordered_map<int,int> offset_lut;

  for(int offset = 0; offset < levelData.size(); offset++) {
    LevelData md = levelData[offset];
    if(md.occupied) {
      for(int i = 0; i < NBORDER; i++) {
        if(levelData[offset + doff[i]].occupied && md.id < levelData[offset + doff[i]].id) {
          md.connections.push_back(levelData[offset + doff[i]].id);
          md.directions.push_back(i);
        }
      }
      levelHash[md.id] = md;
    }
  }

  levelData.clear();
  levelData.resize(width*height);
  LevelData md;
  for(int i = 0; i < width*height; i++) {
    levelData.push_back(md);
  }

  // Map of rooms in order they were created
  //printf("Level = %d\n", level + 1);
  offset_lut[0] = width/2 + (height/2)*width;
  for(std::unordered_map<int,LevelData>::iterator it=levelHash.begin(); it!=levelHash.end(); ++it) {
    char value = ROOM;
    int offset = width/2 + (height/2)*width;
    bool visible = true;
    std::unordered_map<int,LevelData>::iterator findit = levelHash.find(it->first);
    if (findit != levelHash.end()) {
      offset = offset_lut[it->first];
    } else {
      continue;
    }
    if(it == levelHash.begin() || std::next(it) == levelHash.end()) {
      value = START;
    }

    for(int j = 0; j < NBOX; j++) {
      levelData[offset + db[j]].value = value;
      levelData[offset + db[j]].visible = visible;
    }

    int cor, offset2;
    for(int j = 0; j < it->second.connections.size(); j++) {
      char c = it->second.connections[j];
      char d = it->second.directions[j];
      switch (d) {
      case 0:
        cor = offset - 2*width;
        offset2 = offset - 4*width;
        break;
      case 1:
        cor = offset - 2;
        offset2 = offset - 4;
        break;
      case 2:
        cor = offset + 2;
        offset2 = offset + 4;
        break;
      case 3:
        cor = offset + 2*width;
        offset2 = offset + 4*width;
        break;
      }
      offset_lut[c] = offset2;
      levelData[cor].value = ROOM;
      levelData[cor].visible = visible;
      for(int j = 0; j < NBOX; j++) {
        levelData[offset2 + db[j]].value = ROOM;
      }
    }

    // // Connection information
    // if(it->second.connections.size() > 0) {
    //   std::cout << "mapID = " << it->first << ", connections = ";
    // } else {
    //   std::cout << "mapID = " << it->first;
    // }
    // for(int j = 0; j < it->second.connections.size(); j++) {
    //   int c = it->second.connections[j];
    //   int d = it->second.directions[j];
    //   char dir = ' ';
    //   switch (d) {
    //   case 0:
    //     dir = 'N';
    //     break;
    //   case 1:
    //     dir = 'W';
    //     break;
    //   case 2:
    //     dir = 'E';
    //     break;
    //   case 3:
    //     dir = 'S';
    //     break;
    //   }
    //   std::cout << c << "(" << dir << ") ";
    // }
    // std::cout << std::endl;
  }
}

void LevelMap::create() {
  bool started = true;
  bool restart = true;

  //std::cout << "Level = " << level << ", # of rooms = " << minCells << " to " << maxCells << std::endl;

  while(started) {
    if(restart) {
      restart = false;
      started = true;
      init();
    }
    update(started, restart);
  }

  //std::cout << "# of Restarts = " << restartCount << std::endl;

  connect_cells();
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
      if(levelData[offset].visible) {
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