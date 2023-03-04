class Pickable {
public:
    bool pick(Actor *owner, Actor *wearer);
    void drop(Actor *owner, Actor *wearer);
    virtual bool use(Actor *owner, Actor *wearer);
    virtual bool cast(int x, int y) = 0;
};

class Healer: public Pickable {
public:
    float amount; // how many hp
    Healer(float amount);
    bool use(Actor *owner, Actor *wearer);
    bool cast(int x, int y);
};

class LightningBolt: public Pickable {
public:
    float range, damage;
    LightningBolt(float range, float damage);
    bool use(Actor *owner, Actor *wearer);
    bool cast(int x, int y);
};

class Fireball: public Pickable {
public:
    float range, damage;
    Fireball(float range, float damage);
    bool use(Actor *owner, Actor *wearer);
    bool cast(int x, int y);
};

class Confuser: public Pickable {
public:
    int nbTurns;
    float range;
    Confuser(int nbTurns, float range);
    bool use(Actor *owner, Actor *wearer);
    bool cast(int x, int y);
};