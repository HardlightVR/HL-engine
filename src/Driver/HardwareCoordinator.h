#pragma once
#include <type_traits>

#include "HardwareDataModel.h"

class DriverMessenger;
class HardwareCoordinator
{
public:
	HardwareCoordinator(DriverMessenger& messenger);
	~HardwareCoordinator();

	void Register(nsvr_request_type type, nsvr_request_handler handler, unsigned int, void* user_data);
	HardwareDataModel& Get(const std::string& name);

	//template<class TArgs>
//	void dispatch(const TArgs& args);

	template<class TEvent, class...TArgs>
	void dispatch(TArgs&&...);

private:
	
	DriverMessenger& m_messenger;
	std::unordered_map<std::string, HardwareDataModel> m_hardware;

	struct user_event_handler {
		nsvr_request_handler invoke;
		void* user_data;
		unsigned int target_version;
	};
	std::unordered_map<nsvr_request_type, std::vector<user_event_handler>> m_handlers;
	void updateTrackingForMessenger(const std::string& region, NSVR_Core_Quaternion quat);
};

template<typename T, typename = void>
struct has_event_type : std::false_type { };

template<typename T>
struct has_event_type<T, decltype(std::declval<T>().request_type, void())> : std::true_type { };

//template<class TArgs>
//inline void HardwareCoordinator::dispatch(const TArgs& args)
//{
//	
//	static_assert(has_event_type<TArgs>::value, "Event must specify which event type it is (add a const static nsvr_event_type member variable)");
//	using namespace nsvr::cevents;
//	for (auto& handler : m_handlers[TArgs::event_type]) {
//
//		auto correct_version = args.getVersion(handler.target_version);
//
//		if (correct_version.which() != 0) { //.which() index #0 is boost::blank, aka version not found
//			void* extracted_void_ptr = boost::apply_visitor([](auto& x) -> void* { return std::addressof(x); }, correct_version);
//			handler.invoke(extracted_void_ptr, TArgs::event_type, handler.user_data);
//		}
//		else {
//			std::cout << "Unknown version or event type\n";
//		}
//	}
//}

template<class TEvent, class ...TArgs>
inline void HardwareCoordinator::dispatch(TArgs && ...args)
{

	static_assert(has_event_type<TEvent>::value, "Event must specify which event type it is (add a const static nsvr_event_type member variable)");
	using namespace nsvr::cevents;
	for (auto& handler : m_handlers[TEvent::request_type]) {
		TEvent args2(std::forward<TArgs>(args)...);

		nsvr_request* r = reinterpret_cast<nsvr_request*>(&args2);
		//auto correct_version = args2.getVersion(handler.target_version);
		handler.invoke(r, TEvent::request_type, handler.user_data);
		//if (correct_version.which() != 0) { //.which() index 0 is boost::blank, aka version not found
		//	void* extracted_void_ptr = boost::apply_visitor([](auto& x) -> void* { return std::addressof(x); }, correct_version);
	//		handler.invoke(extracted_void_ptr, TEvent::event_type, handler.user_data);
	//	}
	//	else {
	////		std::cout << "Unknown version or event type\n";
		//}
	}
}

