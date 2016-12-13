#pragma once
namespace NullSpaceDLL {
	struct Quaternion {
		float w;
		float x;
		float y;
		float z;
	};
	struct InteropTrackingUpdate {
		Quaternion chest;
		Quaternion left_upper_arm;
		Quaternion left_forearm;
		Quaternion right_upper_arm;
		Quaternion right_forearm;
	};
}