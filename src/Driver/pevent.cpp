#include "stdafx.h"
#include "pevent.h"

namespace nsvr {
	namespace pevents {

	
		typedef type_operation<
			nsvr_device_event_device_connected, 
			nsvr_device_event_device_disconnected
		> valid_events;


		device_event::device_event(nsvr_device_event_type type) 
			: type(type)
			, device_id{ 0 }
			, valid_creation{ false } 
		{
			event = valid_events::make_concrete_type(type);
		}

		bool device_event::valid() const
		{
			return event.which() != 0;
		}

		

	}
}