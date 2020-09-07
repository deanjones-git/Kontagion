// Dean Jones
// 005-299-127

#include "Actor.h"
#include "StudentWorld.h"
#include <list>
#include <math.h>
using namespace std;

// Simply return or set private variables
StudentWorld* Actor::getWorld() const {
	return m_sw;
}
bool Actor::getAlive() const {
	return m_alive;
}
void Actor::setDead() {
	m_alive = false;
}

void Actor::damage(int hp) {
	setDead();
}

bool Actor::isTarget() const {
	return m_target;
}
bool Actor::isEdible() const {
	return m_edible;
}
bool Actor::blocks() const {
	return m_blocks;
}

void Socrates::doSomething() {
	// If dead, don't do anything
	if (m_hp <= 0) {
		setDead();
		return;
	}

	// Get the last key that was pressed
	int key;
	if (getWorld()->getKey(key)) {
		switch (key) {
			// Move ccw
			case (KEY_PRESS_LEFT):
				moveRotational(1, getX(), getY(), VIEW_RADIUS);
				break;

			// Move cw
			case (KEY_PRESS_RIGHT):
				moveRotational(-1, getX(), getY(), VIEW_RADIUS);
				break;

			// Space key adds spray object
			case (KEY_PRESS_SPACE):
				if (m_spray > 0) {
					double endX, endY;
					getPositionInThisDirection(getDirection(), SPRITE_WIDTH, endX, endY);
					getWorld()->addActor(new Spray(getWorld(), endX, endY, getDirection()));
					m_spray--;
					getWorld()->playSound(SOUND_PLAYER_SPRAY);
				}
				break;

			// Enter key adds 16 flames in 22 degree increments
			case (KEY_PRESS_ENTER):
				if (m_flame > 0) {
					for (int flame = 0; flame < 16; flame++) {
						double x, y;
						getPositionInThisDirection(getDirection() + flame * 22, SPRITE_WIDTH, x, y);
						getWorld()->addActor(new Flame(getWorld(), x, y, getDirection() + flame * 22));
					}
					m_flame--;
					getWorld()->playSound(SOUND_PLAYER_FIRE);
				}
				break;
		}
	}
	// If the player did not press a key, start to refill sprays
	else {
		if (m_spray < 20) {
			m_spray++;
		}
	}
}

void Socrates::damage(int hp) {
	m_hp -= hp;

	// If still alive...
	if (m_hp > 0) {
		getWorld()->playSound(SOUND_PLAYER_HURT);
		return;
	}
	// If dead...
	setDead();
	getWorld()->playSound(SOUND_PLAYER_DIE);
	getWorld()->decLives();
}

int Socrates::getHP() const {
	return m_hp;
}

int Socrates::getSpray() const {
	return m_spray;
}

int Socrates::getFlame() const {
	return m_flame;
}

void Socrates::restoreHealth() {
	m_hp = 100;
}

void Socrates::increaseFlame() {
	m_flame += 5;
}

// Move clockwise or counterclockwise 5 degrees
void Socrates::moveRotational(int dir, double startX, double startY, int radius) {
	// Get PI for further use
	const double PI = atan(1) * 4;
	// Get the angle of initial position
	double angle = getAngle(startX, startY, radius);

	// Update endX and endY - where Socrates should move
	double endX, endY;
	// Also, update direction of Socrates so that he always faces the center
	switch (dir) {
	case(1):
		getRadialPosition(angle + PI / 36, radius, endX, endY);
		setDirection(getDirection() + 5);
		break;
	case(-1):
		getRadialPosition(angle - PI / 36, radius, endX, endY);
		setDirection(getDirection() - 5);
		break;
	}
	// Move to the new position
	moveTo(endX, endY);
}

