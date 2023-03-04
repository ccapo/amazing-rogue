class Ai;

class Entity {
public:
    float maxHp; // maximum health points
    float hp; // current health points
    float atk; // hit points given
    float def; // hit points deflected
    const char *corpseName; // the object's name once dead/destroyed
    int xp; // XP gained when killing this monster (or player xp)
    Ai *ai; // Entity's AI

    Entity(int hpMax, int atk, int def, const char *corpseName, int xp);
    virtual ~Entity();
    inline bool isDead() { return hp <= 0; }
    void attack(Object *owner, Object *target);
    int damage(Object *owner, float damage);
    int heal(int amount);
    virtual void die(Object *owner);
};

class CreatureEntity: public Entity {
public:
    CreatureEntity(int hpMax, int atk, int def, const char *corpseName, int xp);
    void die(Object *owner);
};

class PlayerEntity: public Entity {
public:
    PlayerEntity(int hpMax, int atk, int def, const char *corpseName);
    void die(Object *owner);
};
