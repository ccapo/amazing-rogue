#ifndef GLOBAL_HPP
#define GLOBAL_HPP
//------------------------------------------------------------------------
//
// Name: Global.hpp
//
// Description: Global constants
//
// Author: Chris Capobianco, 2012-09-29
//
//------------------------------------------------------------------------
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <typeinfo>
#include <unistd.h>
#include <vector>

using namespace std;

// Version
#define VERSION "1.0.0"

// Size of the map portion shown on-screen
#define DISPLAY_WIDTH 70
#define DISPLAY_HEIGHT 42
//#define DISPLAY_WIDTH 120
//#define DISPLAY_HEIGHT 72

// Size of the window
#define SCREEN_WIDTH (DISPLAY_WIDTH)
#define SCREEN_HEIGHT (5*DISPLAY_HEIGHT/4 + 5)

// Size of the map
#define MAP_WIDTH (2*DISPLAY_WIDTH)
#define MAP_HEIGHT (2*DISPLAY_HEIGHT)

// Maximum frame rate
#define FPSMAX 24

// Max. size of lists
#define NCREATUREMAX 1024

// Max. of messages
#define NMSGS 8

// Number of cells that border a cell
#define NBORDER 8

// Maximum size for character strings
#define CHARMAX 128

// List of Entities
enum EntityType
{
	ENTITY_DEFAULT = -1,
	ENTITY_PLAYER,
	ENTITY_CREATURE
};

#endif
