// Dean Jones
// 005-299-127

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>

class StudentWorld;

class Actor : public GraphObject {
public:
	// Actor object will be base class for all videogame objects and needs the following properties
	Actor(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth, double size, bool target, bool edible, bool blocks) : m_alive(true), m_sw(sw), m_target(target), m_edible(edible), m_blocks(blocks),
		GraphObject(imageID, startX, startY, dir, depth, size) {}
	virtual ~Actor() {} // virtual destructor
	// doSomething() controls action for each tick
	virtual void doSomething() = 0;

	// return pointer to StudentWorld
	StudentWorld* getWorld() const;

	// return alive status
	bool getAlive() const;

	// set to dead
	void setDead();

	// damage by input amount - default now will just call setDead() because it's unknown whether Actor has hp
	virtual void damage(int hp);

	// tell spray/flame whether actor should be damaged
	bool isTarget() const;

	// tell whether actor is edible (is Food)
	bool isEdible() const;

	// tell whether actors blocks movement (is Dirt)
	bool blocks() const;
private:
	// StudentWorld pointer, alive status, and target status
	StudentWorld* m_sw;
	bool m_alive;
	bool m_target;
	bool m_edible;
	bool m_blocks;
};

// Player class
class Socrates : public Actor {
public:
	// Initialize Socrates object at 180 degree position in dish
	Socrates(StudentWorld* sw) :
		Actor(sw, IID_PLAYER, VIEW_WIDTH / 2 - VIEW_RADIUS, VIEW_HEIGHT / 2, 0, 0, 1.0, false, false, false), m_spray(20), m_flame(5), m_hp(100) {}
	virtual ~Socrates() {} // virtual destructor
	virtual void doSomething();
	virtual void damage(int hp);

	// Get private member variables 
	int getHP() const;
	int getSpray() const;
	int getFlame() const;

	// Update member variables when goodies are taken
	void restoreHealth();
	void increaseFlame();
private:
	// Move around dish 5 degrees clockwise or counterclockwise
	void moveRotational(int dir, double startX, double startY, int radius);
	// Get angle of initial position in dish
	double getAngle(double startX, double startY, int radius) const;
	// Helper function to get radial position to move to 
	void getRadialPosition(double angle, int radius, double& endX, double& endY) const;

	// Private member variables
	int m_spray, m_flame, m_hp;
};

// Dirt pile class
class Dirt : public Actor {
public:
	Dirt(StudentWorld* sw, double startX, double startY) :
		Actor(sw, IID_DIRT, startX, startY, 0, 1, 1.0, true, false, true) {}
	virtual ~Dirt() {} // virtual destructor
	virtual void doSomething();
};

class Pit : public Actor {
public:
	Pit(StudentWorld* sw, double startX, double startY) :
		Actor(sw, IID_PIT, startX, startY, 0, 1, 1.0, false, false, false), m_salmonella(5), m_aggressive(3), m_ecoli(2) {}
	virtual ~Pit() {} // virtual destructor
	virtual void doSomething();
private:
	int m_salmonella, m_aggressive, m_ecoli;
};

// Bacteria abstract class (Aggressive Salmonella, Salmonella, and Ecoli)
class Bacteria : public Actor {
public:
	Bacteria(StudentWorld* sw, double startX, double startY, int imageID, int damage, int hp, int hurtSound, int deadSound) :
		Actor(sw, imageID, startX, startY, 90, 0, 1.0, true, false, false), m_damage(damage), m_plan(0), m_hp(hp), m_food(0), m_hurtSound(hurtSound), m_deadSound(deadSound) {}
	virtual ~Bacteria() {} // virtual destructor
	virtual void doSomething();
	virtual void damage(int hp);

	// Get planned distance and set it for derived classes
	int getPlan() const;
	void setPlan(int plan);

private:
	// Specialized actions before and after the core overlap-with-Socrates, divide, or eat food sequence defined
	virtual void specialActionA() = 0;
	virtual void specialActionB() = 0;

	// Clone object adds a new Bacteria when enough food (3) has been eaten
	virtual void clone(double x, double y) = 0;
	int m_damage, m_plan, m_hp, m_food, m_hurtSound, m_deadSound;
};

// Salmonella is a Bacteria that does 1 damage and has 4 hp
class Salmonella : public Bacteria {
public:
	Salmonella(StudentWorld* sw, double startX, double startY) :
		Bacteria(sw, startX, startY, IID_SALMONELLA, 1, 4, SOUND_SALMONELLA_HURT, SOUND_SALMONELLA_DIE) {}
	virtual ~Salmonella() {} // virtual destructor
private:
	virtual void specialActionA();
	virtual void specialActionB();
	virtual void clone(double x, double y);
	// Sets Salmonella direction randomly and m_plan to 10
	void moveRandom();
};

// AggressiveSalmonella is a Bacteria that does 2 damage and has 5 hp
class AggressiveSalmonella : public Bacteria {
public:
	AggressiveSalmonella(StudentWorld* sw, double startX, double startY) :
		Bacteria(sw, startX, startY, IID_SALMONELLA, 2, 5, SOUND_SALMONELLA_HURT, SOUND_SALMONELLA_DIE), m_socratesLock(false) {}
	virtual ~AggressiveSalmonella() {} // virtual destructor

private:
	virtual void specialActionA();
	virtual void specialActionB();
	virtual void clone(double x, double y);

