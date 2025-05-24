#pragma once

enum LOGIN_SCREEN							{ FRESH_SCREEN = 0, NORMAL_SCREEN, TINY_SCREEN, MAINTENANCE_SCREEN };
enum POWER_SYSTEM							{ SYSTEM_START = 100, SYSTEM_EXIT, CLOSE_MAINTENANCE_SCREEN, SYSTEM_SHUTDOWN, SYSTEM_SHUTDOWN_ACK, SYSTEM_RESTART, SYSTEM_RESTART_ACK };

#define	SPAN_USER							_T("spanuser")
#define VISION_STUDIO						_T("Vision Studio")
#define COMPANY_NAME						_T("SPAN Inspection Systems Pvt. Ltd.")
#define STD_DATE_FORMAT						_T("DD/MM/YYYY")
#define DB_DATE_FORMAT						_T("YYYY-MM-DD")

#define STRING_PATTERN						_T("~`!^*\\\"")
#define MB_OK								0x00000000L
#define MB_OKCANCEL							0x00000001L
#define MB_ABORTRETRYIGNORE					0x00000002L
#define MB_YESNOCANCEL						0x00000003L
#define MB_YESNO							0x00000004L
#define MB_RETRYCANCEL						0x00000005L
#define MB_CANCELTRYCONTINUE				0x00000006L
#define MB_RESET							0x00000007L

/*#define BACKCOLOR1						RGB(131,131,129)//RGB(2,80,163)
#define BACKCOLOR1_SEL						RGB(155,155,155)//RGB(24,133,255)		//RGB(0,111,221)
#define BACKCOLOR2							RGB(255,85,85)//RGB(249,162,25)
#define LABEL_COLOR							RGB(255,206,0)//RGB(43,129,181)		//RGB(41, 57, 85) //RGB(0,142,119) //RGB(117,117,117) //RGB(98,129,179) //RGB(77,96,130) //RGB(0,128,198)
#define LABEL_COLOR2						RGB(211,166,165)//RGB(124,187,224)
#define LABEL_COLOR3						RGB(230,203,202)//RGB(186,218,237)*/
#define BACKCOLOR1							RGB(2,80,163)
#define BACKCOLOR1_SEL						RGB(24,133,255)		//RGB(0,111,221)
#define BACKCOLOR2							RGB(249,162,25)
#define LABEL_COLOR							RGB(43,129,181)		//RGB(41, 57, 85) //RGB(0,142,119) //RGB(117,117,117) //RGB(98,129,179) //RGB(77,96,130) //RGB(0,128,198)
#define LABEL_COLOR2						RGB(109,179,220)
#define LABEL_COLOR3						RGB(186,218,237)
#define LABEL_LINE_COLOR					RGB(66,156,210)		
#define GRIDCOLOR1							RGB(194,197,211)
#define GRIDCOLOR2							RGB(215,217,225)
#define BORDER_COLOR						BACKCOLOR1			//RGB(128, 188, 235)
#define SCROLL_COLOR						RGB(162,162,162)
#define SCROLL_SEL_COLOR					RGB(108,108,108)
#define GROUP_COLOR							RGB(34, 177, 76)
#define KEY_COLOR							RGB(0, 162, 232)

#define WHITE_COLOR							RGB(255,255,255)
#define WHITE_COLOR2						RGB(243,243,243)
#define WHITE_COLOR3						RGB(234,234,234)
#define BLACK_COLOR							RGB(0,0,0)
#define BLACK_COLOR2						RGB(45,45,45)
#define BLACK_COLOR3						RGB(81,81,81)

#define DISABLE_COLOR						RGB(205,205,205)
#define DARK_GRAY							RGB(211,211,211)
#define GRAY_COLOR							RGB(240,240,240)
#define BLUE_COLOR							RGB(0,0,255)
#define YELLOW_COLOR						RGB(255,255,0)
#define GREEN_COLOR							RGB(0,255,0)
#define RED_COLOR							RGB(255,0,0)
#define ACCEPT_COLOR						RGB(0,234,0)			//RGB(36, 237, 27)
#define REJECT_COLOR						RGB(255,45,45)			//RGB(237, 27, 36)

#define MODERN_LIGHT_BLUE RGB(230,240,255)
#define MODERN_GRAY RGB(43, 129, 181)

