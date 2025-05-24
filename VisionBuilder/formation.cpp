#include "stdafx.h"

#define INCLUDE_VISION_FRAMEWORK __declspec(dllexport)
#include "formation.h"

//VERSION DETAILS
#include <lm.h>
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "version.lib")

Formation* Formation::formation_ = NULL;

Formation::Formation() {

	formation_ = NULL;
	cs_for_console_.OwningThread = NULL;
}
Formation::~Formation() {

	Destroy();
}

Formation* Formation::Instance() {

	if (formation_ == NULL) {

		formation_ = new Formation();

		InitializeCriticalSection(&formation_->cs_for_console_);
		
		formation_->flash_wnd_count_ = 0;
		formation_->flash_wnd_time_ = 0;

		Gdiplus::GdiplusStartupInput gdiplus_startup_input;
		VERIFY(Gdiplus::GdiplusStartup(&formation_->gdiplus_token_, &gdiplus_startup_input, NULL) == Gdiplus::Ok);

		double one_percent = GetSystemMetrics(SM_CYSCREEN) / 100.0;	//1 percent of height
		formation_->spacing_[0] = int(ceil(one_percent * 0.2));	//0.2 percent of height
		formation_->spacing_[1] = int(ceil(one_percent * 0.4));	//0.4 percent of height
		formation_->spacing_[2] = formation_->spacing_[1] * 2;
		formation_->spacing_[3] = formation_->spacing_[1] * 3;
		formation_->spacing_[4] = int(ceil(one_percent * 2.4));			//2.4 percent of height
		formation_->title_height_ = int(ceil(one_percent * 3.6));		//3.6 percent of height
		formation_->control_height_ = int(ceil(one_percent * 4.8));		//4.8 percent of height

		formation_->image_area_width_in_percentage_ = 65;
		formation_->thumb_area_width_in_percentage_ = 8;
		formation_->toolbox_area_width_in_percentage_ = 27;

		int screen_width = GetSystemMetrics(SM_CXSCREEN); //Get the system metrics - VERT
		int screen_height = GetSystemMetrics(SM_CYSCREEN); //Get the system metrics - HORZ

		if (screen_width <= 1152 && screen_height <= 864) { //LOWER RESOLUTION
				
			formation_->spacing_[0] = int(ceil(one_percent * 0.1));	//0.1 percent of height
			formation_->spacing_[1] = int(ceil(one_percent * 0.2));	//0.2 percent of height
			formation_->spacing_[2] = formation_->spacing_[1] * 2;
			formation_->spacing_[3] = formation_->spacing_[1] * 3;
			formation_->spacing_[4] = int(ceil(one_percent * 1.8));			//1.8 percent of height
			formation_->title_height_ = int(ceil(one_percent * 2.7));		//2.7 percent of height
			formation_->control_height_ = int(ceil(one_percent * 3.6));		//3.6 percent of height

			formation_->font_[0].CreateFontW(-8, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[1].CreateFontW(-9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[2].CreateFontW(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[3].CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[4].CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[5].CreateFontW(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[6].CreateFontW(-20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[7].CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));

			formation_->icon_size_[0] = 16;
			formation_->icon_size_[1] = 16;
			formation_->icon_size_[2] = 32;

			formation_->screen_resolution_ = SCREEN_RESOLUTION::LOWER;

		} else if (screen_width <= 1680 && screen_height <= 1050) { //STANDARD RESOLUTION
			
			formation_->font_[0].CreateFontW(-10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[1].CreateFontW(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[2].CreateFontW(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[3].CreateFontW(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[4].CreateFontW(-21, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[5].CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[6].CreateFontW(-28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));
			formation_->font_[7].CreateFontW(-32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial Unicode MS"));

			formation_->icon_size_[0] = 16;
			formation_->icon_size_[1] = 24;
			formation_->icon_size_[2] = 32;

			formation_->screen_resolution_ = SCREEN_RESOLUTION::STANDARD;

		} else { //if (screen_width <= 1600 && screen_height <= 900) { //HIGHER RESOLUTION

			//if (screen_height < 1100) {

				formation_->font_[0].CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[1].CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[2].CreateFontW(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[3].CreateFontW(-21, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[4].CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[5].CreateFontW(-28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[6].CreateFontW(-32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[7].CreateFontW(-36, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));

			/*} else {

				formation_->font_[0].CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[1].CreateFontW(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[2].CreateFontW(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[3].CreateFontW(-21, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[4].CreateFontW(-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[5].CreateFontW(-28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[6].CreateFontW(-33, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
				formation_->font_[7].CreateFontW(-38, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial Unicode MS"));
			}*/

			formation_->icon_size_[0] = 24;
			formation_->icon_size_[1] = 32;
			formation_->icon_size_[2] = 40;
			formation_->screen_resolution_ = SCREEN_RESOLUTION::HIGHER;
		}

		formation_->taskbar_height_ = formation_->control_height_ + formation_->title_height_ - formation_->spacing_[2];

		formation_->image_bank_size_ = 2048.00;
		formation_->date_format_ = DB_DATE_FORMAT;

		printf("FORMATION PERCENT  1.00 %.2f\n", one_percent);
		printf("FORMATION SPACINGH 0.20 %d\n", formation_->spacing_[0]);
		printf("FORMATION SPACING1 0.40 %d\n", formation_->spacing_[1]);
		printf("FORMATION SPACING2 S1*2 %d\n", formation_->spacing_[2]);
		printf("FORMATION SPACING3 S1*3 %d\n", formation_->spacing_[3]);
		printf("FORMATION SPACING4 2.40 %d\n", formation_->spacing_[4]);
		printf("FORMATION HEADING  3.60 %d\n", formation_->title_height_);
		printf("FORMATION CONTROL  4.80 %d\n", formation_->control_height_);
		printf("FORMATION TASKBARH %d\n", formation_->taskbar_height_);
		printf("FORMATION SCRWIDTH %d\n", screen_width);
		printf("FORMATION SCHEIGHT %d\n", screen_height);

		//POPUP WINDOWS
		formation_->popup_dlg_array_.RemoveAll();
		formation_->popup_type_array_.RemoveAll();

		//WAIT CONTROL
		formation_->waitcontrol_.m_hWnd = NULL;

		formation_->spancolor1_brush_.CreateSolidBrush(BACKCOLOR1);
		formation_->spancolor1_sel_brush_.CreateSolidBrush(BACKCOLOR1_SEL);
		formation_->spancolor2_brush_.CreateSolidBrush(BACKCOLOR2);
		formation_->labelcolor_brush_.CreateSolidBrush(LABEL_COLOR);
		formation_->labelcolor2_brush_.CreateSolidBrush(LABEL_COLOR2);
		formation_->labelcolor3_brush_.CreateSolidBrush(LABEL_COLOR3);
		formation_->disablecolor_brush_.CreateSolidBrush(DISABLE_COLOR);
		formation_->accept_brush_.CreateSolidBrush(ACCEPT_COLOR);
		formation_->reject_brush_.CreateSolidBrush(REJECT_COLOR);
		formation_->scrollcolor_brush_.CreateSolidBrush(SCROLL_COLOR);
		formation_->whitecolor_brush_.CreateSolidBrush(WHITE_COLOR);
		formation_->blackcolor_brush_.CreateSolidBrush(BLACK_COLOR);
		formation_->bluecolor_brush_.CreateSolidBrush(BLUE_COLOR);

		formation_->spancolor1_pen_.CreatePen(PS_SOLID, 1, BACKCOLOR1);
		formation_->spancolor1_sel_pen_.CreatePen(PS_SOLID, 1, BACKCOLOR1_SEL);
		formation_->spancolor2_pen_.CreatePen(PS_SOLID, 1, BACKCOLOR2);
		formation_->labelcolor_pen_.CreatePen(PS_SOLID, 1, LABEL_COLOR);
		formation_->labellinecolor_pen_.CreatePen(PS_SOLID, 1, LABEL_LINE_COLOR);
		formation_->labelcolor2_pen_.CreatePen(PS_SOLID, 1, LABEL_COLOR2);
		formation_->labelcolor3_pen_.CreatePen(PS_SOLID, 1, LABEL_COLOR3);
		formation_->disablecolor_pen_.CreatePen(PS_SOLID, 1, DISABLE_COLOR);
		formation_->blackcolor_pen_.CreatePen(PS_SOLID, 1, BLACK_COLOR);
		formation_->blackcolor_pen2_.CreatePen(PS_SOLID, 2, BLACK_COLOR);
		formation_->whitecolor_pen_.CreatePen(PS_SOLID, 1, WHITE_COLOR);
		formation_->whitecolor_pen2_.CreatePen(PS_SOLID, 2, WHITE_COLOR);
		formation_->acceptcolor_pen_.CreatePen(PS_SOLID, 1, ACCEPT_COLOR);
		formation_->rejectcolor_pen_.CreatePen(PS_SOLID, 1, REJECT_COLOR);
		formation_->rejectcolor_pen2_.CreatePen(PS_SOLID, 2, REJECT_COLOR);
		formation_->scrollcolor_pen_.CreatePen(PS_SOLID, 1, SCROLL_COLOR);
		formation_->bluecolor_pen_.CreatePen(PS_SOLID, 1, BLUE_COLOR);

		formation_->spancolor1_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(BACKCOLOR1), GetGValue(BACKCOLOR1), GetBValue(BACKCOLOR1)));
		formation_->spancolor1_sel_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(BACKCOLOR1_SEL), GetGValue(BACKCOLOR1_SEL), GetBValue(BACKCOLOR1_SEL)));
		formation_->spancolor2_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(BACKCOLOR2), GetGValue(BACKCOLOR2), GetBValue(BACKCOLOR2)));
		formation_->blackcolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(BLACK_COLOR), GetGValue(BLACK_COLOR), GetBValue(BLACK_COLOR)));
		formation_->whitecolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(WHITE_COLOR), GetGValue(WHITE_COLOR), GetBValue(WHITE_COLOR)));
		formation_->disablecolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(DISABLE_COLOR), GetGValue(DISABLE_COLOR), GetBValue(DISABLE_COLOR)));
		formation_->labelcolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(LABEL_COLOR), GetGValue(LABEL_COLOR), GetBValue(LABEL_COLOR)));
		formation_->labelcolor2_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(LABEL_COLOR2), GetGValue(LABEL_COLOR2), GetBValue(LABEL_COLOR2)));
		formation_->labelcolor3_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(LABEL_COLOR3), GetGValue(LABEL_COLOR3), GetBValue(LABEL_COLOR3)));
		formation_->yellowcolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(YELLOW_COLOR), GetGValue(YELLOW_COLOR), GetBValue(YELLOW_COLOR)));
		formation_->acceptcolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(ACCEPT_COLOR), GetGValue(ACCEPT_COLOR), GetBValue(ACCEPT_COLOR)));
		formation_->rejectcolor_brush_gdi_ = new Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(REJECT_COLOR), GetGValue(REJECT_COLOR), GetBValue(REJECT_COLOR)));

		formation_->spancolor1_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BACKCOLOR1), GetGValue(BACKCOLOR1), GetBValue(BACKCOLOR1)));
		formation_->spancolor1_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BACKCOLOR1), GetGValue(BACKCOLOR1), GetBValue(BACKCOLOR1)), 2);
		formation_->spancolor1_sel_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BACKCOLOR1_SEL), GetGValue(BACKCOLOR1_SEL), GetBValue(BACKCOLOR1_SEL)));
		formation_->spancolor2_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BACKCOLOR2), GetGValue(BACKCOLOR2), GetBValue(BACKCOLOR2)));
		formation_->blackcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BLACK_COLOR), GetGValue(BLACK_COLOR), GetBValue(BLACK_COLOR)));
		formation_->blackcolor_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(BLACK_COLOR), GetGValue(BLACK_COLOR), GetBValue(BLACK_COLOR)), 2);
		formation_->whitecolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(WHITE_COLOR), GetGValue(WHITE_COLOR), GetBValue(WHITE_COLOR)), 1);
		formation_->whitecolor_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(WHITE_COLOR), GetGValue(WHITE_COLOR), GetBValue(WHITE_COLOR)), 2);
		formation_->disablecolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(DISABLE_COLOR), GetGValue(DISABLE_COLOR), GetBValue(DISABLE_COLOR)), 1);
		formation_->disablecolor_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(DISABLE_COLOR), GetGValue(DISABLE_COLOR), GetBValue(DISABLE_COLOR)), 2);
		formation_->scrollcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(SCROLL_COLOR), GetGValue(SCROLL_COLOR), GetBValue(SCROLL_COLOR)), 1);
		formation_->labelcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(LABEL_COLOR), GetGValue(LABEL_COLOR), GetBValue(LABEL_COLOR)), 1);
		formation_->labellinecolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(LABEL_LINE_COLOR), GetGValue(LABEL_LINE_COLOR), GetBValue(LABEL_LINE_COLOR)), 1);
		formation_->labelcolor2_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(LABEL_COLOR2), GetGValue(LABEL_COLOR2), GetBValue(LABEL_COLOR2)), 1);
		formation_->labelcolor3_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(LABEL_COLOR3), GetGValue(LABEL_COLOR3), GetBValue(LABEL_COLOR3)), 1);
		formation_->yellowcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(YELLOW_COLOR), GetGValue(YELLOW_COLOR), GetBValue(YELLOW_COLOR)), 1);
		formation_->acceptcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(ACCEPT_COLOR), GetGValue(ACCEPT_COLOR), GetBValue(ACCEPT_COLOR)), 1);
		formation_->rejectcolor_pen_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(REJECT_COLOR), GetGValue(REJECT_COLOR), GetBValue(REJECT_COLOR)), 1);
		formation_->acceptcolor_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(ACCEPT_COLOR), GetGValue(ACCEPT_COLOR), GetBValue(ACCEPT_COLOR)), 2);
		formation_->rejectcolor_pen2_gdi_ = new Gdiplus::Pen(Gdiplus::Color(GetRValue(REJECT_COLOR), GetGValue(REJECT_COLOR), GetBValue(REJECT_COLOR)), 2);

		/*Formation::MsgBox(L"Unable to connect with SPAN PLC.\n1. Check SPAN PLC power supply is properly connected and it is power on.\n2. Check SPAN PLC LAN cable is properly connected.\n3. Check all I/O slices are properly connected.\nIf problem still persist, kindly contact to SPAN supoort team.");
		Formation::MsgBox(L"OkCancel", MB_ICONEXCLAMATION | MB_OKCANCEL);
		Formation::MsgBox(L"OkCancel", MB_ICONEXCLAMATION | MB_OKCANCEL);
		Formation::MsgBox(L"AbortRetryIgnore", MB_ICONEXCLAMATION | MB_ABORTRETRYIGNORE);
		Formation::MsgBox(L"AbortRetryIgnore", MB_ICONEXCLAMATION | MB_ABORTRETRYIGNORE);
		Formation::MsgBox(L"AbortRetryIgnore", MB_ICONEXCLAMATION | MB_ABORTRETRYIGNORE);
		Formation::MsgBox(L"YesNoCancel", MB_ICONERROR | MB_YESNOCANCEL);
		Formation::MsgBox(L"YesNoCancel", MB_ICONERROR | MB_YESNOCANCEL);
		Formation::MsgBox(L"YesNoCancel", MB_ICONERROR | MB_YESNOCANCEL);
		Formation::MsgBox(L"YesNo", MB_ICONERROR | MB_YESNO);
		Formation::MsgBox(L"YesNo", MB_ICONERROR | MB_YESNO);
		Formation::MsgBox(L"RetryCancel", MB_ICONERROR | MB_RETRYCANCEL);
		Formation::MsgBox(L"RetryCancel", MB_ICONERROR | MB_RETRYCANCEL);
		Formation::MsgBox(L"CancelTryContinue", MB_ICONERROR | MB_CANCELTRYCONTINUE);
		Formation::MsgBox(L"CancelTryContinue", MB_ICONERROR | MB_CANCELTRYCONTINUE);
		Formation::MsgBox(L"CancelTryContinue", MB_ICONERROR | MB_CANCELTRYCONTINUE);*/
	}
	return formation_;
}

