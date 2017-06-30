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



	}
}



