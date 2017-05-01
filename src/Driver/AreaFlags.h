#pragma once


enum class AreaFlag : uint32_t {
	None = 0,
	Forearm_Left = 1 << 0,
	Upper_Arm_Left = 1 << 1,
	Shoulder_Left = 1 << 2,
	Back_Left = 1 << 3,
	Chest_Left = 1 << 4,
	Upper_Ab_Left = 1 << 5,
	Mid_Ab_Left = 1 << 6,
	Lower_Ab_Left = 1 << 7,

	Forearm_Right = 1 << 16,
	Upper_Arm_Right = 1 << 17,
	Shoulder_Right = 1 << 18,
	Back_Right = 1 << 19,
	Chest_Right = 1 << 20,
	Upper_Ab_Right = 1 << 21,
	Mid_Ab_Right = 1 << 22,
	Lower_Ab_Right = 1 << 23,

	Forearm_Both = Forearm_Left | Forearm_Right,
	Upper_Arm_Both = Upper_Arm_Left | Upper_Arm_Right,
	Shoulder_Both = Shoulder_Left | Shoulder_Right,
	Back_Both = Back_Left | Back_Right,
	Chest_Both = Chest_Left | Chest_Right,
	Upper_Ab_Both = Upper_Ab_Left | Upper_Ab_Right,
	Mid_Ab_Both = Mid_Ab_Left | Mid_Ab_Right,
	Lower_Ab_Both = Lower_Ab_Left | Lower_Ab_Right,

	Left_All = 0x000000FF,
	Right_All = 0x00FF0000,
	All_Areas = Left_All | Right_All
};
inline AreaFlag& operator|=(AreaFlag& a, const AreaFlag b)
{
	a = static_cast<AreaFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	return a;
}