// Get the angle Socrates is initially at in the Petri dish
double Socrates::getAngle(double startX, double startY, int radius) const {
	const double PI = atan(1) * 4;

	// Get the acos and asin
	startX -= radius;
	startY -= radius;
	double arccos = acos(startX / radius);
	double arcsin = asin(startY / radius);

	// If asin is positive, the angle is simply acos
	if (0 <= arcsin) {
		return arccos;
	}

	// Else the angle is > PI
	return (2 * PI - arccos);
}

// Calculate coordinates of new position on circumference
void Socrates::getRadialPosition(double angle, int radius, double& endX, double& endY) const {
	endX = (VIEW_WIDTH / 2 + radius * cos(angle));
	endY = (VIEW_HEIGHT / 2 + radius * sin(angle));
}

// Dirt doesn't do anything
void Dirt::doSomething() {
	return;
}

void Pit::doSomething() {
	// If Pit has emitted all its Bacteria
	if (m_salmonella == 0 && m_aggressive == 0 && m_ecoli == 0) {
		getWorld()->decPits();
		setDead();
		return;
	}
	// Pit has a 1/50 chance to emit Bacteria 
	int emitChance = randInt(0, 49);
	if (emitChance == 0) {
		bool emitted = false;
		while (!emitted) {
			// Pick 1 of the 3 Bacteria types to randomly emit
			int randType = randInt(0, 2);
			switch (randType) {
				case(0):
					// If there are salmonella left in the Pit, emit one
					if (m_salmonella > 0) {
						getWorld()->addActor(new Salmonella(getWorld(), getX(), getY()));
						m_salmonella--;
						emitted = true;
					}
					break;
				case(1):
					// If there are aggressive salmonella left in the Pit, emit one
					if (m_aggressive > 0) {
						getWorld()->addActor(new AggressiveSalmonella(getWorld(), getX(), getY()));
						m_aggressive--;
						emitted = true;
					}
					break;
				case(2):
					// If there are ecoli left in the Pit, emit one
					if (m_ecoli > 0) {
						getWorld()->addActor(new Ecoli(getWorld(), getX(), getY()));
						m_ecoli--;
						emitted = true;
					}
					break;
			}
			// If the selected type wasn't still in the Pit, go back into the loop and randomly select a type again
		}
		// Tell StudentWorld a Bacteria was unleashed and play the appropriate sound
		getWorld()->incBacteria();
		getWorld()->playSound(SOUND_BACTERIUM_BORN);
	}
}


void Bacteria::doSomething() {
	// If dead, don't do anything
	if (!getAlive()) {
		return;
	}

	// Perform Bacteria's first unique action
	specialActionA();

	// Get Socrates' coordinates to calculate distance from Bacteria
	Socrates* m_socrates = getWorld()->getSocrates();
	double socrates_x, socrates_y;
	socrates_x = m_socrates->getX();
	socrates_y = m_socrates->getY();
	double x, y;
	x = getX();
	y = getY();
	double distance = sqrt(pow(socrates_x - x, 2) + pow(socrates_y - y, 2));

	// If Socrates and Bacteria are touching, damage Socrates 
	if (distance <= SPRITE_WIDTH) {
		m_socrates->damage(m_damage);
	}

	// Else, if the Bacteria ate enough food, perform binary fission
	else if (m_food >= 3) {

		// newX will be x plus SPRITE_WIDTH if x < VIEW_WIDTH/2, x minus that if x >, and the same x if x is the center
		double newX;
		newX = x + (VIEW_WIDTH / 2 - x) / abs(VIEW_WIDTH / 2 - x) * SPRITE_WIDTH / 2;

		// newY will be y plus SPRITE_WIDTH if y < VIEW_HEIGHT/2, y minus that if y >, and the same y if y is the center
		double newY;
		newY = y + (VIEW_HEIGHT / 2 - y) / abs(VIEW_HEIGHT / 2 - y) * SPRITE_WIDTH / 2;

		//  Add a new Bacteria and reset food-eaten count
		clone(newX, newY);
		getWorld()->incBacteria();
		m_food -= 3;
	}

	// Check if Bacteria overlaps with food objects
	else {
		// If a food overlapped, set it to dead (eaten) and increase the food eaten count
		Actor* food = getWorld()->overlapFood(x, y);
		if (food != nullptr) {
			food->setDead();
			m_food++;
		}
	}

	// Perform Bacteria's second unique action
	specialActionB();
}

