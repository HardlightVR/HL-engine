#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include "Enums.h"
#include "LiveBasicHapticEvent.h"
#include "MotorStateChanger.h"
#include "HardwareCommands.h"
#include <vector>
#include <mutex>
#include <chrono>
class ZoneModel {
public:

	ZoneModel(Location area);

	void Put(LiveBasicHapticEvent event);
	void Remove(ParentId id);
	void Play(ParentId id);
	void Pause(ParentId id);


	typedef std::vector<LiveBasicHapticEvent> PlayingContainer;
	typedef std::vector<LiveBasicHapticEvent> PausedContainer;

	const PausedContainer& PausedEvents();
	const PlayingContainer& PlayingEvents();

	CommandBuffer Update(float dt);

	boost::optional<LiveBasicHapticEvent> GetCurrentlyPlayingEvent();


private:
	class UserCommand {
	public:
		enum class Command {
			Unknown = 0, Play = 1, Pause = 2, Remove = 3
		};
		ParentId id;
		Command command;
		UserCommand();
		UserCommand(ParentId id, Command c);
	};
	PlayingContainer playingEvents;
	PausedContainer pausedEvents;

	boost::lockfree::spsc_queue<LiveBasicHapticEvent> incomingEvents;
	boost::lockfree::spsc_queue<UserCommand> incomingCommands;

	MotorStateChanger stateChanger;

	void pauseAllChildren(ParentId id);
	void resumeAllChildren(ParentId id);
	void removeAllChildren(ParentId id);

	CommandBuffer generateCommands();
	void updateExistingEvents(float dt);
	void removeExpiredEvents();
	void handleNewCommands();
	void handleNewEvents();

	std::chrono::time_point<std::chrono::steady_clock> m_lastSample;
	std::mutex eventsLock;
};