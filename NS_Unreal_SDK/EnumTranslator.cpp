#include "EnumTranslator.h"



template<typename T> struct map_init_helper
{
	T& data;
	map_init_helper(T& d) : data(d) {}
	map_init_helper& operator() (typename T::key_type const& key, typename T::mapped_type const& value)
	{
		data[key] = value;
		return *this;
	}
};

template<typename T> map_init_helper<T> map_init(T& item)
{
	return map_init_helper<T>(item);
};

const char* EnumTranslator::ToString(Location loc) {
	return this->locations[loc];
	if (this->locations.find(loc) != this->locations.end()) {
		return this->locations[loc];
	}
	else {
		return "";
	}
}

const char* EnumTranslator::ToString(Effect effect) {
	return this->effects[effect];
	if (this->effects.find(effect) != this->effects.end()) {
		return this->effects[effect];
	}
	else {
		return "";
	}
}
EnumTranslator::EnumTranslator() {
	init_locations();
	init_effects();

}


EnumTranslator::~EnumTranslator()
{
}
void EnumTranslator::init_effects() {
	map_init(this->effects)
		(Effect::Buzz_100, "Buzz_100")
		(Effect::Buzz_20, "Buzz_20")
		(Effect::Buzz_40, "Buzz_40")
		(Effect::Buzz_60, "Buzz_60")
		(Effect::Buzz_80, "Buzz_80")
		(Effect::Double_Click_100, "Double_Click_100")
		(Effect::Double_Click_60, "Double_Click_60")
		(Effect::Long_Buzz_for_Program_Stop_100, "Long_Buzz_for_Program_Stop_100")
		(Effect::Long_Double_Sharp_Click_Medium_100, "Long_Double_Sharp_Click_Medium_100")
		(Effect::Long_Double_Sharp_Click_Medium_60, "Long_Double_Sharp_Click_Medium_60")
		(Effect::Long_Double_Sharp_Click_Medium_80, "Long_Double_Sharp_Click_Medium_80")
		(Effect::Long_Double_Sharp_Click_Strong_100, "Long_Double_Sharp_Click_Strong_100")
		(Effect::Long_Double_Sharp_Click_Strong_30, "Long_Double_Sharp_Click_Strong_30")
		(Effect::Long_Double_Sharp_Click_Strong_60, "Long_Double_Sharp_Click_Strong_60")
		(Effect::Long_Double_Sharp_Click_Strong_80, "Long_Double_Sharp_Click_Strong_80")
		(Effect::Long_Double_Sharp_Tick_100, "Long_Double_Sharp_Tick_100")
		(Effect::Long_Double_Sharp_Tick_60, "Long_Double_Sharp_Tick_60")
		(Effect::Long_Double_Sharp_Tick_80, "Long_Double_Sharp_Tick_80")
		(Effect::Medium_Click_100, "Medium_Click_100")
		(Effect::Medium_Click_60, "Medium_Click_60")
		(Effect::Medium_Click_80, "Medium_Click_80")
		(Effect::Pulsing_Medium_100, "Pulsing_Medium_100")
		(Effect::Pulsing_Medium_60, "Pulsing_Medium_60")
		(Effect::Pulsing_Sharp_100, "Pulsing_Sharp_100")
		(Effect::Pulsing_Sharp_60, "Pulsing_Sharp_60")
		(Effect::Pulsing_Strong_100, "Pulsing_Strong_100")
		(Effect::Pulsing_Strong_60, "Pulsing_Strong_60")
		(Effect::Sharp_Click_100, "Sharp_Click_100")
		(Effect::Sharp_Click_30, "Sharp_Click_30")
		(Effect::Sharp_Click_60, "Sharp_Click_60")
		(Effect::Sharp_Tick_100, "Sharp_Tick_100")
		(Effect::Sharp_Tick_60, "Sharp_Tick_60")
		(Effect::Sharp_Tick_80, "Sharp_Tick_80")
		(Effect::Short_Double_Click_Medium_100, "Short_Double_Click_Medium_100")
		(Effect::Short_Double_Click_Medium_60, "Short_Double_Click_Medium_60")
		(Effect::Short_Double_Click_Medium_80, "Short_Double_Click_Medium_80")
		(Effect::Short_Double_Click_Strong_100, "Short_Double_Click_Strong_100")
		(Effect::Short_Double_Click_Strong_30, "Short_Double_Click_Strong_30")
		(Effect::Short_Double_Click_Strong_60, "Short_Double_Click_Strong_60")
		(Effect::Short_Double_Click_Strong_80, "Short_Double_Click_Strong_80")
		(Effect::Short_Double_Sharp_Tick_100, "Short_Double_Sharp_Tick_100")
		(Effect::Short_Double_Sharp_Tick_60, "Short_Double_Sharp_Tick_60")
		(Effect::Short_Double_Sharp_Tick_80, "Short_Double_Sharp_Tick_80")
		(Effect::Smooth_Hum_10, "Smooth_Hum_10")
		(Effect::Smooth_Hum_20, "Smooth_Hum_20")
		(Effect::Smooth_Hum_30, "Smooth_Hum_30")
		(Effect::Smooth_Hum_40, "Smooth_Hum_40")
		(Effect::Smooth_Hum_50, "Smooth_Hum_50")
		(Effect::Soft_Bump_100, "Soft_Bump_100")
		(Effect::Soft_Bump_30, "Soft_Bump_30")
		(Effect::Soft_Bump_60, "Soft_Bump_60")
		(Effect::Soft_Fuzz, "Soft_Fuzz")
		(Effect::Strong_Buzz, "Strong_Buzz")
		(Effect::Strong_Click_100, "Strong_Click_100")
		(Effect::Strong_Click_1_100, "Strong_Click_1_100")
		(Effect::Strong_Click_2_80, "Strong_Click_2_80")
		(Effect::Strong_Click_30, "Strong_Click_30")
		(Effect::Strong_Click_3_60, "Strong_Click_3_60")
		(Effect::Strong_Click_4_30, "Strong_Click_4_30")
		(Effect::Strong_Click_60, "Strong_Click_60")
		(Effect::Transition_Click_10, "Transition_Click_10")
		(Effect::Transition_Click_100, "Transition_Click_100")
		(Effect::Transition_Click_20, "Transition_Click_20")
		(Effect::Transition_Click_40, "Transition_Click_40")
		(Effect::Transition_Click_60, "Transition_Click_60")
		(Effect::Transition_Hum_10, "Transition_Hum_10")
		(Effect::Transition_Hum_100, "Transition_Hum_100")
		(Effect::Transition_Hum_20, "Transition_Hum_20")
		(Effect::Transition_Hum_40, "Transition_Hum_40")
		(Effect::Transition_Hum_60, "Transition_Hum_60")
		(Effect::Transition_Hum_80, "Transition_Hum_80")

		(Effect::Triple_Click, "Triple_Click")
		(Effect::Transition_Click_80, "Transition_Click_80");


}

void EnumTranslator::init_locations() {
	map_init(this->locations)
		(Location::Chest_Left, "Chest_Left")
		(Location::Chest_Right, "Chest_Right")
		(Location::Forearm_Left, "Forearm_Left")
		(Location::Forearm_Right, "Forearm_Right")
		(Location::Lower_Ab_Left, "Lower_Ab_Left")
		(Location::Lower_Ab_Right, "Lower_Ab_Right")
		(Location::Mid_Ab_Left, "Mid_Ab_Left")
		(Location::Mid_Ab_Right, "Mid_Ab_Right")
		(Location::Shoulder_Left, "Shoulder_Left")
		(Location::Shoulder_Right, "Shoulder_Right")
		(Location::Upper_Ab_Left, "Upper_Ab_Left")
		(Location::Upper_Ab_Right, "Upper_Ab_Right")
		(Location::Upper_Arm_Left, "Upper_Arm_Left")
		(Location::Upper_Arm_Right, "Upper_Arm_Right")
		(Location::Upper_Back_Left, "Upper_Back_Left")
		(Location::Upper_Back_Right, "Upper_Back_Right")
		;
}