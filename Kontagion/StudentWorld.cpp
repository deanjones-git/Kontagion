// Dean Jones
// 005-299-127

#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <algorithm>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_socrates(nullptr), m_pits(0), m_bacteria(0)
{
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

// Initializes Socrates and all other game objects
int StudentWorld::init()
{
	// Initialize Pit and Bacteria count at 0
	m_pits = 0;
	m_bacteria = 0;

	// Create new Socrates
	m_socrates = new Socrates(this);

	// Add L Pits at random locations (that don't overlap with other Pits)
	for (int pit = 0; pit < getLevel(); ++pit) {
		double x, y;
		getRandomPositionFromCenter(x, y);
		addActor(new Pit(this, x, y));
		m_pits++;
	}

	// Add min(5 * L, 25) Food at random locations (that don't overlap with other Food or Pits)
	for (int food = 0; food < min(5 * getLevel(), 25); ++food) {
		double x, y;
		getRandomPositionFromCenter(x, y);
		addActor(new Food(this, x, y));
	}

	list<Actor*> m_dirt;
	// Add max(180 - 20 * L, 20) Dirt piles
	// They only can't overlap with Pits and Food, so hold off on adding them to the actors list
	for (int dirt = 0; dirt < max(180 - 20 * getLevel(), 20); ++dirt) {
		double x, y;
		getRandomPositionFromCenter(x, y);
		m_dirt.push_back(new Dirt(this, x, y));
	}

	// Once every Dirt pile has been initialized with a location, they can be added to the actors list
	for (list<Actor*>::iterator dirtIt = m_dirt.begin(); dirtIt != m_dirt.end(); ++dirtIt) {
		addActor(*dirtIt);
	}

	// Start the game after initializing it
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// Let Socrates do something
	m_socrates->doSomething();

	list<Actor*>::iterator it = m_actors.begin();
	// Let all the other actors do something...
	while (it != m_actors.end()) {
		// If the actor is alive, it should do something
		if ((*it)->getAlive()) {
			(*it)->doSomething();

			// If Socrates died, tell the game
			if (!m_socrates->getAlive()) {
				return GWSTATUS_PLAYER_DIED;
			}
			// If there are no Pits or Bacteria, then the level has been completed
			if (m_pits == 0 && m_bacteria == 0) {
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
		it++;
	}

	// Remove dead actors after each tick
	it = m_actors.begin();
	while (it != m_actors.end()) {
		// If the actor is dead...
		if (!(*it)->getAlive()) {
			// Delete object referred to by the pointer and erase it from the list
			delete (*it);
			it = m_actors.erase(it);
		}
		// If the actor is alive, just move to the next element of the list
		else {
			it++;
		}
	}

	// Add fungi with chance of 1 / randInt(0, max(510 - L * 10, 200))
	int randomFungus = randInt(0, max(510 - getLevel() * 10, 200) - 1);
	if (randomFungus == 0) {
		double x, y;
		// Set its position to a random point on the circumference
		getRandomPositionCircumference(x, y);
		m_actors.push_back(new Fungus(this, x, y));
	}

	// Add new goodies with chance of 1 / randInt(0, max(510 - L * 10, 250))
	int randomGoodie = randInt(0, max(510 - getLevel() * 10, 250) - 1);
	if (randomGoodie == 0) {
		double x, y;
		// Get a random position on the circumference to drop the goodie
		getRandomPositionCircumference(x, y);
		int randomType = randInt(0, 9);

		// 60% chance of RestoreHealthGoodie
		if (randomType < 6)
			m_actors.push_back(new RestoreHealthGoodie(this, x, y));
		// 30& chance of FlamethrowerGoodie
		else if (randomType < 9)
			m_actors.push_back(new FlamethrowerGoodie(this, x, y));
		// 10% chance of ExtralifeGoodie
		else
			m_actors.push_back(new ExtraLifeGoodie(this, x, y));
	}

	// Display the game stat text

	// ostringstream will keep score as 6-digit string
	ostringstream score;
	score.fill('0');
	score << setw(6) << getScore();
	setGameStatText("Score: " + score.str() +
		"  Level: " + to_string(getLevel()) +
		"  Lives: " + to_string(getLives()) +
		"  Health: " + to_string(m_socrates->getHP()) +
		"  Sprays: " + to_string(m_socrates->getSpray()) +
		"  Flames: " + to_string(m_socrates->getFlame()));
	
	// Otherwise, just continue the game
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// Delete Socrates and all other actors
	delete m_socrates;
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		delete (*it);
	}
	// Set m_socrates to nullptr and clear the list
	m_socrates = nullptr;
	m_actors.clear();
}

Socrates* StudentWorld::getSocrates() const {
	return m_socrates;
}

void StudentWorld::addActor(Actor* a) {
	m_actors.push_back(a);
}

void StudentWorld::decPits() {
	m_pits--;
}

void StudentWorld::incBacteria() {
	m_bacteria++;
}

void StudentWorld::decBacteria() {
	m_bacteria--;
}

Actor* StudentWorld::overlapFood(double x, double y) {
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		// If the actor isn't edible, it is not of interest
		if (!(*it)->isEdible()) {
			continue;
		}

		// Otherwise, see if the food overlaps with the Bacteria
		if (pow((*it)->getX() - x, 2) + pow((*it)->getY() - y, 2) <= pow(SPRITE_WIDTH, 2)) {
			// If so, return the food pointer
			return *it;
		}
	}
	// If nothing overlapped, return the nullptr
	return nullptr;
}

Actor* StudentWorld::overlapTarget(double x, double y) {
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		// If the actor isn't a target or alive, it's not of interest
		if (!(*it)->isTarget() || !(*it)->getAlive()) {
			continue;
		}

		// Otherwise, see if the target overlaps with the Projectile
		if (pow(x - (*it)->getX(), 2) + pow(y - (*it)->getY(), 2) <= pow(SPRITE_WIDTH, 2)) {
			// If so, return the target pointer
			return (*it);
		}
	}
	// If nothing overlapped, return the nullptr
	return nullptr;
}