#define CONSOLE_TEXT_COLOR_BLACK			0
#define CONSOLE_TEXT_COLOR_BLUE				1
#define CONSOLE_TEXT_COLOR_GREEN			2
#define CONSOLE_TEXT_COLOR_CYAN				3
#define CONSOLE_TEXT_COLOR_RED				4
#define CONSOLE_TEXT_COLOR_MAGENTA			5
#define CONSOLE_TEXT_COLOR_BROWN			6
#define CONSOLE_TEXT_COLOR_LIGHT_GRAY		7
#define CONSOLE_TEXT_COLOR_DARK_GRAY		8
#define CONSOLE_TEXT_COLOR_LIGHT_BLUE		9
#define CONSOLE_TEXT_COLOR_LIGHT_GREEN		10
#define CONSOLE_TEXT_COLOR_LIGHT_CYAN		11
#define CONSOLE_TEXT_COLOR_LIGHT_RED		12
#define CONSOLE_TEXT_COLOR_LIGHT_MAGENTA	13
#define CONSOLE_TEXT_COLOR_YELLOW			14
#define CONSOLE_TEXT_COLOR_WHITE			15

#ifndef INCLUDE_VISION_FRAMEWORK
#define INCLUDE_VISION_FRAMEWORK  __declspec(dllimport)
#endif //INCLUDE_VISION_FRAMEWORK

#include <timeapi.h>
#include <mmsystem.h>
#include <gdiplus.h>			
#include <string>
#include <iostream>
#pragma comment(lib, "gdiplus.lib")

#include "waitcontrol.h"
#include "waitmodelcontrol.h"
#include "messagebox_dlg.h"
#include "find_inner_rect.h"

class COfflineDC : public CDC {

private:
	CBitmap    m_bitmap;        // Offscreen bitmap
	CBitmap*   m_oldBitmap;		// bitmap originally found in CMemDC
	CDC*       m_pDC;           // Saves CDC passed in constructor
	BOOL       m_bMemDC;        // TRUE if CDC really is a Memory DC.
	CRect      m_dc_rect;       // Rectangle of drawing area.
	int		   m_start_y;		// Top of DC Rectangle
	CRect	   m_source_rect;   // Rectangle of source area.

public:
	COfflineDC(CDC* pDC, const CRect* pSourceRect = NULL, const CRect* pDCRect = NULL, int dc_top = 0) : CDC() {

		ASSERT(pDC != NULL);

		// Some initialization
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the rectangle to draw
		if (pSourceRect == NULL) {
			pDC->GetClipBox(&m_source_rect);
		} else {
			m_source_rect = *pSourceRect;
		}

		if (pDCRect == NULL) {
			pDCRect = pSourceRect;
		}
		m_dc_rect = pDCRect;
		m_start_y = dc_top;

		if (m_bMemDC) {

			// Create a Memory DC
			CreateCompatibleDC(pDC);
			pDC->LPtoDP(&m_dc_rect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_dc_rect.Width(), m_dc_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);

			SetMapMode(pDC->GetMapMode());
			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_dc_rect);
			SetWindowOrg(m_dc_rect.left, m_dc_rect.top);

		} else {
			// Make a copy of the relevent parts of the current 
			// DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		// Fill background 
		FillSolidRect(m_dc_rect, pDC->GetBkColor());
	}
	
	~COfflineDC() {

		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->StretchBlt(m_source_rect.left, m_source_rect.top, m_source_rect.Width(), m_source_rect.Height(), this, m_source_rect.left, m_start_y, m_source_rect.Width(), m_source_rect.Height(), SRCCOPY);
			//Swap back the original bitmap.
			SelectObject(m_oldBitmap);
			m_bitmap.DeleteObject();
		} else {
			// All we need to do is replace the DC with an illegal
			// value, this keeps us from accidentally deleting the 
			// handles associated with the CDC that was passed to 
			// the constructor.              
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer    
	COfflineDC* operator->() {
		return this;
	}

	// Allow usage as a pointer    
	operator COfflineDC*() {
		return this;
	}
};

class CGdiPlusBitmap
{
public:
	Gdiplus::Bitmap* m_pBitmap;

public:
	CGdiPlusBitmap()							{ m_pBitmap = NULL; }
	CGdiPlusBitmap(LPCWSTR pFile)				{ m_pBitmap = NULL; Load(pFile); }
	virtual ~CGdiPlusBitmap()					{ Empty(); }

