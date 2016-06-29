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

//Pull in MFC support
//#include <afxext.h> 
//#include <afxtempl.h>

//Or Pull in Standard Windows support
#include <Windows.h>

//Pull in ATL support
#include <atlbase.h>
#include <atlstr.h>

//Other includes
#include <tchar.h>
#include <setupapi.h>
#include <malloc.h>
#include <winspool.h>
#include <Wbemcli.h>
#include <comdef.h>
#include <stdio.h>

#define CENUMERATESERIAL_USE_STL //Uncomment this line if you want to test the MFC / ATL support in CEnumerateSerial

#ifdef CENUMERATESERIAL_USE_STL
//Pull in STL support
#include <vector>
#include <string>
#include <memory>
#endif

//Out of the box lets exclude support for CEnumerateSerial::UsingComDB on the Windows SDK 7.1 or earlier since msports.h
//is only available with the Windows SDK 8 or later.
#include <ntverp.h>
#if VER_PRODUCTBUILD <= 7600
#define NO_CENUMERATESERIAL_USING_COMDB
#endif

#ifndef NO_CENUMERATESERIAL_USING_COMDB
#include <msports.h>
#endif //#ifndef NO_CENUMERATESERIAL_USING_COMDB