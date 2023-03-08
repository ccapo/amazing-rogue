#include "main.hpp"

Entity::Entity(int hpMax, int atk, int def, const char *corpseName, int xp) :
  maxHp(hpMax), hp(hpMax), atk(atk), def(def), corpseName(corpseName), xp(xp), ai(NULL) {
}

Entity::~Entity() {
  corpseName = NULL;
  if ( ai ) delete ai;
}

void Entity::attack(Object *owner, Object *target) {
  if ( target->entity && !target->entity->isDead() ) {
    int damage = target->entity->damage(target, atk);
    if ( damage > 0 ) {
      TCODColor msg_colour = owner == engine.player ? TCODColor::red : TCODColor::lightGrey;
      engine.gui->message(msg_colour, "%s attacks %s for %d hit points.", owner->name, target->name, damage);
    } else {
      engine.gui->message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", owner->name, target->name);
    }
  } else {
    engine.gui->message(TCODColor::lightGrey, "%s attacks %s in vain.", owner->name, target->name);
  }
}

int Entity::damage(Object *owner, float damage) {
    damage -= def;
    if ( damage > 0 ) {
        hp -= damage;
        if ( hp <= 0 ) {
            die(owner);
        }
    } else {
        damage = 0;
    }
    return damage;
}

int Entity::heal(int amount) {
  hp += amount;
  if ( hp > maxHp ) {
    amount -= hp - maxHp;
    hp = maxHp;
  }
  return amount;
}

void Entity::die(Object *owner) {
  // transform the object into a corpse!
  owner->ch = '%';
  owner->colour = TCODColor::darkRed;  
  owner->name = corpseName;
  owner->blocks = false;
  if( owner != engine.player ) {
    delete owner->entity->ai;
    owner->entity->ai = NULL;
    delete owner->entity;
    owner->entity = NULL;
  }
  // make sure corpses are drawn before living objects
  engine.sendToBack(owner);
}

CreatureEntity::CreatureEntity(int hpMax, int atk, int def, const char *corpseName, int xp) :
  Entity(hpMax, atk, def, corpseName, xp) {
}

void CreatureEntity::die(Object *owner) {
  // transform it into a nasty corpse! it doesn't block, can't be
  // attacked and doesn't move
  engine.gui->message(TCODColor::lightGrey,"%s is dead. You gain %d xp", owner->name, xp);
  engine.player->entity->xp += xp;
  Entity::die(owner);
}

PlayerEntity::PlayerEntity(int hpMax, int atk, int def, const char *corpseName) :
  Entity(hpMax, atk, def, corpseName, 0) {
}

void PlayerEntity::die(Object *owner) {
  engine.gui->message(TCODColor::red, "You died!");
  Entity::die(owner);
  engine.gameStatus = Engine::DEFEAT;
}
