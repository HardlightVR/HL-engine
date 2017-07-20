#include "stdafx.h"
#include "ZoneModel.h"
#include <experimental/vector>
void ZoneModel::Put(LiveBasicHapticEvent event) {
	incomingEvents.push(std::move(event));
}

void ZoneModel::Remove(ParentId id) {
	//incomingCommands.push(UserCommand(id, UserCommand::Command::Remove));


	//Had to actually remove the effects at this time to satisfy constraint that nsvr_playback_handle is only
	//valid from the time that an effect is received to the time the cancel command is 
	std::lock_guard<std::mutex> guard(eventsLock);
	removeAllChildren(id);
	
}

void ZoneModel::Play(ParentId id) {
	incomingCommands.push(UserCommand(id, UserCommand::Command::Play));
}

void ZoneModel::Pause(ParentId id) {
	incomingCommands.push(UserCommand(id, UserCommand::Command::Pause));
}

const ZoneModel::PausedContainer& ZoneModel::PausedEvents() {
	return pausedEvents;
}

const ZoneModel::PlayingContainer& ZoneModel::PlayingEvents() {
	return playingEvents;
}


CommandBuffer ZoneModel::Update(float dt) {

	std::lock_guard<std::mutex> guard(eventsLock);

	updateExistingEvents(dt);
	removeExpiredEvents();

	handleNewEvents();
	handleNewCommands();

	return generateCommands();
}

void ZoneModel::updateExistingEvents(float dt)
{
	for (auto& event : playingEvents) {
		event.update(dt);
	}
}

void ZoneModel::removeExpiredEvents() {
	auto expired = [](auto& e) {
		return e.isFinished();
	};

	std::experimental::erase_if(playingEvents, expired);
}







template<class T>
std::vector<T> consumeAll(boost::lockfree::spsc_queue<T>& queue) {
	std::vector<T> result;
	queue.consumeAll([&](auto element) {
		result.push_back(element);
	});
	return result;
}


void ZoneModel::handleNewCommands() {
	std::vector<UserCommand> newCommands = consumeAll(incomingCommands);
	for (const auto& command : newCommands) {
		switch (command.command) {
		case UserCommand::Command::Play:
			resumeAllChildren(command.id);
			break;
		case UserCommand::Command::Pause:
			pauseAllChildren(command.id);
			break;
		case UserCommand::Command::Remove:
			removeAllChildren(command.id);
			break;
		default:
			break;
		}
	}
}


void pruneOneshots(std::vector<LiveBasicHapticEvent>* events) {
	if (events->empty()) {
		return;
	}

	auto isOneshot = [](const LiveBasicHapticEvent& e) {return e.isOneshot(); };

	auto secondToLast = events->end() - 1;

	auto toRemove = std::remove_if(events->begin(), secondToLast, isOneshot);
	events->erase(toRemove, secondToLast);
}




void ZoneModel::handleNewEvents() {
	std::vector<LiveBasicHapticEvent> newlyArrivedEvents = consumeAll(incomingEvents);

	pruneOneshots(&newlyArrivedEvents);

	playingEvents.insert(playingEvents.end(), newlyArrivedEvents.begin(), newlyArrivedEvents.end());
}





CommandBuffer ZoneModel::generateCommands()
{

	if (playingEvents.empty()) {
		return stateChanger.transitionToIdle();
	}
	else {
		return stateChanger.transitionTo(playingEvents.back());

	}
}

boost::optional<LiveBasicHapticEvent> ZoneModel::GetCurrentlyPlayingEvent()
{
	if (playingEvents.empty()) {
		return boost::optional<LiveBasicHapticEvent>();
	}
	else {
		return playingEvents.back();
	}
}

ZoneModel::ZoneModel(Location area) :
	pausedEvents(),
	playingEvents(),
	stateChanger(area),
	incomingEvents(512),
	incomingCommands(512),
	eventsLock()
{

}

template<class T, class Predicate>
void copy_then_remove_if(std::vector<T>& from, std::vector<T>& to, Predicate predicate) {
	std::copy_if(
		from.begin(),
		from.end(),
		std::back_inserter(to),
		predicate
	);

	std::experimental::erase_if(from, predicate);
}


struct is_child_of {
	const ParentId& id;
	is_child_of(const ParentId& id) : id(id) {}
	bool operator()(const LiveBasicHapticEvent& e) {
		return e.isChildOf(id);
	}
};

void ZoneModel::pauseAllChildren(ParentId id)
{
	copy_then_remove_if(playingEvents, pausedEvents, is_child_of(id));
}


void ZoneModel::resumeAllChildren(ParentId id)
{
	copy_then_remove_if(pausedEvents, playingEvents, is_child_of(id));
}

void ZoneModel::removeAllChildren(ParentId id)
{
	std::experimental::erase_if(playingEvents, is_child_of(id));
	std::experimental::erase_if(pausedEvents, is_child_of(id));
}

