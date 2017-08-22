#pragma once

#include "better_enum.h"
enum class Side
{
	/// <summary>
	/// Left side
	/// </summary>
	Left,
	/// <summary>
	/// Right side
	/// </summary>
	Right,
	/// <summary>
	/// Both sides
	/// </summary>
	Mirror,
	/// <summary>
	/// Inherit from a higher level, or be programatically set
	/// </summary>
	Inherit,
	/// <summary>
	/// Not specified
	/// </summary>
	NotSpecified
};

enum class Location
{
	/// <summary>
	/// Right lower abdominal feedback zone
	/// </summary>
	Lower_Ab_Right = 0,
	/// <summary>
	/// Right shoulder feedback zone
	/// </summary>
	Shoulder_Right,
	/// <summary>
	/// Middle right abdominal feedback zone
	/// </summary>
	Mid_Ab_Right,
	/// <summary>
	/// Upper right abdominal feedback zone
	/// </summary>
	Upper_Ab_Right,
	/// <summary>
	/// Right chest feedback zone
	/// </summary>
	Chest_Right,
	/// <summary>
	/// Upper right arm feedback zone
	/// </summary>
	Upper_Arm_Right,
	/// <summary>
	/// Right forearm feedback zone
	/// </summary>
	Forearm_Right,
	/// <summary>
	/// Upper right back feedback zone
	/// </summary>
	Upper_Back_Right,
	/// <summary>
	/// Left shoulder feedback zone
	/// </summary>
	Shoulder_Left,
	/// <summary>
	/// Left forearm feedback zone
	/// </summary>
	Forearm_Left,
	/// <summary>
	/// Left chest feedback zone
	/// </summary>
	Chest_Left,
	/// <summary>
	/// Upper left abdominal feedback zone
	/// </summary>
	Upper_Ab_Left,
	/// <summary>
	/// Middle left abdominal feedback zone
	/// </summary>
	Mid_Ab_Left,
	/// <summary>
	/// Lower left abdominal feedback zone
	/// </summary>
	Lower_Ab_Left,
	/// <summary>
	/// Upper left arm feedback zone
	/// </summary>
	Upper_Arm_Left,
	/// <summary>
	/// Upper left back feedback zone
	/// </summary>
	Upper_Back_Left,
	/// <summary>
	/// Indicates an invalid zone
	/// </summary>
	Error
};

enum class Effect
{
	Strong_Click_100 = 0,
	Strong_Click_60,
	Strong_Click_30,
	Sharp_Click_100,
	Sharp_Click_60,
	Sharp_Click_30,
	Soft_Bump_100,
	Soft_Bump_60,
	Soft_Bump_30,
	Double_Click_100,
	Double_Click_60,
	Triple_Click,
	Soft_Fuzz,
	Strong_Buzz,
	Strong_Click_2_80,
	Strong_Click_3_60,
	Strong_Click_4_30,
	Strong_Click_1_100,
	Medium_Click_100,
	Medium_Click_80,
	Medium_Click_60,

	Sharp_Tick_100,
	Sharp_Tick_80,
	Sharp_Tick_60,

	Short_Double_Click_Strong_100,
	Short_Double_Click_Strong_80,
	Short_Double_Click_Strong_60,
	Short_Double_Click_Strong_30,

	Short_Double_Click_Medium_100,
	Short_Double_Click_Medium_80,
	Short_Double_Click_Medium_60,

	Short_Double_Sharp_Tick_100,
	Short_Double_Sharp_Tick_80,
	Short_Double_Sharp_Tick_60,

	Long_Double_Sharp_Click_Strong_100,
	Long_Double_Sharp_Click_Strong_80,
	Long_Double_Sharp_Click_Strong_60,
	Long_Double_Sharp_Click_Strong_30,

	Long_Double_Sharp_Click_Medium_100,
	Long_Double_Sharp_Click_Medium_80,
	Long_Double_Sharp_Click_Medium_60,

	Long_Double_Sharp_Tick_100,
	Long_Double_Sharp_Tick_80,
	Long_Double_Sharp_Tick_60,

