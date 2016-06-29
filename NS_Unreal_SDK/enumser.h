/*
Module : enumser.h
Purpose: Defines the interface for a class to enumerate the serial ports installed on a PC
         using a number of different approaches
Created: PJN / 03-11-1998

Copyright (c) 1998 - 2016 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////////// Macros / Defines ////////////////////////////////////

#pragma once

#ifndef __ENUMSER_H__
#define __ENUMSER_H__

#ifndef CENUMERATESERIAL_EXT_CLASS
#define CENUMERATESERIAL_EXT_CLASS
#endif //#ifndef CENUMERATESERIAL_EXT_CLASS

#ifndef _Return_type_success_
#define _Return_type_success_(expr)
#endif //#ifndef _Return_type_success_

#ifndef _In_
#define _In_
#endif //#ifndef _In_

#ifndef _Out_
#define _Out_
#endif //#ifndef _Out_


///////////////////////// Includes ////////////////////////////////////////////                      

#ifndef __ATLBASE_H__
#pragma message("To avoid this message, please put atlbase.h in your pre compiled header (normally stdafx.h)")
#include <atlbase.h>
#endif //#ifndef __ATLBASE_H__

#ifdef CENUMERATESERIAL_USE_STL
  #ifndef _VECTOR_
    #pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
    #include <vector>
  #endif //#ifndef _VECTOR_
  #ifndef _STRING_
    #pragma message("To avoid this message, please put string in your pre compiled header (normally stdafx.h)")
    #include <string>
  #endif //#ifndef _STRING_
#else
  #ifdef _AFX
    #ifndef __AFXTEMPL_H__
      #pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (normally stdafx.h)")
      #include <afxtempl.h> 
    #endif //#ifndef __AFXTEMPL_H__
  #else
    #ifndef __ATLSTR_H__
      #pragma message("To avoid this message, please put atlstr.h in your pre compiled header (normally stdafx.h)")
      #include <atlstr.h>
    #endif //#ifndef __ATLSTR_H__
  #endif //#ifdef _AFX
#endif //#ifdef CENUMERATESERIAL_USE_STL


///////////////////////// Classes /////////////////////////////////////////////

class CENUMERATESERIAL_EXT_CLASS CEnumerateSerial
{
public:
//Typdefs
#ifdef CENUMERATESERIAL_USE_STL
  typedef std::vector<UINT> CPortsArray;
#ifdef _UNICODE
  typedef std::vector<std::wstring> CNamesArray;
#else
  typedef std::vector<std::string> CNamesArray;
#endif
#elif defined _AFX
  typedef CUIntArray CPortsArray;
  typedef CStringArray CNamesArray;
#else
  typedef CSimpleArray<UINT> CPortsArray;
  typedef CSimpleArray<ATL::CString> CNamesArray;
#endif //#ifdef CENUMERATESERIAL_USE_STL

//Methods
#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE
  static _Return_type_success_(return != 0) BOOL UsingCreateFile(_Out_ CPortsArray& ports);
#endif //#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE

#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE
  static _Return_type_success_(return != 0) BOOL UsingQueryDosDevice(_Out_ CPortsArray& ports);
#endif //#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE

#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG
  static _Return_type_success_(return != 0) BOOL UsingGetDefaultCommConfig(_Out_ CPortsArray& ports);
#endif //#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
  static _Return_type_success_(return != 0) BOOL UsingSetupAPI1(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
  static _Return_type_success_(return != 0) BOOL UsingSetupAPI2(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2

#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS
  static _Return_type_success_(return != 0) BOOL UsingEnumPorts(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS

#ifndef NO_CENUMERATESERIAL_USING_WMI
  static HRESULT UsingWMI(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_WMI

#ifndef NO_CENUMERATESERIAL_USING_COMDB
  static _Return_type_success_(return != 0) BOOL UsingComDB(_Out_ CPortsArray& ports);
#endif //#ifndef NO_CENUMERATESERIAL_USING_COMDB

#ifndef NO_CENUMERATESERIAL_USING_REGISTRY
  static _Return_type_success_(return != 0) BOOL UsingRegistry(_Out_ CNamesArray& ports);
#endif //#ifndef NO_CENUMERATESERIAL_USING_REGISTRY

protected:
//Methods
#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
  static _Return_type_success_(return != 0) BOOL RegQueryValueString(_In_ ATL::CRegKey& key, _In_ LPCTSTR lpValueName, _Out_ LPTSTR& pszValue);
  static _Return_type_success_(return != 0) BOOL QueryRegistryPortName(_In_ ATL::CRegKey& deviceKey, _Out_ int& nPort);
  static _Return_type_success_(return != 0) BOOL QueryUsingSetupAPI(const GUID& guid, _In_ DWORD dwFlags, _Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames);
  static _Return_type_success_(return != 0) BOOL QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName);
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
  static _Return_type_success_(return != 0) BOOL IsNumeric(_In_ LPCSTR pszString, _In_ BOOL bIgnoreColon);
  static _Return_type_success_(return != 0) BOOL IsNumeric(_In_ LPCWSTR pszString, _In_ BOOL bIgnoreColon);
};


#endif //#ifndef __ENUMSER_H__