void Formation::SetConsoleText(CString text, DWORD color) {

	if (GetStdHandle(STD_OUTPUT_HANDLE) == NULL) { //If AllocConsole is disabled
		return;
	}

	EnterCriticalSection(&formation_->cs_for_console_);

	color = CONSOLE_TEXT_COLOR_WHITE - color;
	if (color < CONSOLE_TEXT_COLOR_BLACK || color > CONSOLE_TEXT_COLOR_WHITE) {
		color = CONSOLE_TEXT_COLOR_WHITE;
	}

	//This handle is needed to get the current background attribute
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//csbi is used for wAttributes word
	if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
		//To mask out all but the background attribute, and to add the color
		SetConsoleTextAttribute(hStdOut, (csbi.wAttributes & 0xF0) + (color & 0x0F));
	}

	wprintf(L"%s", text.GetString());

	LeaveCriticalSection(&formation_->cs_for_console_);
}

CString Formation::GetFileVersion(CString filename) {

	CString version;

	DWORD dwLen, dwUseless;
	dwLen = GetFileVersionInfoSize(filename, &dwUseless);
	if (dwLen == 0) {
		return 0;
	}

	LPTSTR lpVI = (LPTSTR)GlobalAlloc(GPTR, dwLen);
	if (lpVI) {

		VS_FIXEDFILEINFO* lpFFI;
		DWORD dwBufSize;

		GetFileVersionInfo(filename, NULL, dwLen, lpVI);

		//Get the Version Info
		if (VerQueryValue(lpVI, _T("\\"), (LPVOID *)&lpFFI, (UINT *)&dwBufSize)) {
			version.Format(L"%d.%d.%d.%d", lpFFI->dwFileVersionMS >> 16, lpFFI->dwFileVersionMS & 0xFFFF, lpFFI->dwFileVersionLS >> 16, lpFFI->dwFileVersionLS & 0xFFFF);
		}

		//Cleanup
		GlobalFree((HGLOBAL)lpVI);
	}

	return version;
}

