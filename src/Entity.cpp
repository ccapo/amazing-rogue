#include "main.hpp"

Entity::Entity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName):
  hpmax(hpmax), hp(hpmax), baseAtk(atk), atk(atk), baseDef(def), def(def),
  baseMatk(matk), matk(matk), baseMdef(def), mdef(mdef), corpseName(corpseName), ai(NULL) {
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

int Entity::damage(Object *owner, int amount) {
  amount -= def;
  if ( amount > 0 ) {
    TCODRandom *rng = TCODRandom::getInstance();
    int r = rng->getInt(1, amount);
    amount = r;
    hp -= amount;
    if ( hp <= 0 ) {
      die(owner);
    }
  } else {
    amount = 0;
  }
  return amount;
}

int Entity::heal(int amount) {
  hp += amount;
  if ( hp > hpmax ) {
    amount -= hp - hpmax;
    hp = hpmax;
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

CreatureEntity::CreatureEntity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName):
  Entity(hpmax, atk, def, matk, mdef, corpseName) {
}

void CreatureEntity::die(Object *owner) {
  // transform it into a nasty corpse! it doesn't block, can't be
  // attacked and doesn't move
  engine.gui->message(TCODColor::lightGrey,"%s is dead", owner->name);
  Entity::die(owner);
}

PlayerEntity::PlayerEntity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName):
  Entity(hpmax, atk, def, matk, mdef, corpseName) {
}

void PlayerEntity::die(Object *owner) {
  engine.gui->message(TCODColor::red, "You died!");
  Entity::die(owner);
  engine.gameStatus = Engine::DEFEAT;
}
