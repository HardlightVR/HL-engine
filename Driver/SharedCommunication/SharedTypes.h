#pragma once
#include <boost\serialization\serialization.hpp>
namespace NullSpace {
	namespace SharedMemory {
		struct ExecutionCommand {
			int Location;
			int Effect;
			short Command;

		private:
			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				ar & Location;
				ar & Effect;
				ar & Command;
			}
		};

		
		struct Quaternion {
			float x;
			float y;
			float z;
			float w;
			Quaternion() : x(0), y(0), z(0), w(0) {}
			Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
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