	Buzz_100,
	Buzz_80,
	Buzz_60,
	Buzz_40,
	Buzz_20,

	Pulsing_Strong_100,
	Pulsing_Strong_60,
	Pulsing_Medium_100,
	Pulsing_Medium_60,

	Pulsing_Sharp_100,
	Pulsing_Sharp_60,

	Transition_Click_100,
	Transition_Click_80,
	Transition_Click_60,
	Transition_Click_40,
	Transition_Click_20,
	Transition_Click_10,

	Transition_Hum_100,
	Transition_Hum_80,
	Transition_Hum_60,
	Transition_Hum_40,
	Transition_Hum_20,
	Transition_Hum_10,

	Long_Buzz_for_Program_Stop_100,

	Smooth_Hum_50,
	Smooth_Hum_40,
	Smooth_Hum_30,
	Smooth_Hum_20,
	Smooth_Hum_10,
	Error

};

enum class Duration
{
	OneShot = 0,
	Infinite = -1,
	Variable = -2
};

enum class JsonLocation
{
	Shoulder,
	Upper_Back,
	Lower_Ab,
	Mid_Ab,
	Upper_Ab,
	Chest,
	Upper_Arm,
	Forearm

};

enum class Imu {
	Unknown = 0,
	Chest = 1, //0 
	Left_Forearm = 2, //6
	Left_Upper_Arm = 3, //2
	Right_Forearm = 4, //5
	Right_Upper_Arm = 5 //1
};

enum class Register {
	Status = 0x00,
	Control = 0x01,
	RtpVol = 0x02,
	WaveForm = 0x04
};

BETTER_ENUM(SubRegionId, uint64_t, 
	nsvr_region_unknown = 0,
	nsvr_region_torso = 50000,
	nsvr_region_torso_front = 100000,
	nsvr_region_torso_front_left = 110000,
	nsvr_region_chest_front_left = 111000,
	nsvr_region_abdomen_front_left = 112000,
	nsvr_region_ab_upper_left = 112100,
	nsvr_region_ab_middle_left = 112200,
	nsvr_region_ab_lower_left = 112300,
	nsvr_region_torso_front_right = 120000,
	nsvr_region_chest_front_right = 121000,
	nsvr_region_abdomen_front_right = 122000,
	nsvr_region_torso_back = 200000,
	nsvr_region_torso_back_left = 210000,
	nsvr_region_back_upper_left = 211000,
	nsvr_region_back_lower_left = 212000,
	nsvr_region_torso_back_right = 220000,
	nsvr_region_back_upper_right = 221000,
	nsvr_region_back_lower_right = 222000,
	nsvr_region_arm_left = 300000,
	nsvr_region_shoulder_left = 310000,
	nsvr_region_upper_arm_left = 320000,
	nsvr_region_lower_arm_left = 330000,
	nsvr_region_hand_left = 340000,
	nsvr_region_arm_right = 400000,
	nsvr_region_shoulder_right = 410000,
	nsvr_region_upper_arm_right = 420000,
	nsvr_region_lower_arm_right = 430000,
	nsvr_region_hand_right = 440000,
	nsvr_region_leg_left = 500000,
		nsvr_region_upper_leg_left = 500001,

		nsvr_region_upper_leg_front_left = 510000,
		nsvr_region_upper_leg_back_left = 520000,
		nsvr_region_lower_leg_left = 520001,

		nsvr_region_lower_leg_front_left = 530000,
		nsvr_region_lower_leg_back_left = 540000,
	nsvr_region_leg_right = 600000,
		nsvr_region_upper_leg_right = 600001,
		nsvr_region_upper_leg_front_right = 610000,
		nsvr_region_upper_legt_back_right = 620000,
		nsvr_region_lower_leg_right = 620001,

		nsvr_region_lower_leg_front_right = 630000,
		nsvr_region_lower_leg_back_right = 640000,
	nsvr_region_groin = 700000,
	nsvr_region_gluteal = 800000,
	nsvr_region_head = 900000



)

