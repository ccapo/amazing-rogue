#include "main.hpp"

Attacker::Attacker(float power) : power(power) {}

void Attacker::attack(Actor *owner, Actor *target) {
    if ( target->destructible && ! target->destructible->isDead() ) {
        if ( power - target->destructible->defense > 0 ) {
            TCODColor msg_colour = owner == engine.player ? TCODColor::red : TCODColor::lightGrey;
            float dmg = power - target->destructible->defense;
            engine.gui->message(msg_colour, "%s attacks %s for %g hit points.", owner->name, target->name, dmg);
        } else {
            engine.gui->message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", owner->name, target->name);
        }
        target->destructible->takeDamage(target,power);
    } else {
        engine.gui->message(TCODColor::lightGrey, "%s attacks %s in vain.",owner->name,target->name);
    }
}