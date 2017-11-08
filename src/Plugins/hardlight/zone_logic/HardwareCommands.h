#pragma once
#include <vector>
#include "Enums.h"
#include <boost/variant/variant.hpp>


struct AudioOptions {
	int VibeCtrl;
	int AudioMin;
	int AudioMax;
	int MinDrv;
	int MaxDrv;
};

struct PlaySingle {
	Location Area;
	uint32_t Effect;
	float Strength;
	PlaySingle() = default;
	PlaySingle(Location area, uint32_t eff, float strength) : 
		Area(area), 
		Effect(eff), 
		Strength(strength) {}
};

struct PlayCont {
	uint32_t Effect;
	Location Area;
	float Strength;
	float Duration;
	PlayCont(Location area, uint32_t eff, float strength, float duration) : 
		Area(area),
		Effect(eff), 
		Strength(strength),
		Duration(duration) {}

};

struct PlayVol {
	Location Area;
	uint16_t Volume;
	PlayVol(Location area, uint16_t volume) :
		Area(area),
		Volume(volume) {}
};

struct Halt {
	Location Area;
	Halt(Location area) :
		Area(area) {}
};

struct EnableIntrig {
	Location Area;
	EnableIntrig(Location area) :
		Area(area) {}
};

struct EnableRtp {
	Location Area;
	EnableRtp(Location area) :
		Area(area) {}
};

struct EnableAudio {
	Location Area;
	AudioOptions Opts;
};

struct DisableAudio {
	Location Area;
};

using FirmwareCommand = 
boost::variant<
	PlaySingle, 
	PlayCont, 
	PlayVol, 
	Halt, 
	EnableIntrig, 
	EnableRtp,
	EnableAudio,
	DisableAudio
>;

using CommandBuffer = std::vector<FirmwareCommand>;