#pragma once
#include "IPlayable.h"
#include "IntermediateHapticFormats.h"
#include "HapticEvent.h"
#include "HapticEventGenerator.h"
class PlayableEffect :
	public IPlayable
{
public:
	struct EffectHandle {
		boost::uuids::uuid Id;
		AreaFlag Area;
		EffectHandle(boost::uuids::uuid id,  AreaFlag area) :Id(id),  Area(area) {}
	};
	struct PausedEffect {
		HapticEvent Event;
		AreaFlag Area;
		PausedEffect(HapticEvent e, AreaFlag a) : Event(e), Area(a) {}
	};
	//Precondition: the vector is not empty
	PlayableEffect(std::vector<TinyEffect> effects, HapticEventGenerator& gen);
	~PlayableEffect();
	void Play() override;
	void Reset() override;
	void Pause() override;
	void Update(float dt,const std::unordered_map<std::string, Atom>&) override;
	uint32_t GetHandle() const override;
	float GetTotalPlayTime() const override;
	float CurrentTime() const override;
	bool IsPlaying() const override;
	void Release() const override;
	void PlayFromStart() override;
private:
	bool _paused;
	float _time;
	HapticEventGenerator& _gen;
	std::vector<boost::uuids::uuid> _activeEffects;
	std::vector<TinyEffect>::iterator _lastExecutedEffect;
	std::vector<TinyEffect> _effects;
};

