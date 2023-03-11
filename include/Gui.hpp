#define PANEL_HEIGHT (11)

class Menu {
public :
    enum MenuItemCode {
        NONE,
        NEW_GAME,
        EXIT,
        CONSTITUTION,
        STRENGTH,
        AGILITY,
        MIND,
        ACUMEN
    };
    enum DisplayMode {
        MAIN,
        PAUSE
    };
    ~Menu();
    void clear();
    void addItem(MenuItemCode code, const char *label);
    MenuItemCode pick(DisplayMode mode = MAIN);
protected :
    struct MenuItem {
        MenuItemCode code;
        const char *label;
    };
    TCODList<MenuItem *> items;
};

class Gui {
public :
    Gui();
    ~Gui();
    void render();
    void clear();
    void message(const TCODColor &col, const char *text, ...);
    Menu menu;

protected :
    TCODConsole *conMsg;
    TCODConsole *conStatus;
    TCODConsole *conMiniMap;
    struct Message {
        char *text;
        TCODColor col;
        Message(const char *text, const TCODColor &col);
        ~Message();
    };
    TCODList<Message *> log;
 
    void renderBar(int x, int y, int width, const char *name,
        float value, float maxValue, const TCODColor &barColor,
        const TCODColor &backColor);
    void renderMouseLook();
};