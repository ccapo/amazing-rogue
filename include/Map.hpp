struct Tile {
   bool explored; // has the player already seen this tile ?
   bool exit;
   float scent, scentPrev;
   Tile() : explored(false), scent(0.0), scentPrev(0.0) {}
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
   float getScent(int x, int y) const;
   void moveDisplay(int x, int y);
   void computeFov();
   void render() const;

   void init(int type, bool reset, bool withObjects);
protected :
   Tile *tiles;
   TCODMap *map;
   long seed;
   TCODRandom *rng;

   void makeRoom(int type, bool reset = false);
   void addMonster(int x, int y);
   void addItem(int x, int y);
};