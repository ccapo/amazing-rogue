#include "libtcod.hpp"

#define SCREEN_WIDTH  (86)
#define SCREEN_HEIGHT (53)
#define MAP_WIDTH (SCREEN_WIDTH)
#define MAP_HEIGHT (SCREEN_HEIGHT - PANEL_HEIGHT)
#define LEVELMAX (2)

class Object;
class Map;
class LevelMap;

class Engine {
public :
    TCODList<Object *> objects;
    Object *player;
    Object *stairs;
    TCODList<LevelMap *> levelMaps;
    TCODList<Object *> exits;
    Map *map;
    Gui *gui;
    bool computeFov;
    int fovRadius;
    int screenWidth;
    int screenHeight;
    int level, roomID;
    TCOD_mouse_t mouse;
    TCOD_key_t lastKey;
 
    Engine(int screenWidth, int screenHeight);
    ~Engine();
    void init();
    void term();
    void load();
    void update();
    void render();
    void nextLevel();
    void nextRoom(int destID, int type, bool reset);
    void sendToBack(Object *object);
    Object *getClosestMonster(int x, int y, float range) const;
    Object *getObject(int x, int y) const;
    Object *getExit(int x, int y) const;

    enum GameStatus {
        STARTUP,
        IDLE,
        NEW_TURN,
        VICTORY,
        DEFEAT
    } gameStatus;

    enum RenderMode {
        DEFAULT,
        TARGET
    } renderMode;
};

extern Engine engine;