CString Formation::GetCompanyName(CString filename) {

	CString company_name;

	DWORD dwLen, dwUseless;
	dwLen = GetFileVersionInfoSize(filename, &dwUseless);
	if (dwLen == 0) {
		return 0;
	}

	LPTSTR lpVI = (LPTSTR)GlobalAlloc(GPTR, dwLen);
	if (lpVI) {

		WORD* langInfo;
		UINT cbLang;
		TCHAR tszVerStrName[128];
		LPVOID lpt;
		UINT cbBufSize;

		GetFileVersionInfo(filename, NULL, dwLen, lpVI);

		//Get the Company Name.
		//First, to get string information, we need to get language information.
		if (VerQueryValue(lpVI, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langInfo, &cbLang)) {
			//Prepare the label -- default lang is bytes 0 & 1 of langInfo
			wsprintf(tszVerStrName, _T("\\StringFileInfo\\%04x%04x\\%s"), langInfo[0], langInfo[1], _T("CompanyName"));
			//Get the string from the resource data
			if (VerQueryValue(lpVI, tszVerStrName, &lpt, &cbBufSize)) {
				company_name.Format((TCHAR*)lpt);
			}
		}

		//Cleanup
		GlobalFree((HGLOBAL)lpVI);
	}

	return company_name;
}

