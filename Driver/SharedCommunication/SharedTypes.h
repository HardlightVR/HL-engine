#pragma once

struct Quaternion {
	float x;
};

struct TrackingUpdate {
	Quaternion a;
	Quaternion b;

};