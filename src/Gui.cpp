#include <stdio.h>
#include <stdarg.h>
#include "main.hpp"

static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 2;

Gui::Gui() {
    conMsg = new TCODConsole(engine.screenWidth - BAR_WIDTH - 1, PANEL_HEIGHT);
    conStatus = new TCODConsole(engine.screenWidth, 3);
    conMiniMap = new TCODConsole(BAR_WIDTH + 1, PANEL_HEIGHT);
}

Gui::~Gui() {
    delete conMsg;
    delete conStatus;
    delete conMiniMap;
    clear();
}

void Gui::render() {
    static const TCODColor darkWall(0,0,100);
    static const TCODColor darkGround(50,50,150);
    static const TCODColor lightWall(130,110,50);
    static const TCODColor lightGround(200,180,50);

    // clear the GUI console
    conMsg->setDefaultBackground(TCODColor::black);
    conMsg->clear();
    conStatus->setDefaultBackground(TCODColor::black);
    conStatus->clear();
    conMiniMap->setDefaultBackground(TCODColor::black);
    conMiniMap->clear();

    conMsg->printFrame(0, 0, engine.screenWidth - BAR_WIDTH - 1, PANEL_HEIGHT, false, TCOD_BKGND_SET, "Message Log");
    conMiniMap->printFrame(0, 0, BAR_WIDTH + 1, PANEL_HEIGHT, false, TCOD_BKGND_SET, "Lvl %d", engine.level + 1);

    // draw the health bar
    renderBar(1,1,BAR_WIDTH,"HP",engine.player->entity->hp,
        engine.player->entity->hpmax,
        TCODColor::lightRed,TCODColor::darkerRed);

    // dungeon level
    conStatus->setDefaultForeground(TCODColor::white);
    int atk = engine.player->entity->atk, def = engine.player->entity->def;
    int matk = engine.player->entity->matk, mdef = engine.player->entity->mdef;
    conStatus->print(BAR_WIDTH + 2,1,"ATK: %d DEF: %d MATK: %d MDEF: %d", atk, def, matk, mdef);

    // draw the message log
    int y = 1;
    float colorCoef = 0.4f;
    for (Message **it = log.begin(); it != log.end(); it++) {
        Message *message = *it;
        conMsg->setDefaultForeground(message->col * colorCoef);
        conMsg->print(2, y, message->text);
        y++;
        if ( colorCoef < 1.0f ) {
            colorCoef += 0.3f;
        }
    }

    // mouse look
    renderMouseLook();

    conMsg->setDefaultForeground(TCODColor::white);

    LevelMap *lm = engine.levelMaps.get(engine.level);
    int offset = lm->levelHash[engine.roomID].offset;
    int xRoom = offset % lm->width;
    int yRoom = (offset - xRoom)/lm->width;
    int cx = xRoom - BAR_WIDTH/2, cy = yRoom - (PANEL_HEIGHT - 1)/2;
    if(cx < 0) cx = 0;
    if(cy < 0) cy = 0;
    if(cx > lm->width - BAR_WIDTH - 1) cx = lm->width - BAR_WIDTH - 1;
    if(cy > lm->height - (PANEL_HEIGHT - 1) - 1) cy = lm->height - (PANEL_HEIGHT - 1) - 1;

    for (int y = 0; y < lm->height; y++) {
        for (int x = 0; x < lm->width; x++) {
            int offset = x + y*lm->width;
            TCODColor col = lightGround;
            if(lm->levelData[offset].visible) {
                if(!lm->levelData[offset].active) col = darkGround;
                conMiniMap->setCharBackground(x - cx, y - cy, col);
            }
        }
    }

    // blit the GUI console on the root console
    TCODConsole::blit(conMsg, 0, 0, engine.screenWidth - BAR_WIDTH - 1, PANEL_HEIGHT, TCODConsole::root, BAR_WIDTH + 1, engine.screenHeight - PANEL_HEIGHT);
    // blit the GUI console on the root console
    TCODConsole::blit(conStatus, 0, 0, engine.screenWidth, 3, TCODConsole::root, 0, 0);

    TCODConsole::blit(conMiniMap, 0, 0, BAR_WIDTH + 1, PANEL_HEIGHT, TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, const char *name,
    float value, float maxValue, const TCODColor &barColor,
    const TCODColor &backColor) {
    // fill the background
    conStatus->setDefaultBackground(backColor);
    conStatus->rect(x,y,width,1,false,TCOD_BKGND_SET);

    int barWidth = (int)(value / maxValue * width);
    if ( barWidth > 0 ) {
        // draw the bar
        conStatus->setDefaultBackground(barColor);
        conStatus->rect(x,y,barWidth,1,false,TCOD_BKGND_SET);
    }

    // print text on top of the bar
    conStatus->setDefaultForeground(TCODColor::white);
    conStatus->printEx(x+width/2,y,TCOD_BKGND_NONE,TCOD_CENTER,
        "%s : %g/%g", name, value, maxValue);
}

void Gui::clear() {
    log.clearAndDelete();
}

Gui::Message::Message(const char *text, const TCODColor &col) :
    text(strdup(text)),col(col) {   
}

Gui::Message::~Message() {
    free(text);
}

void Gui::renderMouseLook() {
    if (! engine.map->isInFov(engine.mouse.cx, engine.mouse.cy)) {
        // if mouse is out of fov, nothing to render
        return;
    }
    char buf[128]="";
    bool first = true;
    for (Object **it = engine.objects.begin(); it != engine.objects.end(); it++) {
        Object *object=*it;
        // find objects under the mouse cursor
        if (object->x == engine.mouse.cx && object->y == engine.mouse.cy ) {
            if (! first) {
                strcat(buf,", ");
            } else {
                first = false;
            }
            strcat(buf,object->name);
        }
    }
    // display the list of objects under the mouse cursor
    conMsg->setDefaultForeground(TCODColor::lightGrey);
    conMsg->print(1,0,buf);
}

void Gui::message(const TCODColor &col, const char *text, ...) {
    // build the text
    va_list ap;
    char buf[128];
    va_start(ap,text);
    vsprintf(buf,text,ap);
    va_end(ap);

    char *lineBegin = buf;
    char *lineEnd;
    do {
        // make room for the new message
        if ( log.size() == MSG_HEIGHT ) {
            Message *toRemove = log.get(0);
            log.remove(toRemove);
            delete toRemove;
        }

        // detect end of the line
        lineEnd = strchr(lineBegin,'\n');
        if ( lineEnd ) {
            *lineEnd='\0';
        }

        // add a new message to the log
        Message *msg = new Message(lineBegin, col);
        log.push(msg);

        // go to next line
        lineBegin = lineEnd+1;
    } while ( lineEnd );
}

Menu::~Menu() {
    clear();
}

void Menu::clear() {
    items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char *label) {
    MenuItem *item = new MenuItem();
    item->code = code;
    item->label = label;
    items.push(item);
}

const int PAUSE_MENU_WIDTH = 30;
const int PAUSE_MENU_HEIGHT = 18;
Menu::MenuItemCode Menu::pick(DisplayMode mode) {
    int selectedItem = 0;
    int menux,menuy;
    if (mode == PAUSE) {
        menux = engine.screenWidth/2-PAUSE_MENU_WIDTH/2;
        menuy = engine.screenHeight/2-PAUSE_MENU_HEIGHT/2;
        TCODConsole::root->setDefaultForeground(TCODColor(200,180,50));
        TCODConsole::root->printFrame(menux,menuy,PAUSE_MENU_WIDTH,PAUSE_MENU_HEIGHT,true,
            TCOD_BKGND_ALPHA(70),"menu");       
        menux += 2;
        menuy += 2;
    } else {
        static TCODImage img("data/img/main_menu.png");
        img.blit2x(TCODConsole::root,0,0);
        menux = 10;
        menuy = TCODConsole::root->getHeight()/3;
    }

    while( !TCODConsole::isWindowClosed() ) {
        int currentItem = 0;
        TCODConsole::root->setDefaultForeground(TCODColor::lightRed);
        if (mode == PAUSE) {
            TCODConsole::root->print(menux, menuy, "Attribute Upgrade");
        } else {
            TCODConsole::root->print(menux, menuy, "Amazing Rogue");
        }
        for (MenuItem **it = items.begin(); it!=items.end(); it++) {
            if ( currentItem == selectedItem ) {
                TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
            } else {
                TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
            }
            if (mode == PAUSE) {
                TCODConsole::root->print(menux,menuy+4+currentItem*2,(*it)->label);
            } else {
                TCODConsole::root->print(menux,menuy+6+currentItem*3,(*it)->label);
            }
            currentItem++;
        }
        TCODConsole::flush();

        // check key presses
        TCOD_key_t key;
        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
        switch (key.vk) {
            case TCODK_UP : 
                selectedItem--; 
                if (selectedItem < 0) {
                    selectedItem = items.size()-1;
                }
            break;
            case TCODK_DOWN : 
                selectedItem = (selectedItem + 1) % items.size(); 
            break;
            case TCODK_ENTER : 
                return items.get(selectedItem)->code; 
            default : break;
        }
    }
    return NONE;
}