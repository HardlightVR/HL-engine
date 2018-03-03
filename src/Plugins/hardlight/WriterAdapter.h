#pragma once




class WriterAdapter {
public:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual std::size_t total_bytes_written() const = 0;

};
