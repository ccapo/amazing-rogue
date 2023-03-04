#include <iostream>
#include <chrono>
#include <random>
#include <functional>

struct entity_t {
  std::string name;
  int hp, hpmax;
  int atk;
  int def;
  entity_t(std::string n, int h, int a, int d): name(n), hp(h), hpmax(h), atk(a), def(d) {}
  entity_t(): name(""), hp(0), hpmax(0), atk(0), def(0) {}
  ~entity_t() {}

  void status() {
    if (hp <= 0) {
      std::cout << name << " died" << std::endl;
    } else {
      std::cout << name << ": " << hp << "/" << hpmax << std::endl;
    }
  }

  void attackedBy(entity_t o) {
    int dmg = damage(o);
    if (hp - dmg <= 0) {
      hp = 0;
    } else {
      if (dmg > 0) {
        hp -= dmg;
      }
    }
  }

  int damage(entity_t o) {
    int dmg = (o.atk - def/2)/2;
    if (dmg <= 0) {
      dmg = 0;
    }
    return dmg;
  }
};

struct combat_results_t {
  int iterations;
  entity_t winner;
  entity_t loser;
  combat_results_t(): iterations(0), winner(entity_t()), loser(entity_t()) {}
};

std::ostream& operator << (std::ostream &o, const combat_results_t &cr) {
  o << cr.iterations << " ";
  o << cr.winner.hpmax << " " << cr.winner.atk << " " << cr.winner.def << " ";
  o << cr.loser.hpmax << " " << cr.loser.atk << " " << cr.loser.def; 
  return o;
}

combat_results_t singleCombat(int php, int patk, int pdef, int chp, int catk, int cdef) {
  entity_t p("Player", php, patk, pdef);
  entity_t c("Creature", chp, catk, cdef);
  combat_results_t cr;
  int iterations = 0;

  int pMaxDmg = c.damage(p);
  int cMaxDmg = p.damage(c);

  // Both p and c cannot cause any damage
  // This would result in an infinite loop
  if (pMaxDmg <= 0 && cMaxDmg <= 0) {
    return cr;
  }

  while (p.hp > 0 && c.hp > 0) {
    c.attackedBy(p);
    p.attackedBy(c);
    iterations++;
  }

  if (p.hp > 0) {
    cr.winner = p;
    cr.loser = c;
  } else {
    cr.winner = c;
    cr.loser = p;
  }
  cr.iterations = iterations;

  return cr;
}

int main() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> idist(1, 15);
  auto irandom = std::bind(idist, rng);
  auto ncombats = 0L;
  auto nMaxCombats = 1000000;

  //std::cout << "# Iterations, wHP, wATK, wDEF, lHP, lATK, lDEF" << std::endl;
  /*while (ncombats < nMaxCombats) {
    auto k1 = irandom();
    auto k2 = irandom();
    auto k3 = irandom();
    auto k4 = irandom();
    auto k5 = irandom();
    auto k6 = irandom();
    auto cr = singleCombat(k1, k2, k3, k4, k5, k6);
    if (cr.iterations > 0) {
      std::cout << cr << std::endl;
      ncombats++;
    }
  }*/
  
  for (auto ph = 10; ph <= 100; ph += 5) {
    for (auto pa = 2; pa <= 20; pa += 2) {
      for (auto pd = 2; pd <= 20; pd += 2) {
        for (auto ch = 5; ch <= 100; ch += 5) {
          for (auto ca = 2; ca <= 20; ca += 2) {
            for (auto cd = 2; cd <= 20; cd += 2) {
              auto cr = singleCombat(ph, pa, pd, ch, ca, cd);
              if (cr.iterations > 0) {
                ncombats++;
                std::cout << cr << std::endl;
              }
            }
          }
        }
      }
    }
  }

  std::cerr << "# of combats simulated = " << ncombats << std::endl;

  return 0;
}
