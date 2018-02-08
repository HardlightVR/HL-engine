
#include "PluginAPI.h"
#include <unordered_map>
#include <mutex>
#include <vector>
#include "RetrySender.h"
#include "MotorDiagnosis.h"
#include "HardwareDiagnostic.h"


class PacketDispatcher;

class Doctor {
public:
	Doctor(boost::asio::io_service& io);
	using motor_id = uint8_t;
	enum class Status : int{
		Unknown = 0,
		InProgress = 1,
		NoErrors,
		Unplugged
	};
	using Report = boost::variant<Status, HardwareFailures>;

	//enum class Status {
	//	BadMotors = -100,
	//	SomeMotorsDidntRespond = -101,
	//	LeftDriverBoardUnplugged = -102, //16 and 23
	//	RightDriverBoardUnplugged = -103,
	//	BadImus = -200,
	//	GenericError = -1,
	//	Unknown = 0,	//don't know anything about this device
	//	OkDiagnostics = 1,			//this device is verified ok
	//	Unplugged = 2,	//this device was known but is now unplugged
	//	CheckingDiagnostics = 3	//this device has responded to pings, but has not yet had diagnostics run

	//};


	Report query_patient() const;


	void release_patient();
	void accept_patient(nsvr_device_id id, PacketDispatcher* dispatcher, boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data);

private:
	boost::asio::io_service& m_io;
	boost::optional<nsvr_device_id> m_patient;

	std::vector<std::unique_ptr<HardwareDiagnostic>> m_tests;


	void cancel_all_tests();
	void Doctor::restart_tests(PacketDispatcher* dispatcher, boost::lockfree::spsc_queue<uint8_t>* device_outgoing_data);

	bool all_finished() const;
	bool all_succeeded() const;

	HardwareFailures aggregate_failures() const;

};