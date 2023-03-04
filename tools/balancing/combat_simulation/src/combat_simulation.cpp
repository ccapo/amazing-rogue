#include <cstdlib>
#include <iostream>
#include <ctime>

#define STATMAX 100

using namespace std;

// Stats struct
struct Stats {
  int str, dex, wis;
  int hp, atk, def;
  double patk, pdef;
  Stats(): str(0), dex(0), wis(0), hp(0), atk(0), def(0), patk(0.0), pdef(0.0) {}
  Stats(int _str, int _dex, int _wis): str(_str), dex(_dex), wis(_wis) {
    hp = 5*(_str + _dex);
    atk = (_str + _dex + _wis)/3;
    def = (_str + _dex + _wis)/3 - 1;
    patk = 0.25 + 0.5*static_cast<double>(_str - 1)/static_cast<double>(STATMAX - 1);
    pdef = 0.25 + 0.5*static_cast<double>(_dex - 1)/static_cast<double>(STATMAX - 1);
  }
};

// Prototypes
int combat_outcome(Stats stats_a, Stats stats_b);
double random_double();

//============================================================================
//
//  Purpose:
//
//    COMBAT_SIMULATION simulates a battle.
//
//  Modified:
//
//    2016-07-29
//
//  Author:
//
//    Chris Capobianco
//
//============================================================================
int main(int narg, char *argv[]) {
  int seed, a_wins = 0, b_wins = 0, ncombats = 1000000;
  double a_prob, b_prob;

  Stats entity_a(3, 3, 3);
  Stats entity_b(2, 2, 2);

  cout << "Combat Simulation:\n\n";

  cout << "Entity A:\n";
  cout << "HP: " << entity_a.hp << ", ATK: " << entity_a.atk << ", DEF: " << entity_a.def << ", ATK %: " << 100.0*entity_a.patk << ", DEF %: " << 100.0*entity_a.pdef << "\n\n";

  cout << "Entity B:\n";
  cout << "HP: " << entity_b.hp << ", ATK: " << entity_b.atk << ", DEF: " << entity_b.def << ", ATK %: " << 100.0*entity_b.patk << ", DEF %: " << 100.0*entity_b.pdef << "\n";

  seed = time(0);
  srand(seed);

  for(int i = 0; i < ncombats; i++) {
    if(combat_outcome(entity_a, entity_b) == 1) {
      a_wins++;
    } else {
      b_wins++;
    }
  }

  a_prob = static_cast<double>(a_wins)/static_cast<double>(ncombats);
  b_prob = static_cast<double>(b_wins)/static_cast<double>(ncombats);

  cout << "\n";
  cout << "Entity A Wins with Probability: " << a_prob << "\n";
  cout << "Entity B Wins with Probability: " << b_prob << "\n";

  return 0;
}

//============================================================================
//
//  Purpose:
//
//    combat_outcome returns the outcome of a single battle.
//
//  Modified:
//
//    2016-07-29
//
//  Author:
//
//    Chris Capobianco
//
//============================================================================
int combat_outcome(Stats stats_a, Stats stats_b) {
  int outcome, damage;
  double r;

  // The battle continues until one entity's HP reaches zero.
  while(stats_a.hp > 0 || stats_b.hp > 0) {
    // Determine if entity A is able to attack
    r = random_double();
    if(r <= stats_a.patk) {
      // Set damage to attack power of entity A
      damage = stats_a.atk;

      r = random_double();
      if(r <= stats_b.pdef) damage -= stats_b.def;
      if(damage > 0) stats_b.hp -= damage;
      if(stats_b.hp <= 0) {
        outcome = 1;
        break;
      }
    }

    r = random_double();
    if(r <= stats_b.patk) {
      damage = stats_b.atk;
      r = random_double();
      if(r <= stats_a.pdef) damage -= stats_a.def;
      if(damage > 0) stats_a.hp -= damage;
      if(stats_a.hp <= 0) {
        outcome = 2;
        break;
      }
    }
  }

  return outcome;
}

//============================================================================
//
//  RANDOM_DOUBLE returns a random number between 0 and 1.
//
//============================================================================
double random_double() {
  return static_cast<double>(rand())/static_cast<double>(RAND_MAX);
}
