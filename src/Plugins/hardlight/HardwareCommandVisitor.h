#pragma once
#include <boost/variant/static_visitor.hpp>
#include "zone_logic/HardwareCommands.h"
class FirmwareInterface;

class HardwareCommandVisitor : public boost::static_visitor<void> {
	private:
		FirmwareInterface& firmware;
	public:
		HardwareCommandVisitor(FirmwareInterface& f);
		void operator()(const PlaySingle& single);
		void operator()(const PlayCont& cont);
		void operator()(const PlayVol& vol);
		void operator()(const Halt& Halt);
		void operator()(const EnableIntrig& intrig);
		void operator()(const EnableRtp& rtp);

	
};

