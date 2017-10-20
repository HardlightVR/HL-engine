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

DeviceId<global> IdentificationService::FromLocal(PluginId plugin, DeviceId<local> id)
{
	LocalDevice local{ id, plugin };
	auto it = deviceLocalToGlobal.find(local);
	if (it != deviceLocalToGlobal.end()) {
		return it->second;
	}
	else {
		auto global = nextGlobalDeviceId();
		deviceLocalToGlobal[local] = global;
		deviceGlobalToLocal[global] = local;
		return global;
	}
}

boost::optional<LocalDevice> IdentificationService::FromGlobalDevice(DeviceId<global> id) const
{
	auto it = deviceGlobalToLocal.find(id);
	if (it != deviceGlobalToLocal.end()) {
		return it->second;
	}
	else {
		return boost::none;
	}
}

NodeId<global> IdentificationService::FromLocal(PluginId pluginId, DeviceId<local> deviceId, NodeId<local> nodeId)
{
	LocalNode local{ nodeId, pluginId, deviceId };
	auto it = nodeLocalToGlobal.find(local);
	if (it != nodeLocalToGlobal.end()) {
		return it->second;
	}
	else {
		auto globalId = nextGlobalNodeId();
		nodeLocalToGlobal[local] = globalId;
		nodeGlobalToLocal[globalId] = local;
		return globalId;
	}
}

boost::optional<LocalNode> IdentificationService::FromGlobalNode(NodeId<global> id) const
{
	auto it = nodeGlobalToLocal.find(id);
	if (it != nodeGlobalToLocal.end()) {
		return it->second;
	}
	else {
		return boost::none;
	}
}



DeviceId<global> IdentificationService::nextGlobalDeviceId()
{
	return DeviceId<global>{ ++currentGlobalDeviceId };
}

NodeId<global> IdentificationService::nextGlobalNodeId()
{
	return NodeId<global>{ ++currentGlobalNodeId };
}
