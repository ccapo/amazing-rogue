#include "libtcod.hpp"

class Object;
class Map;

class Engine {
public :
    TCODList<Object *> objects;
    Object *player;
    Object *stairs;
    Map *map;
    Gui *gui;
    bool computeFov;
    int fovRadius;
    int screenWidth;
    int screenHeight;
    int level;
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
    void sendToBack(Object *object);
    Object *getClosestMonster(int x, int y, float range) const;
    Object *getObject(int x, int y) const;

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