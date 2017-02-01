#include "stdafx.h"
#include "Locator.h"

NullLogger Locator::_nullLogger = NullLogger();

ILogger *Locator::_logger;