bool StudentWorld::overlapDirt(double x, double y) {
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		// Objects that do not block are not of interest
		if (!(*it)->blocks()) {
			continue;
		}
		// Otherwise, see if the Dirt pile overlaps with the Bacteria
		if (pow((*it)->getX() - x, 2) + pow((*it)->getY() - y, 2) <= pow(SPRITE_WIDTH / 2, 2)) {
			// If so, return true
			return true;
		}
	}
	// If nothing overlapped, return false
	return false;
}

Actor* StudentWorld::closestFood(double x, double y, double& dist) {
	Actor* food = nullptr;
	double foodDistance = 128;
	// Try to find closest food
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
		// If actor isn't edible, it isn't of interest
		if (!(*it)->isEdible()) {
			continue;
		}

		// Otherwise, get the distance
		double distance = sqrt(pow((*it)->getX() - x, 2) + pow((*it)->getY() - y, 2));
		// If it's within the initial 128 pixels or the new smaller distance of a nearby food, update that information
		if (distance <= foodDistance) {
			food = (*it);
			foodDistance = distance;
		}
	}
	dist = foodDistance;
	return food;
}

// Returns random position within 120 pixels of center that does not overlap with other actor
void StudentWorld::getRandomPositionFromCenter(double& dx, double& dy) {
	const double PI = 4 * atan(1);
	bool validCoord = false;

	// While the coordinate isn't valid...
	while (!validCoord) {
		// Get a random angle and distance
		int angle = randInt(0, 359);
		int units = randInt(0, 120);

		// Get the x and y with that angle and distance
		dx = (VIEW_WIDTH / 2 + units * cos(angle * 1.0 / 360 * 2 * PI));
		dy = (VIEW_HEIGHT / 2 + units * sin(angle * 1.0 / 360 * 2 * PI));
		
		// Initially assume the coordinate is valid
		validCoord = true;

		// Check each actor's distance
		for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {	
			// If the coordinate overlaps with the actor, the coordinate isn't valid
			// Set validCoord to false which will restart the loop
			if (pow(dx - (*it)->getX(), 2) + pow(dy - (*it)->getY(), 2) <= pow(SPRITE_WIDTH, 2)) {
				validCoord = false;
				break;
			}
		}
		// Otherwise, if validCoord is still true then there's no need to go back into the loop
	}
}

void StudentWorld::getRandomPositionCircumference(double& dx, double& dy) {
	const double PI = 4 * atan(1);
	int angle = randInt(0, 359);
	dx = (VIEW_WIDTH / 2 + VIEW_RADIUS * cos(angle * 1.0 / 360 * 2 * PI));
	dy = (VIEW_HEIGHT / 2 + VIEW_RADIUS * sin(angle * 1.0 / 360 * 2 * PI));
}