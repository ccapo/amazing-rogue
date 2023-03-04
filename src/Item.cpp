#include "main.hpp"

bool Item::pick(Object *owner, Object *wearer) {
    if ( wearer->container && wearer->container->add(owner) ) {
        engine.objects.remove(owner);
        return true;
    }
    return false;
}

bool Item::use(Object *owner, Object *wearer) {
    if ( wearer->container ) {
        wearer->container->remove(owner);
        delete owner;
        return true;
    }
    return false;
}

void Item::drop(Object *owner, Object *wearer) {
    if ( wearer->container ) {
        wearer->container->remove(owner);
        engine.objects.push(owner);
        owner->x=wearer->x;
        owner->y=wearer->y;
        engine.gui->message(TCODColor::lightGrey,"%s drops a %s.",wearer->name,owner->name);
    }
}

Healer::Healer(float amount): amount(amount) {}

bool Healer::use(Object *owner, Object *wearer) {
    if ( wearer->entity ) {
        float amountHealed = wearer->entity->heal(amount);
        if ( amountHealed > 0 ) {
            return Item::use(owner,wearer);
        }
    }
    return false;
}

bool Healer::cast(int x, int y) { return false; }

LightningBolt::LightningBolt(float range, float damage): range(range),damage(damage) {}

bool LightningBolt::use(Object *owner, Object *wearer) {
    Object *closestMonster=engine.getClosestMonster(wearer->x,wearer->y,range);
    if (! closestMonster ) {
        engine.gui->message(TCODColor::lightGrey,"No enemy is close enough to strike.");
        return false;
    }
    // hit closest monster for <damage> hit points
    engine.gui->message(TCODColor::lightBlue,
        "A lighting bolt strikes the %s with a loud thunder!\n"
        "The damage is %g hit points.",
        closestMonster->name,damage);
    closestMonster->entity->damage(closestMonster,damage);
    return Item::use(owner,wearer);
}

bool LightningBolt::cast(int x, int y) { return false; }

Fireball::Fireball(float range, float damage): range(range),damage(damage) {}

bool Fireball::use(Object *owner, Object *wearer) {
    engine.gui->message(TCODColor::cyan, "Left-click a target tile for the fireball.");

    engine.renderMode = Engine::TARGET;
    wearer->entity->ai->castable = new Object(*owner);
    wearer->entity->ai->castable->item = new Fireball(3,12);

    return Item::use(owner,wearer);
}

bool Fireball::cast(int x, int y) {
    // burn everything in <range> (including player)
    engine.gui->message(TCODColor::orange,"The fireball explodes, burning everything within %g tiles!",range);
    for (Object **iterator=engine.objects.begin();
        iterator != engine.objects.end(); iterator++) {
        Object *object=*iterator;
        if ( object->entity && !object->entity->isDead()
            && object->getDistance(x,y) <= range) {
            engine.gui->message(TCODColor::orange,"The %s gets burned for %g hit points.",
                object->name,damage);
            object->entity->damage(object,damage);
        }
    }
    return true;
}

Confuser::Confuser(int nbTurns, float range): nbTurns(nbTurns), range(range) {}

bool Confuser::use(Object *owner, Object *wearer) {
    engine.gui->message(TCODColor::cyan, "Left-click an enemy to confuse it.");

    engine.renderMode = Engine::TARGET;
    wearer->entity->ai->castable = new Object(*owner);
    wearer->entity->ai->castable->item = new Confuser(10,8);

    return Item::use(owner,wearer);
}

bool Confuser::cast(int x, int y) {
    Object *object=engine.getObject(x,y);
    if ( !object ) {
        return false;
    }

    // confuse the monster for <nbTurns> turns
    Ai *confusedAi=new ConfusedMonsterAi(nbTurns, object->entity->ai);
    object->entity->ai = confusedAi;
    engine.gui->message(TCODColor::lightGreen,"The eyes of the %s look vacant,\nas he starts to stumble around!",
        object->name);

    return false;
}