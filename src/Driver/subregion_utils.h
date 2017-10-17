#pragma once
///
/// Represents a range of values, from min to max, in the context of a body segment.
/// For example, on the lower arm, you might have a segment that ranges from 0-1, which is the entire 
/// lower arm. You may then have a segment contained within that, which goes from 0.5-1, representing the top half 
/// of the lower arm.
struct segment_range {
	double min;
	double max;
	static const segment_range full;
	static const segment_range lower_half;
	static const segment_range upper_half;
};


/// Represents an angular section of a body segment, from min to max in degrees. The values
/// should always be specified from min to max, that is, from 0-360, where 0 && 360 represent dead ahead of the body.
/// If you need to specify something like the chest, just do 350-10, don't use negative values. 
struct angle_range {
	double min;
	double max;
	static const angle_range full;
	static const angle_range left_half;
	static const angle_range right_half;
	static const angle_range front_half;
	static const angle_range back_half;
};


///
/// Represents the center of a body segment. Imagine these segments as pie-wedge shaped sections, all the way
/// up to a full pie. The barycenter is the center of mass of one of these segments.
struct cartesian_barycenter {
	double x; //forward-back
	double y; //left-right
	double z; //height
};

double cartesian_distance(double x, double y, double z, double x2, double y2, double z2);

double to_radians(double degrees);
double to_degrees(double radians);

double angle_between(double min_degrees, double max_degrees);
double angular_distance(double min_degrees, double max_degrees);

bool is_between(double value_degrees, double min_degrees, double max_degrees);


double get_distance(const cartesian_barycenter& barycenter, double segment_ratio, double angle_degrees);