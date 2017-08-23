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

constexpr uint64_t SUBREGION_BLOCK_SIZE = (uint64_t(1) << 56);
constexpr uint64_t NUM_SUBREGION_BLOCKS = 256;
static_assert(std::numeric_limits<uint64_t>::max() >= (SUBREGION_BLOCK_SIZE * (NUM_SUBREGION_BLOCKS - 1)) + (SUBREGION_BLOCK_SIZE - 1), "Too many subregions or block size too big");

enum class SubRegionAllocation : uint64_t {
	reserved_block_1		= 0,
	reserved_block_2		= 1 * SUBREGION_BLOCK_SIZE,
	foot_left				= 2 * SUBREGION_BLOCK_SIZE,
	expansion_foot_left		= 6 * SUBREGION_BLOCK_SIZE,
	foot_right				= 9 * SUBREGION_BLOCK_SIZE,
	expansion_foot_right	= 13 * SUBREGION_BLOCK_SIZE,
	torso					= 16 * SUBREGION_BLOCK_SIZE,
	expansion_chest			= 24 * SUBREGION_BLOCK_SIZE,
	back					= 32 * SUBREGION_BLOCK_SIZE,
	expansion_back			= 40 * SUBREGION_BLOCK_SIZE,
	groin					= 48 * SUBREGION_BLOCK_SIZE,
	expansion_groin			= 52 * SUBREGION_BLOCK_SIZE,
	butt					= 56 * SUBREGION_BLOCK_SIZE,
	expansion_butt			= 60 * SUBREGION_BLOCK_SIZE,
	arm_left				= 64 * SUBREGION_BLOCK_SIZE,
	expansion_arm_left		= 68 * SUBREGION_BLOCK_SIZE,
	arm_right				= 72 * SUBREGION_BLOCK_SIZE,
	expansion_arm_right		= 76 * SUBREGION_BLOCK_SIZE,
	head					= 80 * SUBREGION_BLOCK_SIZE,
	hand_left				= 112 * SUBREGION_BLOCK_SIZE,
	hand_right				= 144 * SUBREGION_BLOCK_SIZE,
	unreserved				= 176 * SUBREGION_BLOCK_SIZE
};
BETTER_ENUM(SubRegionId, uint64_t, 
	nsvr_region_unknown = 0,
	nsvr_region_torso = 500000,
		nsvr_region_torso_front = 100000,
		nsvr_region_torso_front_left = 1100000,
		nsvr_region_chest_front_left = 1110000,
	nsvr_region_abdomen_front_left = 1120000,
		nsvr_region_ab_upper_left = 1121000,
		nsvr_region_ab_middle_left = 1122000,
		nsvr_region_ab_lower_left = 1123000,
	nsvr_region_torso_front_right = 1200000,
	nsvr_region_chest_front_right = 1210000,
	nsvr_region_abdomen_front_right = 1220000,
		nsvr_region_ab_upper_right = 1221000,
		nsvr_region_ab_middle_right = 1222000,
		nsvr_region_ab_lower_right = 1223000,
	nsvr_region_torso_back = 2000000,
	nsvr_region_torso_back_left = 2100000,
	nsvr_region_back_upper_left = 2110000,
	nsvr_region_back_lower_left = 2120000,
	nsvr_region_torso_back_right = 2200000,
	nsvr_region_back_upper_right = 2210000,
	nsvr_region_back_lower_right = 2220000,
	nsvr_region_arm_left = 3000000,
	nsvr_region_shoulder_left = 3100000,
	nsvr_region_upper_arm_left = 3200000,
	nsvr_region_lower_arm_left = 3300000,
	nsvr_region_hand_left = 3400000,
	nsvr_region_arm_right = 4000000,
	nsvr_region_shoulder_right = 4100000,
	nsvr_region_upper_arm_right = 4200000,
	nsvr_region_lower_arm_right = 4300000,
	nsvr_region_hand_right = 4400000,
	nsvr_region_leg_left = 5000000,
		nsvr_region_upper_leg_left = 5000001,

		nsvr_region_upper_leg_front_left = 5100000,
		nsvr_region_upper_leg_back_left = 5200000,
		nsvr_region_lower_leg_left = 5200001,

		nsvr_region_lower_leg_front_left = 5300000,
		nsvr_region_lower_leg_back_left = 5400000,
	nsvr_region_leg_right = 6000000,
		nsvr_region_upper_leg_right = 6000001,
		nsvr_region_upper_leg_front_right = 6100000,
		nsvr_region_upper_legt_back_right = 6200000,
		nsvr_region_lower_leg_right = 6200001,

		nsvr_region_lower_leg_front_right = 6300000,
		nsvr_region_lower_leg_back_right = 6400000,
	nsvr_region_groin = 7000000,
	nsvr_region_gluteal = 8000000,
	nsvr_region_head = 9000000



)

