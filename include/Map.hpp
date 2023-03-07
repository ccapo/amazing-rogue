struct Tile {
   bool explored; // has the player already seen this tile ?
   bool exit;
   float scent;
   Tile() : explored(false), scent(0.0) {}
};

class Map {
public :
   int width,height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   bool isInFov(int x, int y) const;
   bool isExplored(int x, int y) const;
   bool canWalk(int x, int y) const;
   void computeFov();
   void render() const;

   void init(int type, bool withObjects);
protected :
   Tile *tiles;
   TCODMap *map;
   long seed;
   TCODRandom *rng;

   void makeRoom(int type);
   void createRoom(bool first, int x1, int y1, int x2, int y2, bool withObjects);
   void addMonster(int x, int y);
   void addItem(int x, int y);
};