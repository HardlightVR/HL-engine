#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
#include "ClientMessenger.h" 
#include "IntermediateHapticFormats.h"
#include "Encoder.h"
Driver::Driver() :
	_io(new IoService()),
	_running(false),
	_pollTimer(_io->GetIOService()),
	_pollInterval(50),
	_hardware(_io),
	_messenger(_io->GetIOService()),
	_cMessenger(_io->GetIOService())

{
/*	_messenger.Receive([this](void const* data, std::size_t size) { 
		flatbuffers::Verifier verifier(reinterpret_cast<uint8_t const*>(data), size);
		if (NullSpace::HapticFiles::VerifyExecutionCommandBuffer(verifier)) {
			auto packet = NullSpace::HapticFiles::GetExecutionCommand(data);
			_hardware.ReceiveExecutionCommand(_encoder.Decode(packet));
		}
	
	
	
	});
	*/
//	_pollTimer.expires_from_now(_pollInterval);
	//_pollTimer.async_wait(boost::bind(&Driver::_UpdateLoop, this));
}

Driver::~Driver()
{
	if (_workThread.joinable()) {
		_workThread.join();
	}
}

bool Driver::StartThread()
{
	_running = true;

	_workThread = std::thread([this]() {

		while (_running.load()) {
			counter++;
			if (counter % 1000 == 0) {
				SuitsConnectionInfo info;
				info.SuitsFound[1] = true;
				info.Suits[1] = SuitInfo();
				info.Suits[1].Id = counter+5;
				info.Suits[1].Status = SuitStatus::Connected;
				_messenger.WriteSuits(info);
			}
			if (counter % 2500 == 0) {
				SuitsConnectionInfo info;
				info.SuitsFound[1] = true;
				info.SuitsFound[2] = true;
				info.Suits[1] = SuitInfo();
				info.Suits[1].Id = counter + 5;
				info.Suits[1].Status = SuitStatus::Connected;
				info.Suits[2] = SuitInfo();
				info.Suits[2].Id = counter + 5;
				info.Suits[2].Status = SuitStatus::Disconnected;
				_messenger.WriteSuits(info);
			}
			TrackingUpdate t;
			t.a = Quaternion();
			t.b = Quaternion();
			t.a.x = counter;
			t.b.x = counter + 14;
			_messenger.WriteTracking(t);

			if (auto optionalCommand = _messenger.ReadHaptics()) {
				auto c = optionalCommand.get();
				std::cout << "Command loication: " << c.Location << '\n';
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

//	_workThread = std::thread(boost::bind(&Driver::_UpdateLoop, this));
	_clientThread = std::thread([this]() {
		while (_running.load()) {
			auto tracking = _cMessenger.ReadTracking();
			auto suits = _cMessenger.ReadSuits();
			for (int i = 0; i < 4; i++) {
			//	if (suits.SuitsFound[i]) {
				//	std::cout << "Suit id: " << suits.Suits[i].Id << ", status = " << suits.Suits[i].Status << '\n';
			//	}
			}
			//std::cout << tracking.a.x << ", " << tracking.b.x << '\n';
			ExecutionCommand e;
			e.Command = 0;
			e.Effect = 124;
			e.Location = counter;
			_cMessenger.WriteHaptics(e);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	});
//	_pollTimer.expires_from_now(_pollInterval);
	//_pollTimer.async_wait(boost::bind(&Driver::_PollHandler, this, boost::asio::placeholders::error));

	
	return true;
}

bool Driver::Shutdown()
{
	_running.store(false);
//	_messenger.Disconnect();
	
	if (_messengerThread.joinable()) {
		_messengerThread.join();
	}
	//_pollTimer.cancel();
	
	_io->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::_UpdateLoop()
{

	//Encoder encoder;

	//encoder.AquireEncodingLock();
//	auto offset = encoder.Encode(c);
	//encoder._finalize(offset, [&messenger, &encoder, this](uint8_t* data, int size) {
	//	messenger.Send(data, size);
		//encoder.ReleaseEncodingLock();

	//	_pollTimer.expires_from_now(_pollInterval);
	//	_pollTimer.async_wait(boost::bind(&Driver::_UpdateLoop, this));


	//});

}

