#include <math.h>
#include "main.hpp"

PlayerAi::PlayerAi() : xpLevel(1) {}

const int LEVEL_UP_BASE=200;
const int LEVEL_UP_FACTOR=150;

int PlayerAi::getNextLevelXp() {
    return LEVEL_UP_BASE + xpLevel*LEVEL_UP_FACTOR;
}

// how many turns the monster chases the player
// after losing his sight
static const int TRACKING_TURNS=3;

void PlayerAi::update(Object *owner) {
    int levelUpXp = getNextLevelXp();
    if ( owner->entity->xp >= levelUpXp ) {
        xpLevel++;
        owner->entity->xp -= levelUpXp;
        engine.gui->message(TCODColor::yellow,"Your battle skills grow stronger! You reached level %d",xpLevel);
        engine.gui->menu.clear();
        engine.gui->menu.addItem(Menu::CONSTITUTION,"Constitution (+20HP)");
        engine.gui->menu.addItem(Menu::STRENGTH,"Strength (+1 attack)");
        engine.gui->menu.addItem(Menu::AGILITY,"Agility (+1 defense)");
        Menu::MenuItemCode menuItem=engine.gui->menu.pick(Menu::PAUSE);
        switch (menuItem) {
            case Menu::CONSTITUTION :
                owner->entity->maxHp+=20;
                owner->entity->hp+=20;
                break;
            case Menu::STRENGTH :
                owner->entity->atk += 1;
                break;
            case Menu::AGILITY :
                owner->entity->def += 1;
                break;
            default:break;
        }
    }

    if ( owner->entity && owner->entity->isDead() ) {
        return;
    }

    if (engine.renderMode == Engine::TARGET) {
        if ( engine.map->isInFov(engine.mouse.cx,engine.mouse.cy) ) {
            if ( engine.mouse.lbutton_pressed ) {
                if ( owner->entity->ai->castable ) {
                    owner->entity->ai->castable->item->cast(engine.mouse.cx, engine.mouse.cy);
                    delete owner->entity->ai->castable;
                }
                engine.gameStatus=Engine::NEW_TURN;
                engine.renderMode=Engine::DEFAULT;
            }
        }
    }

    int dx=0,dy=0;
    switch(engine.lastKey.vk) {
    case TCODK_UP : dy=-1; break;
    case TCODK_DOWN : dy=1; break;
    case TCODK_LEFT : dx=-1; break;
    case TCODK_RIGHT : dx=1; break;
    case TCODK_ENTER: {
        if ( engine.stairs->x == owner->x && engine.stairs->y == owner->y ) {
            engine.nextLevel();
        } else {
            engine.gui->message(TCODColor::lightGrey,"There are no stairs here.");
        }
    }
    case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c); break;
        default:break;
    }

    if (dx != 0 || dy != 0) {
        engine.gameStatus=Engine::NEW_TURN;
        if (engine.map->isExit(owner->x+dx,owner->y+dy)) {
            printf("Exiting Room\n");
        }
        if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
            engine.map->computeFov();
        }
    }
}

bool PlayerAi::moveOrAttack(Object *owner, int targetx,int targety) {
    if ( engine.map->isWall(targetx,targety) ) return false;
    // look for living objects to attack
    for (Object **iterator=engine.objects.begin();
        iterator != engine.objects.end(); iterator++) {
        Object *object=*iterator;
        if ( object->entity && !object->entity->isDead()
             && object->x == targetx && object->y == targety ) {
            owner->entity->attack(owner, object);
            return false;
        }
    }

    // look for corpses or items
    for (Object **iterator=engine.objects.begin();
        iterator != engine.objects.end(); iterator++) {
        Object *object=*iterator;
        bool corpseOrItem=(object->entity && object->entity->isDead())
            || object->item;
        if ( corpseOrItem && object->entity && object->entity->isDead()
             && object->x == targetx && object->y == targety ) {
            engine.gui->message(TCODColor::lightGrey,"There's a %s here",object->name);
        }
    }

    owner->x=targetx;
    owner->y=targety;
    return true;
}

