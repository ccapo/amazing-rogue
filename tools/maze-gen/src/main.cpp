#include "maze.hpp"

int main(int argc, char *argv[]) {
  int width, height;

  if(argc < 3) {
    std::cerr << "Usage: " << argv[0] << " width height" << std::endl;
    exit(1);
  }

  /* Get width and height */
  width = atoi(argv[1]);
  height = atoi(argv[2]);

  /* Width and height must be positive */
  if(width < 0 || height < 0) {
    std::cerr << "Error: Width and height must be positive" << std::endl;
    exit(1);
  }

  /* Width and height must be odd */
  if (width % 2 == 0) {
    std::cout << "Warning: Width is even, increasing by one (" << width << " => " << ++width << ")" << std::endl;
  }
  if (height % 2 == 0) {
    std::cout << "Warning: Height is even, increasing by one (" << height << " => " << ++height << ")" << std::endl;
  }

  // Create a maze object
  Maze m(width, height);

  // Generate the maze
  m.createMaze();

  // Render maze to the terminal
  m.displayMaze();

  // Save maze to a file
  m.saveMaze();

  return 0;
}