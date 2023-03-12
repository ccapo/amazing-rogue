# 2023 7DRL Challenge Entry
* Title: Amazing Rogue
* Start with C++ Rogue-like tutorial, Part 11 by jice
  - Modified to work with libtcod v1.23.1
* Modify map generation to be a dungeon with eight levels, plus a final boss level [Final boss not implemented]
  - Rooms will be a mixture of mazes and open spaces
* Use scent/goal tracking for creature AI
* Player stats displayed in GUI
* Inventory list is scrollable (not sorted or aggregated)
    * Can use or drop items/spells
    * Inventory limit: 12
    * Armour/Weapon are automatically equipped when picked up [Not implemented]

# How to Play
* Arrow keys to move around the player, and to navigate menus
* `g` to grab an item
* `d` to drop an item
* `i` to display your inventory
* Use the mouse for targetting when casting certain spells

# How to build
* These instructions are for Ubuntu 18

## Install vcpkg
* `sudo apt install vcpkg`

## Install libtcod
* `vcpkg install libtcod`

## Build
* `make all`

## Run
* `./amazing-rogue`