	void Empty()								{ delete m_pBitmap; m_pBitmap = NULL; }

	bool Load(LPCWSTR pFile)
	{
		Empty();
		m_pBitmap = Gdiplus::Bitmap::FromFile(pFile);
		return m_pBitmap->GetLastStatus() == Gdiplus::Ok;
	}

	operator Gdiplus::Bitmap*() const			{ return m_pBitmap; }
};

class CGdiPlusBitmapResource : public CGdiPlusBitmap
{
protected:
	HGLOBAL m_hBuffer;

public:
	CGdiPlusBitmapResource()					{ m_hBuffer = NULL; }
	CGdiPlusBitmapResource(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL; Load(pName, pType, hInst);
	}
	CGdiPlusBitmapResource(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL; Load(id, pType, hInst);
	}
	CGdiPlusBitmapResource(UINT id, UINT type, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL; Load(id, type, hInst);
	}
	virtual ~CGdiPlusBitmapResource()			{ Empty(); }

	void Empty();

	bool Load(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL);
	bool Load(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		return Load(MAKEINTRESOURCE(id), pType, hInst);
	}
	bool Load(UINT id, UINT type, HMODULE hInst = NULL)
	{
		return Load(MAKEINTRESOURCE(id), MAKEINTRESOURCE(type), hInst);
	}
};

inline void CGdiPlusBitmapResource::Empty()
{
	CGdiPlusBitmap::Empty();
	if (m_hBuffer)
	{
		::GlobalUnlock(m_hBuffer);
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
}

inline bool CGdiPlusBitmapResource::Load(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Empty();

	HRSRC hResource = ::FindResource(hInst, pName, pType);
	if (!hResource)
		return false;

	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if (!imageSize)
		return false;

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if (!pResourceData)
		return false;

	m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (m_pBitmap)
				{
					if (m_pBitmap->GetLastStatus() == Gdiplus::Ok)
						return true;

					delete m_pBitmap;
					m_pBitmap = NULL;
				}
			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return false;
}

struct ThreadData {

	ThreadData() {
		object = object2 = NULL;
		thread_executed = 0;
		thread_result = 0;
		thread_result2 = 0;
		thread_result3 = 0;
		thread_string = _T("");
		thread_string2 = _T("");
		thread_string3 = _T("");
		thread_flag = FALSE;
		thread_flag2 = FALSE;
		thread_flag3 = FALSE;
	}

public:
	void* object;
	void* object2;
	LONG thread_executed;
	int thread_result;
	int thread_result2;
	int thread_result3;
	CString thread_string;
	CString thread_string2;
	CString thread_string3;
	BOOL thread_flag;
	BOOL thread_flag2;
	BOOL thread_flag3;
};

struct WaitModelData {

	WaitModelData() {
		wait_model_control_wnd_list_.RemoveAll();
		wait_model_control_text_list_.RemoveAll();
		wait_model_control_handle_list_.RemoveAll();
	}

public:
	CObArray wait_model_control_wnd_list_;
	CStringArray wait_model_control_text_list_;
	CArray<HANDLE> wait_model_control_handle_list_;
};

struct MsgBoxData {

	MsgBoxData() {
		msgbox_button_text_.RemoveAll();
		msgbox_button_text_.Add(L"Ok");
		msgbox_button_text_.Add(L"Cancel");
		msgbox_button_text_.Add(L"Abort");
		msgbox_button_text_.Add(L"Retry");
		msgbox_button_text_.Add(L"Ignore");
		msgbox_button_text_.Add(L"Yes");
		msgbox_button_text_.Add(L"No");
		msgbox_button_text_.Add(L"Close");
		msgbox_button_text_.Add(L"Help");
		msgbox_button_text_.Add(L"TryAgain");
		msgbox_button_text_.Add(L"Continue");
		msgbox_button_text_.Add(L"Reset");
		
		msgbox_count_ = 0;
	}

public:
	CStringArray msgbox_button_text_;
	LONG msgbox_count_;
};

class INCLUDE_VISION_FRAMEWORK Formation
{
public:
	enum ICON_SIZE { SMALL_ICON, MEDIUM_ICON, BIG_ICON };
	enum FONT_SIZE { ULTRA_SMALL_FONT, SMALL_FONT, MEDIUM_FONT, BIG_FONT, ULTRA_BIG_FONT, LARGE_FONT, ULTRA_LARGE_FONT, EXTRA_LARGE_FONT };
	enum SCREEN_RESOLUTION { LOWER, STANDARD, HIGHER };

	Formation();
	virtual ~Formation();

	static Formation* Instance();
	static void Destroy();
	static void SetConsoleText(CString text, DWORD color);
	static CString GetFileVersion(CString filename);
	static CString GetCompanyName(CString filename);
	static CString GetProductName(CString filename);
	static CString DateFormat();
	static void SetDateFormat(CString date_format);
	static CString GetDateTime(BOOL date_format = FALSE);
	static CString GetDateTime(CString source_datetime, CString source_date_format = L"", CString dest_date_format = L"");
	static CString GetDate(int dd, int mm, int yy, CString date_format = L"");
	static CString GetDate(CString source_date, CString source_date_format = L"", CString dest_date_format = L"");
	static CString GetTime();
	static void FlashWnd(CWnd* wnd, BOOL start);

	static CString PrepareString(CDC& cdc, CString text, CRect bounding_rect);
	static CString PrepareMultilineString(CDC& cdc, CString text, CRect bounding_rect);
	static void LockDirectory(CString directory, BOOL lock);
	static void LockDirectories(CString directory, BOOL lock);
	static void HideDirectory(CString directory, BOOL hide);
	static void HideDirectories(CString directory, BOOL hide);
	static HANDLE StartProcess(CString process_name, CString command_line, WORD show_window, DWORD creation_flag = CREATE_NO_WINDOW);
	static void KillProcess(CString process_name);
	static BOOL IsProcessRunning(CString process_name);

	static int image_area_width_in_percentage() { return formation_->image_area_width_in_percentage_; }
	static int thumb_area_width_in_percentage() { return formation_->thumb_area_width_in_percentage_; }
	static int toolbox_area_width_in_percentage() { return formation_->toolbox_area_width_in_percentage_; }
	static int taskbar_height() { return formation_->taskbar_height_; }

	static SCREEN_RESOLUTION screen_resolution() { return formation_->screen_resolution_; }
	static int& spacingHalf() { return formation_->spacing_[0]; }
	static int& spacing() { return formation_->spacing_[1]; }
	static int& spacing2() { return formation_->spacing_[2]; }
	static int& spacing3() { return formation_->spacing_[3]; }
	static int& spacing4() { return formation_->spacing_[4]; }
	static int& heading_height() { return formation_->title_height_; }
	static const int control_height() { return formation_->control_height_; }

	static CFont& font(FONT_SIZE size) { return formation_->font_[size]; }
	static HFONT hfont(FONT_SIZE size) { return (HFONT)formation_->font_[size].GetSafeHandle(); }
	static int icon_size(ICON_SIZE size) { return formation_->icon_size_[size]; }
	static void set_image_bank_size(double size) { formation_->image_bank_size_ = size; }
	static double image_bank_size() { return formation_->image_bank_size_; }

	static CBrush& spancolor1_brush() { return formation_->spancolor1_brush_; }
	static CBrush& spancolor1_brush_for_selection() { return formation_->spancolor1_sel_brush_; }
	static CBrush& spancolor2_brush() { return formation_->spancolor2_brush_; }
	static CBrush& labelcolor_brush() { return formation_->labelcolor_brush_; }
	static CBrush& labelcolor2_brush() { return formation_->labelcolor2_brush_; }
	static CBrush& labelcolor3_brush() { return formation_->labelcolor3_brush_; }
	static CBrush& disablecolor_brush() { return formation_->disablecolor_brush_; }
	static CBrush& accept_brush() { return formation_->accept_brush_; }
	static CBrush& reject_brush() { return formation_->reject_brush_; }
	static CBrush& scrollcolor_brush() { return formation_->scrollcolor_brush_; }
	static CBrush& whitecolor_brush() { return formation_->whitecolor_brush_; }
	static CBrush& blackcolor_brush() { return formation_->blackcolor_brush_; }
	static CBrush& bluecolor_brush() { return formation_->bluecolor_brush_; }
	static CPen& spancolor1_pen() { return formation_->spancolor1_pen_; }
	static CPen& spancolor1_pen_for_selection() { return formation_->spancolor1_sel_pen_; }
	static CPen& spancolor2_pen() { return formation_->spancolor2_pen_; }
	static CPen& labelcolor_pen() { return formation_->labelcolor_pen_; }
	static CPen& labellinecolor_pen() { return formation_->labellinecolor_pen_; }
	static CPen& labelcolor2_pen() { return formation_->labelcolor2_pen_; }
	static CPen& labelcolor3_pen() { return formation_->labelcolor3_pen_; }
	static CPen& disablecolor_pen() { return formation_->disablecolor_pen_; }
	static CPen& blackcolor_pen() { return formation_->blackcolor_pen_; }
	static CPen& blackcolor_pen2() { return formation_->blackcolor_pen2_; }
	static CPen& whitecolor_pen() { return formation_->whitecolor_pen_; }
	static CPen& whitecolor_pen2() { return formation_->whitecolor_pen2_; }
	static CPen& acceptcolor_pen() { return formation_->acceptcolor_pen_; }
	static CPen& rejectcolor_pen() { return formation_->rejectcolor_pen_; }
	static CPen& rejectcolor_pen2() { return formation_->rejectcolor_pen2_; }
	static CPen& scrollcolor_pen() { return formation_->scrollcolor_pen_; }
	static CPen& bluecolor_pen() { return formation_->bluecolor_pen_; }

	static Gdiplus::SolidBrush* spancolor1_brush_gdi() { return formation_->spancolor1_brush_gdi_; }
	static Gdiplus::SolidBrush* spancolor1_brush_for_selection_gdi() { return formation_->spancolor1_sel_brush_gdi_; }
	static Gdiplus::SolidBrush* spancolor2_brush_gdi() { return formation_->spancolor2_brush_gdi_; }
	static Gdiplus::SolidBrush* blackcolor_brush_gdi() { return formation_->blackcolor_brush_gdi_; }
	static Gdiplus::SolidBrush* whitecolor_brush_gdi() { return formation_->whitecolor_brush_gdi_; }
	static Gdiplus::SolidBrush* disablecolor_brush_gdi() { return formation_->disablecolor_brush_gdi_; }
	static Gdiplus::SolidBrush* labelcolor_brush_gdi() { return formation_->labelcolor_brush_gdi_; }
	static Gdiplus::SolidBrush* labelcolor2_brush_gdi() { return formation_->labelcolor2_brush_gdi_; }
	static Gdiplus::SolidBrush* labelcolor3_brush_gdi() { return formation_->labelcolor3_brush_gdi_; }
	static Gdiplus::SolidBrush* yellowcolor_brush_gdi() { return formation_->yellowcolor_brush_gdi_; }
	static Gdiplus::SolidBrush* acceptcolor_brush_gdi() { return formation_->acceptcolor_brush_gdi_; }
	static Gdiplus::SolidBrush* rejectcolor_brush_gdi() { return formation_->rejectcolor_brush_gdi_; }

	static Gdiplus::Pen* spancolor1_pen_gdi() { return formation_->spancolor1_pen_gdi_; }
	static Gdiplus::Pen* spancolor1_pen2_gdi() { return formation_->spancolor1_pen2_gdi_; }
	static Gdiplus::Pen* spancolor1_pen_for_selection_gdi() { return formation_->spancolor1_sel_pen_gdi_; }
	static Gdiplus::Pen* spancolor2_pen_gdi() { return formation_->spancolor2_pen_gdi_; }
	static Gdiplus::Pen* blackcolor_pen_gdi() { return formation_->blackcolor_pen_gdi_; }
	static Gdiplus::Pen* blackcolor_pen2_gdi() { return formation_->blackcolor_pen2_gdi_; }
	static Gdiplus::Pen* whitecolor_pen_gdi() { return formation_->whitecolor_pen_gdi_; }
	static Gdiplus::Pen* whitecolor_pen2_gdi() { return formation_->whitecolor_pen2_gdi_; }
	static Gdiplus::Pen* disablecolor_pen_gdi() { return formation_->disablecolor_pen_gdi_; }
	static Gdiplus::Pen* disablecolor_pen2_gdi() { return formation_->disablecolor_pen2_gdi_; }
	static Gdiplus::Pen* scrollcolor_pen_gdi() { return formation_->scrollcolor_pen_gdi_; }
	static Gdiplus::Pen* labelcolor_pen_gdi() { return formation_->labelcolor_pen_gdi_; }
	static Gdiplus::Pen* labellinecolor_pen_gdi() { return formation_->labellinecolor_pen_gdi_; }
	static Gdiplus::Pen* labelcolor2_pen_gdi() { return formation_->labelcolor2_pen_gdi_; }
	static Gdiplus::Pen* labelcolor3_pen_gdi() { return formation_->labelcolor3_pen_gdi_; }
	static Gdiplus::Pen* yellowcolor_pen_gdi() { return formation_->yellowcolor_pen_gdi_; }
	static Gdiplus::Pen* acceptcolor_pen_gdi() { return formation_->acceptcolor_pen_gdi_; }
	static Gdiplus::Pen* rejectcolor_pen_gdi() { return formation_->rejectcolor_pen_gdi_; }
	static Gdiplus::Pen* acceptcolor_pen2_gdi() { return formation_->acceptcolor_pen2_gdi_; }
	static Gdiplus::Pen* rejectcolor_pen2_gdi() { return formation_->rejectcolor_pen2_gdi_; }

	static BOOL IsPopupOpen();

	//POPUP WINDOWS
	static int OpenDialog(CDialog* dialog, int type = -1);
	static void CloseAllPopups();

	//WAIT CONTROL
	static void WaitOpen(CString title, CString text);
	static BOOL IsWaitOpen();
	static void WaitHide(DWORD hide);
	static void WaitClose();
	
	//WAIT MODEL CONTROL
	static WaitModelData& GetWaitModelData();
	
	//MSG BOX
	static MsgBoxData& GetMsgBoxData();
	static void SetButtonsText(CString yes, CString no, CString ok, CString cancel, CString reset);
	static int MsgBox(CString text, UINT type = MB_OK | MB_ICONERROR, BOOL remain_popup = FALSE);
	static BOOL IsMsgBoxOpen();

	//PATTERN MATCHING
	static BOOL IsStringMatch(CString str, CString pattern, CString field_name, INT string_length = 45);

	//INNER RECT
	static long DivideInSquare(long width, long height, long n, int& rows, int& cols);
	static void GetInsideBox(std::vector<POINT> &subRectInfo, RECT mainRect, long sW, int allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect);
	static void GetStrokeImagePosition(long window_width, long window_height, long spacing_between_images, long image_width, long image_height, long total_images, std::vector<RECT> &image_rect, BOOL top_to_bottom = FALSE);
	static CRect GetRectInCenter(CRect ParentRect, CSize ImgSize);
	static CRect GetRectInCenterAndLeftAlligned(CRect ParentRect, CSize ImgSize);
	static CRect GetRectInCenterAndBottomAlligned(CRect ParentRect, CSize ImgSize);

private:
	static Formation* formation_;

	CRITICAL_SECTION cs_for_console_;

	WaitControl waitcontrol_;

	WaitModelControl waitmodelcontrol_; //Temp
	WaitModelData wait_model_data_;
	MsgBoxData msg_box_data_;

	InnerRect inner_rect_;

	ULONG_PTR gdiplus_token_;

	CString date_format_;

	int flash_wnd_count_;
	int flash_wnd_time_;
	MMRESULT flash_wnd_timer_event_;

	int image_area_width_in_percentage_ = 65;
	int thumb_area_width_in_percentage_ = 9;
	int toolbox_area_width_in_percentage_ = 26;

	int control_height_;
	int title_height_;
	int spacing_[5];
	int taskbar_height_;

	SCREEN_RESOLUTION screen_resolution_;

	CBrush spancolor1_brush_;
	CBrush spancolor1_sel_brush_;
	CBrush spancolor2_brush_;
	CBrush labelcolor_brush_;
	CBrush labelcolor2_brush_;
	CBrush labelcolor3_brush_;
	CBrush disablecolor_brush_;
	CBrush accept_brush_;
	CBrush reject_brush_;
	CBrush scrollcolor_brush_;
	CBrush whitecolor_brush_;
	CBrush blackcolor_brush_;
	CBrush bluecolor_brush_;
	CPen spancolor1_pen_;
	CPen spancolor1_sel_pen_;
	CPen spancolor2_pen_;
	CPen labelcolor_pen_;
	CPen labellinecolor_pen_;
	CPen labelcolor2_pen_;
	CPen labelcolor3_pen_;
	CPen disablecolor_pen_;
	CPen blackcolor_pen_;
	CPen blackcolor_pen2_;
	CPen whitecolor_pen_;
	CPen whitecolor_pen2_;
	CPen acceptcolor_pen_;
	CPen rejectcolor_pen_;
	CPen rejectcolor_pen2_;
	CPen scrollcolor_pen_;
	CPen bluecolor_pen_;

	Gdiplus::SolidBrush* spancolor1_brush_gdi_;
	Gdiplus::SolidBrush* spancolor1_sel_brush_gdi_;
	Gdiplus::SolidBrush* spancolor2_brush_gdi_;
	Gdiplus::SolidBrush* blackcolor_brush_gdi_;
	Gdiplus::SolidBrush* whitecolor_brush_gdi_;
	Gdiplus::SolidBrush* disablecolor_brush_gdi_;
	Gdiplus::SolidBrush* labelcolor_brush_gdi_;
	Gdiplus::SolidBrush* labelcolor2_brush_gdi_;
	Gdiplus::SolidBrush* labelcolor3_brush_gdi_;
	Gdiplus::SolidBrush* yellowcolor_brush_gdi_;
	Gdiplus::SolidBrush* acceptcolor_brush_gdi_;
	Gdiplus::SolidBrush* rejectcolor_brush_gdi_;
	Gdiplus::Pen* spancolor1_pen_gdi_;
	Gdiplus::Pen* spancolor1_pen2_gdi_;
	Gdiplus::Pen* spancolor1_sel_pen_gdi_;
	Gdiplus::Pen* spancolor2_pen_gdi_;
	Gdiplus::Pen* blackcolor_pen_gdi_;
	Gdiplus::Pen* blackcolor_pen2_gdi_;
	Gdiplus::Pen* whitecolor_pen_gdi_;
	Gdiplus::Pen* whitecolor_pen2_gdi_;
	Gdiplus::Pen* disablecolor_pen_gdi_;
	Gdiplus::Pen* disablecolor_pen2_gdi_;
	Gdiplus::Pen* scrollcolor_pen_gdi_;
	Gdiplus::Pen* labelcolor_pen_gdi_;
	Gdiplus::Pen* labellinecolor_pen_gdi_;
	Gdiplus::Pen* labelcolor2_pen_gdi_;
	Gdiplus::Pen* labelcolor3_pen_gdi_;
	Gdiplus::Pen* yellowcolor_pen_gdi_;
	Gdiplus::Pen* acceptcolor_pen_gdi_;
	Gdiplus::Pen* rejectcolor_pen_gdi_;
	Gdiplus::Pen* acceptcolor_pen2_gdi_;
	Gdiplus::Pen* rejectcolor_pen2_gdi_;

	int icon_size_[3];
	CFont font_[8];
	double image_bank_size_;
	CArray<CDialog*> popup_dlg_array_;
	CArray<UINT> popup_type_array_;
};

INCLUDE_VISION_FRAMEWORK BOOL IsWaitModelOpen();
INCLUDE_VISION_FRAMEWORK void WaitModelOpen(CString title, CString text, HANDLE event_handle = NULL, BOOL show_progressbar = FALSE);
INCLUDE_VISION_FRAMEWORK void WaitModelChangeText(CString current_text, CString new_text);
INCLUDE_VISION_FRAMEWORK void WaitModelHide(DWORD hide, CString text);
INCLUDE_VISION_FRAMEWORK void WaitModelClose(CString text);
INCLUDE_VISION_FRAMEWORK void WaitModelClose2(HANDLE event_handle);
INCLUDE_VISION_FRAMEWORK void WaitModelCloseAll();
INCLUDE_VISION_FRAMEWORK float& AssignProgressValue(CString text);
INCLUDE_VISION_FRAMEWORK CString& AssignProgressText(CString text);
INCLUDE_VISION_FRAMEWORK int MsgBox(CString text, UINT type = MB_OK | MB_ICONERROR, BOOL remain_popup = FALSE);
INCLUDE_VISION_FRAMEWORK BOOL IsStringMatch(CString str, std::string pattern, CString field, int string_length);