#pragma once

#include "HardwareIO.h"


class SerialPortIO : public HardwareIO {
public:
	SerialPortIO(std::unique_ptr<boost::asio::serial_port> port);
private:
	void do_interface_cleanup() override;
	void do_interface_creation() override;

	ReaderAdapter * get_reader() const override;
	WriterAdapter * get_writer() const override;

	std::unique_ptr<boost::asio::serial_port> m_port;

	std::shared_ptr<ReaderAdapter> m_reader;
	std::shared_ptr<WriterAdapter> m_writer;

};