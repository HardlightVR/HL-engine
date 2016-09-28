#pragma once
#include "DependencyResolver.h"
#include "HapticsExecutor.h"
#include "SuitHardwareInterface.h"
#include "HapticFileInfo.h"
#include "Enums.h"
class HapticsPlayer
{
public:
	HapticsPlayer(std::shared_ptr<SuitHardwareInterface> hardware, const std::string& basePath);
	~HapticsPlayer();
	void Play(PatternFileInfo fileInfo, Side side = Side::Mirror);
private:
	DependencyResolver _resolver;
	HapticsExecutor _executor;

};

