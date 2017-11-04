#pragma once
#include <queue>
#include "Enums.h"
#include <boost/lockfree/spsc_queue.hpp>
#include "InstructionBuilder.h"
#include <boost/asio/io_service.hpp>
#include "zone_logic/HardwareCommands.h"
#include "PacketVersion.h"
namespace nsvr {
	namespace config {
		struct Instruction;
	}
}

class FirmwareInterface  : public std::enable_shared_from_this<FirmwareInterface>
{
	
public:
	

	struct AudioOptions {
		int VibeCtrl;
		int AudioMin;
		int AudioMax;
		int MinDrv;
		int MaxDrv;
	};
	FirmwareInterface(const std::string& data_dir, std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::io_service& io);
	~FirmwareInterface();

	

	void Execute(const CommandBuffer& buffer);
	void PlayEffect(Location location, uint32_t effect, float strength);
	void HaltEffect(Location location);
	void PlayEffectContinuous(Location location, uint32_t effect, float strength);
	void RequestUuid();
	void RequestTrackingStatus();
	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();



	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions&);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);
	void PlayRtp(Location location, int strength);
	void Ping();

	std::size_t GetTotalBytesSent() const;
private:


	std::shared_ptr<InstructionSet> m_instructionSet;

	InstructionBuilder m_instructionBuilder;

	std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> m_outgoing;

	PacketVersion m_packetVersion;

	bool m_isBatching;

	std::size_t m_totalBytesSent;

	void verifyThenQueue(const nsvr::config::Instruction& inst);
	void verifyThenQueue(InstructionBuilder& builder);
	void verifyThenQueue(InstructionBuilder& builder, const nsvr::config::Instruction& alternate);
	void queuePacket(const std::vector<uint8_t>& packet);
	
	std::mutex m_packetLock;
	
	
};

