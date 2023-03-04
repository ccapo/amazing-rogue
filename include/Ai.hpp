class Ai {
public:
    virtual ~Ai() {};
    virtual void update(Object *owner)=0;
    Object *castable;
};

class PlayerAi: public Ai {
public:
    int xpLevel;
    PlayerAi();
    int getNextLevelXp();
    void update(Object *owner);

protected:
    bool moveOrAttack(Object *owner, int targetx, int targety);
    void handleActionKey(Object *owner, int ascii);
    Object *choseFromInventory(Object *owner);
};

class MonsterAi: public Ai {
public:
    void update(Object *owner);
 
protected:
    int moveCount = 0;
    void moveOrAttack(Object *owner, int targetx, int targety);
};

class ConfusedMonsterAi: public Ai {
public:
    ConfusedMonsterAi(int nbTurns, Ai *oldAi);
    void update(Object *owner);
protected:
    int nbTurns;
    Ai *oldAi;
};