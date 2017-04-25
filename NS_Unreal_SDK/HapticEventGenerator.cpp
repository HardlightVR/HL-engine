#include "HapticEventGenerator.h"
#include "PriorityModel.h"


#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>




HapticEventGenerator::HapticEventGenerator(PriorityModel & model):_model(model)
{
}

HapticEventGenerator::~HapticEventGenerator()
{
}

void HapticEventGenerator::NewEvent(AreaFlag area, float duration, Effect effect, boost::uuids::uuid id)
{
	auto ev = GeneratedEvent(HapticEvent(effect, duration, id), area);
	if (auto optionalId = _model.Put(area, ev.Event)) {
		_events[uuid_hasher(optionalId.get())].push_back(ev);
		
	}


	
}

void HapticEventGenerator::Resume(boost::uuids::uuid id)
{
	if (_events.find(uuid_hasher(id)) != _events.end()) {
		for (const auto& generatedEvent : _events.at(uuid_hasher(id))) {
			_model.Put(generatedEvent.Area, generatedEvent.Event);
		}
	}
}

void HapticEventGenerator::Pause(boost::uuids::uuid id)
{
	if (_events.find(uuid_hasher(id)) != _events.end()) {
		auto& eventList = _events.at(uuid_hasher(id));

		for (auto& generatedEvent : eventList) {
			if (auto optionalEv = _model.Remove(generatedEvent.Area, id)) {
				HapticEvent removedEvent = optionalEv.get();
				generatedEvent.Event.Duration = removedEvent.Duration - removedEvent.TimeElapsed;
				
			}
			else {
				//mark for removal
				generatedEvent.NeedsDeletion = true;
			}
			
		}

		//remove in one go
		auto toRemove = std::remove_if(eventList.begin(), eventList.end(), [](const GeneratedEvent& e) {return e.NeedsDeletion; });
		eventList.erase(toRemove, eventList.end());
	}
}

void HapticEventGenerator::Remove(boost::uuids::uuid id)
{
	if (_events.find(uuid_hasher(id)) != _events.end()) {
		for (auto& generatedEvent : _events.at(uuid_hasher(id))) {
			_model.Remove(generatedEvent.Area, id);
		}

		_events.at(uuid_hasher(id)).clear();

	}
}



