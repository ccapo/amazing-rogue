#include <stdio.h>
#include <stdarg.h>
#include "main.hpp"

static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH+2;
static const int MSG_HEIGHT = PANEL_HEIGHT-1;

Gui::Gui() {
    con = new TCODConsole(engine.screenWidth,PANEL_HEIGHT);
    status = new TCODConsole(engine.screenWidth,2);
}

Gui::~Gui() {
    delete con;
    delete status;
    clear();
}

void Gui::render() {
    // clear the GUI console
    con->setDefaultBackground(TCODColor::black);
    con->clear();
    status->setDefaultBackground(TCODColor::black);
    status->clear();

    // draw the health bar
    renderBar(1,1,BAR_WIDTH,"HP",engine.player->entity->hp,
        engine.player->entity->maxHp,
        TCODColor::lightRed,TCODColor::darkerRed);

    // dungeon level
    status->setDefaultForeground(TCODColor::white);
    status->print(BAR_WIDTH + 2,1,"Lvl: %d",engine.level + 1);

    // draw the message log
    int y = 1;
    float colorCoef = 0.4f;
    for (Message **it = log.begin(); it != log.end(); it++) {
        Message *message=*it;
        con->setDefaultForeground(message->col * colorCoef);
        con->print(MSG_X,y,message->text);
        y++;
        if ( colorCoef < 1.0f ) {
            colorCoef += 0.3f;
        }
    }

    // mouse look
    renderMouseLook();

    con->setDefaultForeground(TCODColor::white);

    // blit the GUI console on the root console
    TCODConsole::blit(con,0,0,engine.screenWidth,PANEL_HEIGHT,TCODConsole::root,0,engine.screenHeight-PANEL_HEIGHT);
    // blit the GUI console on the root console
    TCODConsole::blit(status,0,0,engine.screenWidth,3,TCODConsole::root,0,0);
}

void Gui::renderBar(int x, int y, int width, const char *name,
    float value, float maxValue, const TCODColor &barColor,
    const TCODColor &backColor) {
    // fill the background
    status->setDefaultBackground(backColor);
    status->rect(x,y,width,1,false,TCOD_BKGND_SET);

    int barWidth = (int)(value / maxValue * width);
    if ( barWidth > 0 ) {
        // draw the bar
        status->setDefaultBackground(barColor);
        status->rect(x,y,barWidth,1,false,TCOD_BKGND_SET);
    }

    // print text on top of the bar
    status->setDefaultForeground(TCODColor::white);
    status->printEx(x+width/2,y,TCOD_BKGND_NONE,TCOD_CENTER,
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
    con->setDefaultForeground(TCODColor::lightGrey);
    con->print(1,0,buf);
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
const int PAUSE_MENU_HEIGHT = 15;
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
        menuy += 3;
    } else {
        static TCODImage img("data/img/main_menu.png");
        img.blit2x(TCODConsole::root,0,0);
        menux = 10;
        menuy = TCODConsole::root->getHeight()/3;
    }

    while( !TCODConsole::isWindowClosed() ) {
        int currentItem = 0;
        TCODConsole::root->setDefaultForeground(TCODColor::lightRed);
        TCODConsole::root->print(menux, menuy, "Amazing Rogue");
        for (MenuItem **it = items.begin(); it!=items.end(); it++) {
            if ( currentItem == selectedItem ) {
                TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
            } else {
                TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
            }
            TCODConsole::root->print(menux,menuy+6+currentItem*3,(*it)->label);
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