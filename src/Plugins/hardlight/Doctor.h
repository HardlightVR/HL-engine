
#include "PluginAPI.h"
#include <unordered_map>
#include <mutex>
class Doctor {
public:
	enum class Status {
		BadMotors = -1,
		BadImus = -2,
		Unknown = 0,	//don't know anything about this device
		OkDiagnostics = 1,			//this device is verified ok
		Unplugged = 2,	//this device was known but is now unplugged
		CheckingDiagnostics = 3	//this device has responded to pings, but has not yet had diagnostics run

	};

	std::vector<nsvr_device_id> get_devices() const;
//	std::vector<nsvr_device_id> get_potential_devices() const;
//	std::vector<nsvr_device_id> get_verified_devices() const;

	Status get_device_status(nsvr_device_id id) const;


	//Diagnostics updates interface
	void notify_device_status(nsvr_device_id id, Status status);

private:
	std::unordered_map<nsvr_device_id, Status> m_devices;
	mutable std::mutex m_lock;
};