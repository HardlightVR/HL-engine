
#include "stdafx.h"
#include "Driver.h"
#include "Locator.h"
#include "ConsoleLogger.h"


int main()
{
	
	Locator::initialize();

	Locator::provide(new ConsoleLogger());

	Driver driver;

	driver.StartThread();
	std::cin.get();
	driver.Shutdown();
	return 0;
}

