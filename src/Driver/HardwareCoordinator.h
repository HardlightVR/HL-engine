#pragma once


#include "HardwareDataModel.h"
typedef boost::variant<boost::blank, nsvr_cevent_brief_haptic_v1, nsvr_cevent_brief_haptic_v2> brief_haptic;

class DriverMessenger;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DriverMessenger& messenger);
	~HardwareCoordinator();

	void Register(nsvr_cevent_type type, nsvr_cevent_handler handler, unsigned int, void* user_data);
	HardwareDataModel& Get(const std::string& name);
	void dispatchBriefHaptic(float strength, int effect, const char* region);

	template<class TEventVariant, class TArgs>
	void dispatch(nsvr_cevent_type type, const TArgs& args);

	struct BriefHapticArgs {
		uint32_t effect;
		float strength;
		const char* region;
		BriefHapticArgs(uint32_t effect, float strnegth, const char* region);
	};

	template<class THapticVariant, class TLatestEvent>
	THapticVariant getVersion(const unsigned int version, const TLatestEvent&);

	template<>
	brief_haptic getVersion(const unsigned int version, const BriefHapticArgs& args);

private:
	DriverMessenger& m_messenger;
	std::unordered_map<std::string, HardwareDataModel> m_hardware;

	struct user_event_handler {
		nsvr_cevent_handler handler;
		void* user_data;
		unsigned int target_version;
	};
	std::unordered_map<nsvr_cevent_type, std::vector<user_event_handler>> m_handlers;
	void updateTrackingForMessenger(const std::string& region, NSVR_Core_Quaternion quat);
};



template<class THapticVariant, class TLatestEvent>
inline THapticVariant HardwareCoordinator::getVersion(const unsigned int version, const TLatestEvent &)
{
	return THapticVariant();
}

template<>
inline brief_haptic HardwareCoordinator::getVersion(const unsigned int version, const BriefHapticArgs & args)
{
	if (version == 1) {
		nsvr_cevent_brief_haptic_v1 v1;
		v1.effect = args.effect;
		v1.strength = args.strength;
		return v1;
	}
	else if (version == 2) {
		nsvr_cevent_brief_haptic_v2 v2;
		v2.whacky = 32;
		return v2;
	}
}



template<class TEventVariant, class TArgs>
void HardwareCoordinator::dispatch(nsvr_cevent_type type, const TArgs& args)
{
	for (auto& handler : m_handlers[type]) {
		TEventVariant thing = getVersion<TEventVariant>(handler.target_version, args);
		if (thing.which() != 0) {
			void* ptr = boost::apply_visitor([](auto& x) -> void* { return std::addressof(x); }, thing);
			handler.handler(ptr, type, handler.user_data);
		}
		else {
			std::cout << "Unknown version\n";
		}
	}
}