void Bacteria::damage(int hp) {
	// Decrease hp
	m_hp -= hp;

	// If still alive, just play the hurt sound
	if (m_hp > 0) {
		getWorld()->playSound(m_hurtSound);
		return;
	}
	// If dead, set dead, play the dead sound, increase the score, and transform into nutrients if lucky
	setDead();
	getWorld()->playSound(m_deadSound);
	getWorld()->increaseScore(100);
	int foodChance = randInt(0, 1);
	if (foodChance == 0) {
		getWorld()->addActor(new Food(getWorld(), getX(), getY()));
	}
	// Also, tell the StudentWorld
	getWorld()->decBacteria();
}


int Bacteria::getPlan() const {
	return m_plan;
}

void Bacteria::setPlan(int plan) {
	m_plan = plan;
}

void Salmonella::specialActionA() {
	return;
}

void Salmonella::specialActionB() {

	// Try to move in current direction
	if (getPlan() > 0) {
		setPlan(getPlan() - 1);

		// Get new position to move to
		double endX, endY;
		getPositionInThisDirection(getDirection(), 3, endX, endY);

		// If the move is out of bounds, choose a random direction to move
		if (pow(endX - VIEW_WIDTH / 2, 2) + pow(endY - VIEW_HEIGHT / 2, 2) >= pow(VIEW_RADIUS, 2)) {
			// Pick a random direction and try moving 10 ticks in that direction
			moveRandom();
			return;
		}

		// If position is blocked by dirt pile, move randomly
		if (getWorld()->overlapDirt(endX, endY)) {
			moveRandom();
			return;
		}

		// The move was valid if it wasn't out of bounds or passing through a dirt pile
		moveTo(endX, endY);
		return;
	}

	// If m_plan was 0, try to get the closest Food within 128 pixels and its distance
	double x, y;
	x = getX();
	y = getY();
	double foodDistance;
	Actor* food = getWorld()->closestFood(x, y, foodDistance);

	// If food was found, try to move toward it
	if (food != nullptr) {

		// Get direction of that food and set Salmonella's direction to that
		const double PI = atan(1) * 4;
		double diffX = food->getX() - x;
		double diffY = food->getY() - y;
		double arccos = acos(diffX / foodDistance);
		double arcsin = asin(diffY / foodDistance);
		if (0 <= arcsin) {
			setDirection(arccos / (2 * PI) * 360);
		}
		else {
			setDirection((2 * PI - arccos) / (2 * PI) * 360);
		}

		// Get position in that direction and see if it's a valid move
		double endX, endY;
		getPositionInThisDirection(getDirection(), 3, endX, endY);

		// If this coordinate would overlap with Dirt, pick a random direction
		if (getWorld()->overlapDirt(endX, endY)) {
			moveRandom();
			return;
		}

		// Otherwise, move toward the food
		moveTo(endX, endY);
		return;
	}
	// If no nearby food was found, just pick a random direction
	moveRandom();
}

void Salmonella::clone(double x, double y) {
	getWorld()->addActor(new Salmonella(getWorld(), x, y));
}


void Salmonella::moveRandom() {

	setDirection(randInt(0, 359));
	setPlan(10);
}

