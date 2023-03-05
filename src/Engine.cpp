#include <stdio.h>
#include "main.hpp"
#include <SDL2/SDL.h>

Engine::Engine(int screenWidth, int screenHeight): gameStatus(STARTUP), renderMode(DEFAULT), fovRadius(10), screenWidth(screenWidth),screenHeight(screenHeight),level(1) {
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
    player->entity = new PlayerEntity(30,5,2,"your cadaver");
    player->entity->ai = new PlayerAi();
    player->container = new Container(12);
    objects.push(player);
    stairs = new Object(0,0,'>',"stairs",TCODColor::white);
    stairs->blocks=false;
    objects.push(stairs);
    // TODO: Put in a separate thread
    for (int l = 0; l < 1; l++) {
        LevelMap *lm = new LevelMap(l);
        lm->create();
#ifdef DEBUG
        lm->save();
#endif
        levelMaps.push(lm);
    }
    map = new Map(86,46);
    map->init(true);
    gui->message(TCODColor::red, "Welcome to Amazing Rogue!\nPrepare to perish in the Labyrinth of the Minotaur.");
    gameStatus=STARTUP;
}

void Engine::term() {
    objects.clearAndDelete();
    levelMaps.clearAndDelete();
    if ( map ) delete map;
    gui->clear();
}

void Engine::load() {
    TCODConsole::credits();
    gui->menu.clear();
    gui->menu.addItem(Menu::NEW_GAME,"New Game");
    gui->menu.addItem(Menu::EXIT,"Exit");

    Menu::MenuItemCode menuItem=engine.gui->menu.pick();
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
    gameStatus=IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    player->update();
    if ( gameStatus == NEW_TURN ) {
        for (Object **iterator=objects.begin(); iterator != objects.end(); iterator++) {
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
    map->render();
    // draw the objects
    for (Object **iterator=objects.begin(); iterator != objects.end(); iterator++) {
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
    Object *closest=NULL;
    float bestDistance=1E6f;
    for (Object **iterator=objects.begin();
        iterator != objects.end(); iterator++) {
        Object *object = *iterator;
        if ( object != player && object->entity 
            && !object->entity->isDead() ) {
            float distance=object->getDistance(x,y);
            if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
                bestDistance=distance;
                closest=object;
            }
        }
    }
    return closest;
}

Object *Engine::getObject(int x, int y) const {
    for (Object **iterator=objects.begin();
        iterator != objects.end(); iterator++) {
        Object *object=*iterator;
        if ( object->x == x && object->y ==y && object->entity
            && ! object->entity->isDead()) {
            return object;
        }
    }
    return NULL;
}

void Engine::nextLevel() {
    level++;
    gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
    player->entity->heal(player->entity->maxHp/2);
    gui->message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");
    delete map;
    // delete all objects but player and stairs
    for (Object **it=objects.begin(); it!=objects.end(); it++) {
        if ( *it != player && *it != stairs ) {
            delete *it;
            it = objects.remove(it);
        }
    }
    // create a new map
    map = new Map(86,46);
    map->init(true);
    gameStatus=STARTUP; 
}