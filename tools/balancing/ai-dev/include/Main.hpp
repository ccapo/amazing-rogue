#ifndef MAIN_HPP
#define MAIN_HPP
//------------------------------------------------------------------------
//
// Name: Main.hpp
//
// Desc: A list of all the header files
//
// Author: Christopher Capobianco, 2012-09-29
//
//------------------------------------------------------------------------
#include "Global.hpp"

// Global Seed Value
extern int seed;

class Entity {

	public:
	int x;
	int hp;
	int nattack,nkill;
	double raggression;

	Entity(): x(0), hp(0), nattack(0), nkill(0), raggression(0.0) {}
	Entity( int type, int pos = 0 ): x(pos), nattack(0), nkill(0), raggression(0.0) {
		switch( type ) {
			case ENTITY_PLAYER: {
				hp = 100;
				break;
			}
			case ENTITY_CREATURE: {
				hp = 24;
				break;
			}
			default: break;
	
		}
	}
};

#endif
