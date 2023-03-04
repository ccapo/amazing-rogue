#include "main.hpp"

static const int ROOM_MAX_SIZE = 13;
static const int ROOM_MIN_SIZE = 7;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener: public ITCODBspCallback {
private:
    Map &map; // a map to dig
    int roomNum; // room number
    int lastx,lasty; // center of the last room
public:
    BspListener(Map &map) : map(map), roomNum(0) {}
    bool visitNode(TCODBsp *node, void *userData) {
    if ( node->isLeaf() ) {
        int x,y,w,h;
        // dig a room
        bool withObjects=(bool)userData;
        w=map.rng->getInt(ROOM_MIN_SIZE, node->w-2);
        h=map.rng->getInt(ROOM_MIN_SIZE, node->h-2);
        x=map.rng->getInt(node->x+1, node->x+node->w-w-1);
        y=map.rng->getInt(node->y+1, node->y+node->h-h-1);
        map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1, withObjects);
        if ( roomNum != 0 ) {
            // dig a corridor from last room
            map.dig(lastx,lasty,x+w/2,lasty);
            map.dig(x+w/2,lasty,x+w/2,y+h/2);
        }
        lastx=x+w/2;
        lasty=y+h/2;
        roomNum++;
    }
    return true;
    }
};

Map::Map(int width, int height) : width(width),height(height) {
    seed=TCODRandom::getInstance()->getInt(0,0x7FFFFFFF);
}

void Map::init(bool withObjects) {
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    tiles=new Tile[width*height];
    map=new TCODMap(width,height);
    TCODBsp bsp(0,0,width,height);
    bsp.splitRecursive(rng,8,ROOM_MAX_SIZE,ROOM_MAX_SIZE,1.5f,1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener,(void *)withObjects);
}

Map::~Map() {
    delete [] tiles;
    delete map;
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x,y);
}

bool Map::isExplored(int x, int y) const {
    return tiles[x+y*width].explored;
}

bool Map::isInFov(int x, int y) const {
    if ( x < 0 || x >= width || y < 0 || y >= height ) {
        return false;
    }
    if ( map->isInFov(x,y) ) {
        tiles[x+y*width].explored=true;
        return true;
    }
    return false;
}

void Map::computeFov() {
    map->computeFov(engine.player->x,engine.player->y,engine.fovRadius);
}

void Map::render() const {
    static const TCODColor darkWall(0,0,100);
    static const TCODColor darkGround(50,50,150);

    static const TCODColor lightWall(130,110,50);
    static const TCODColor lightGround(200,180,50);

    for (int x=0; x < width; x++) {
        for (int y=0; y < height; y++) {
            if ( isInFov(x,y) ) {
                TCODColor col = isWall(x,y) ? lightWall : lightGround;
                if (engine.renderMode == Engine::TARGET) {
                    col = col * 1.25f;
                }
                TCODConsole::root->setCharBackground(x,y,col);
            } 
            else if ( isExplored(x,y) ) {
                TCODConsole::root->setCharBackground(x,y,isWall(x,y) ? darkWall : darkGround );
            }
        }    
    }

    if ( engine.renderMode == Engine::TARGET && isInFov(engine.mouse.cx,engine.mouse.cy) ) {
        TCODConsole::root->setCharBackground(engine.mouse.cx,engine.mouse.cy,TCODColor::white);
    }
}

void Map::dig(int x1, int y1, int x2, int y2) {
    if ( x2 < x1 ) {
       int tmp=x2;
       x2=x1;
       x1=tmp;
    }
    if ( y2 < y1 ) {
       int tmp=y2;
       y2=y1;
       y1=tmp;
    }
    for (int tilex=x1; tilex <= x2; tilex++) {
        for (int tiley=y1; tiley <= y2; tiley++) {
            map->setProperties(tilex,tiley,true,true);
        }
    }
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withObjects) {
    dig(x1,y1,x2,y2);
    if (!withObjects) {
        return;
    }   
    if ( first ) {
       // put the player in the first room
       engine.player->x=(x1+x2)/2;
       engine.player->y=(y1+y2)/2;
    } 
    else {
        TCODRandom *rng=TCODRandom::getInstance();
        // add monsters
        int nbMonsters=rng->getInt(0,MAX_ROOM_MONSTERS);
        while (nbMonsters > 0) {
            int x=rng->getInt(x1,x2);
            int y=rng->getInt(y1,y2);
            if ( canWalk(x,y) ) {
                addMonster(x,y);
            }
            nbMonsters--;
        }

        // add items
        int nbItems=rng->getInt(0,MAX_ROOM_ITEMS);
        while (nbItems > 0) {
            int x=rng->getInt(x1,x2);
            int y=rng->getInt(y1,y2);
            if ( canWalk(x,y) ) {
                addItem(x,y);
            }
            nbItems--;
        }

        // set stairs position
        engine.stairs->x=(x1+x2)/2;
        engine.stairs->y=(y1+y2)/2;
    }
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x,y)) {
        // this is a wall
        return false;
    }
    for (Object **iterator=engine.objects.begin();
        iterator!=engine.objects.end();iterator++) {
        Object *object=*iterator;
        if ( object->blocks && object->x == x && object->y == y ) {
            // there is an object there. cannot walk
            return false;
        }
    }
    return true;
}

void Map::addMonster(int x, int y) {
    TCODRandom *rng=TCODRandom::getInstance();
    if ( rng->getInt(0,100) < 80 ) {
        // create an orc
        Object *orc = new Object(x,y,'o',"orc", TCODColor::desaturatedGreen);
        orc->entity = new CreatureEntity(10,3,0,"dead orc",35);
        orc->entity->ai = new MonsterAi();
        engine.objects.push(orc);
    } else {
        // create a troll
        Object *troll = new Object(x,y,'T',"troll", TCODColor::darkerGreen);
        troll->entity = new CreatureEntity(16,4,1,"troll carcass",100);
        troll->entity->ai = new MonsterAi();
        engine.objects.push(troll);
    }
}

void Map::addItem(int x, int y) {
    TCODRandom *rng=TCODRandom::getInstance();
    int dice = rng->getInt(0,100);
    if ( dice < 70 ) {
        // create a health potion
        Object *healthPotion=new Object(x,y,'!',"health potion",TCODColor::violet);
        healthPotion->blocks=false;
        healthPotion->item=new Healer(4);
        engine.objects.push(healthPotion);
    } else if ( dice < 70+10 ) {
        // create a scroll of lightning bolt 
        Object *scrollOfLightningBolt=new Object(x,y,'#',"scroll of lightning bolt",TCODColor::lightYellow);
        scrollOfLightningBolt->blocks=false;
        scrollOfLightningBolt->item=new LightningBolt(5,20);
        engine.objects.push(scrollOfLightningBolt);
    } else if ( dice < 70+10+10 ) {
        // create a scroll of fireball
        Object *scrollOfFireball=new Object(x,y,'#',"scroll of fireball",TCODColor::lightRed);
        scrollOfFireball->blocks=false;
        scrollOfFireball->item=new Fireball(3,15);
        engine.objects.push(scrollOfFireball);
    } else {
        // create a scroll of confusion
        Object *scrollOfConfusion=new Object(x,y,'#',"scroll of confusion",TCODColor::lightBlue);
        scrollOfConfusion->blocks=false;
        scrollOfConfusion->item=new Confuser(10,8);
        engine.objects.push(scrollOfConfusion);
    }
}