#include "StdAfx.h"

#include "SuitHardwareInterface.h"

#include <boost/thread/thread.hpp> 
#include "SerialAdapter.h"

#include "HapticDirectoryTools.h"
#include "DependencyResolver.h"
#include "HapticFileInfo.h"
#include "Synchronizer.h"
#include "HapticsExecutor.h"
int main() {
	

	auto suit = std::make_shared<SuitHardwareInterface>();
	
	std::shared_ptr<ICommunicationAdapter> adapter(new SerialAdapter());
	adapter->Connect();

	suit->SetAdapter(adapter);


	//Parser p;
	//p.SetBasePath("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	//p.EnumerateHapticFiles();

	DependencyResolver resolver("C:/Users/NullSpace Team/Documents/API_Environment/Assets/StreamingAssets");
	resolver.Load(SequenceFileInfo("ns.strong_click2"));
	resolver.Load(PatternFileInfo("ns.full_body_jolt2"));

	auto effect = resolver.ResolvePattern("ns.full_body_jolt2", Side::Mirror);
	

	//Synchronizer sync(adapter->GetDataStream(), std::make_shared<PacketDispatcher>());
	HapticsExecutor exec(suit);
	
//	std::vector<HapticEffect> effect;
//	effect.push_back(HapticEffect(Effect::Smooth_Hum_50, Location::Chest_Left, 1.0, 0.0, 1));
	exec.Play(effect);
	for (auto i = 0; i < 1000; i++)
	{
	
		exec.Update(.1f);
		boost::this_thread::sleep(boost::posix_time::millisec(100));

	}


	
	
}
