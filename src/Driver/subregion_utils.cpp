#include "stdafx.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include "subregion_utils.h"
#undef _USE_MATH_DEFINES



 double angular_distance(double min, double max) {
	if (max < min) {
		max += 360;
	}
	return max - min;
}

 bool is_between(double value_deg, double min_deg, double max_deg) {
	if (min_deg < max_deg) {
		//think pie slice
		return (min_deg <= value_deg && value_deg <= max_deg);
	}
	else {
		//think: you cut a pie slice, and they took the pie 
		//- Palmer
		return !(min_deg <= value_deg && value_deg <= max_deg);
	}
}




double get_distance(const cartesian_barycenter& barycenter, double segment_ratio, double angle_degrees)  {
	double x = std::cos(to_radians(angle_degrees));
	//our Y axis is swapped in relation to a normal circle's. Our Y goes negative where it goes positive. Could correct this.
	double y = -std::sin(to_radians(angle_degrees));
	double z = segment_ratio;

	return cartesian_distance(barycenter.x, barycenter.y, barycenter.z, x, y, z);
}

double to_radians(double degrees) {
	return (degrees * M_PI) / 180.0;
}
double to_degrees(double radians)
{
	return (radians * 180.0) / M_PI;
}

double cartesian_distance(double x, double y, double z, double x2, double y2, double z2) {
	return sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
}

double angle_between(double min, double max) {
	return std::fmod(min + (angular_distance(min, max) / 2.0), 360.0);
}