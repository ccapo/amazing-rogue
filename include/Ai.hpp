class Ai {
public:
    virtual ~Ai() {};
    virtual void update(Actor *owner)=0;
    Actor *castable;
};

class PlayerAi: public Ai {
public:
    void update(Actor *owner);

protected:
    bool moveOrAttack(Actor *owner, int targetx, int targety);
    void handleActionKey(Actor *owner, int ascii);
    Actor *choseFromInventory(Actor *owner);
};

class MonsterAi: public Ai {
public:
    void update(Actor *owner);
 
protected:
    int moveCount = 0;
    void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi: public Ai {
public:
    ConfusedMonsterAi(int nbTurns, Ai *oldAi);
    void update(Actor *owner);
protected:
    int nbTurns;
    Ai *oldAi;
};