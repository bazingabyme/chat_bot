// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "..\VisionBuilder\formation.h"
#ifdef _DEBUG
	#ifdef _M_X64 
		#pragma comment(lib, "..\\x64\\Debug\\VisionBuilder.lib")
	#else 
		#pragma comment(lib, "..\\Debug\\VisionBuilder.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Release\\VisionBuilder.lib")
	#else
		#pragma comment(lib, "..\\Release\\VisionBuilder.lib")
	#endif
#endif

#include "Resource.h"

#include "language_dlg.h"

extern HINSTANCE g_resource_handle;

//LANGUAGE SEQUENCES IN STRING TABLE (.rc)
//Hindi (India)
//Czech (Czech Republic) 
//Polish (Poland)
//Romanian (Romania)
//Russian (Russia)
//German
//Italian
//English (US)
//Portuguese (Brazil)
//Spanish (Mexico)
//Turkish (Turkey)