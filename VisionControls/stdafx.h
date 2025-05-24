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
#include <afxcontrolbars.h>
#include <afxShellTreeCtrl.h>  
#include <afxShellListCtrl.h>  

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#ifdef _DEBUG
#pragma comment(lib, "opencv_core480d.lib")
#pragma comment(lib, "opencv_imgproc480d.lib")
#pragma comment(lib, "opencv_highgui480d.lib")
#pragma comment(lib, "opencv_imgcodecs480d.lib")
#else
#pragma comment(lib, "opencv_core480.lib")
#pragma comment(lib, "opencv_imgproc480.lib")
#pragma comment(lib, "opencv_highgui480.lib")
#pragma comment(lib, "opencv_imgcodecs480.lib")
#endif

#include "resource.h"

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

#include "..\Language\language.h"
#include <afxdlgs.h>
#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Debug\\Language.lib")
	#else
		#pragma comment(lib, "..\\Debug\\Language.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Release\\Language.lib")
	#else
		#pragma comment(lib, "..\\Release\\Language.lib")
	#endif
#endif

extern HINSTANCE g_resource_handle;