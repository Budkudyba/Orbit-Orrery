
#ifndef PLANETS_H
#define PLANETS_H

#include "structs.h" //contains space0bject structs
#include <deque>
#include <vector>

class planets
{
public:

	planets();
	~planets();

	int numofplanets;

	void initFromFile(string);
	//reads from planets.txt the data needed to simulate the system
	//calls initDefaults(), setDefaultPlanets()


	Planet getPlanet(int);
	OrbitalElements getOE(int);
	posVector getPos(int n);
	posVector getRotSpeed(int n);
	float getMass(int);
	float getradius(int n);
	string getName(int n);

	float CalcPeriod(int n, int Timescale);

	float AvgSpeed(int n);
	float distance(int n);

	//updates the position of one planet (doesn't store any data)
	posVector UpdatePos(OrbitalElements &OE, float time);
	//same as UpdatePos (stores info for further calculations)
	posVector UpdatePos2(OrbitalElements &OE, float time, int n);

	//calculates hill sphere, mean motion, and other constants
	void initConst(OrbitalElements &OE, float mass);


private:

	void setPos(posVector pos, int n);
	void pushPos(posVector pos, int n);



	Planet* P; //pointer to planet array

	bool drawSprite;
	bool drawPlanet;

	vector<deque<posVector>> posList;

	//defaults for init
	SpaceObject DefaultSO;
	OrbitalElements DefaultElements;
	Planet DefaultPlanet;
	
	//update orbital elements from state vector
	//this could enable a planet to move to a different orbit
	//OrbitalElements UpdateOE(posVector, vexVector, OrbitalElements &OE);

	//initializes default structs to set to each planet to ensure no null data
	void initDefaults(SpaceObject&, Planet&, OrbitalElements&);
	
	//sets each planet (after creation) to default values
	void setDefaultPlanets(Planet*, Planet, int);
};


#endif // PLANETS_H