#include "PluginAPI.h"

namespace nsvr {



	class Event {
	public:
		Event(nsvr_device_event_type type) {
			nsvr_device_event_create(&m_event, type);
		}
		~Event() {
			nsvr_device_event_destroy(&m_event);
		}
		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;
		void raise(nsvr_core* core) {
			nsvr_device_event_raise(core, m_event);
		}
	private:
		nsvr_device_event* m_event;
	};
}