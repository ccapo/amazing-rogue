# 2023 7DRL Challenge Entry
* Title: Amazing Rogue
* Start with C++ Rogue-like tutorial, Part 11 by jice
  - Modified to work with libtcod v1.23.1
* Modify map generation to be a dungeon with eight levels, plus a final boss level
  - Rooms will be a mixture of mazes and open spaces
* Use scent/goal tracking for creature AI
* Player stats displayed in GUI
* Inventory list is scrollable (not sorted or aggregated)
    * Can use or drop items/spells
    * Inventory limit: 12
    * Armour/Weapon/Relics are automatically equipped when picked up