#include "HapticsPlayer.h"



HapticsPlayer::HapticsPlayer(std::shared_ptr<SuitHardwareInterface> hardware, const std::string& basePath):
	_resolver(basePath), _executor(hardware)
{
	


}	


HapticsPlayer::~HapticsPlayer()
{
}
