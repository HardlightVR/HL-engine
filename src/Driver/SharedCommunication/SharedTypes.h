#pragma once

#include <stdint.h>
namespace NullSpace {
	namespace SharedMemory {

		struct Quaternion {
			float x;
			float y;
			float z;
			float w;


		};



		struct TrackingUpdate {
			Quaternion chest;
			Quaternion left_upper_arm;
			Quaternion right_upper_arm;
		};

		enum DeviceStatus { Unknown, Connected, Disconnected };
		
		typedef uint32_t SystemId;

		constexpr int MAX_DEVICES_PER_SYSTEM = 16;

	
		struct DeviceInfo {
			char DeviceName[128];
			DeviceStatus Status;
		};
		struct SystemInfo {
			SystemId Id; //0
			char SystemName[128]; //Hardlight Mk III Suit
			bool NodesFound[MAX_DEVICES_PER_SYSTEM] = { false };
			DeviceInfo Devices[MAX_DEVICES_PER_SYSTEM];
		};
		

	

		struct ServiceInfo {
			int ServiceMajor;
			int ServiceMinor;

		};

		static const TrackingUpdate NullTrackingUpdate = {};


		struct Color {
			float r;
			float g;
			float b;
			float a;
		};

		struct Data {
			float data_0;
			float data_1;
			float data_2;
			float data_3;
		};
		struct Intensity {
			float intensity;
		};

		enum class RegionPairType {
			Unknown = 0,
			Color = 1,
			Intensity = 2
		};
		struct RegionPair {
			using RegionType = uint32_t;
			RegionType Region;
			uint32_t Type;
			uint64_t Id;
			Data Value;


		};

		inline bool operator==(const RegionPair& lhs, const RegionPair& rhs) {
			return lhs.Id == rhs.Id;
		}

		/*enum nsvr_shared_region {
			nsvr_region_unknown = 0,

			nsvr_region_chest = 1000,
			nsvr_region_chest_left,
			nsvr_region_chest_right,

			nsvr_region_abs = 2000,
			nsvr_region_abs_left,
			nsvr_region_abs_upper_left,
			nsvr_region_abs_middle_left,
			nsvr_region_abs_lower_left,
			nsvr_region_abs_right,
			nsvr_region_abs_upper_right,
			nsvr_region_abs_middle_right,
			nsvr_region_abs_lower_right,

			nsvr_region_arm_left = 3000,
			nsvr_region_forearm_left,
			nsvr_region_upperarm_left,

			nsvr_region_arm_right = 4000,
			nsvr_region_forearm_right,
			nsvr_region_upperarm_right,

			nsvr_region_shoulder_left = 5000,
			nsvr_region_shoulder_right = 6000,

			nsvr_region_back = 7000,
			nsvr_region_back_left,
			nsvr_region_back_right,

			nsvr_region_hand_left = 8000,
			nsvr_region_hand_right = 9000,
			nsvr_region_leg_left = 10000,
			nsvr_region_leg_right = 11000
		};
*/


		constexpr uint64_t SUBREGION_BLOCK = 1000000;

		enum class nsvr_shared_region : uint32_t {
			identifier_unknown,
			identifier_body = 1 * SUBREGION_BLOCK,
			identifier_torso = 2 * SUBREGION_BLOCK,
			identifier_torso_front = 3 * SUBREGION_BLOCK,
			identifier_chest_left = 4 * SUBREGION_BLOCK,
			identifier_chest_right = 5 * SUBREGION_BLOCK,
			identifier_upper_ab_left = 6 * SUBREGION_BLOCK,
			identifier_middle_ab_left = 7 * SUBREGION_BLOCK,
			identifier_lower_ab_left = 8 * SUBREGION_BLOCK,
			identifier_upper_ab_right = 9 * SUBREGION_BLOCK,
			identifier_middle_ab_right = 10 * SUBREGION_BLOCK,
			identifier_lower_ab_right = 11 * SUBREGION_BLOCK,
			identifier_torso_back = 12 * SUBREGION_BLOCK,
			identifier_torso_left = 13 * SUBREGION_BLOCK,
			identifier_torso_right = 14 * SUBREGION_BLOCK,
			identifier_upper_back_left = 15 * SUBREGION_BLOCK,
			identifier_upper_back_right = 16 * SUBREGION_BLOCK,
			identifier_upper_arm_left = 17 * SUBREGION_BLOCK,
			identifier_lower_arm_left = 18 * SUBREGION_BLOCK,
			identifier_upper_arm_right = 19 * SUBREGION_BLOCK,
			identifier_lower_arm_right = 20 * SUBREGION_BLOCK,
			identifier_shoulder_left = 21 * SUBREGION_BLOCK,
			identifier_shoulder_right = 22 * SUBREGION_BLOCK,
			identifier_upper_leg_left = 23 * SUBREGION_BLOCK,
			identifier_lower_leg_left = 24 * SUBREGION_BLOCK,
			identifier_upper_leg_right = 25 * SUBREGION_BLOCK,
			identifier_lower_leg_right = 26 * SUBREGION_BLOCK,
			identifier_head = 27 * SUBREGION_BLOCK,
			identifier_palm_left = 28 * SUBREGION_BLOCK,
			identifier_palm_right = 29 * SUBREGION_BLOCK
		};

	}
}



