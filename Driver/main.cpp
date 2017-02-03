
#include "stdafx.h"
#include "Driver.h"
#include "Locator.h"
#include "ConsoleLogger.h"
#include "EnumTranslator.h"

int main()
{
	
	Locator::initialize();

	Locator::provide(new ConsoleLogger());
	Locator::provide(new EnumTranslator());


	Driver driver;

	driver.StartThread();
	std::cin.get();
	driver.Shutdown();
	return 0;
}

