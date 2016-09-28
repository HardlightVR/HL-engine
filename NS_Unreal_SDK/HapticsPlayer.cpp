#include "HapticsPlayer.h"



HapticsPlayer::HapticsPlayer(std::shared_ptr<SuitHardwareInterface> hardware, const std::string& basePath):
	_resolver(basePath), _executor(hardware)
{
	


}	

void HapticsPlayer::Play(PatternFileInfo fileInfo, Side side = Side::Mirror) {
	_executor.Play(_resolver.ResolvePattern(fileInfo.FullId, side));
}



HapticsPlayer::~HapticsPlayer()
{
}
