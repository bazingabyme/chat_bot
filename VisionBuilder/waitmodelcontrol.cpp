// waitmodelcontrol.cpp : implementation file
//

#include "stdafx.h"
#include "formation.h"

// WaitModelControl dialog

IMPLEMENT_DYNAMIC(WaitModelControl, CDialogEx)

WaitModelControl::WaitModelControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(WaitModelControl::IDD, pParent)
{
	model_dialog_created_handle_ = NULL;
	close_modal_dialog_handle_ = NULL;
	close_thread_handle_ = NULL;

	text_ = L"";
	percentage_ = 0;
	show_progress_bar_ = FALSE;
	winthread_ = NULL;
	interlock_open_ = 0;
	interlock_close_ = 0;
}

WaitModelControl::~WaitModelControl()
{
}

void WaitModelControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WaitModelControl, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(WM_WAIT_MODEL_CLOSE, Close)
END_MESSAGE_MAP()

void WaitModelControl::Open(CString title, CString text, BOOL show_progress) {

	if (interlock_open_ != 0) {
//wprintf(L"WaitModelControl::Open<%s>...Return\n", text);
		return;
	}
	interlock_open_ = 1;

	//model_dialog_created_handle_ = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	AfxGetApp()->DoWaitCursor(1);

	title_ = L" " + title;
	text_ = text;
	percentage_ = 0;
	show_progress_bar_ = show_progress;
	winthread_ = NULL;

//wprintf(L"WaitModelControl::Open<%s>\n", text);

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	DoModal();
	AfxSetResourceHandle(old_resource_handle);

	AfxGetApp()->DoWaitCursor(-1);

	interlock_open_ = 0;
}

BOOL WaitModelControl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		CPoint cur_pos;
		GetCursorPos(&cur_pos);
		CRect title_rect;
		GetWindowRect(&title_rect);
		title_rect.bottom = title_rect.top + Formation::control_height();
		if (title_rect.PtInRect(cur_pos)) {
			ReleaseCapture();
			SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
			return 1;
		}
	}
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_F1 || pMsg->wParam == VK_F2 ||
		pMsg->wParam == VK_F3 || pMsg->wParam == VK_F4 || pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6 ||
		pMsg->wParam == VK_F7 || pMsg->wParam == VK_F8 || pMsg->wParam == VK_F9 || pMsg->wParam == VK_F10 ||
		pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 || pMsg->wParam == VK_END || pMsg->wParam == VK_HOME) {
		return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL WaitModelControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(WHITE_COLOR);
	ModifyStyle(WS_CAPTION, 0);

	GetClientRect(client_rect_);

	//Calculate rect size based on text
	CDC* dc = GetDC();
	dc->SelectObject(Formation::font(Formation::MEDIUM_FONT));
	CRect rect(0, 0, 0, 0);
	dc->DrawText(text_, &rect, DT_CALCRECT);
	client_rect_.right = rect.right + Formation::control_height() * 5;
	if (show_progress_bar_) {
		client_rect_.bottom = rect.bottom + Formation::control_height() * 4;
	} else {
		client_rect_.bottom = rect.bottom + Formation::control_height() * 3;
	}
	ReleaseDC(dc);

	wnd_rect_.left = (GetSystemMetrics(SM_CXSCREEN) - client_rect_.Width()) / 2;
	wnd_rect_.top = (GetSystemMetrics(SM_CYSCREEN) - client_rect_.Height()) / 2;
	wnd_rect_.right = wnd_rect_.left + client_rect_.Width();
	wnd_rect_.bottom = wnd_rect_.top + client_rect_.Height();
	MoveWindow(wnd_rect_.left, wnd_rect_.top, wnd_rect_.Width(), wnd_rect_.Height());

	caption_rect_ = client_rect_;
	caption_rect_.bottom = caption_rect_.top + Formation::control_height();

	text_rect_.SetRect(client_rect_.left, caption_rect_.bottom, client_rect_.right, client_rect_.bottom);
	progress_rect_.SetRectEmpty();
	if (show_progress_bar_) {
		CRect rect(caption_rect_.left, caption_rect_.bottom, caption_rect_.right, client_rect_.bottom - Formation::control_height());
		progress_rect_.SetRect(rect.CenterPoint().x - Formation::heading_height(), rect.CenterPoint().y - Formation::heading_height(),
			rect.CenterPoint().x + Formation::heading_height(), rect.CenterPoint().y + Formation::heading_height());
		text_rect_.top = progress_rect_.bottom;

		SetTimer(1, 100, NULL);
	}

	//SetEvent(model_dialog_created_handle_);

	return TRUE;
}

