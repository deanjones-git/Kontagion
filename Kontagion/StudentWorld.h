// Dean Jones
// 005-299-127

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Socrates;

class StudentWorld : public GameWorld
{
public:
	// Construct student world with assetPath string
	StudentWorld(std::string assetPath);

	// Calls cleanUp to free dynamically allocated memory
	~StudentWorld();

	// Populates level with proper number of objects
	virtual int init();

	// Calls doSomething for every actor each tick and update top stats
	virtual int move();

	// Free everything in m_actors
	virtual void cleanUp();

	// Return m_socrates
	Socrates* getSocrates() const;

	// Add actor to m_actors;
	void addActor(Actor* a);

	// Let StudentWorld know a Pit died
	void decPits();

	// Increment or decrement Bacteria count
	void incBacteria();
	void decBacteria();

	// Returns pointer for Food object that Bacteria overlaps with (or nullptr otherwise)
	Actor* overlapFood(double x, double y);
	// Returns pointer for closest Food object within 128 pixels of Bacteria (or nullptr otherwise) and its distance
	Actor* closestFood(double x, double y, double& dist);

	// Returns pointer for target object that Projectile overlaps with (or nullptr otherwise)
	Actor* overlapTarget(double x, double y);

	// Returns boolean for Dirt object that Bacteria could overlap with (or nullptr otherwise)
	bool overlapDirt(double x, double y);

private:
	// Helper functions to get random open position from center and any random position on circumference
	void getRandomPositionFromCenter(double& dx, double& dy);
	void getRandomPositionCircumference(double& dx, double& dy);

	// Pointers for Socrates and other actors
	Socrates* m_socrates;
	std::list<Actor*> m_actors;

	// Pit and Bacteria counts
	int m_pits, m_bacteria;
};

#endif // STUDENTWORLD_H_
