#include <math.h>
#include "main.hpp"

Object::Object(int x, int y, int ch, const char *name, const TCODColor &col) :
    x(x), y(y), ch(ch), colour(col), name(name), blocks(true), entity(NULL), item(NULL), container(NULL), connectedID(-1) {
}

Object::~Object() {
    if ( entity ) delete entity;
    if ( item ) delete item;
    if ( container ) delete container;
}

void Object::update() {
    if ( entity && entity->ai ) entity->ai->update(this);
}

void Object::render() const{
    TCODConsole::root->setChar(x, y, ch);
    TCODConsole::root->setCharForeground(x, y, colour);
}

float Object::getDistance(int cx, int cy) const {
    int dx = x - cx;
    int dy = y - cy;
    return sqrtf(dx*dx + dy*dy);
}