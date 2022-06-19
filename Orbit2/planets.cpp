//#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "planets.h"
#include "functions.h"

using namespace std;

planets::planets()
: numofplanets(0)
{
}
 
planets::~planets()
{
}

void planets::initDefaults(SpaceObject& DefaultSO, Planet& DefaultPlanet, OrbitalElements& DefaultElements)
{
	int i;

	//init Default SpaceObject
	DefaultSO.name = "Default";
	DefaultSO.mass = 1;
	DefaultSO.owner = "NULL";
	for (i=0; i<4; i++)
	{
	DefaultSO.position[i] = 0;
	DefaultSO.accel[i] = 0;
	DefaultSO.vector[i] = 0;
	}

	//init Deafult Colony object
	DefaultElements.eccentricity = 0.0f;
	DefaultElements.inclination = 0.0f;
	DefaultElements.longitudeAscendingNode = 0.0f;
	DefaultElements.meanAnomaly = 0.0f;
	DefaultElements.meanLongitude = 0.0f;
	DefaultElements.meanMotion = 0.0f;
	DefaultElements.periapsis = 0.0f;
	DefaultElements.simiMajorAxis = 0.0f;

	//init Default Planet
	DefaultPlanet.SpaceObject = DefaultSO;
	DefaultPlanet.habitaionZone = 0;
	DefaultPlanet.infrastructure = 0;
	DefaultPlanet.political = 0;
	DefaultPlanet.population = 0;
	DefaultPlanet.hillSphere = 0;
	DefaultPlanet.radius = 1;
	DefaultPlanet.OrbitalElements = DefaultElements;
	for (i=0; i<3; i++)
	{
		DefaultPlanet.output[i] =0 ;
	}
}

void planets::setDefaultPlanets(Planet* P, Planet DefaultPlanet, int nop)
{
	int i;

	for (i=0; i<nop; i++)
	{
		P[i] = DefaultPlanet;
	}
}

void planets::initFromFile(string filename)
{
	int i, linenum = -1;
	ifstream myfile;
	string suffix, line = "no data to read";
	float linef;

	suffix = ".txt";
	filename += suffix;

	cout << filename << endl;

	myfile.open(filename);
	if (myfile.is_open())
	{
		myfile >> linef; //get first line # of planets
		numofplanets = int(linef);
		//create system of planets
		P = new Planet[numofplanets];

		//resize math vectors
		posList.resize(numofplanets);

		//create default Objects
		SpaceObject DefaultSO;
		Planet DefaultPlanet;
		Colony DefaultColony;
		initDefaults(DefaultSO, DefaultPlanet, DefaultElements);

		setDefaultPlanets(P, DefaultPlanet, numofplanets);
		numofplanets = 0;
		
		//parse for start keyword
		/*while (line != "start")
		{
			myfile >> line;
		}*/

		myfile >> line;//setup for data loading

		while (!myfile.eof())
		{	
			
			linenum++;

			switch (linenum)
			{
				case 0: //name
					P[numofplanets].SpaceObject.name = line;
					myfile >> line;
					break;
				case 1: //owner
					P[numofplanets].SpaceObject.owner = line;
					myfile >> linef;
					break;
				case 2: //pos
					for (i=0; i<4; i++)
					{
						P[numofplanets].SpaceObject.position[i] = linef;
						myfile >> linef;
					}
					break;
				case 3: //vector used for rotation speed
					for (i=0; i<4; i++)
					{
						P[numofplanets].SpaceObject.vector[i] = linef;
						myfile >> linef;
					}
					break;
				case 4: //accel
					for (i=0; i<4; i++)
					{
						P[numofplanets].SpaceObject.accel[i] = linef;
						myfile >> linef;
					}
					break;
				case 5: //mass
					P[numofplanets].SpaceObject.mass = linef;
					myfile >> linef;
					break;
				case 6: //radius
					P[numofplanets].radius = linef * 6.68458134e-9;//convert Km to au
					myfile >> linef;
					break;
				case 7: //inclination (-1 to 1)
					P[numofplanets].OrbitalElements.inclination = linef;
					myfile >> linef;
					break;
				case 8: //longitudeAscendingNode
					P[numofplanets].OrbitalElements.longitudeAscendingNode = linef;
					myfile >> linef;
					break;
				case 9: //eccentricity (0 to 1) //not handled
					P[numofplanets].OrbitalElements.eccentricity = linef;
					myfile >> linef;
					break;
				case 10: //periapsis
					P[numofplanets].OrbitalElements.periapsis = linef;
					myfile >> linef;
					break;
				case 11: //simiMajorAxis
					P[numofplanets].OrbitalElements.simiMajorAxis = linef;
					myfile >> linef;
					break;
				case 12: //meanAnomaly
					P[numofplanets].OrbitalElements.meanAnomaly = linef;
					myfile >> linef;
					break;
				case 13: //meanMotion
					P[numofplanets].OrbitalElements.meanMotion = linef;
					myfile >> linef;
					break;
				case 14: //meanLongitude
					P[numofplanets].OrbitalElements.meanLongitude = linef;
					myfile >> line;
					linenum = -1;
					numofplanets++;
					break;
			}
		}
		myfile.close();
	}
	else
		cout << "unable to open planet data file" << endl;
}

Planet planets::getPlanet(int n)
{
	return P[n];
}

OrbitalElements planets::getOE(int n)
{
	return P[n].OrbitalElements;
}

posVector planets::getPos(int n)
{
	posVector t;
	t.x = P[n].SpaceObject.position[0];
	t.y = P[n].SpaceObject.position[1];
	t.z = P[n].SpaceObject.position[2];
	t.w = P[n].SpaceObject.position[3];
	return t;
}

