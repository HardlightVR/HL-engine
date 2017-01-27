#pragma once
#include "IPlayable.h"
#include "IntermediateHapticFormats.h"
#include "HapticEvent.h"
#include "HapticEventGenerator.h"
class PlayableEffect :
	public IPlayable
{
public:
	
	//Precondition: the vector is not empty
	PlayableEffect(std::vector<TinyEffect> effects, HapticEventGenerator& gen);
	~PlayableEffect();

	void Play() override;
	void Pause() override;
	void Stop() override;
	void Restart() override;
	void Update(float dt,const std::unordered_map<std::string, Atom>&) override;
	float GetTotalPlayTime() const override;
	float CurrentTime() const override;
	bool IsPlaying() const override;
	void Release() const override;

	
private:
	enum class PlaybackState {
		PLAYING,
		PAUSED,
		IDLE
	};
	
	PlaybackState _state;

	float _time;
	HapticEventGenerator& _gen;

	std::vector<TinyEffect>::iterator _lastExecutedEffect;
	std::vector<TinyEffect> _effects;
	boost::uuids::uuid _id;

	void reset();
	void pause();
	void resume();

};

