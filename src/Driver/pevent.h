#pragma once
#include "PluginAPI.h"

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/type_index.hpp>
namespace nsvr {
	namespace pevents {

		struct tracking_event {
			tracking_event() = default;
			tracking_event(const tracking_event&) = default;
			nsvr_quaternion quat;
			std::string region;
		};

		struct connected_event {

		};

		struct disconnected_event {

		};

		// I did the following to prevent errors when creating new events in the future.
		// This may turn out to be too complex. The previous way of using a switch or if statement was
		// quite error prone - now it's a compiler error.

		// Although, you can screw up by mapping the wrong enum type to the concrete type :(
		// I welcome a simpler solution.

		// You know what, I think it's overkill. We can just do if (incoming_event_type == event_type::whatever) to 
		// catch the assignment errors...although this represents the mapping as data, instead of logic.

		template<nsvr_device_event_type t> 
		struct getType;

		template<> 
		struct getType<nsvr_device_event_device_disconnected> { typedef disconnected_event type; };

		template<> 
		struct getType<nsvr_device_event_device_connected> { typedef connected_event type; };



		// Add new mapping here ---v






		
		typedef boost::variant<
			boost::blank,
			tracking_event,
			connected_event,
			disconnected_event
		> device_event_variant;


		template<nsvr_device_event_type...>
		struct type_operation;

		template<nsvr_device_event_type t1, nsvr_device_event_type...rest>
		struct type_operation<t1, rest...>
		{
			static device_event_variant make_concrete_type(nsvr_device_event_type t) {
				if (t == t1) {
					typename getType<t1>::type a;
					return a;
				}
				else {
					return type_operation<rest...>::make_concrete_type(t);
				}
			}
		};

		template<>
		struct type_operation<>
		{
			static device_event_variant make_concrete_type(nsvr_device_event_type t) {
				return boost::blank{};
			}
		};

		class device_event {
		public:
			device_event(nsvr_device_event_type type);
			nsvr_device_event_type type;
			uint64_t device_id;
			device_event_variant event;
			bool valid() const;

			template<typename T>
			boost::optional<T> get() const;

		private:
			bool valid_creation;
		};




		template<typename T>
		inline boost::optional<T> device_event::get() const
		{
			try {
				T result = boost::get<T>(event);
				return result;
			}
			catch (const boost::bad_get&) {

				BOOST_LOG_TRIVIAL(error) << "[PluginEvent] Bad get! Event true type = " << type << ", but requested template type was " << boost::typeindex::type_id<T>().pretty_name();
				return boost::none;
			}
		}

	}
}