// If Socrates is close, try to move towards him
void AggressiveSalmonella::specialActionA() {

	Socrates* m_socrates = getWorld()->getSocrates();

	// Set m_socratesLock initially to false
	m_socratesLock = false;

	// Get distance of Socrates from AggressiveSalmonella
	double socrates_x, socrates_y;
	socrates_x = m_socrates->getX();
	socrates_y = m_socrates->getY();
	double x, y;
	x = getX();
	y = getY();
	double distance = sqrt(pow(socrates_x - x, 2) + pow(socrates_y - y, 2));
	
	// If the distance is less than or equal to 72, try to move 3 pixels toward Socrates
	if (distance <= 72) {
		// Socrates is nearby, so target is acquired
		m_socratesLock = true;

		// Get direction Socrates is in and set direction to that
		const double PI = atan(1) * 4;
		double diffX, diffY;
		diffX = socrates_x - x;
		diffY = socrates_y - y;
		double arccos = acos(diffX / distance);
		double arcsin = asin(diffY / distance);
		if (0 <= arcsin) {
			setDirection(arccos / (2 * PI) * 360);
		}
		else {
			setDirection((2 * PI - arccos) / (2 * PI) * 360);
		}

		// Get position in that direction and see if it's a valid move
		double endX, endY;
		getPositionInThisDirection(getDirection(), 3, endX, endY);

		// If something blocks the path, the Bacteria is stuck
		if (getWorld()->overlapDirt(endX, endY)) {
			return;
		}

		// Otherwise, move toward Socrates
		moveTo(endX, endY);
	}
}

// Finds a direction for AggressiveSalmonella to move if Socrates isn't nearby
void AggressiveSalmonella::specialActionB() {
	double x = getX();
	double y = getY();

	// If Socrates is nearby, don't do anything
	if (m_socratesLock) {
		return;
	}

	// Try to move in current direction
	if (getPlan() > 0) {
		setPlan(getPlan() - 1);

		// Get new position to move to
		double endX, endY;
		getPositionInThisDirection(getDirection(), 3, endX, endY);
		
		// If this would be out of bounds...
		if (pow(endX - VIEW_WIDTH / 2, 2) + pow(endY - VIEW_HEIGHT / 2, 2) >= pow(VIEW_RADIUS, 2)) {
			// Pick a random direction and try moving 10 ticks in that direction
			moveRandom();
			return;
		}

		// If position is blocked by dirt pile, move randomly
		if (getWorld()->overlapDirt(endX, endY)) {
			moveRandom();
			return;
		}

		// The move was valid if it wasn't out of bounds or passing through a dirt pile
		moveTo(endX, endY);
		return;
	}

	// If m_plan was 0, try to find the closest food within 128 pixels and its distance
	double foodDistance = 128;
	Actor* food = getWorld()->closestFood(x, y, foodDistance);

	// If food was found, try to move toward it
	if (food != nullptr) {
		// Get direction of that food and set Salmonella's direction to that
		const double PI = atan(1) * 4;
		double diffX = food->getX() - x;
		double diffY = food->getY() - y;
		double arccos = acos(diffX / foodDistance);
		double arcsin = asin(diffY / foodDistance);
		if (0 <= arcsin) {
			setDirection(arccos / (2 * PI) * 360);
		}
		else {
			setDirection((2 * PI - arccos) / (2 * PI) * 360);
		}

		// Get position in that direction and see if it's a valid move
		double endX, endY;
		getPositionInThisDirection(getDirection(), 3, endX, endY);
		
		// If Dirt pile is in the way, move randomly
		if (getWorld()->overlapDirt(endX, endY)) {
			moveRandom();
			return;
		}

		// Otherwise, move toward the food
		moveTo(endX, endY);
		return;
	}
	// If no nearby food was found, just pick a random direction
	moveRandom();
}

// Add a new AggressiveSalmonella at the indicated position
void AggressiveSalmonella::clone(double x, double y) {
	getWorld()->addActor(new AggressiveSalmonella(getWorld(), x, y));
}

void AggressiveSalmonella::moveRandom() {
	setDirection(randInt(0, 359));
	setPlan(10);
}

void Ecoli::specialActionA() {
	return;
}