CString Formation::GetProductName(CString filename) {

	CString product_name;

	DWORD dwLen, dwUseless;
	dwLen = GetFileVersionInfoSize(filename, &dwUseless);
	if (dwLen == 0) {
		return 0;
	}

	LPTSTR lpVI = (LPTSTR)GlobalAlloc(GPTR, dwLen);
	if (lpVI) {

		WORD* langInfo;
		UINT cbLang;
		TCHAR tszVerStrName[128];
		LPVOID lpt;
		UINT cbBufSize;

		GetFileVersionInfo(filename, NULL, dwLen, lpVI);

		//Get the Product Name.
		//First, to get string information, we need to get language information.
		if (VerQueryValue(lpVI, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langInfo, &cbLang)) {
			//Prepare the label -- default lang is bytes 0 & 1 of langInfo
			wsprintf(tszVerStrName, _T("\\StringFileInfo\\%04x%04x\\%s"), langInfo[0], langInfo[1], _T("ProductName"));
			//Get the string from the resource data
			if (VerQueryValue(lpVI, tszVerStrName, &lpt, &cbBufSize)) {
				product_name.Format((TCHAR*)lpt);
			}
		}

		//Cleanup
		GlobalFree((HGLOBAL)lpVI);
	}

	return product_name;
}

CString Formation::DateFormat() {
	
	return formation_->date_format_;
}

void Formation::SetDateFormat(CString date_format) {

	formation_->date_format_ = date_format;
}

CString Formation::GetDate(int dd, int mm, int yy, CString date_format) {

	if (date_format.IsEmpty()) date_format = formation_->date_format_;

	CString str_date = date_format;
	CString left_string, middle_string, right_string;

	//DAY
	int find = str_date.Find(L"DD");
	if (find != -1) {
		left_string = str_date.Mid(0, find);
		middle_string.Format(L"%.2d", dd);
		find += 2;
		right_string = str_date.Mid(find, str_date.GetLength() - find);
		str_date = left_string + middle_string + right_string;
	} else {
		find = str_date.Find(L"D");
		if (find != -1) {
			left_string = str_date.Mid(0, find);
			middle_string.Format(L"%d", dd);
			find += 1;
			right_string = str_date.Mid(find, str_date.GetLength() - find);
			str_date = left_string + middle_string + right_string;
		}
	}

	//MONTH
	find = str_date.Find(L"MMM");
	if (find != -1) {
		left_string = str_date.Mid(0, find);
		if (mm == 1) {
			middle_string = L"Jan";
		} else if (mm == 2) {
			middle_string = L"Feb";
		} else if (mm == 3) {
			middle_string = L"Mar";
		} else if (mm == 4) {
			middle_string = L"Apr";
		} else if (mm == 5) {
			middle_string = L"May";
		} else if (mm == 6) {
			middle_string = L"Jun";
		} else if (mm == 7) {
			middle_string = L"Jul";
		} else if (mm == 8) {
			middle_string = L"Aug";
		} else if (mm == 9) {
			middle_string = L"Sep";
		} else if (mm == 10) {
			middle_string = L"Oct";
		} else if (mm == 11) {
			middle_string = L"Nov";
		} else {
			middle_string = L"Dec";
		}
		find += 3;
		right_string = str_date.Mid(find, str_date.GetLength() - find);
		str_date = left_string + middle_string + right_string;
	} else {
		find = str_date.Find(L"MM");
		if (find != -1) {
			left_string = str_date.Mid(0, find);
			middle_string.Format(L"%.2d", mm);
			find += 2;
			right_string = str_date.Mid(find, str_date.GetLength() - find);
			str_date = left_string + middle_string + right_string;
		} else {
			find = str_date.Find(L"M");
			if (find != -1) {
				left_string = str_date.Mid(0, find);
				middle_string.Format(L"%d", mm);
				find += 1;
				right_string = str_date.Mid(find, str_date.GetLength() - find);
				str_date = left_string + middle_string + right_string;
			}
		}
	}

	//YEAR
	find = str_date.Find(L"YYYY");
	if (find != -1) {
		left_string = str_date.Mid(0, find);
		CString str_year;
		str_year.Format(L"%d", yy);
		middle_string = str_year;
		if(middle_string.GetLength() != 4) middle_string = L"20" + str_year;
		find += 4;
		right_string = str_date.Mid(find, str_date.GetLength() - find);
		str_date = left_string + middle_string + right_string;
	} else {
		find = str_date.Find(L"YY");
		if (find != -1) {
			left_string = str_date.Mid(0, find);
			CString str_year;
			str_year.Format(L"%d", yy);
			middle_string = str_year;
			if (middle_string.GetLength() == 4) middle_string = str_year.Mid(2, 2);
			find += 2;
			right_string = str_date.Mid(find, str_date.GetLength() - find);
			str_date = left_string + middle_string + right_string;
		}
	}

	return (str_date.Trim());
}

CString Formation::GetDateTime(CString source_datetime, CString source_date_format, CString dest_date_format) {

	if (source_datetime.GetLength() < 12) return source_datetime;

	CString str_date = source_datetime.Mid(0, source_datetime.GetLength() - 9);
	CString str_time = source_datetime.Mid(source_datetime.GetLength() - 8, 8);

	return (Formation::GetDate(str_date, source_date_format, dest_date_format) + L" " + str_time);
}

CString Formation::GetDate(CString source_date, CString source_date_format, CString dest_date_format) {

	if (source_date_format.IsEmpty()) source_date_format = DB_DATE_FORMAT;
	if (dest_date_format.IsEmpty()) dest_date_format = formation_->date_format_;

	//FIND DAY/MONTH/YEAR FROM SOURCE 
	int dd = 0, mm = 0, yy = 0;

	//DAY
	int find = source_date_format.Find(L"DD");
	if (find != -1) {
		CString str_day = source_date.Mid(find, 2);
		dd = _wtoi(str_day);
	} else {
		find = source_date_format.Find(L"D");
		if (find != -1) {
			CString str_day;
			if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
				str_day += source_date.Mid(find, 1);
			}
			++find;
			if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
				str_day += source_date.Mid(find, 1);
				++find;
				if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
					str_day += source_date.Mid(find, 1);
				}
			}
			dd = _wtoi(str_day);
		}
	}

	//MONTH
	find = source_date_format.Find(L"MMM");
	if (find != -1) {
		CString str_month = source_date.Mid(find, 3);
		if (str_month == L"Jan") {
			mm = 1;
		} else if (str_month == L"Feb") {
			mm = 2;
		} else if (str_month == L"Mar") {
			mm = 3;
		} else if (str_month == L"Apr") {
			mm = 4;
		} else if (str_month == L"May") {
			mm = 5;
		} else if (str_month == L"Jun") {
			mm = 6;
		} else if (str_month == L"Jul") {
			mm = 7;
		} else if (str_month == L"Aug") {
			mm = 8;
		} else if (str_month == L"Sep") {
			mm = 9;
		} else if (str_month == L"Oct") {
			mm = 10;
		} else if (str_month == L"Nov") {
			mm = 11;
		} else {
			mm = 12;
		}
	} else {
		find = source_date_format.Find(L"MM");
		if (find != -1) {
			CString str_month = source_date.Mid(find, 2);
			mm = _wtoi(str_month);
		} else {
			find = source_date_format.Find(L"M");
			if (find != -1) {
				CString str_month;
				if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
					str_month += source_date.Mid(find, 1);
				}
				++find;
				if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
					str_month += source_date.Mid(find, 1);
					++find;
					if (isdigit(source_date.Mid(find, 1).GetAt(0))) {
						str_month += source_date.Mid(find, 1);
					}
				}
				mm = _wtoi(str_month);
			}
		}
	}

	//YEAR
	BOOL reversed = FALSE;
	find = source_date_format.Find(L"YYYY");
	if (find != 0) {
		find = source_date_format.Find(L"YY");
		if (find != 0) {
			reversed = TRUE;
			source_date_format.MakeReverse();
			source_date.MakeReverse();
		}
	}

	find = source_date_format.Find(L"YYYY");
	if (find != -1) {
		CString str_year = source_date.Mid(find, 4);
		if (reversed) str_year.MakeReverse();
		yy = _wtoi(str_year);
	} else {
		find = source_date_format.Find(L"YY");
		if (find != -1) {
			CString str_year = source_date.Mid(find, 2);
			if (reversed) str_year.MakeReverse();
			yy = _wtoi(str_year);
		}
	}

	return GetDate(dd, mm, yy, dest_date_format);
}