void WaitModelControl::ChangeText(CString text) {

	text_ = text;

	InvalidateRect(client_rect_, FALSE);
}

void WaitModelControl::Hide(DWORD hide)
{
	ShowWindow(hide);
}

LRESULT WaitModelControl::Close(WPARAM wparam, LPARAM lparam) {

	if (interlock_close_ != 0 || interlock_open_ == 0) {
//		wprintf(L"WaitModelControl::Close:<%s>...Return\n", text_);
		return 0;
	}
	interlock_close_ = 1;

	KillTimer(1);

	//WaitForSingleObject(model_dialog_created_handle_, 100);

	EndDialog(IDCANCEL);
//	wprintf(L"WaitModelControl::Close:<%s>\n", text_);

	interlock_close_ = 0;

	return 0;
}

void WaitModelControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.FillSolidRect(client_rect_, LABEL_COLOR3);
	offdc.FillSolidRect(caption_rect_, BACKCOLOR1);
	offdc.SetTextColor(WHITE_COLOR);
	offdc.SelectObject(Formation::font(Formation::BIG_FONT));
	offdc.DrawText(L" " + title_, caption_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	offdc.SetTextColor(BLACK_COLOR);
	offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));
	CRect rect;
	offdc.DrawText(text_, &rect, DT_CALCRECT);
	rect.MoveToY(text_rect_.top + (text_rect_.Height() - rect.Height()) / 2);
	rect.MoveToX(text_rect_.left + (text_rect_.Width() - rect.Width()) / 2);
	offdc.DrawText(text_, rect, DT_LEFT);

	if (show_progress_bar_) {
		Gdiplus::Graphics g(offdc.GetSafeHdc());
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		g.DrawEllipse(Formation::labelcolor2_pen_gdi(), progress_rect_.left, progress_rect_.top, progress_rect_.Width(), progress_rect_.Height());
		Gdiplus::GraphicsPath gp;
		gp.AddArc(progress_rect_.left, progress_rect_.top, progress_rect_.Width(), progress_rect_.Height(), 0, Gdiplus::REAL(percentage_ * 3.6));
		g.DrawPath(Formation::spancolor1_pen2_gdi(), &gp);
		CString str;
		str.Format(L"%d%%", (int)percentage_);
		offdc.DrawText(str, progress_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
}
//
//void WaitModelControl::Paint()
//{
//	if (GetSafeHwnd() != NULL) {
//
//		
//	}
//}
//
//UINT ProgressBarThread(LPVOID param) {
//
//	WaitModelControl* waitdlg = (WaitModelControl*)param;
//	if (waitdlg) {
//		printf("WaitModelControl::ProgressBarThread...\n");
//		while (1) {
//			if (WaitForSingleObject(waitdlg->close_thread_handle_, 0) == WAIT_OBJECT_0) {
//				printf("WaitModelControl::ProgressBarThread...Done\n");
//				break;
//			}
//			waitdlg->SetWindowPos(&CWnd::wndTopMost, waitdlg->wnd_rect_.left, waitdlg->wnd_rect_.top, waitdlg->wnd_rect_.Width(), waitdlg->wnd_rect_.Height(), SWP_SHOWWINDOW);
//			if (waitdlg->show_progress_bar_) {
//				waitdlg->Paint();
//				Sleep(250);
//			}
//		}
//	}
//
//	return 0;
//}

void WaitModelControl::OnTimer(UINT_PTR nIDEvent)
{
	InvalidateRect(progress_rect_, FALSE);

	CDialogEx::OnTimer(nIDEvent);
}
