#include <stdio.h>
#include "main.hpp"
#include <SDL2/SDL.h>

Engine::Engine(int screenWidth, int screenHeight): gameStatus(STARTUP), renderMode(DEFAULT), fovRadius(10), screenWidth(screenWidth),screenHeight(screenHeight) {
    TCODConsole::setCustomFont("data/fonts/arial10x10.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE, 32, 8);
    TCODConsole::initRoot(screenWidth,screenHeight,"libtcod C++ tutorial",false);
    gui = new Gui();
}

Engine::~Engine() {
    term();
    delete gui;
}

void Engine::init() {
    player = new Actor(40,25,'@',"player",TCODColor::black);
    player->destructible=new PlayerDestructible(30,2,"your cadaver");
    player->attacker=new Attacker(5);
    player->ai = new PlayerAi();
    player->container = new Container(26);
    actors.push(player);
    map = new Map(80,43);

    gui->message(TCODColor::red, "Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");
    gameStatus=STARTUP;
}

void Engine::term() {
    actors.clearAndDelete();
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
        for (Actor **iterator=actors.begin(); iterator != actors.end();
            iterator++) {
            Actor *actor=*iterator;
            if ( actor != player ) {
                actor->update();
            }
        }
    }
}

void Engine::render() {
    TCODConsole::root->clear();
    // draw the map
    map->render();
    // draw the actors
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
        Actor *actor=*iterator;
        if ( map->isInFov(actor->x,actor->y) ) {
            actor->render();
        }
    }
    player->render();
    // show the player's stats
    gui->render();
}

void Engine::sendToBack(Actor *actor) {
    actors.remove(actor);
    actors.insertBefore(actor,0);
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
    Actor *closest=NULL;
    float bestDistance=1E6f;
    for (Actor **iterator=actors.begin();
        iterator != actors.end(); iterator++) {
        Actor *actor=*iterator;
        if ( actor != player && actor->destructible 
            && !actor->destructible->isDead() ) {
            float distance=actor->getDistance(x,y);
            if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
                bestDistance=distance;
                closest=actor;
            }
        }
    }
    return closest;
}

Actor *Engine::getActor(int x, int y) const {
    for (Actor **iterator=actors.begin();
        iterator != actors.end(); iterator++) {
        Actor *actor=*iterator;
        if ( actor->x == x && actor->y ==y && actor->destructible
            && ! actor->destructible->isDead()) {
            return actor;
        }
    }
    return NULL;
}