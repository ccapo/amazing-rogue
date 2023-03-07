#include "main.hpp"

Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT);

int main() {
   engine.load();
   while ( !TCODConsole::isWindowClosed() ) {
       engine.update();
       engine.render();
       TCODConsole::flush();
   }
   return 0;
}