#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
const double tpi = 2*3.14159265358979323846;
//-------
double FNrange(double);
float FNkep(float, float, int);
float sgn(float);
//-------------------
double FNrange(double x)
{
	double a;
	a = x/tpi;
	double c = sgn(a);
	double b = int(a);
	double w = (a - (sgn(a)*int(a)) );
	a = tpi * (a - (sgn(a)*int(a)) );
	if (a<0)
		a = tpi+a;
	return a;
}

float FNkep(float m, float ecc, int eps)//Still needs work!!!!
{
	float e, v, delta = 0.05f;
	int i=0;
	e = m;
	float Dabs,Peps;//--
	do
	{
		delta = e - ecc * sin(e) - m;
		e = e - delta / (1 - ecc *cos(e));
		//--
		Dabs = abs(delta);
		Peps = pow(10.0f,(-eps));
		//--
		i++;
		if (i>15) //prevent getting stuck in loop
			break;
	}while(abs(delta) >= pow(10.0f,(-eps)));
	v = 2 * atan(pow((1 + ecc) / (1 - ecc), 0.5f) * tan(.5 * e));
	if (v<0)
		v+=tpi;
	return v;
}

float sgn(float a)
{
	if (a < 0)
		return -1;
	if (a > 0)
		return 1;
	return 0;
}

#endif