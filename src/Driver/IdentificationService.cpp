#include "stdafx.h"
#include "IdentificationService.h"



bool operator==(const LocalNode& lhs, const LocalNode& rhs) {
	return lhs.id == rhs.id && lhs.device_id == rhs.device_id && lhs.plugin == rhs.plugin;
}

bool operator==(const LocalDevice& lhs, const LocalDevice& rhs) {
	return lhs.id == rhs.id && lhs.plugin == rhs.plugin;
}



IdentificationService::IdentificationService()
	: currentGlobalNodeId(0)
	, currentGlobalDeviceId(0)

{
}

IdentificationService::GlobalDeviceId IdentificationService::FromLocal(PluginId plugin, LocalDeviceId id)
{
	LocalDevice local{ id, plugin };
	auto it = deviceLocalToGlobal.find(local);
	if (it != deviceLocalToGlobal.end()) {
		return it->second;
	}
	else {
		uint32_t globalId = nextGlobalDeviceId();
		deviceLocalToGlobal[local] = globalId;
		deviceGlobalToLocal[globalId] = local;
		return globalId;
	}
}

boost::optional<LocalDevice> IdentificationService::FromGlobalDevice(GlobalDeviceId id) const
{
	auto it = deviceGlobalToLocal.find(id);
	if (it != deviceGlobalToLocal.end()) {
		return it->second;
	}
	else {
		return boost::none;
	}
}

IdentificationService::GlobalNodeId IdentificationService::FromLocal(PluginId pluginId, LocalDeviceId deviceId, LocalNodeId nodeId)
{
	LocalNode local{ nodeId, pluginId, deviceId };
	auto it = nodeLocalToGlobal.find(local);
	if (it != nodeLocalToGlobal.end()) {
		return it->second;
	}
	else {
		uint32_t globalId = nextGlobalNodeId();
		nodeLocalToGlobal[local] = globalId;
		nodeGlobalToLocal[globalId] = local;
		return globalId;
	}
}

boost::optional<LocalNode> IdentificationService::FromGlobalNode(GlobalNodeId id) const
{
	auto it = nodeGlobalToLocal.find(id);
	if (it != nodeGlobalToLocal.end()) {
		return it->second;
	}
	else {
		return boost::none;
	}
}



uint32_t IdentificationService::nextGlobalDeviceId()
{
	return ++currentGlobalDeviceId;
}

uint32_t IdentificationService::nextGlobalNodeId()
{
	return ++currentGlobalNodeId;
}
