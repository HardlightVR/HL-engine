#include "stdafx.h"
#include <functional>
#include "protobuff_defs/DriverCommand.pb.h"
#include "SuitVersionInfo.h"
#include "runtime_include/NSDriverApi.h"
#include "logger.h"


#include "Driver.h"
#include "Enums.h"
#include "IoService.h"
#include "DeviceBuilder.h"
#include "FakeDiscoverer.h"

//Belongs in the hardlight plugin now
//void extractDrvData(const packet& packet) {
//	//as status register:
//	uint8_t whichDrv = packet.raw[4];
//	uint8_t whichReg = packet.raw[5];
//	uint8_t data = packet.raw[3];
//
//	bool over_current = data & 0x01;
//	bool over_temperature = data & 0x02;
//	
//	bool mode = data & (0x07);
//
//	bool standby = data & 0b00100000;
//
//
//
//	BOOST_LOG_TRIVIAL(info) << "[DriverMain] DRVDIAG " << int(whichDrv) << "," <<int(over_current) << "," << int(over_temperature);
//
//	
//}

Driver::Driver() :
	m_ioService(new IoService()),
	m_io(m_ioService->GetIOService()),
	m_messenger(m_io),

	m_pluginManager(m_io),
	m_devices(),
	m_coordinator(m_io, m_messenger, m_devices),

	m_statusPush(m_io, boost::posix_time::millisec(250)),
	m_hapticsPull(m_io, boost::posix_time::millisec(5)),
	m_commandPull(m_io, boost::posix_time::millisec(50)),
	m_trackingPush(m_io, boost::posix_time::millisec(10)),
	m_curveEngineUpdate(m_io, boost::posix_time::millisec(5)),
	m_cachedTracking({}),
	m_eventDispatcher()



{
	//BOOST_LOG_SEV(clogger::get(), nsvr_loglevel_info) << "Plugin host initialized";
	LOG_INFO() << "Plugin host initialized";

	m_pluginManager.OnFatalError([this]() {
		this->Shutdown();
		std::exit(-100);
	});
	m_pluginManager.SetDeviceContainer(&m_devices);

	m_pluginManager.Discover();
	m_pluginManager.LoadAll();

	m_coordinator.SetupSubscriptions(m_eventDispatcher);




	/*m_hardware.RegisterPacketCallback(SuitPacket::PacketType::SuitVersion, [this](auto packet) {
		SuitVersionInfo version(packet);
		BOOST_LOG_TRIVIAL(info) << "[DriverMain] Suit diagnostics: Running firmware v" << version.Major << "." << version.Minor;

		if (version.Major == 2 && version.Minor == 5) {

			m_imus.AssignMapping(1, Imu::Left_Upper_Arm);
			m_imus.AssignMapping(3, Imu::Right_Upper_Arm);
			m_imus.AssignMapping(0, Imu::Chest);
		}
		else if (version.Major == 2 && version.Minor == 3) {
			m_imus.AssignMapping(0, Imu::Chest);
		}
		else if (version.Major == 2 && version.Minor == 4) {
			m_imus.AssignMapping(3, Imu::Chest);
		}
	});*/

	/*m_hardware.RegisterPacketCallback(SuitPacket::PacketType::DrvStatus, [&](auto packet) {
		extractDrvData(packet);

	});*/

	
	
}

Driver::~Driver()
{
	std::cout << "DRIVER DESTRUCTOR\n";

}

bool Driver::StartThread()
{
	m_curveEngineUpdate.SetEvent([this]() {
		constexpr auto fraction_of_second = (1.0f / 1000.f);
		auto dt = 5 * fraction_of_second;

		//m_curveEngine.Update(dt);

	});

	m_curveEngineUpdate.Start();

	m_hapticsPull.SetEvent([this]() { handleHaptics(); });
	m_hapticsPull.Start();

	m_statusPush.SetEvent([this]() { handleStatus(); });
	m_statusPush.Start();

	m_commandPull.SetEvent([this]() {handleCommands(); });
	m_commandPull.Start();

	m_trackingPush.SetEvent([this]() {handleTracking(); });
	m_trackingPush.Start();
	return true;
}

