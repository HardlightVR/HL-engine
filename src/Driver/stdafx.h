// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
/* std */

#include <stdio.h>
#include <tchar.h>
#include <memory>
#include <vector>
#include <thread>

#include <string>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <mutex>

/* boost */


#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/trivial.hpp>
#include <boost\log\expressions.hpp>

#include <boost\thread.hpp>
#include <boost\asio\placeholders.hpp>
#include <boost\asio\io_service.hpp>
#include <boost\asio\use_future.hpp>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>
#include <boost\bind.hpp>


#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost\asio\serial_port.hpp>


/* 3rd party*/
#include "Json/json-forwards.h"
#include "Json/json.h"

// TODO: reference additional headers your program requires here
