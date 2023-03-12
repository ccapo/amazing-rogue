#include <stdio.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include "main.hpp"
#include <SDL2/SDL.h>

Engine::Engine(int screenWidth, int screenHeight): gameStatus(STARTUP), renderMode(DEFAULT), fovRadius(10), screenWidth(screenWidth),screenHeight(screenHeight),level(0),roomID(0) {
    TCODConsole::setCustomFont("data/fonts/arial10x10.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE, 32, 8);
    TCODConsole::initRoot(screenWidth,screenHeight,"Amazing Rogue",false);
    gui = new Gui();
}

Engine::~Engine() {
    term();
    delete gui;
}

void Engine::init() {
    player = new Object(0,0,'@',"player",TCODColor::black);
    player->entity = new PlayerEntity(25,5,2,2,1,"your cadaver");
    player->entity->ai = new PlayerAi();
    player->container = new Container(12);
    objects.push(player);
    stairs = new Object(0,0,'>',"stairs",TCODColor::white);
    stairs->blocks = false;
    objects.push(stairs);

    // Create first level map
    LevelMap *lm = new LevelMap(0);
    lm->create();
#ifdef DEBUG
    lm->save();
#endif
    levelMaps.push(lm);

    // Launch a thread for generateLevelMaps
    std::thread levelMapsThread(&generateLevelMaps, std::ref(levelMapsReady));

    // Detach thread so that it does not block
    levelMapsThread.detach();

    for(int i = 0; i < levelMaps.get(level)->levelHash[roomID].connections.size(); i++) {
        int c = levelMaps.get(level)->levelHash[roomID].connections[i];
        int d = levelMaps.get(level)->levelHash[roomID].directions[i];
        int xe, ye;
        switch (d) {
        case 0:
            // North
            xe = screenWidth/2;
            ye = (screenHeight - PANEL_HEIGHT)/4;
            break;
        case 1:
            // West
            xe = screenWidth/4;
            ye = (screenHeight - PANEL_HEIGHT)/2;
            break;
        case 2:
            // East
            xe = 3*screenWidth/4;
            ye = (screenHeight - PANEL_HEIGHT)/2;
            break;
        case 3:
            // South
            xe = screenWidth/2;
            ye = 3*(screenHeight - PANEL_HEIGHT)/4;
            break;
        }
        Object *exit = new Object(xe, ye, ' ', "exit", TCODColor::white);
        exit->blocks = false;
        exit->connectedID = c;
        exits.push(exit);
    }
    map = new Map(screenWidth, screenHeight - PANEL_HEIGHT);
    map->init(0, false, true);
    gui->message(TCODColor::red, "Welcome to Amazing Rogue!\nPrepare to perish in the Labyrinth of the Minotaur.");
    gameStatus = STARTUP;
}

void Engine::term() {
    objects.clearAndDelete();
    exits.clearAndDelete();
    levelMaps.clearAndDelete();
    if ( map ) delete map;
    gui->clear();
}

void Engine::load() {
    //TCODConsole::credits();
    gui->menu.clear();
    gui->menu.addItem(Menu::NEW_GAME,"New Game");
    gui->menu.addItem(Menu::EXIT,"Exit");

    Menu::MenuItemCode menuItem = engine.gui->menu.pick();
    if ( menuItem == Menu::EXIT || menuItem == Menu::NONE ) {
        // Exit or window closed
        exit(0);
    } else if ( menuItem == Menu::NEW_GAME ) {
        // New game
        term();
        init();
    }
}

void Engine::update() {
    if ( gameStatus == STARTUP ) map->computeFov();
    gameStatus = IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    player->update();
    if ( gameStatus == NEW_TURN ) {
        for (Object **iterator = objects.begin(); iterator != objects.end(); iterator++) {
            Object *object = *iterator;
            if ( object != player ) {
                object->update();
            }
        }
    }
}

void Engine::render() {
    TCODConsole::root->clear();
    // draw the map
    //map->moveDisplay(player->x, player->y);
    map->render();
    // draw the objects
    for (Object **iterator = objects.begin(); iterator != objects.end(); iterator++) {
        Object *object = *iterator;
        if ( map->isInFov(object->x, object->y) ) {
            object->render();
        }
    }

    player->render();
    // show the player's stats
    gui->render();
}

void Engine::sendToBack(Object *object) {
    objects.remove(object);
    objects.insertBefore(object,0);
}

Object *Engine::getClosestMonster(int x, int y, float range) const {
    Object *closest = NULL;
    float bestDistance = 1E6f;
    for (Object **iterator = objects.begin();
        iterator != objects.end(); iterator++) {
        Object *object = *iterator;
        if ( object != player && object->entity && !object->entity->isDead() ) {
            float distance = object->getDistance(x,y);
            if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
                bestDistance = distance;
                closest = object;
            }
        }
    }
    return closest;
}

Object *Engine::getObject(int x, int y) const {
    for (Object **iterator = objects.begin();
        iterator != objects.end(); iterator++) {
        Object *object=*iterator;
        if ( object->x == x && object->y ==y && object->entity && ! object->entity->isDead()) {
            return object;
        }
    }
    return NULL;
}

Object *Engine::getExit(int x, int y) const {
    for (Object **iterator = exits.begin(); iterator != exits.end(); iterator++) {
        Object *exit = *iterator;
        if ( exit->x == x && exit->y == y && !exit->entity) {
            return exit;
        }
    }
    return NULL;
}