posVector planets::getRotSpeed(int n)
{
	posVector t;
	t.x = P[n].SpaceObject.vector[0];
	t.y = P[n].SpaceObject.vector[1];
	t.z = P[n].SpaceObject.vector[2];
	t.w = P[n].SpaceObject.vector[3];
	return t;
}

string planets::getName(int n)
{
	return P[n].SpaceObject.name;
}

float planets::getMass(int n)
{
	return P[n].SpaceObject.mass;
}

float planets::getradius(int n)
{
	return P[n].radius;
}

float planets::AvgSpeed(int n)
{
	//check to see if enough points
	if (posList[n].size() < 2)
		return 0.0f;
	
	// distance/Time
	float Tdist = 0, Ttime = 0;
	float sumX = 0, sumY = 0, sumZ = 0, sumW = 0;
	for (int i=0; i<posList[n].size()-1; i++)
	{
		//distance sqrt((x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2)
			//point 1
		sumX = posList[n].at(i).x;
		sumY = posList[n].at(i).y;
		sumZ = posList[n].at(i).z;
			//point2
		sumX -= posList[n].at(i+1).x;
		sumY -= posList[n].at(i+1).y;
		sumZ -= posList[n].at(i+1).z;
			//square
		sumX = sumX*sumX;
		sumY = sumY*sumY;
		sumZ = sumZ*sumZ;
			//add and sqrt
		Tdist += sqrt(sumX + sumY + sumZ);
	}
	// time difference
	Ttime = posList[n].front().w - posList[n].back().w;
	//divide
	return Tdist/Ttime;
}

float planets::distance(int n)
{
	posVector PPos = getPos(n);
	return sqrt((PPos.x*PPos.x)+(PPos.y*PPos.y)+(PPos.z*PPos.z));
}

void planets::setPos(posVector pos, int n)
{
	P[n].SpaceObject.position[0] = pos.x;
	P[n].SpaceObject.position[1] = pos.y;
	P[n].SpaceObject.position[2] = pos.z;
	P[n].SpaceObject.position[3] = pos.w;
}

void planets::pushPos(posVector pos, int n)
{
	int maxAvg = 100;
	//store new position on front
	posList[n].push_front(pos);
	//remove first position if max avg is reached
	if (posList[n].size() >= maxAvg)
		posList[n].pop_back();
}

posVector planets::UpdatePos(OrbitalElements &OE, float t)
{
	float eldate;
	float rp, vp;
	double mp;
	//float vp_pp_op, Svp_pp_op, Cvp_pp_op;
	//double P;
	//const double G = 398600.4418;
	//const double SunMass = 1.9891e30;
	//const double u = 132712440018;//GM constant
	const double pi = 3.14159265358979323846;
	posVector pos;

	eldate = 2450680.5f - 2451545.0f;//?
	
	//one time calculations
	//P = 2 * pi * sqrt(pow(OE.simiMajorAxis,3)/u);	
	//OE.meanMotion = (2*pi)/P;

	//OE.meanMotion = sqrt(G*(mass + SunMass)/pow(OE.simiMajorAxis,3));

	//OE.meanMotion = 0.9855796;
	//debug override
	//OE.meanMotion = 1/(365.25/(5*1.25667));
	//OE.meanLongitude =328.40353;
	//OE.meanLongitude = OE.meanAnomaly + OE.longitudeAscendingNode + OE.periapsis;

	//iterative calculations
	double mpstep1 = OE.meanMotion*(t);
	double mpstep2 = OE.meanMotion*(t) + OE.meanLongitude - OE.periapsis;

	mp= FNrange(OE.meanMotion*(t) + OE.meanLongitude - OE.periapsis);
	vp= FNkep(mp, OE.eccentricity, 12);//Still Needs WORK!!!
	rp= OE.simiMajorAxis * (1 - OE.eccentricity*OE.eccentricity)/
		(1 + OE.eccentricity * cos(vp));

	pos.x = rp * ( cos(OE.longitudeAscendingNode) *
			cos(vp + OE.periapsis - OE.longitudeAscendingNode)-
			sin(OE.longitudeAscendingNode)*
			sin(vp + OE.periapsis - OE.longitudeAscendingNode)*
			cos(OE.inclination) );

	pos.y = rp * ( sin(OE.longitudeAscendingNode) *
			cos(vp + OE.periapsis - OE.longitudeAscendingNode)+
			cos(OE.longitudeAscendingNode)*
			sin(vp + OE.periapsis - OE.longitudeAscendingNode)*
			cos(OE.inclination) );

	pos.z = rp * sin(vp + OE.periapsis - OE.longitudeAscendingNode) * 
			sin(OE.inclination);
	pos.w = t;//time
	
	//for calculating dist, speed
	//setPos(pos, n);
	//pushPos(pos, n);//for avg speed

	return pos;
}

posVector planets::UpdatePos2(OrbitalElements &OE, float t, int n)
{
	posVector pos = UpdatePos(OE, t);

	//for calculating dist, speed
	setPos(pos, n);
	pushPos(pos, n);//for avg speed

	return pos;
}

float planets::CalcPeriod(int n, int Timescale)//only works for sun orbit
{
	const double pi = 3.14159265358979323846;
	const double u = 132712440018;//GM sun
	const double km = 149598000;//AU to Km
	const float gametime = 37*Timescale;//offset
	float seconds = 2 * pi * sqrt(pow(P[n].OrbitalElements.simiMajorAxis*km,3)/u);
	float years = seconds / (60 * 60 * 24 * 7 * 52);
	return years*gametime;
}