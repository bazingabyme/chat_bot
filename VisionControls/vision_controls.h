#pragma once

#include <afxShellTreeCtrl.h>  
#include <afxShellListCtrl.h>  

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

//WM_USER + 459

#include "resource.h"

#include "button.h"
#include "dropdownlist.h"
#include "dropdownlistex.h"
#include "listctrl.h"
#include "listctrlstyled.h"
#include "sortlistctrl.h"
#include "thumblistctrl.h"
#include "thumblistctrlpro.h"
#include "queuectrl.h"
#include "spinctrl.h"
#include "sliderctrl.h"
#include "itemctrl.h"
#include "tabitemctrl.h"
#include "tabctrl.h"
#include "gridctrl.h"
#include "progressctrl.h"
#include "labelctrl.h"
#include "editctrl.h"
#include "searchctrl.h"
#include "capsulectrl.h"
#include "keyboard_dlg.h"
#include "menuctrl.h"
#include "onoffctrl.h"
#include "captionbarctrl.h"
#include "scrollbar.h"
#include "imagectrl.h"
#include "file_dlg.h"
#include "gridctrlex.h"
#include "win_paint.h"