class Ai;

class Entity {
public:
    int hpmax; // maximum health points
    int hp; // current health points
    int baseAtk;
    int atk; // attack power
    int baseDef;
    int def; // defensive power
    int baseMatk;
    int matk; // magic attack power
    int baseMdef;
    int mdef; // magic defensive power
    const char *corpseName; // the object's name once dead/destroyed
    Ai *ai; // Entity's AI

    Entity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName);
    virtual ~Entity();
    inline bool isDead() { return hp <= 0; }
    void attack(Object *owner, Object *target);
    int damage(Object *owner, int amount);
    int heal(int amount);
    virtual void die(Object *owner);
};

class CreatureEntity: public Entity {
public:
    CreatureEntity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName);
    void die(Object *owner);
};

class PlayerEntity: public Entity {
public:
    PlayerEntity(int hpmax, int atk, int def, int matk, int mdef, const char *corpseName);
    void die(Object *owner);
};
