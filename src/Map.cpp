#include "main.hpp"

static const int ROOM_MAX_SIZE = 13;
static const int ROOM_MIN_SIZE = 7;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

Map::Map(int width, int height) : width(width),height(height) {
    seed = TCODRandom::getInstance()->getInt(0,0x7FFFFFFF);
}

void Map::init(int type, bool reset, bool withObjects) {
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    tiles = new Tile[width*height];
    map = new TCODMap(width,height);
    makeRoom(type, reset);
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
        tiles[x+y*width].explored = true;
        return true;
    }
    return false;
}

float Map::getScent(int x, int y) const {
    return tiles[x + y*width].scent;
}

void Map::computeFov() {
    map->computeFov(engine.player->x,engine.player->y,engine.fovRadius);

    const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const double dcoef = 1.0/8.0, lambda = 1.0;

    int offset = engine.player->x + width*engine.player->y;
    tiles[offset].scentPrev = 1.0f;

    for(int x = 1; x < width - 2; x++) {
        for(int y = 1; y < height - 2; y++) {
            offset = x + y*width;
            if( map->isWalkable(x, y) ) {
                double sdiff = 0.0f;
                for(int z = 0; z < 8; z++) {
                    int doffset = x + dx[z] + (y + dy[z])*width;
                    sdiff += tiles[doffset].scentPrev - tiles[offset].scentPrev;
                }
                tiles[offset].scent = lambda*(tiles[offset].scentPrev + dcoef*sdiff);
            } else {
                tiles[offset].scent = 0.0;
            }
        }
    }

    for(int x = 1; x < width - 2; x++) {
        for(int y = 1; y < height - 2; y++) {
            offset = x + width*y;
            tiles[offset].scentPrev = tiles[offset].scent;
        }
    }
}

void Map::render() const {
    static const TCODColor darkWall(0,0,100);
    static const TCODColor darkGround(50,50,150);
    static const TCODColor lightWall(130,110,50);
    static const TCODColor lightGround(200,180,50);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float scent = getScent(x, y);
            if ( isInFov(x, y) ) {
                TCODColor base = isWall(x, y) ? lightWall : lightGround;
                if (engine.renderMode == Engine::TARGET) {
                    base = base * 1.25f;
                }
                //TCODColor final = TCODColor::lerp(base, TCODColor::darkestGreen, scent);
                TCODConsole::root->setCharBackground(x, y, base);
            } else if ( isExplored(x, y) ) {
                TCODColor base = isWall(x, y) ? darkWall : darkGround;
                //TCODColor final = TCODColor::lerp(base, TCODColor::darkestGreen, scent);
                TCODConsole::root->setCharBackground(x, y, base);
            }
        }
    }

    if ( engine.renderMode == Engine::TARGET && isInFov(engine.mouse.cx, engine.mouse.cy) ) {
        TCODConsole::root->setCharBackground(engine.mouse.cx, engine.mouse.cy, TCODColor::white);
    }
}

