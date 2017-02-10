#pragma once


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