#pragma once
#include <queue>
#include "Enums.h"

#include "BoostSerialAdapter.h"
#include "InstructionBuilder.h"
#include <boost/asio/io_service.hpp>
#include "zone_logic/HardwareCommands.h"
#include "PacketVersion.h"
namespace nsvr {
	namespace config {
		struct Instruction;
	}
}

class FirmwareInterface
{
	
public:
	

	struct AudioOptions {
		int AudioMax;
		int AudioMin;
		int PeakTime;
		int Filter;
	};
	FirmwareInterface(const std::string& data_dir, BoostSerialAdapter* adapter, boost::asio::io_service& io);


	void Execute(const CommandBuffer& buffer);
	void PlayEffect(Location location, uint32_t effect, float strength);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, uint32_t effect, float strength);

	void RequestTrackingStatus();
	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();
	void ReadDriverData(Location loc, Register reg);
	void ResetDrivers();

	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions&);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);
	void PlayRtp(Location location, int strength);
	void Ping();
	void RawCommand(const uint8_t* bytes, std::size_t length);

	std::size_t GetTotalBytesSent() const;
private:
	//So as far as I can tell.. the queue is written to from the IO thread, and then read from the IO thread, all using boost::asio.
	//So the handlers will run synchronously, therefore we don't need a lock-free queue in the first place. 
	boost::lockfree::spsc_queue<uint8_t, boost::lockfree::capacity<10240>> m_queue;

	std::shared_ptr<InstructionSet> m_instructionSet;

	InstructionBuilder m_instructionBuilder;

	BoostSerialAdapter* m_serial;

	PacketVersion m_packetVersion;

	bool m_isBatching;

	std::size_t m_totalBytesSent;

	boost::posix_time::milliseconds m_writeInterval;
	boost::asio::deadline_timer m_writeTimer;
	boost::posix_time::milliseconds m_batchingTimeout;
	boost::asio::deadline_timer m_batchingDeadline;
	void verifyThenQueue(const nsvr::config::Instruction& inst);
	void verifyThenQueue(InstructionBuilder& builder);
	void verifyThenQueue(InstructionBuilder& builder, const nsvr::config::Instruction& alternate);
	void queuePacket(const std::vector<uint8_t>& packet);
	void writeBuffer();
	
	
	
	
};

