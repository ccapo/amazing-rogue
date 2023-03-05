#include "main.hpp"

Engine engine(86,53);

int main() {
   engine.load();
   while ( !TCODConsole::isWindowClosed() ) {
       engine.update();
       engine.render();
       TCODConsole::flush();
   }
   return 0;
}