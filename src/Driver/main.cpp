
#include "stdafx.h"
#include "Driver.h"
#include "Locator.h"
#include "ConsoleLogger.h"
#include "MessengerLogger.h"
#include "EnumTranslator.h"
#include <boost\interprocess\interprocess_fwd.hpp>


int main()
{
	
	BOOST_LOG_TRIVIAL(trace) << "Booting";
	Locator::initialize();

	//Locator::provide(new MessengerLogger();
	Locator::provide(new EnumTranslator());

	
	Driver driver;
		driver.StartThread();
		std::cin.get();
		driver.Shutdown();
	
	return 0;
}

