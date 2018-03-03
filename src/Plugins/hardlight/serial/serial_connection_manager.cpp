 #include "stdafx.h"
#include "serial_connection_manager.h"
#include <iostream>
serial_connection_manager::serial_connection_manager() 
: m_thread()
, m_io(std::make_unique<boost::asio::io_service>())
, m_work(std::make_unique<boost::asio::io_service::work>(*m_io))
, m_connections()

{
	m_thread = std::thread([this]() {
		m_io->run();
	});
}

void serial_connection_manager::start(serial_connection_ptr s)
{
	m_connections.insert(s);
	s->start();
}

void serial_connection_manager::reset_io() {
	m_work.reset();
	m_io->stop();
	
	//wait for io_service to shut down
	if (m_thread.joinable()) {
		m_thread.join();
	}
	m_io = std::make_unique<boost::asio::io_service>();
//	std::cout << "IO Service was destroyed completely and remade\n";
	m_work = std::make_unique<boost::asio::io_service::work>(*m_io);
	m_thread = std::thread([this]() {
		m_io->run();
	});
	
}
void serial_connection_manager::raise_connect(serial_connection_ptr s) {
	m_connections.erase(s);
	s->stop();
	m_onConnect(wired_connection{ s->port_name() });
}

void serial_connection_manager::stop(serial_connection_ptr s)
{

	m_connections.erase(s);
	s->stop();

}

void serial_connection_manager::stop_all()
{
	for (auto c : m_connections) {
		c->stop();
	}

	m_connections.clear();
}

std::unique_ptr<boost::asio::serial_port> serial_connection_manager::make_port() {
	return std::make_unique<boost::asio::serial_port>(*m_io);
}

size_t serial_connection_manager::get_num_connections() const
{
	return m_connections.size();
}

void serial_connection_manager::on_connect(connection_event slot)
{
	m_onConnect = slot;
}