void Map::makeRoom(int type, bool reset) {
    static bool first = true;

    if (reset) first = true;

    switch(type) {
        case 0: {
            // Standard Room
            for (int x = width/4 + 1; x < 3*width/4; x++) {
                for (int y = height/4 + 1; y < 3*height/4; y++) {
                    map->setProperties(x, y, true, true);
                }
            }

            int subtype = rng->getInt(0, 2);
            if (first) {
                first = false;
                subtype = 0;
            }
            switch (subtype) {
                case 0: {
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

                    // Put the player in the middle of the room
                    engine.player->x = width/2;
                    engine.player->y = height/2;
                    break;
                }
                case 1: {
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

                    // Add a hole
                    xc = width/2;
                    yc = height/2;
                    map->setProperties(xc - 2, yc - 2, true, false);
                    map->setProperties(xc - 1, yc - 2, true, false);
                    map->setProperties(xc + 0, yc - 2, true, false);
                    map->setProperties(xc + 1, yc - 2, true, false);
                    map->setProperties(xc + 2, yc - 2, true, false);

                    map->setProperties(xc - 2, yc - 1, true, false);
                    map->setProperties(xc - 1, yc - 1, true, false);
                    map->setProperties(xc + 0, yc - 1, true, false);
                    map->setProperties(xc + 1, yc - 1, true, false);
                    map->setProperties(xc + 2, yc - 1, true, false);

                    map->setProperties(xc - 2, yc + 0, true, false);
                    map->setProperties(xc - 1, yc + 0, true, false);
                    map->setProperties(xc + 0, yc + 0, true, false);
                    map->setProperties(xc + 1, yc + 0, true, false);
                    map->setProperties(xc + 2, yc + 0, true, false);

                    map->setProperties(xc - 2, yc + 1, true, false);
                    map->setProperties(xc - 1, yc + 1, true, false);
                    map->setProperties(xc + 0, yc + 1, true, false);
                    map->setProperties(xc + 1, yc + 1, true, false);
                    map->setProperties(xc + 2, yc + 1, true, false);

                    map->setProperties(xc - 2, yc + 2, true, false);
                    map->setProperties(xc - 1, yc + 2, true, false);
                    map->setProperties(xc + 0, yc + 2, true, false);
                    map->setProperties(xc + 1, yc + 2, true, false);
                    map->setProperties(xc + 2, yc + 2, true, false);

                    // Put the player in the middle of the room
                    engine.player->x = 1 + width/4;
                    engine.player->y = height/2;
                    break;

                }
                case 2: {
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

                    // Add holes
                    xc = width/2;
                    yc = height/2;
                    map->setProperties(xc - 1, yc - 6, true, false);
                    map->setProperties(xc + 0, yc - 6, true, false);
                    map->setProperties(xc + 1, yc - 6, true, false);
                    map->setProperties(xc - 1, yc - 5, true, false);
                    map->setProperties(xc + 0, yc - 5, true, false);
                    map->setProperties(xc + 1, yc - 5, true, false);
                    map->setProperties(xc - 1, yc - 4, true, false);
                    map->setProperties(xc + 0, yc - 4, true, false);
                    map->setProperties(xc + 1, yc - 4, true, false);

                    map->setProperties(xc - 6, yc - 1, true, false);
                    map->setProperties(xc - 6, yc + 0, true, false);
                    map->setProperties(xc - 6, yc + 1, true, false);
                    map->setProperties(xc - 5, yc - 1, true, false);
                    map->setProperties(xc - 5, yc + 0, true, false);
                    map->setProperties(xc - 5, yc + 1, true, false);
                    map->setProperties(xc - 4, yc - 1, true, false);
                    map->setProperties(xc - 4, yc + 0, true, false);
                    map->setProperties(xc - 4, yc + 1, true, false);

                    map->setProperties(xc + 6, yc - 1, true, false);
                    map->setProperties(xc + 6, yc + 0, true, false);
                    map->setProperties(xc + 6, yc + 1, true, false);
                    map->setProperties(xc + 5, yc - 1, true, false);
                    map->setProperties(xc + 5, yc + 0, true, false);
                    map->setProperties(xc + 5, yc + 1, true, false);
                    map->setProperties(xc + 4, yc - 1, true, false);
                    map->setProperties(xc + 4, yc + 0, true, false);
                    map->setProperties(xc + 4, yc + 1, true, false);

                    map->setProperties(xc - 1, yc + 6, true, false);
                    map->setProperties(xc + 0, yc + 6, true, false);
                    map->setProperties(xc + 1, yc + 6, true, false);
                    map->setProperties(xc - 1, yc + 5, true, false);
                    map->setProperties(xc + 0, yc + 5, true, false);
                    map->setProperties(xc + 1, yc + 5, true, false);
                    map->setProperties(xc - 1, yc + 4, true, false);
                    map->setProperties(xc + 0, yc + 4, true, false);
                    map->setProperties(xc + 1, yc + 4, true, false);

                    // Put the player in the middle of the room
                    engine.player->x = 1 + width/4;
                    engine.player->y = height/2;
                    break;
                }
                default: {
                    printf("Unrecognized room subtype = %d\n", subtype);
                    return;
                }
            }

            std::vector<int> offsets;
            for (int x = width/4; x < 3*width/4; x++) {
                for (int y = height/4 + 1; y < 3*height/4; y++) {
                    if (canWalk(x, y)) {
                        int offset = x + y*width;
                        offsets.push_back(offset);
                    }
                }
            }

            // set the exits
            for (Object **iterator = engine.exits.begin(); iterator != engine.exits.end(); iterator++) {
                Object *exit = *iterator;
                map->setProperties(exit->x, exit->y, true, true);
            }

            // add monsters
            int noffsets = offsets.size();
            int nbMonsters = rng->getInt(0,MAX_ROOM_MONSTERS);
            while (nbMonsters > 0) {
                noffsets = offsets.size();
                int index = rng->getInt(0, noffsets - 1);
                int offset = offsets[index];
                int cx = offset % width;
                int cy = (offset - cx)/width;
                addMonster(cx, cy);
                nbMonsters--;
                offsets.erase(offsets.begin() + index);
            }

            // add items
            int nbItems = rng->getInt(0,MAX_ROOM_ITEMS);
            while (nbItems > 0) {
                noffsets = offsets.size();
                int index = rng->getInt(0, noffsets - 1);
                int offset = offsets[index];
                int cx = offset % width;
                int cy = (offset - cx)/width;
                addItem(cx, cy);
                nbItems--;
                offsets.erase(offsets.begin() + index);
            }

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

            // set the exits
            for (Object **iterator = engine.exits.begin(); iterator != engine.exits.end(); iterator++) {
                Object *exit = *iterator;
                map->setProperties(exit->x, exit->y, true, true);
            }

            // add monsters
            int nbMonsters = rng->getInt(0,MAX_ROOM_MONSTERS);
            int sz = m.floor_tiles.size();
            while (nbMonsters > 0) {
                sz = m.floor_tiles.size();
                int index = rng->getInt(0, sz - 1);
                int offset = m.floor_tiles[index];
                int cx = offset % (width/2);
                int cy = (offset - cx)/(width/2);
                addMonster(cx + width/4, cy + height/4);
                nbMonsters--;
                m.floor_tiles.erase(m.floor_tiles.begin() + index);
            }

            // add items
            int nbItems = rng->getInt(0,MAX_ROOM_ITEMS);
            while (nbItems > 0) {
                sz = m.floor_tiles.size();
                int index = rng->getInt(0, sz - 1);
                int offset = m.floor_tiles[index];
                int cx = offset % (width/2);
                int cy = (offset - cx)/(width/2);
                addItem(cx + width/4, cy + height/4);
                nbItems--;
                m.floor_tiles.erase(m.floor_tiles.begin() + index);
            }

            break;
        }
        case 2: {
            // Boss Level
            for (int x = width/4; x < 3*width/4; x++) {
                for (int y = height/4; y < 3*height/4; y++) {
                    map->setProperties(x, y, true, true);
                }
            }

            engine.player->x = 1 + width/4;
            engine.player->y = height/2;
            break;
        }
        default: {
            printf("Unrecognized room type: %d\n", type);
            break;
        }
    }
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x,y)) {
        // this is a wall
        return false;
    }
    for (Object **iterator = engine.objects.begin();
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
    TCODRandom *rng = TCODRandom::getInstance();
    if ( rng->getInt(0,100) < 80 ) {
        // create an orc
        Object *orc = new Object(x,y,'o',"orc", TCODColor::desaturatedGreen);
        orc->entity = new CreatureEntity(10,3,0,0,0,"dead orc");
        orc->entity->ai = new MonsterAi();
        engine.objects.push(orc);
    } else {
        // create a troll
        Object *troll = new Object(x,y,'T',"troll", TCODColor::darkerGreen);
        troll->entity = new CreatureEntity(16,4,1,0,0,"troll carcass");
        troll->entity->ai = new MonsterAi();
        engine.objects.push(troll);
    }
}