bool Driver::Shutdown()
{
	BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "Shutting down plugin host";

	m_curveEngineUpdate.Stop();
	m_statusPush.Stop();
	m_hapticsPull.Stop();
	m_commandPull.Stop();
	m_trackingPush.Stop();
	m_messenger.Disconnect();
	
	m_ioService->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::DrawDiagnostics(uint32_t id)
{
	nsvr_diagnostics_ui ui;
	ui.button = m_renderingApi.button;
	ui.keyval = m_renderingApi.keyval;

	m_pluginManager.DrawDiagnostics(id, &ui);

}

int Driver::EnumeratePlugins(hvr_plugin_list * outPlugins)
{
	auto ids = m_pluginManager.GetPluginIds();
	std::copy(ids.begin(), ids.end(), outPlugins->ids);
	outPlugins->count = ids.size();
	return 1;
}

int Driver::GetPluginInfo(hvr_plugin_id id, hvr_plugin_info* outInfo) {
	if (auto optionalInfo = m_pluginManager.GetPluginInfo(id)) {
		std::copy(std::begin(optionalInfo->first), std::end(optionalInfo->first), outInfo->name);
		return 1;
	}
	else {
		return 0;
	}
}



void Driver::handleHaptics()
{
	//note: changed to ReadEvents instead of ReadHaptics
	if (auto commands = m_messenger.ReadEvents()) {
		for (const auto& command : *commands) {
			m_eventDispatcher.ReceiveHighLevelEvent(command);
		}
	}

	
}

void Driver::handleStatus()
{
	//m_messenger.WriteSystems(m_hardware.PollDevice());
}

void DoForEachBit(std::function<void(Location l)> fn, uint32_t bits) {
	for (uint32_t bit = 1; bits >= bit; bit *= 2) if (bits & bit) fn(Location(bit));

}

//todo: delete this
void Driver::handleCommands()
{
	if (auto commands = m_messenger.ReadCommands()) {
		for (const auto& command : *commands) {
			switch (command.command()) {
			case NullSpaceIPC::DriverCommand_Command_DISABLE_TRACKING:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Disabling tracking";

				//m_hardware.DisableTracking();
				break;
			case NullSpaceIPC::DriverCommand_Command_ENABLE_TRACKING:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Enabling tracking";
				//m_hardware.EnableTracking();
				break;
			case NullSpaceIPC::DriverCommand_Command_ENABLE_AUDIO:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Enabling audio mode for all pads";
				//FirmwareInterface::AudioOptions options;
				//options.AudioMin =  command.params().at("audio_min");
				//options.AudioMax = command.params().at("audio_max");
				//options.PeakTime = command.params().at("peak_time");
				//options.Filter = command.params().at("filter");
				for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {
				//	m_hardware.EnableAudioMode(static_cast<Location>(loc), options);
				}
				break;
			case NullSpaceIPC::DriverCommand_Command_DISABLE_AUDIO:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Disabling audio mode for all pads";
			
				for (int loc = (int)Location::Lower_Ab_Right; loc != (int)Location::Error; loc++) {
				//	m_hardware.EnableIntrigMode(static_cast<Location>(loc));
				}
				break;
			case NullSpaceIPC::DriverCommand_Command_RAW_COMMAND:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Submitting raw command to the suit";
				//m_hardware.RawCommand(command.raw_command());
				break;
			case NullSpaceIPC::DriverCommand_Command_DUMP_DEVICE_DIAGNOSTICS:
				BOOST_LOG_TRIVIAL(info) << "[DriverMain] Requesting device diagnostics..";
				for (int loc = (int)Location::Lower_Ab_Right; loc != (int) Location::Error; loc++) {
				//	m_hardware.ReadDriverData(static_cast<Location>(loc));
				}
				break;
			default: 
				break;
			}
		}
	}
}

void Driver::handleTracking()
{
	NullSpace::SharedMemory::TrackingUpdate update = m_cachedTracking;

	
	//if (auto quat = m_imus.GetOrientation(Imu::Chest)) {
	//	update.chest = *quat;
	//}

	//if (auto quat = m_imus.GetOrientation(Imu::Left_Upper_Arm)) {
	//	update.left_upper_arm = *quat;
	//}

	//if (auto quat = m_imus.GetOrientation(Imu::Right_Upper_Arm)) {
	//	update.right_upper_arm = *quat;
	//}
	//
	//m_cachedTracking = update;

	//m_messenger.WriteTracking(update);
}

void Driver::ProvideRenderingApi(hvr_diagnostics_ui * api)
{
	m_renderingApi = *api;
}