void Ecoli::specialActionB() {

	// Get Socrates' coordinates to calculate distance from Bacteria (CHANGE TO ACTOR PRIVATE FUNCTION!!!)
	Socrates* m_socrates = getWorld()->getSocrates();
	double socrates_x, socrates_y;
	socrates_x = m_socrates->getX();
	socrates_y = m_socrates->getY();
	double x, y;
	x = getX();
	y = getY();
	double distance = sqrt(pow(socrates_x - x, 2) + pow(socrates_y - y, 2));

	if (distance <= 256) {
		const double PI = atan(1) * 4;
		double diffX, diffY;
		diffX = socrates_x - x;
		diffY = socrates_y - y;
		double arccos = acos(diffX / distance);
		double arcsin = asin(diffY / distance);
		if (0 <= arcsin) {
			setDirection(arccos / (2 * PI) * 360);
		}
		else {
			setDirection((2 * PI - arccos) / (2 * PI) * 360);
		}
		// Try to move forward 10 times
		int tried = 0;
		while (tried < 10) {
			// Get position in that direction and see if it's a valid move
			double endX, endY;
			getPositionInThisDirection(getDirection(), 2, endX, endY);

			// If nothing blocks the path, move there
			if (!getWorld()->overlapDirt(endX, endY)) {
				moveTo(endX, endY);
				return;
			}
			// Else, try a different direction 10 degrees greater
			setDirection(getDirection() + 10);
			tried++;
		}
	}
}

void Ecoli::clone(double x, double y) {
	getWorld()->addActor(new Ecoli(getWorld(), x, y));
}


void Chance::doSomething() {
	// If dead, don't do anything
	if (!getAlive()) {
		return;
	}
	// If overlapping with Socrates...
	Socrates* m_socrates = getWorld()->getSocrates();
	if (pow(m_socrates->getX() - getX(), 2) + pow(m_socrates->getY() - getY(), 2) <= pow(SPRITE_WIDTH, 2)) {
		// Increase the score based on the constructor, and set to dead
		getWorld()->increaseScore(m_scoreIncrease);
		setDead();

		// If it's a Goodie, play the sound
		if (m_goodie) {
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}

		// Do Chance action
		action();
		return;
	}

	// Else, see if the lifetime has expired and Chance is dead
	if (m_lifetime <= 0) {
		setDead();
		return;
	}

	// Decrement the lifetime each tick
	m_lifetime--;
}

// ExtraLifeGoodie increases life count by 1
void ExtraLifeGoodie::action() {
	getWorld()->incLives();
}

// Increases Socrates flame count by 5
void FlamethrowerGoodie::action() {
	getWorld()->getSocrates()->increaseFlame();
}

// Damages Socrates by 20, potentially killing him
void Fungus::action() {
	getWorld()->getSocrates()->damage(20);
}

// RestoreHealthGoodie restores health fully (to 100)
void RestoreHealthGoodie::action() {
	getWorld()->getSocrates()->restoreHealth();
}

void Projectile::doSomething() {
	// If projectile is dead, don't do anything
	if (!getAlive()) {
		return;
	}

	// If any alive target (Bacteria, Goodie, Dirt Pile) overlaps, damage it by m_damage and set Projectile to dead
	double x, y;
	x = getX();
	y = getY();

	// If a target overlaps, damage it and set the Projectile to dead
	Actor* target = getWorld()->overlapTarget(x, y);
	if (target != nullptr) {
		target->damage(m_damage);
		setDead();
		return;
	}

	// Otherwise, move in the same direction the Projectile is facing by SPRITE_WIDTH pixels
	moveAngle(getDirection(), SPRITE_WIDTH);
	m_travel += SPRITE_WIDTH;

	// If you've reached or exceeded the maximum travel distance, set to dead
	if (m_travel >= m_maxTravel) {
		setDead();
	}
}

// Food does nothing
void Food::doSomething() {
	return;
}