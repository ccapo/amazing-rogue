#include "libtcod.hpp"

class Actor;
class Map;

class Engine {
public :
    TCODList<Actor *> actors;
    Actor *player;
    Map *map;
    Gui *gui;
    bool computeFov;
    int fovRadius;
    int screenWidth;
    int screenHeight;
    TCOD_mouse_t mouse;
    TCOD_key_t lastKey;
 
    Engine(int screenWidth, int screenHeight);
    ~Engine();
    void init();
    void term();
    void load();
    void update();
    void render();
    void sendToBack(Actor *actor);
    Actor *getClosestMonster(int x, int y, float range) const;
    Actor *getActor(int x, int y) const;

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