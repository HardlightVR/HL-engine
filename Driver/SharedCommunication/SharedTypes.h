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
			Quaternion() : x(0) {}
		};

		struct TrackingUpdate {
			Quaternion a;
			Quaternion b;
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

		static const SuitsConnectionInfo NullSuitsConnectionInfo = {};
		static const TrackingUpdate NullTrackingUpdate = {};



	}
}