void Map::addItem(int x, int y) {
    TCODRandom *rng = TCODRandom::getInstance();
    int dice = rng->getInt(0,100);
    if ( dice < 70 ) {
        // create a health potion
        Object *healthPotion = new Object(x,y,'!',"health potion",TCODColor::violet);
        healthPotion->blocks = false;
        healthPotion->item = new Healer(4);
        engine.objects.push(healthPotion);
    } else if ( dice < 70+10 ) {
        // create a scroll of bolt 
        Object *scrollOfLightningBolt = new Object(x,y,'#',"scroll of bolt",TCODColor::lightYellow);
        scrollOfLightningBolt->blocks = false;
        scrollOfLightningBolt->item = new LightningBolt(5,20);
        engine.objects.push(scrollOfLightningBolt);
    } else if ( dice < 70+10+10 ) {
        // create a scroll of flame
        Object *scrollOfFireball = new Object(x,y,'#',"scroll of flame",TCODColor::lightRed);
        scrollOfFireball->blocks = false;
        scrollOfFireball->item = new Fireball(3,15);
        engine.objects.push(scrollOfFireball);
    } else {
        // create a scroll of deluge
        Object *scrollOfConfusion = new Object(x,y,'#',"scroll of deluge",TCODColor::lightBlue);
        scrollOfConfusion->blocks = false;
        scrollOfConfusion->item = new Confuser(10,8);
        engine.objects.push(scrollOfConfusion);
    }
}

// void Map::moveDisplay(int x, int y) {
//     // New display coordinates (top-left corner of the screen relative to the map)
//     // Coordinates so that the target is at the center of the screen
//     int cx = x - engine.displayWidth/2;
//     int cy = y - engine.displayHeight/2;

//     // Make sure the DISPLAY doesn't see outside the map
//     if(cx < 0) cx = 0;
//     if(cy < 0) cy = 0;
//     if(cx > width - engine.displayWidth - 1) cx = width - engine.displayWidth - 1;
//     if(cy > height - engine.displayHeight - 1) cy = height - engine.displayHeight - 1;

//     display_x = cx; display_y = cy;
// }