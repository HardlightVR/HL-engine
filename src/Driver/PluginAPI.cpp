#include "stdafx.h"
#include "PluginAPI.h"
#include "PluginInstance.h"
#include "pevent.h"
#include "HardwareCoordinator.h"

NSVR_CORE_RETURN(int) nsvr_pevent_create(nsvr_pevent ** event, nsvr_pevent_type type)
{
	using namespace nsvr::pevents;
	
	pevent* possible = pevent::make(type);

	if (possible == nullptr) {
		return -1;
	}
	else {
		*event = reinterpret_cast<nsvr_pevent*>(possible);
		return 1;
	}
}




NSVR_CORE_RETURN(int) nsvr_register_cevent_hook(nsvr_core_ctx* core, nsvr_cevent_type eventType, nsvr_cevent_callback cb){
	if (core == nullptr) { return -1; }
	HardwareDataModel* model = reinterpret_cast<HardwareDataModel*>(core);
	auto& coordinator = model->GetParentCoordinator();
	coordinator.Register(eventType, cb.handler, cb.targetVersion, cb.user_data);
	return 1;

}
NSVR_CORE_RETURN(int) nsvr_pevent_raise(nsvr_core_ctx* core, nsvr_pevent* event) {
	if (event == nullptr) {
		return -1;
	}
	reinterpret_cast<HardwareDataModel*>(core)->Raise(*reinterpret_cast<nsvr::pevents::pevent*>(event));
	return 1;
}

NSVR_CORE_RETURN(int) nsvr_pevent_destroy(nsvr_pevent** event) {
	if (event != nullptr) {
		delete reinterpret_cast<nsvr::pevents::pevent*>(*event);
		*event = nullptr;
		return 1;
	}
	return -1;

}
NSVR_CORE_RETURN(int) NSVR_Configuration_GetCallback(const NSVR_Configuration * config, const char * name, nsvr_callback* callback)
{
	if (config->Callbacks.find(name) != config->Callbacks.end()) {
		const auto& cb = config->Callbacks.at(name);
		callback->callback = cb.callback;
		callback->context = cb.context;
		return 1;
	}
	
	return -1;

}







