#pragma once


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

		enum SuitStatus { Unknown, Connected, Disconnected };
		typedef unsigned int SuitId;

		struct SuitInfo {
			SuitId Id;
			SuitStatus Status;
			SuitInfo() :Id(0), Status(SuitStatus::Unknown) {}
		};

		struct SuitsConnectionInfo {
			std::time_t timestamp;
			bool SuitsFound[4] = { false };
			SuitInfo Suits[4];

		};

		struct ServiceInfo {
			int ServiceMajor;
			int ServiceMinor;

		};

		static const SuitsConnectionInfo NullSuitsConnectionInfo = {};
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
			using RegionType = uint64_t;
			RegionType Region;
			uint32_t Type;
			uint64_t Id;
			Data Value;


		};

		inline bool operator==(const RegionPair& lhs, const RegionPair& rhs) {
			return lhs.Id == rhs.Id;
		}

		enum nsvr_shared_region {
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

	}
}



