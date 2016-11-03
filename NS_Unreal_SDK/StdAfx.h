#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifndef WINVER
#define WINVER 0x0502
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0502
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#define _AFX_ALL_WARNINGS // turns off MFC's hiding of some common and often safely ignored warning messages

#ifndef _SECURE_ATL
#define _SECURE_ATL 1 //Use the Secure C Runtime in ATL
#endif
#define NOMINMAX
//Pull in MFC support
//#include <afxext.h> 
//#include <afxtempl.h>
#include "stdint.h"
#include <thread>

#include <boost\thread.hpp>
#include <boost\asio.hpp>
#include <boost\asio\placeholders.hpp>
#include <boost\asio\deadline_timer.hpp>
#include <boost\asio\io_service.hpp>
#include <boost\asio\use_future.hpp>
#include "Enums.h"
#include "HexUtils.h"
#include <mutex>
#include <cassert>
#include <chrono>
#include <memory>
#include "Wire.h"
#include "EncodingOperations.h"
#include "zmq.hpp"
#include "zmq_addon.hpp"
#include "IoService.h"
#include <boost\optional\optional_io.hpp>
#include "enumser.h" //windows specific
