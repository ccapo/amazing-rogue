#include "main.hpp"

static const int ROOM_MAX_SIZE = 13;
static const int ROOM_MIN_SIZE = 7;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

Map::Map(int width, int height) : width(width),height(height) {
    seed=TCODRandom::getInstance()->getInt(0,0x7FFFFFFF);
}

void Map::init(int type, bool withObjects) {
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    tiles = new Tile[width*height];
    map = new TCODMap(width,height);
    makeRoom(type);
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

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if ( isInFov(x, y) ) {
                TCODColor col = isWall(x, y) ? lightWall : lightGround;
                if (engine.renderMode == Engine::TARGET) {
                    col = col * 1.25f;
                }
                TCODConsole::root->setCharBackground(x, y, col);
            } else if ( isExplored(x, y) ) {
                TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? darkWall : darkGround);
            }
        }
    }

    if ( engine.renderMode == Engine::TARGET && isInFov(engine.mouse.cx, engine.mouse.cy) ) {
        TCODConsole::root->setCharBackground(engine.mouse.cx, engine.mouse.cy, TCODColor::white);
    }
}

void Map::makeRoom(int type) {
    switch(type) {
        case 0: {
            // Standard Room
            for (int x = width/4 + 1; x < 3*width/4; x++) {
                for (int y = height/4 + 1; y < 3*height/4; y++) {
                    map->setProperties(x, y, true, true);
                }
            }

            // Add some pillars
            int xc = 3*width/8;
            int yc = 3*height/8;
            map->setProperties(xc, yc, false, false);
            map->setProperties(xc + 1, yc, false, false);
            map->setProperties(xc, yc + 1, false, false);
            map->setProperties(xc + 1, yc + 1, false, false);
            xc = 3*width/8;
            yc = 5*height/8;
            map->setProperties(xc, yc, false, false);
            map->setProperties(xc + 1, yc, false, false);
            map->setProperties(xc, yc - 1, false, false);
            map->setProperties(xc + 1, yc - 1, false, false);
            xc = 5*width/8;
            yc = 3*height/8;
            map->setProperties(xc, yc, false, false);
            map->setProperties(xc - 1, yc, false, false);
            map->setProperties(xc, yc + 1, false, false);
            map->setProperties(xc - 1, yc + 1, false, false);
            xc = 5*width/8;
            yc = 5*height/8;
            map->setProperties(xc, yc, false, false);
            map->setProperties(xc - 1, yc, false, false);
            map->setProperties(xc, yc - 1, false, false);
            map->setProperties(xc - 1, yc - 1, false, false);

            std::vector<int> offsets;
            for (int x = width/4; x < 3*width/4; x++) {
                for (int y = height/4; y < 3*height/4; y++) {
                    if (canWalk(x, y)) {
                        int offset = x + y*width;
                        offsets.push_back(offset);
                    }
                }
            }

            // put the player in the first room
            engine.player->x = width/2;
            engine.player->y = height/2;

            // set stairs position
            //engine.stairs->x = width/2;
            //engine.stairs->y = 5*height/8;

            // set the exits
            for (Object **iterator = engine.exits.begin(); iterator != engine.exits.end(); iterator++) {
                Object *exit = *iterator;
                map->setProperties(exit->x, exit->y, true, true);
            }

            int sz = offsets.size();
            int index = rng->getInt(0, sz - 1);
            int offset = offsets[index];
            int cx = offset % width;
            int cy = (offset - cx)/width;
            addMonster(cx, cy);
            break;
        }
        case 1: {
            // Maze Room
            int w2 = (width/2 - 1)/2, h2 = (height/2 - 1)/2;
            Maze m(w2, h2);
            m.createMaze();
#ifdef DEBUG
            m.saveMaze();
#endif
            for (int x = 0; x < width/2; x++) {
                for (int y = 0; y < height/2; y++) {
                    int offset = x + y*(width/2);
                    switch (m.maze_tiles[offset]) {
                    case WALL:
                        map->setProperties(x + width/4, y + height/4, false, false);
                        break;
                    case FLOOR:
                        map->setProperties(x + width/4, y + height/4, true, true);
                        break;
                    default:
                        printf("Unrecognized tile type: %c\n", m.maze_tiles[offset]);
                    }
                }
            }

            // put the player in the first room
            engine.player->x = 1 + width/4;
            engine.player->y = 0 + height/4;

            // set stairs position
            //engine.stairs->x = 2 * w2 - 1 + width/4;
            //engine.stairs->y = 2 * h2 + height/4;

            // set the exits
            for (Object **iterator = engine.exits.begin(); iterator != engine.exits.end(); iterator++) {
                Object *exit = *iterator;
                map->setProperties(exit->x, exit->y, true, true);
            }

            int sz = m.floor_tiles.size();
            int index = rng->getInt(0, sz - 1);
            int offset = m.floor_tiles[index];
            int cx = offset % (width/2) + width/4;
            int cy = (offset - cx)/(width/2) + height/4;
            addMonster(cx, cy);

            break;
        }
        default: {
            printf("Unrecognized room type: %d\n", type);
            break;
        }
    }
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withObjects) {
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