CString Formation::GetTime() {

	SYSTEMTIME local_time = { 0 };
	GetLocalTime(&local_time);

	CString str_time;
	str_time.Format(_T("%02u:%02u:%02u"), local_time.wHour, local_time.wMinute, local_time.wSecond);

	return str_time;
}

CString Formation::GetDateTime(BOOL date_format) {

	SYSTEMTIME local_time = { 0 };
	GetLocalTime(&local_time);

	CString str_date, str_time;
	if (date_format) {
		str_date = GetDate(local_time.wDay, local_time.wMonth, local_time.wYear, formation_->date_format_);
		str_time = GetTime();
	} else {
		//str_date.Format(_T("%02u-%02u-%u"), local_time.wDay, local_time.wMonth, local_time.wYear);
		str_date.Format(_T("%u-%02u-%02u"), local_time.wYear, local_time.wMonth, local_time.wDay);
		str_time.Format(_T("%02u:%02u:%02u"), local_time.wHour, local_time.wMinute, local_time.wSecond);
	}

	return (str_date + L" " + str_time);
}

CString Formation::PrepareString(CDC& cdc, CString text, CRect bounding_rect) {

	if (!text.IsEmpty()) {
		while (cdc.GetTextExtent(text).cx > bounding_rect.Width() - 1) {
			if (text.GetLength() <= 1) { //Minimum one char should be present in text
				break;
			}
			text = text.Mid(0, text.GetLength() - 1);
		}
		int find = text.Find(L"&");
		while (find != -1) {
			find++;
			text.Insert(find, L"&");
			find = text.Find(L"&", find + 1);
		}
	}
	return text;
}

void Formation::FlashWnd(CWnd* wnd, BOOL start) {

	if (start) {

		if (formation_->flash_wnd_count_ == 0) {
			formation_->flash_wnd_count_ = 1;

			wnd->SetTimer(10, 1500, NULL);

			CRect wnd_rect;
			wnd->GetWindowRect(wnd_rect);
			HDC hdcDesktop = ::GetDC(NULL);
			HDC hdcDesktop2 = ::GetDC(NULL);
			//for (int i = 0; i < 3; i++) {
				BitBlt(hdcDesktop, wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height(), hdcDesktop2, wnd_rect.left, wnd_rect.top, NOTSRCCOPY);
				Sleep(10);
				BitBlt(hdcDesktop, wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height(), hdcDesktop2, wnd_rect.left, wnd_rect.top, SRCCOPY);
			//}
		}

	} else {

		CRect wnd_rect;
		wnd->GetWindowRect(wnd_rect);
		HDC hdcDesktop = ::GetDC(NULL);
		HDC hdcDesktop2 = ::GetDC(NULL);
		//BitBlt(hdcDesktop, wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height(), hdcDesktop2, wnd_rect.left, wnd_rect.top, SRCCOPY);

		formation_->flash_wnd_count_ = 0;
		wnd->KillTimer(10);
	}
}

CString Formation::PrepareMultilineString(CDC& cdc, CString text, CRect bounding_rect) {

	CString str_text = text;

	if (!text.IsEmpty()) {
		
		str_text.Empty();
		CString temp_text;
		int current_position = 0;
		int start_position = 0;
		BOOL add_new_line = FALSE;
		while (1) {
			temp_text = text.Mid(start_position, current_position - start_position);
			if (text[current_position] == '\n') {
				if (add_new_line) {
					str_text += "\n";
				}
				str_text += temp_text;
				start_position = current_position;
				add_new_line = FALSE;
				temp_text.Empty();
			} else if ((cdc.GetTextExtent(temp_text).cx > bounding_rect.Width() - 1)) {
				if (add_new_line) {
					str_text += "\n";
				}
				str_text += temp_text;
				start_position = current_position;
				add_new_line = TRUE;
				temp_text.Empty();
			}
			if (++current_position > text.GetLength()) {
				break;
			}
		}
		if (temp_text.GetLength() > 0) {
			if (add_new_line) {
				str_text += "\n";
			}
			str_text += L" " + temp_text;
		}
	}
	return str_text;
}

