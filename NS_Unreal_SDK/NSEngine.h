#pragma once

#include "Engine.h"
class NSEngine
{
public:
	NSEngine();
	~NSEngine();
	void StartThread();
	void Update();
	bool Shutdown();
private:
	const boost::posix_time::milliseconds suit_status_update_interval;
	EncodingOperations _encoder;
	std::shared_ptr<IoService> io;
	zmq::context_t context; //order dependency
	zmq::socket_t server_updates; //order dependency
	zmq::socket_t haptic_requests;//order dependency
	Engine engine; //order dependency
	boost::asio::deadline_timer suitStatusTimer; //order dependency
	void sendSuitStatusMsg(const boost::system::error_code& ec, zmq::socket_t* socket);

	std::chrono::steady_clock::time_point lastFrameTime;
	std::thread _workThread;
	std::atomic<bool> _running;
	void _UpdateLoop();

};

