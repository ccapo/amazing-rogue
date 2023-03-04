class Item {
public:
    bool pick(Object *owner, Object *wearer);
    void drop(Object *owner, Object *wearer);
    virtual bool use(Object *owner, Object *wearer);
    virtual bool cast(int x, int y) = 0;
};

class Healer: public Item {
public:
    float amount; // how many hp
    Healer(float amount);
    bool use(Object *owner, Object *wearer);
    bool cast(int x, int y);
};

class LightningBolt: public Item {
public:
    float range, damage;
    LightningBolt(float range, float damage);
    bool use(Object *owner, Object *wearer);
    bool cast(int x, int y);
};

class Fireball: public Item {
public:
    float range, damage;
    Fireball(float range, float damage);
    bool use(Object *owner, Object *wearer);
    bool cast(int x, int y);
};

class Confuser: public Item {
public:
    int nbTurns;
    float range;
    Confuser(int nbTurns, float range);
    bool use(Object *owner, Object *wearer);
    bool cast(int x, int y);
};