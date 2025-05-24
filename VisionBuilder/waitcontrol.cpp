// waitcontrol.cpp : implementation file
//

#include "stdafx.h"
#include "formation.h"

// WaitControl dialog

IMPLEMENT_DYNAMIC(WaitControl, CDialogEx)

WaitControl::WaitControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(WaitControl::IDD, pParent)
{

	title_ = text_ = L"";
}

WaitControl::~WaitControl()
{
}

void WaitControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WaitControl, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void WaitControl::Open(CString title, CString text) {

	AfxGetApp()->DoWaitCursor(1);

	title_ = L" " + title;
	text_ = text;

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	Create(IDD_WAIT_DLG);
	AfxSetResourceHandle(old_resource_handle);

	AfxGetApp()->DoWaitCursor(-1);
}

BOOL WaitControl::PreTranslateMessage(MSG* pMsg)
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

BOOL WaitControl::OnInitDialog()
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
	client_rect_.bottom = rect.bottom + Formation::control_height() * 3;
	ReleaseDC(dc);

	CRect wnd_rect;
	wnd_rect.left = (GetSystemMetrics(SM_CXSCREEN) - client_rect_.Width()) / 2;
	wnd_rect.top = (GetSystemMetrics(SM_CYSCREEN) - client_rect_.Height()) / 2;
	wnd_rect.right = wnd_rect.left + client_rect_.Width();
	wnd_rect.bottom = wnd_rect.top + client_rect_.Height();
	MoveWindow(wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height());

	title_rect_ = text_rect_ = client_rect_;
	title_rect_.bottom = title_rect_.top + Formation::control_height();
	text_rect_.top = title_rect_.bottom;

	return TRUE;
}

void WaitControl::Close()
{
	if (GetSafeHwnd() != NULL) {
		DestroyWindow();
	}
}

void WaitControl::Hide(DWORD hide) 
{
	if (hide == SW_HIDE) {
		Close();
	} else {
		Open(title_, text_);
	}
}

void WaitControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.FillSolidRect(client_rect_, LABEL_COLOR3);
	offdc.FillSolidRect(title_rect_, BACKCOLOR1);
	offdc.SetTextColor(WHITE_COLOR);
	offdc.SelectObject(Formation::font(Formation::BIG_FONT));
	offdc.DrawText(L" " + title_, title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	offdc.SetTextColor(BLACK_COLOR);
	offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));
	CRect rect;
	offdc.DrawText(text_, &rect, DT_CALCRECT);
	rect.MoveToY(text_rect_.top + (text_rect_.Height() - rect.Height()) / 2);
	rect.MoveToX(text_rect_.left + (text_rect_.Width() - rect.Width()) / 2);
	offdc.DrawText(text_, rect, DT_LEFT);
}