void Formation::LockDirectory(CString directory, BOOL lock) {
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	si.wShowWindow = SW_HIDE;

	WCHAR str_command[255] = L"\0";
	if (lock) {
		wsprintf(str_command, L"cacls %s /e /p everyone:n", directory);
	} else {
		wsprintf(str_command, L"cacls %s /e /p everyone:f", directory);
	}
	if (CreateProcess(0, str_command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		DWORD dw_err = 0;
		// Wait till cmd do its job
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Check whether our command succeeded?
		GetExitCodeProcess(pi.hProcess, &dw_err);
		// Avoid memory leak by closing process handle
		CloseHandle(pi.hProcess);
	} else {
		printf("Formation::LockDirectory:CreateProcess failed...");
	}
	
	/*char folderdir[1024] = "\0";
	WideCharToMultiByte(CP_UTF8, 0, directory, -1, folderdir, 1024, 0, 0);
	char attributes[1024] = "\0";
	if (hide) {
		strcpy_s(attributes, "attrib +h +s ");
		strcat_s(attributes, folderdir);
		WinExec(attributes, SW_HIDE);
	} else {
		strcpy_s(attributes, "attrib -h -s ");
		strcat_s(attributes, folderdir);
		WinExec(attributes, SW_HIDE);
	}
	char command[1024] = "\0";
	WCHAR str_command[255] = L"\0";
	if (lock) {
		sprintf_s(command, "cacls %s /e /p everyone:n", folderdir);
		//system(command);
		WinExec(command, SW_HIDE);
	} else {
		sprintf_s(command, "cacls %s /e /p everyone:f", folderdir);
		//system(command);
		WinExec(command, SW_HIDE);
	}*/
}

void Formation::HideDirectory(CString directory, BOOL hide) {
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	si.wShowWindow = SW_HIDE;

	WCHAR str_command[255] = L"\0";
	if (hide) {
		wsprintf(str_command, L"attrib +h +s %s", directory);
	} else {
		wsprintf(str_command, L"attrib -h -s %s", directory);
	}
	if (CreateProcess(0, str_command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		DWORD dw_err = 0;
		// Wait till cmd do its job
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Check whether our command succeeded?
		GetExitCodeProcess(pi.hProcess, &dw_err);
		// Avoid memory leak by closing process handle
		CloseHandle(pi.hProcess);
	} else {
		printf("Formation::LockDirectory:CreateProcess failed...");
	}
}

void Formation::LockDirectories(CString directory, BOOL lock) {

	if (!lock) {
		Formation::LockDirectory(directory, lock);
	}
	WIN32_FIND_DATA findfiledata = { 0 };
	HANDLE findfilehandle = FindFirstFile(directory + L"\\*", &findfiledata);
	if (findfilehandle != INVALID_HANDLE_VALUE) {
		do {
			if (wcscmp(findfiledata.cFileName, L".") != 0 && wcscmp(findfiledata.cFileName, L"..") != 0) {
				CString folderdir = directory + L"\\" + CString(findfiledata.cFileName);
				Formation::LockDirectory(folderdir, lock);
			}
		} while (FindNextFile(findfilehandle, &findfiledata) != 0);
		FindClose(findfilehandle);
	}
	if (lock) {
		Formation::LockDirectory(directory, lock);
	}
}

void Formation::HideDirectories(CString directory, BOOL hide) {

	WIN32_FIND_DATA findfiledata = { 0 };
	HANDLE findfilehandle = FindFirstFile(directory + L"\\*", &findfiledata);
	if (findfilehandle != INVALID_HANDLE_VALUE) {
		do {
			if (wcscmp(findfiledata.cFileName, L".") != 0 && wcscmp(findfiledata.cFileName, L"..") != 0) {
				CString folderdir = directory + L"\\" + CString(findfiledata.cFileName);
				Formation::LockDirectory(folderdir, hide);
			}
		} while (FindNextFile(findfilehandle, &findfiledata) != 0);
		FindClose(findfilehandle);
	}
}

HANDLE Formation::StartProcess(CString process_name, CString command_line, WORD show_window, DWORD creation_flag) {

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	memset(&startup_info, 0, sizeof(startup_info));
	memset(&process_info, 0, sizeof(process_info));
	startup_info.wShowWindow = show_window;
	BOOL result = CreateProcess(process_name, command_line.GetBuffer(), NULL, NULL, FALSE, creation_flag, NULL, NULL, &startup_info, &process_info);
	if (!result || process_info.hProcess == NULL) {
	//	wprintf(L"\n%s-%d\n", process_name, GetLastError());
		return NULL;
	}

	return process_info.hProcess;
}

void Formation::KillProcess(CString process_name) {

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes) {
		CString str_process_name(pEntry.szExeFile);
		str_process_name.MakeLower();
		CString str_name(process_name);
		str_name.MakeLower();
		//if (wcscmp(pEntry.szExeFile, name) == 0) {
		if (str_process_name == str_name) {
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL) {
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

BOOL Formation::IsProcessRunning(CString process_name) {

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes) {
		CString str_process_name(pEntry.szExeFile);
		str_process_name.MakeLower();
		CString str_name(process_name);
		str_name.MakeLower();
		//if (wcscmp(pEntry.szExeFile, name) == 0) {
		if (str_process_name == str_name) {
			return TRUE;
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	return FALSE;
}

void Formation::Destroy() {

	if (formation_ != NULL) {

		if (formation_->cs_for_console_.OwningThread != NULL) {
			DeleteCriticalSection(&formation_->cs_for_console_);
		}

		formation_->font_[0].DeleteObject();
		formation_->font_[1].DeleteObject();
		formation_->font_[2].DeleteObject();
		formation_->font_[3].DeleteObject();
		formation_->font_[4].DeleteObject();
		formation_->font_[5].DeleteObject();
		formation_->font_[6].DeleteObject();
		formation_->font_[7].DeleteObject();

		formation_->spancolor1_brush_.DeleteObject();
		formation_->spancolor1_sel_brush_.DeleteObject();
		formation_->spancolor2_brush_.DeleteObject();
		formation_->labelcolor_brush_.DeleteObject();
		formation_->labelcolor2_brush_.DeleteObject();
		formation_->labelcolor3_brush_.DeleteObject();
		formation_->disablecolor_brush_.DeleteObject();
		formation_->accept_brush_.DeleteObject();
		formation_->reject_brush_.DeleteObject();
		formation_->scrollcolor_brush_.DeleteObject();
		formation_->whitecolor_brush_.DeleteObject();
		formation_->blackcolor_brush_.DeleteObject();
		formation_->bluecolor_brush_.DeleteObject();

		formation_->spancolor1_pen_.DeleteObject();
		formation_->spancolor1_sel_pen_.DeleteObject();
		formation_->spancolor2_pen_.DeleteObject();
		formation_->labelcolor_pen_.DeleteObject();
		formation_->labellinecolor_pen_.DeleteObject();
		formation_->labelcolor2_pen_.DeleteObject();
		formation_->labelcolor3_pen_.DeleteObject();
		formation_->disablecolor_pen_.DeleteObject();
		formation_->blackcolor_pen_.DeleteObject();
		formation_->blackcolor_pen2_.DeleteObject();
		formation_->whitecolor_pen_.DeleteObject();
		formation_->whitecolor_pen2_.DeleteObject();
		formation_->acceptcolor_pen_.DeleteObject();
		formation_->rejectcolor_pen_.DeleteObject();
		formation_->rejectcolor_pen2_.DeleteObject();
		formation_->scrollcolor_pen_.DeleteObject();
		formation_->bluecolor_pen_.DeleteObject();

		delete formation_->spancolor1_brush_gdi_;
		delete formation_->spancolor1_sel_brush_gdi_;
		delete formation_->spancolor2_brush_gdi_;
		delete formation_->whitecolor_brush_gdi_;
		delete formation_->disablecolor_brush_gdi_;
		delete formation_->labelcolor_brush_gdi_;
		delete formation_->labelcolor2_brush_gdi_;
		delete formation_->labelcolor3_brush_gdi_;
		delete formation_->yellowcolor_brush_gdi_;
		delete formation_->acceptcolor_brush_gdi_;
		delete formation_->rejectcolor_brush_gdi_;
		delete formation_->spancolor1_pen_gdi_;
		delete formation_->spancolor1_pen2_gdi_;
		delete formation_->spancolor1_sel_pen_gdi_;
		delete formation_->spancolor2_pen_gdi_;
		delete formation_->blackcolor_pen_gdi_;
		delete formation_->blackcolor_pen2_gdi_;
		delete formation_->whitecolor_pen_gdi_;
		delete formation_->whitecolor_pen2_gdi_;
		delete formation_->disablecolor_pen_gdi_;
		delete formation_->disablecolor_pen2_gdi_;
		delete formation_->scrollcolor_pen_gdi_;
		delete formation_->labelcolor_pen_gdi_;
		delete formation_->labellinecolor_pen_gdi_;
		delete formation_->labelcolor2_pen_gdi_;
		delete formation_->labelcolor3_pen_gdi_;
		delete formation_->yellowcolor_pen_gdi_;
		delete formation_->acceptcolor_pen_gdi_;
		delete formation_->rejectcolor_pen_gdi_;
		delete formation_->acceptcolor_pen2_gdi_;
		delete formation_->rejectcolor_pen2_gdi_;

		delete formation_;
		formation_ = NULL;
	}
}

//POPUP WINDOWS (TO KILL ALL OPENED WINDOWS, IF USER GET CHANGED DURING LOCK APPLICATION)
int Formation::OpenDialog(CDialog* dialog, int type) {

	if (formation_ != NULL) {

		formation_->popup_dlg_array_.Add(dialog);
		formation_->popup_type_array_.Add(type);
		int result = int(dialog->DoModal());
		for (int index = 0; index < (int)formation_->popup_dlg_array_.GetSize(); index++) {
			if (dialog == formation_->popup_dlg_array_[index]) {
				formation_->popup_dlg_array_.RemoveAt(index);
				formation_->popup_type_array_.RemoveAt(index);
				break;
			}
		}

		return result;
	}

	return 0;
}
void Formation::CloseAllPopups() {

	for (int index = 0; index < (int)formation_->popup_dlg_array_.GetSize(); index++) {
		if (formation_->popup_dlg_array_[index]) {
			int result = IDCANCEL;
			UINT type = formation_->popup_type_array_[index];
			if (type != -1) {
				if (((type & MB_TYPEMASK) == MB_OKCANCEL) || ((type & MB_TYPEMASK) == MB_YESNOCANCEL) || ((type & MB_TYPEMASK) == MB_RETRYCANCEL) || ((type & MB_TYPEMASK) == MB_CANCELTRYCONTINUE)) {
					result = IDCANCEL;
				} else if ((type & MB_TYPEMASK) == MB_ABORTRETRYIGNORE) {
					result = IDIGNORE;
				} else if (((type & MB_TYPEMASK) == MB_YESNO)) {
					result = IDNO;
				}
			}
			formation_->popup_dlg_array_[index]->EndDialog(result);
		}
	}
	formation_->popup_dlg_array_.RemoveAll();
	formation_->popup_type_array_.RemoveAll();

	WaitModelCloseAll();

	if (IsWaitOpen()) {
		WaitClose();
	}
}

BOOL Formation::IsPopupOpen() {
	
	/*for (int index = 0; index < (int)formation_->popup_dlg_array_.GetSize(); index++) {
		if (formation_->popup_dlg_array_[index]) {
			return TRUE;
		}
	}*/
	if (IsWaitOpen()) {
		return TRUE;
	}
	if (IsMsgBoxOpen()) {
		return TRUE;
	}

	return FALSE;
}

//WAIT CONTROL
void Formation::WaitOpen(CString title, CString text) {

	if (formation_ != NULL) {// && !formation_->waitcontrol_.IsDoModal()) {
		formation_->waitcontrol_.Close();
		formation_->waitcontrol_.Open(title, text);
	}
}
BOOL Formation::IsWaitOpen() {

	if (formation_ != NULL) {
		if (formation_->waitcontrol_.GetSafeHwnd() != NULL) {
			if (formation_->waitcontrol_.IsWindowVisible()) return TRUE;
		}
		if (formation_->wait_model_data_.wait_model_control_wnd_list_.GetSize() > 0) {
			return TRUE;
		}
	}
	return FALSE;
}
void Formation::WaitHide(DWORD hide) {

	if (formation_ != NULL) {
		if (formation_->waitcontrol_.GetSafeHwnd() != NULL) {
			formation_->waitcontrol_.Hide(hide);
		}
	}
}
void Formation::WaitClose() {

	if (formation_ != NULL) {
		formation_->waitcontrol_.Close();
	}
}

//WAIT MODEL CONTROL
WaitModelData& Formation::GetWaitModelData() {
	
	return formation_->wait_model_data_;
}

//MSG BOX
MsgBoxData& Formation::GetMsgBoxData() {

	return formation_->msg_box_data_;
}

void Formation::SetButtonsText(CString yes, CString no, CString ok, CString cancel, CString reset) {

	if (formation_ != NULL) {
		formation_->msg_box_data_.msgbox_button_text_.SetAt(0, ok);
		formation_->msg_box_data_.msgbox_button_text_.SetAt(1, cancel);
		formation_->msg_box_data_.msgbox_button_text_.SetAt(5, yes);
		formation_->msg_box_data_.msgbox_button_text_.SetAt(6, no);
		formation_->msg_box_data_.msgbox_button_text_.SetAt(11, reset);
	}
}
int Formation::MsgBox(CString text, UINT type, BOOL remain_popup) {

	if (formation_ != NULL) {

		InterlockedIncrement(&formation_->msg_box_data_.msgbox_count_);

		HINSTANCE old_resource_handle = AfxGetResourceHandle();
		AfxSetResourceHandle(g_resource_handle);
		MessageBoxDlg messagebox;
		messagebox.SetButtonsText(formation_->msg_box_data_.msgbox_button_text_);
		int result = messagebox.Open(text, type, remain_popup);
		AfxSetResourceHandle(old_resource_handle);

		InterlockedDecrement(&formation_->msg_box_data_.msgbox_count_);

		return result;
	}
	return 0;
}
BOOL Formation::IsMsgBoxOpen() {

	if (formation_ != NULL) {
		return (formation_->msg_box_data_.msgbox_count_ == 0 ? FALSE : TRUE);
	}
	return FALSE;
}

//PATTERN MATCHING
BOOL Formation::IsStringMatch(CString str, CString pattern, CString field_name, INT string_length) {

	CONST CString space = _T(" "), slash = _T("\\");
	BOOL is_matched = TRUE;
	CString err_msg;
	err_msg.Format(L"Field - %s. \n\u2022 This field cannot be empty and cannot end with \\. \n\u2022 This field length cannot be more than %d characters. \n\u2022 This field cannot contain following characters %s", field_name, string_length, pattern);

	if (str.IsEmpty() || str.GetAt(0) == space) {
		is_matched = FALSE;
	}
	else if (str.GetLength() < 1 || str.GetLength() > string_length) {
		is_matched = FALSE;
	}
	else if (str.Trim().GetLength() <= 0) {
		is_matched = FALSE;
	}
	else if (str.GetAt(str.GetLength() - 1) == slash) {
		is_matched = FALSE;
	}
	else if (str.FindOneOf(pattern) != -1) {					
		is_matched = FALSE;
	}
	

	if (!is_matched) {
		Formation::MsgBox(err_msg, MB_OK);
	}

	return is_matched;
}

//INNER RECT
long Formation::DivideInSquare(long width, long height, long n, int& rows, int& cols) {

	return formation_->inner_rect_.DivideInSquare(width, height, n, rows, cols);
}
void Formation::GetInsideBox(std::vector<POINT> &subRectInfo, RECT mainRect, long sW, int allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect) {

	formation_->inner_rect_.GetInsideBox(subRectInfo, mainRect, sW, allignment, offset, bestfloorWiseRect);
}
void Formation::GetStrokeImagePosition(long window_width, long window_height, long spacing_between_images, long image_width, long image_height, long total_images, std::vector<RECT> &image_rect, BOOL top_to_bottom) {

	formation_->inner_rect_.GetStrokeImagePosition(window_width, window_height, spacing_between_images, image_width, image_height, total_images, image_rect, top_to_bottom);
}
CRect Formation::GetRectInCenter(CRect ParentRect, CSize ImgSize) {

	return formation_->inner_rect_.GetRectInCenter(ParentRect, ImgSize);
}
CRect Formation::GetRectInCenterAndLeftAlligned(CRect ParentRect, CSize ImgSize) {

	return formation_->inner_rect_.GetRectInCenterAndLeftAlligned(ParentRect, ImgSize);
}
CRect Formation::GetRectInCenterAndBottomAlligned(CRect ParentRect, CSize ImgSize) {

	return formation_->inner_rect_.GetRectInCenterAndBottomAlligned(ParentRect, ImgSize);
}

//WAIT MODEL
BOOL IsWaitModelOpen() {
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	if (wait_model_data.wait_model_control_wnd_list_.GetSize() > 0) return TRUE;
	return FALSE;
}
void WaitModelOpen(CString title, CString text, HANDLE event_handle, BOOL show_progressbar) {

	Formation::WaitClose();
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	WaitModelControl* wait_model_control = new WaitModelControl();
	if (wait_model_control) {
		wait_model_data.wait_model_control_wnd_list_.Add(wait_model_control);
		wait_model_data.wait_model_control_text_list_.Add(text);
		wait_model_data.wait_model_control_handle_list_.Add(event_handle);
		wait_model_control->Open(title, text, show_progressbar);
	}
}
void WaitModelChangeText(CString current_text, CString new_text) {

	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_text_list_[index] == current_text) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				wait_model_control->ChangeText(new_text);
				wait_model_data.wait_model_control_text_list_[index] = new_text;
			}
			break;
		}
	}
}
void WaitModelHide(DWORD show, CString text) {

	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_text_list_[index] == text) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				wait_model_control->Hide(show);
			}
			break;
		}
	}
}
void WaitModelClose(CString text) {

	Sleep(100);
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_text_list_[index] == text) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				wait_model_control->SendMessageW(WM_WAIT_MODEL_CLOSE);
				Sleep(100);
				delete wait_model_control;
				wait_model_data.wait_model_control_wnd_list_.RemoveAt(index);
				wait_model_data.wait_model_control_text_list_.RemoveAt(index);
				wait_model_data.wait_model_control_handle_list_.RemoveAt(index);
			}
			break;
		}
	}
}
void WaitModelClose2(HANDLE event_handle) {

	Sleep(100);
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_handle_list_[index] == event_handle) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				wait_model_control->SendMessageW(WM_WAIT_MODEL_CLOSE);
				Sleep(100);
				delete wait_model_control;
				wait_model_data.wait_model_control_wnd_list_.RemoveAt(index);
				wait_model_data.wait_model_control_text_list_.RemoveAt(index);
				wait_model_data.wait_model_control_handle_list_.RemoveAt(index);
			}
			break;
		}
	}
}
void WaitModelCloseAll() {

	Sleep(100);
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
		if (wait_model_control) {
			wait_model_control->SendMessageW(WM_WAIT_MODEL_CLOSE);
			Sleep(100);
			delete wait_model_control;
			wait_model_data.wait_model_control_wnd_list_.RemoveAt(index);
			wait_model_data.wait_model_control_text_list_.RemoveAt(index);
			wait_model_data.wait_model_control_handle_list_.RemoveAt(index);
		}
	}
}
float& AssignProgressValue(CString text) {

	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_text_list_[index] == text) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				return wait_model_control->GetProgressValue();
			}
			break;
		}
	}
	float value = 0;
	return value;
}
CString& AssignProgressText(CString text) {

	CString str_value;
	WaitModelData& wait_model_data = Formation::GetWaitModelData();
	for (int index = 0; index < int(wait_model_data.wait_model_control_wnd_list_.GetSize()); index++) {
		if (wait_model_data.wait_model_control_text_list_[index] == text) {
			WaitModelControl* wait_model_control = static_cast<WaitModelControl*>(wait_model_data.wait_model_control_wnd_list_[index]);
			if (wait_model_control) {
				return wait_model_control->GetProgressText();
			}
			break;
		}
	}
	return str_value;
}

//MSG BOX
int MsgBox(CString text, UINT type, BOOL remain_popup) {

	MsgBoxData& msg_box_data = Formation::GetMsgBoxData();
	InterlockedIncrement(&msg_box_data.msgbox_count_);

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	MessageBoxDlg messagebox;
	messagebox.SetButtonsText(msg_box_data.msgbox_button_text_);
	int result = messagebox.Open(text, type, remain_popup);
	AfxSetResourceHandle(old_resource_handle);

	InterlockedDecrement(&msg_box_data.msgbox_count_);
	return result;
}