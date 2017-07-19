#include "stdafx.h"
#include "HardwareDataModel.h"
#include "HardwareCoordinator.h"
#include "pevent.h"



HardwareDataModel::HardwareDataModel(HardwareCoordinator & parentCoordinator) : m_parent(parentCoordinator),
 m_lowlevel(parentCoordinator)

{
}



void HardwareDataModel::Raise(const nsvr::pevents::device_event& event)
{
	switch (event.type) {
	case nsvr_device_event_device_connected:
		m_connected = true;
		break;
	case nsvr_device_event_device_disconnected:
		m_connected = false;
		break;
	case nsvr_device_event_tracking_update:
	{
		auto data = static_cast<const nsvr::pevents::tracking_update*>(&event);
		m_trackingData[data->region] = data->quat;
		break;
	}
	default:
		break;
	}
}

HardwareCoordinator & HardwareDataModel::GetParentCoordinator()
{
	return m_parent;
}

LowLevelInterface & HardwareDataModel::LowLevel()
{
	return m_lowlevel;
}

