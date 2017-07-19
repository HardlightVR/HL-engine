#include "stdafx.h"

#include "openvr.h"
#include <cstdio>
#include <iostream>
int main() {

	vr::IVRSystem* system = nullptr;
	vr::EVRInitError eError = vr::VRInitError_None;
	system = vr::VR_Init(&eError, vr::VRApplication_Scene);
	if (eError != vr::VRInitError_None)
	{
		system = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		std::cout << buf << '\n';
		
	}

	std::cin.get();
	return 1;
}