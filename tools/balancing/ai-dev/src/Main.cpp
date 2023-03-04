//------------------------------------------------------------------------
//
// Name: Main.cpp
//
// Desc: Calls and updates the game instance
//
// Modified: Chris Capobianco, 2012-09-29
//
//------------------------------------------------------------------------
#include "Main.hpp"

// Global Seed Value
int seed = 0;

int main(int narg, char *argv[])
{
	int timemax = 10*FPSMAX;
	Entity *player = new Entity(ENTITY_PLAYER);
	Entity *creature = new Entity(ENTITY_CREATURE);

	if( narg == 2 ) {

		seed = atoi(argv[1]);
		cout << "Seed = " << seed << endl;
		srand(seed);
		cout << endl;

	} else {

		srand(time(NULL));

	}

	cout << "Player:" << endl;
	cout << "Position: " << player->x << endl;
	cout << "HP: " << player->hp << endl;
	cout << endl;

	cout << "Creature:" << endl;
	cout << "Position: " << creature->x << endl;
	cout << "HP: " << creature->hp << endl;
	cout << endl;

	// Loop through a few Update calls
	for(int frame = 1; frame <= timemax; frame++) {
		double r = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
		if( r < 0.25 ) {
			//cout << "Player Attacks" << endl;
			player->nattack++;
			creature->hp--;
			if( creature->hp <= 0 ) {
				cout << "The Creature Died!" << endl;
				delete creature;
				creature = new Entity(ENTITY_CREATURE);
				player->nkill++;
			}
		}

		if( frame % FPSMAX == 0 ) {
			player->raggression = static_cast<double>(player->nattack)/static_cast<double>(FPSMAX);
			player->nattack = 0;
			cout << "Player Aggression Rate = " << player->raggression << " @ time = " << static_cast<double>(frame)/static_cast<double>(FPSMAX) << " s" << endl;
		}
	}

	cout << "Player Killed " << player->nkill << " Creatures" << endl;

	delete player, creature;

	return 0;
}
