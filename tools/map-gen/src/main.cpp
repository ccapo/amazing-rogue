#include "map-gen.hpp"

int main(int argc, char *argv[]) {
  bool save_images = false;
  if (argc > 1) {
    save_images = true;
  }

  for(int level = 0; level < 8; level++) {
    int width, height;
    int nrooms = 4*(level + 1) + 2;
    width = height = 4*(2*nrooms + 1);

    // Create a maze object
    MapGen map(width, height, level);

    // Generate the Map
    map.create();

    // Save Map to a PNG file
    if(save_images) map.save();
  }

  return 0;
}