void PlayerAi::handleActionKey(Object *owner, int ascii) {
    static bool bTarget = false;
    switch(ascii) {
        case 'd' : // drop item 
        {
            Object *object=choseFromInventory(owner);
            if ( object ) {
                object->item->drop(object,owner);
                engine.gameStatus=Engine::NEW_TURN;
            }           
        }
        break;
        case 'g' : // pickup item
        {
            bool found=false;
            for (Object **iterator=engine.objects.begin();
                iterator != engine.objects.end(); iterator++) {
                Object *object=*iterator;
                if ( object->item && object->x == owner->x && object->y == owner->y ) {
                    if (object->item->pick(object,owner)) {
                        found=true;
                        engine.gui->message(TCODColor::lightGrey,"You pick the %s.",
                            object->name);
                        break;
                    } else if (!found) {
                        found=true;
                        engine.gui->message(TCODColor::red,"Your inventory is full.");
                    }
                }
            }
            if (!found) {
                engine.gui->message(TCODColor::lightGrey,"There's nothing here that you can pick.");
            }
            engine.gameStatus=Engine::NEW_TURN;
        }
        break;
        case 'i' : // display inventory
        {
            Object *object=choseFromInventory(owner);
            if ( object ) {
                object->item->use(object,owner);
                engine.gameStatus=Engine::NEW_TURN;
            }
        }
        break;
    }
}

Object *PlayerAi::choseFromInventory(Object *owner) {
    static const int INVENTORY_WIDTH=50;
    static const int INVENTORY_HEIGHT=28;
    static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);

    // display the inventory frame
    con.setDefaultForeground(TCODColor(200,180,50));
    con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,true,
        TCOD_BKGND_DEFAULT,"inventory");

    // display the items with their keyboard shortcut
    con.setDefaultForeground(TCODColor::white);
    int shortcut='a';
    int y=1;
    for (Object **it=owner->container->inventory.begin();
        it != owner->container->inventory.end(); it++) {
        Object *object=*it;
        con.print(2,y,"(%c) %s", shortcut, object->name);
        y++;
        shortcut++;
    }

    // blit the inventory console on the root console
    TCODConsole::blit(&con, 0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,
        TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2,
        engine.screenHeight/2-INVENTORY_HEIGHT/2);
    TCODConsole::flush();

    // wait for a key press
    TCOD_key_t key;
    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
    if ( key.vk == TCODK_CHAR ) {
        int objectIndex=key.c - 'a';
        if ( objectIndex >= 0 && objectIndex < owner->container->inventory.size() ) {
            return owner->container->inventory.get(objectIndex);
        }
    }
    return NULL;
}

void MonsterAi::update(Object *owner) {
    if ( owner->entity && owner->entity->isDead() ) {
        return;
    }
    if ( engine.map->isInFov(owner->x,owner->y) ) {
        // we can see the player. move towards him
        moveCount=TRACKING_TURNS;
    } else {
        moveCount--;
    }
    if ( moveCount > 0 ) {
        moveOrAttack(owner, engine.player->x,engine.player->y);
    }
}

void MonsterAi::moveOrAttack(Object *owner, int targetx, int targety) {
    int dx = targetx - owner->x;
    int dy = targety - owner->y;
    int stepdx = (dx > 0 ? 1:-1);
    int stepdy = (dy > 0 ? 1:-1);
    float distance=sqrtf(dx*dx+dy*dy);
    if ( distance >= 2 ) {
        dx = (int)(round(dx/distance));
        dy = (int)(round(dy/distance));
        if ( engine.map->canWalk(owner->x+dx,owner->y+dy) ) {
            owner->x += dx;
            owner->y += dy;
        } else if ( engine.map->canWalk(owner->x+stepdx,owner->y) ) {
            owner->x += stepdx;
        } else if ( engine.map->canWalk(owner->x,owner->y+stepdy) ) {
            owner->y += stepdy;
        }
    } else if ( owner->entity ) {
        owner->entity->attack(owner,engine.player);
    }
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi): nbTurns(nbTurns),oldAi(oldAi) {}

void ConfusedMonsterAi::update(Object *owner) {
    TCODRandom *rng=TCODRandom::getInstance();
    int dx=rng->getInt(-1,1);
    int dy=rng->getInt(-1,1);
    if ( dx != 0 || dy != 0 ) {
        int destx=owner->x+dx;
        int desty=owner->y+dy;
        if ( engine.map->canWalk(destx, desty) ) {
            owner->x = destx;
            owner->y = desty;
        } else {
            Object *object=engine.getObject(destx, desty);
            if ( object ) {
                owner->entity->attack(owner, object);
            }
        }
    }
    nbTurns--;
    if ( nbTurns == 0 ) {
        owner->entity->ai = oldAi;
        delete this;
    }
}