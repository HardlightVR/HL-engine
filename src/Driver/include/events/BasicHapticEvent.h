//include/Events/BasicHaptic.h



extern "C" {
	typedef struct NSVR_HapticIota_t NSVR_HapticIota;
	typedef struct NSVR_HapticInterval_t NSVR_HapticInterval;

	NSVR_HapticIota_GetEffect();
	NSVR_HapticIota_GetStrength();


	NSVR_HapticInterval_GetEffect();
	NSVR_HapticInterval_GetStrength();
	NSVR_HapticInterval_GetDuration();

	
	NSVR_Haptics_Consume(NSVR_HapticIota* iota);
	NSVR_Haptics_Consume(NSVR_HapticInterval *interval);


	NSVR_HapticIota_Play(uint32_t effect, float strength)
	NSVR_HapticInterval_Play(uint32_t effect, float strength, float duration)


	int NSVR_Plugin_Consume_HapticIota(NSVR_Consumer* consumer, NSVR_HapticIota* iota);
	int NSVR_Plugin_Consume_HapticInterval(NSVR_Consumer* consumer, NSVR_HapticInterval* interval);




}

int NSVR_Plugin_Consume_HapticInterval(NSVR_Consumer* consumer, NSVR_HapticInterval* interval) {
	return AS_TYPE(Plugin, consumer)->dispatch(interval);
}

class Plugin {
	int dispatch(NSVR_HapticInterval* interval) {
		float duration;
		NSVR_HapticInterval_GetDuration(interval, &duration);
		std::cout << "Got an interval: " << duration;
		return true;
	}
}


extern "C" {

}


Interval,
BasicHaptic,

Tap,

IotaEffect,
IntervalEffect
MasterStroke



extern "C" {
	typedef struct NSVR_LoopedBasicHaptic;
	int NSVR_BasicHaptic_PlayEffectWithDuration(NSVR_LoopedBasicHaptic* device, uint32_t effect, float strength, float duration);

}

Init() {

	auto zoneDriver = new ZoneDriver(this, "chest_left"); //satisfies BasicHaptic and DirectDrive interfaces
	Core::RegisterNode("chest_left", "basic-haptic-event",
		reinterpret_cast<NSVR_BasicHaptic_t*>(&zoneDriver));
	Core::RegisterNode("chest_left", "direct-drive",
		reinterpret_cast<NSVR_DirectDrive_t*>(&zoneDriver));
	Core::RegisterNode("chest_left", "")



}

//userland
int NSVR_BasicHaptic_PlayEffect(NSVR_BasicHaptic* device, uint32_t effect, float strength) {
	NSVR_LoopedBasicHaptic_PlayEffectWithDuration(device, effect, 0.0);
}

int NSVR_LoopedBasicHaptic_PlayEffectWithDuration(NSVR_LoopedBasicHaptic* device, uint32_t effect, float strength, float duration) {

}



//inside core:

void HandleBasicHapticEvent(EventDetails& details) {
	auto drivers = getDrivers("basic-haptic-event", details.region);
	for (NSVR_Plugin_t* driver in drivers) {
		HapticInterval p(Effect::Click, 3.0f);
		NSVR_Plugin_Consume_HapticInterval(driver, reinterpret_cast<NSVR_HapticInterval_t*>(&p));



	}
}

NSVR_HapticInterval_GetDuration(NSVR_HapticInterval* interval, float* outDuration) {
	return AS_TYPE(HapticInterval, interval)->Duration;
}