#include <string>
using namespace std;

struct OrbitalElements
{
	float inclination; //(i)
	float longitudeAscendingNode; //(o)
	float eccentricity; //(e)
	float periapsis; //perihelion (w)
	float simiMajorAxis; //mean distance (a)
	float meanAnomaly; //calculated with time and MeanMotion (M)
	float meanMotion; //calculated once with G, mass, and 'a' (n)
	float meanLongitude; //calculated once with M,o,w
};

struct SpaceObject
{
	float position[4];
	float vector[4];
	float accel[4];
	float mass;
	string name;
	string owner;
};

struct posVector
{
	float x;
	float y;
	float z;
	float w;
};

struct Planet
{
	SpaceObject SpaceObject;
	int habitaionZone;
	int infrastructure;
	int population;
	int political;
	int output[3]; //power, material, research
	int hillSphere; //grav well
	float radius;
	float rotation;
	OrbitalElements OrbitalElements;
};

struct Colony
{
	SpaceObject SpaceObject;
	int infrastructure;
	int population;
	int output[3]; //power, material, research
	OrbitalElements OrbitalElements;
};

struct Ship
{
	//const
	SpaceObject SpaceObject;
	int maxAccel;
	int fuelRate;
	int firePower;
	string type;//destroyer,crusier,battleship,carrier,support
	int upgradeLevel; //0<<low, 4<<capital

	//variable
	int fuelLevel;
	int health;
	int shields;
	string orbitParent;
	int orbitLevel;//0<<low, 1<<mid(geo), 2<<high ,3<<free
};