#include "maze.hpp"

Maze::Maze(int n, int m): nx(n), ny(m) {
  width = 2 * nx + 1;
  height = 2 * ny + 1;

  for (int i = 0; i < width * height; i++) {
    maze.push_back(WALL);
  }

  for (int y = 1; y < height; y += 2) {
    for (int x = 1; x < width; x += 2) {
      int offset = x + y*width;
      maze[offset] = FLOOR;
    }
  }
}

Maze::~Maze() {}

void Maze::displayMaze() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int offset = x + y*width;
      std::cout << maze[offset];
    }
    std::cout << std::endl;
  }
}

void Maze::createMaze() {
  std::vector<std::pair<int, std::pair<int, int> > > cell_list;
  std::vector<bool> visited(nx*ny, false);
  std::stack<std::pair<int, std::pair<int, int> > > m_stack;
  std::random_device rdev;
  std::mt19937 rng(rdev());
  std::uniform_int_distribution<std::mt19937::result_type> dist100(1, 100);

  int nVisited = 0;
  int k = 0;

  for (int y = 1; y < height; y += 2) {
    for (int x = 1; x < width; x += 2) {
      cell_list.push_back(make_pair(k, std::make_pair(y, x)));
      k++;
    }
  }

  int randIdx = dist100(rng) % nx*ny;
  m_stack.push(cell_list[randIdx]);
  visited[randIdx] = true;
  nVisited++;

  // Algo
  while(nVisited < nx*ny) {
    std::vector<int> neighbours;
    int y = m_stack.top().second.first;
    int x = m_stack.top().second.second;
    int offset;
    // North
    if (y > 1) {
      offset = (x + 0) + (y - 2)*width;
      if (maze[offset] && 
        !visited[getIdx(y - 2, x + 0, cell_list)]) {
        neighbours.push_back(0);
      }
    }
    // East
    if (x < width - 2) {
      offset = (x + 2) + (y + 0)*width;
      if (maze[offset] && 
        !visited[getIdx(y + 0, x + 2, cell_list)]) { 
        neighbours.push_back(1);
      }
    }
    // South
    if (y < height - 2) {
      offset = (x + 0) + (y + 2)*width;
      if (maze[offset] && 
        !visited[getIdx(y + 2, x + 0, cell_list)]) {
        neighbours.push_back(2);
      }
    }
    // West
    if (x > 1) {
      offset = (x - 2) + (y + 0)*width;
      if (maze[offset] && 
        !visited[getIdx(y + 0, x - 2, cell_list)]) {
        neighbours.push_back(3);
      }
    }

    // Neighbours available?
    if (!neighbours.empty()) {
      // Choose a random direction
      int next_cell_dir = neighbours[dist100(rng) % neighbours.size()];
      // Create a path between this cell and neighbour
      switch (next_cell_dir) {
        case 0: // North
          offset = (x + 0) + (y - 1)*width;
          maze[offset] = FLOOR;
          m_stack.push(cell_list[getIdx(y - 2, x + 0, cell_list)]);
          break;
        case 1: // East
          offset = (x + 1) + (y + 0)*width;
          maze[offset] = FLOOR;
          m_stack.push(cell_list[getIdx(y + 0, x + 2, cell_list)]);
          break;
        case 2: // South
          offset = (x + 0) + (y + 1)*width;
          maze[offset] = FLOOR;
          m_stack.push(cell_list[getIdx(y + 2, x + 0, cell_list)]);                
          break;
        case 3: // West
          offset = (x - 1) + (y + 0)*width;
          maze[offset] = FLOOR;
          m_stack.push(cell_list[getIdx(y + 0, x - 2, cell_list)]);               
          break;
        default:
          std::cerr << "Invalid cell direction: " << next_cell_dir << std::endl;
      }

      visited[m_stack.top().first] = true;
      nVisited++;
    } else {
      m_stack.pop();
    }
  }

  // Store offsets of floor tiles, to easily generate an item or creature at random
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int offset = x + y*width;
      if (maze[offset] == FLOOR) floor_tiles.push_back(offset);
    }
  }

  // Exits
  int x = 1, y = 0;
  int offset = x + y*width;
  maze[offset] = PATH;
  x = 2 * nx - 1;
  y = 2 * ny;
  offset = x + y*width;
  maze[offset] = PATH;
}

void Maze::saveMaze() {
  // Constrict a PGM image of the map
  FILE *fout = fopen("maze.pgm", "w");
  fprintf(fout, "P2\n%d %d\n%d\n", width, height, DEPTHMAX);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int offset = x + y*width;
      int d = 0;
      switch (maze[offset]) {
        case WALL:
          d = DEPTHMAX;
          break;
        case PATH:
          d = DEPTHMAX/2;
          break;
        case FLOOR:
          d = 0;
          break;
        default:
          d = 0;
      }
      fprintf(fout, "%d ", d);
    }
    fprintf(fout, "\n");
  }
  fclose(fout);

  // Convert PGM to PNG
  system("convert maze.pgm -scale 400% maze.png; rm maze.pgm");
}

// A utility function to get the index of cell with indices x, y;
int Maze::getIdx(int y, int x, std::vector < std::pair<int, std::pair<int, int> > > cell_list) {   
  for (int i = 0; i < cell_list.size(); i++) {
    if (cell_list[i].second.first == y && cell_list[i].second.second == x) {
      return cell_list[i].first;
    }
  }
  std::cout << "getIdx() couldn't find the index!" << std::endl;
  return -1;
}