	// Sets AggressiveSalmonella direction randomly and m_plan to 10
	void moveRandom();

	// If Socrates is within 72 pixels
	bool m_socratesLock;
};

// Ecoli is a Bacteria that does 4 damage and has 5 hp
class Ecoli : public Bacteria {
public:
	Ecoli(StudentWorld* sw, double startX, double startY) : Bacteria(sw, startX, startY, IID_ECOLI, 4, 5, SOUND_ECOLI_HURT, SOUND_ECOLI_DIE) {}
	virtual ~Ecoli() {} // virtual destructor
private:
	virtual void specialActionA();
	virtual void specialActionB();
	virtual void clone(double x, double y);
};

// Chance is an abstract base class for all the Goodies and Fungus classes
class Chance : public Actor {
public:
	// Constructor takes in value of score increase for Chance actor and whether it's a Goodie (to play appropriate sounds)
	Chance(StudentWorld* sw, double startX, double startY, int imageID, int scoreIncrease, bool goodie) :
		// Lifetime is always given by max(randInt(0, 300 - 10 * L - 1), 50) for all Chance
		Actor(sw, imageID, startX, startY, 0, 1, 1.0, true, false, false), m_scoreIncrease(scoreIncrease), m_goodie(goodie), m_lifetime(std::max(randInt(0, 300 - 10 * (sw->getLevel()) - 1), 50)) {}
	virtual ~Chance() {} // virtual destructor
	virtual void doSomething();
private:
	int m_scoreIncrease;
	bool m_goodie;
	int m_lifetime;
	// Specialized action for each Goodie or Fungus when it overlaps with Socrates
	virtual void action() = 0;
};

// ExtraLifeGoodie is a Goodie, has a score increase of 500, and should increase Socrates' life count when overlapping with Socrates
class ExtraLifeGoodie : public Chance {
public:
	ExtraLifeGoodie(StudentWorld* sw, double startX, double startY) :
		Chance(sw, startX, startY, IID_EXTRA_LIFE_GOODIE, 500, true) {}
	virtual ~ExtraLifeGoodie() {} // virtual destructor
private:
	// Calls incLives() of StudentWorld
	virtual void action();
};

// FlamethrowerGoodie is a Goodie, has a score increase of 300, and should add 5 flamethrower charges when overlapping with Socrates
class FlamethrowerGoodie : public Chance {
public:
	FlamethrowerGoodie(StudentWorld* sw, double startX, double startY) :
		Chance(sw, startX, startY, IID_FLAME_THROWER_GOODIE, 300, true) {}
	virtual ~FlamethrowerGoodie() {} // virtual destructor
private:
	// Calls increaseFlame() for Socrates
	virtual void action();
};

// Fungus is not a Goodie, has a score increase of -50, and damages Socrates by 20 when they touch
class Fungus : public Chance {
public:
	Fungus(StudentWorld* sw, double startX, double startY) :
		Chance(sw, startX, startY, IID_FUNGUS, -50, false) {}
	virtual ~Fungus() {} // virtual destructor
private:
	// Calls damage(20) for Socrates
	virtual void action();
};

// RestoreHealthGoodie is a Goodie, has score increase of 250, and should restore health when overlapping with Socrates
class RestoreHealthGoodie : public Chance {
public:
	RestoreHealthGoodie(StudentWorld* sw, double startX, double startY) :
		Chance(sw, startX, startY, IID_RESTORE_HEALTH_GOODIE, 250, true) {}
	virtual ~RestoreHealthGoodie() {} // virtual destructor
private:
	// Calls restoreHealth() for Socrates
	virtual void action();
};

// Projectile class for Flames and Sprays
class Projectile : public Actor {
public:
	Projectile(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int maxTravel, int damage) :
		Actor(sw, imageID, startX, startY, dir, 1, 1.0, false, false, false), m_maxTravel(maxTravel), m_travel(0), m_damage(damage) {}
	virtual ~Projectile() {} // virtual destructor
	virtual void doSomething();
private:
	int m_maxTravel, m_travel, m_damage;
};

// Flame class (inherits from Projectile)
class Flame : public Projectile {
public:
	Flame(StudentWorld* sw, double startX, double startY, Direction dir) :
		Projectile(sw, IID_FLAME, startX, startY, dir, 32, 5) {}
	virtual ~Flame() {} // virtual destructor
};

// Spray class (inherits from Projectile)
class Spray : public Projectile {
public:
	Spray(StudentWorld* sw, double startX, double startY, Direction dir) :
		Projectile(sw, IID_SPRAY, startX, startY, dir, 112, 2) {}
	virtual ~Spray() {} // virtual destructor
}; 

// Food class for Bacteria to eat
class Food : public Actor {
public:
	Food(StudentWorld* sw, double startX, double startY) :
		Actor(sw, IID_FOOD, startX, startY, 90, 1, 1.0, false, true, false) {}
	virtual ~Food() {} // virtual destructor
	virtual void doSomething();
};

#endif // ACTOR_H_