void Engine::nextLevel() {
    level++;
    if(level < LEVELMAX) {
        gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
        player->entity->heal(player->entity->hpmax/2);
        gui->message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

        nextRoom(0, 0, true);

        gameStatus = STARTUP;
    } else {
        // printf("Congratulations, you have reached the end of the game!\n");
        // exit(0);
        gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
        player->entity->heal(player->entity->hpmax/2);
        gui->message(TCODColor::red,"After a rare moment of peace, you descend\nto the deepest part of the dungeon...");

        nextRoom(0, 2, true);

        gameStatus = STARTUP;
    }
}

void Engine::nextRoom(int destID, int type, bool reset) {
    int prevRoomID = roomID;
    roomID = destID;

    delete map;
    // delete all objects but player and stairs
    for (Object **it = objects.begin(); it != objects.end(); it++) {
        if ( *it != player && *it != stairs ) {
            delete *it;
            it = objects.remove(it);
        }
    }
    exits.clearAndDelete();


    if(level < LEVELMAX) {
        if(levelMaps.get(level)->levelHash[roomID].connections.size() == 0) {
            if(type == 0) {
                stairs->x = screenWidth/2;
                stairs->y = 5*(screenHeight - PANEL_HEIGHT)/8;
            } else {
                stairs->x = 3*screenWidth/4 - 2;
                stairs->y = 3*(screenHeight - PANEL_HEIGHT)/4 - 1;
            }
            int prevOffset = levelMaps.get(level)->levelHash[prevRoomID].offset;
            int offset = levelMaps.get(level)->levelHash[roomID].offset;
            levelMaps.get(level)->levelData[prevOffset].active = false;
            levelMaps.get(level)->levelData[offset].visible = true;
            levelMaps.get(level)->levelData[offset].active = true;
            map = new Map(screenWidth, screenHeight - PANEL_HEIGHT);
            map->init(type, reset, true);
            if (map->isWall(stairs->x, stairs->y)) {
                stairs->x = screenWidth/2;
                stairs->y = (screenHeight - PANEL_HEIGHT)/2;
            }
            gameStatus = STARTUP;
            return;
        } else {
            stairs->x = -1;
            stairs->y = -1;
        }
    } else {
        stairs->x = -1;
        stairs->y = -1;
    }

    for(int i = 0; i < levelMaps.get(level)->levelHash[roomID].connections.size(); i++) {
        int c = levelMaps.get(level)->levelHash[roomID].connections[i];
        int d = levelMaps.get(level)->levelHash[roomID].directions[i];

        int xe, ye;
        switch (d) {
        case 0:
            // North
            if(type == 0 || type == 2) {
                xe = screenWidth/2;
                ye = (screenHeight - PANEL_HEIGHT)/4;
            } else {
                xe = 3*screenWidth/4 - 2;
                ye = 3*(screenHeight - PANEL_HEIGHT)/4 - 1;
            }
            break;
        case 1:
            // West
            if(type == 0 || type == 2) {
                xe = screenWidth/4;
                ye = (screenHeight - PANEL_HEIGHT)/2;
            } else {
                xe = 3*screenWidth/4 - 2;
                ye = 3*(screenHeight - PANEL_HEIGHT)/4 - 1;
            }
            break;
        case 2:
            // East
            if(type == 0 || type == 2) {
                xe = 3*screenWidth/4;
                ye = (screenHeight - PANEL_HEIGHT)/2;
            } else {
                xe = 3*screenWidth/4 - 2;
                ye = 3*(screenHeight - PANEL_HEIGHT)/4 - 1;
            }
            break;
        case 3:
            // South
            if(type == 0 || type == 2) {
                xe = screenWidth/2;
                ye = 3*(screenHeight - PANEL_HEIGHT)/4;
            } else {
                xe = 3*screenWidth/4 - 2;
                ye = 3*(screenHeight - PANEL_HEIGHT)/4 - 1;
            }
            break;
        }
        Object *exit = new Object(xe, ye, ' ', "exit", TCODColor::white);
        exit->blocks = false;
        exit->connectedID = c;
        exits.push(exit);
    }

    int prevOffset = levelMaps.get(level)->levelHash[prevRoomID].offset;
    int offset = levelMaps.get(level)->levelHash[roomID].offset;
    levelMaps.get(level)->levelData[prevOffset].active = false;
    levelMaps.get(level)->levelData[offset].visible = true;
    levelMaps.get(level)->levelData[offset].active = true;

    // create a new map
    map = new Map(screenWidth, screenHeight - PANEL_HEIGHT);
    map->init(type, reset, true);
    gameStatus = NEW_TURN;
}

// This function will be called from a thread
void Engine::generateLevelMaps(std::atomic<bool> &levelMapsReady) {
    printf("Generating other level maps\n");
    for (int l = 1; l <= LEVELMAX; l++) {
        LevelMap *lm = new LevelMap(l);
        lm->create();
#ifdef DEBUG
        lm->save();
#endif
        levelMaps.push(lm);
    }
    printf("Completed generating all other level maps!\n");
    levelMapsReady = true;
}

std::atomic<bool> Engine::levelMapsReady(false);
TCODList<LevelMap *> Engine::levelMaps;