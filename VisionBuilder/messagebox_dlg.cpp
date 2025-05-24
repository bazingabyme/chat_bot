// messagebox_dlg.cpp : implementation file
//

#include "stdafx.h"
#include "formation.h"

// MessageBoxDlg dialog

IMPLEMENT_DYNAMIC(MessageBoxDlg, CDialogEx)

MessageBoxDlg::MessageBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(MessageBoxDlg::IDD, pParent)
{
}

MessageBoxDlg::~MessageBoxDlg()
{
}

void MessageBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MessageBoxDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// MessageBoxDlg message handlers

BOOL MessageBoxDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		CPoint p;
		GetCursorPos(&p);
		CRect r;
		GetWindowRect(&r);
		CRect t = r;
		t.bottom = t.top + Formation::control_height();
		if (t.PtInRect(p)) {
			ReleaseCapture();
			SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
			return TRUE;
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

BOOL MessageBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(WHITE_COLOR);
	ModifyStyle(WS_CAPTION, 0);

	int min_text_width = Formation::control_height() * 8;
	int max_text_width = GetSystemMetrics(SM_CXSCREEN);
	int min_text_height = Formation::control_height() * 1;
	int max_text_height = Formation::control_height() * 20;

	CDC* dc = GetDC();
	dc->SelectObject(Formation::font(Formation::MEDIUM_FONT));
	CRect rect(0,0,0,0);
	dc->DrawText(text_, &rect, DT_CALCRECT);
	ReleaseDC(dc);

	GetClientRect(client_rect_);

	title_rect_ = client_rect_;
	title_rect_.bottom = title_rect_.top + Formation::heading_height();
	
	icon_rect_ = text_rect_ = title_rect_;
	icon_rect_.top = text_rect_.top = title_rect_.bottom + Formation::spacing();
	icon_rect_.top += Formation::spacing4();
	icon_rect_.left += Formation::spacing4();
	icon_rect_.right = icon_rect_.left + Formation::icon_size(Formation::MEDIUM_ICON);
	text_rect_.top += Formation::spacing4();
	text_rect_.left = icon_rect_.right + Formation::spacing4();
	text_rect_.right = text_rect_.left + rect.Width();
	text_rect_.bottom = text_rect_.top + rect.Height();
	if (rect.Width() < min_text_width) {
		text_rect_.right = text_rect_.left + min_text_width;
	} else if (rect.Width() > max_text_width) {
		text_rect_.right = text_rect_.left + max_text_width;
	}
	if (text_rect_.bottom < min_text_height) {
		text_rect_.bottom = min_text_height;
	} else if (text_rect_.bottom > max_text_height) {
		text_rect_.bottom = max_text_height;
	}

	client_rect_.right = text_rect_.right + Formation::spacing4();
	client_rect_.bottom = text_rect_.bottom + Formation::control_height(); //Spacing between text and buttons

	button_rect_.RemoveAll();
	for (int button_index = 0; button_index < int(button_text_.GetSize()); button_index++) {
		button_rect_.Add(CRect(client_rect_.left, client_rect_.bottom, client_rect_.left, client_rect_.bottom + Formation::heading_height() + Formation::spacing()));
	}
	
	client_rect_.bottom += Formation::heading_height() + Formation::spacing4(); //Buttons
	//client_rect_.bottom += title_rect_.Height() + Formation::spacing() + 1;
	title_rect_.right = client_rect_.right;

	int button_width = 0;
	button_index_ = -1;

	if ((type_ & MB_TYPEMASK) == MB_OK) {

		button_rect_[0].left = client_rect_.CenterPoint().x - Formation::heading_height() * 2;
		button_rect_[0].right = client_rect_.CenterPoint().x + Formation::heading_height() * 2;

	} else if ((type_ & MB_TYPEMASK) == MB_OKCANCEL) {

		button_rect_[0].left = client_rect_.CenterPoint().x - Formation::heading_height() * 4;
		button_rect_[0].right = client_rect_.CenterPoint().x - (Formation::spacing() / 2);

		button_rect_[1].left = button_rect_[0].right + Formation::spacing();
		button_rect_[1].right = button_rect_[1].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_ABORTRETRYIGNORE) {

		button_rect_[2].left = client_rect_.CenterPoint().x - Formation::heading_height() * 6;
		button_rect_[2].right = button_rect_[2].left + (Formation::heading_height() * 4);

		button_rect_[3].left = button_rect_[2].right + Formation::spacing();
		button_rect_[3].right = button_rect_[3].left + (Formation::heading_height() * 4);

		button_rect_[4].left = button_rect_[3].right + Formation::spacing();
		button_rect_[4].right = button_rect_[4].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_YESNOCANCEL) {

		button_rect_[5].left = client_rect_.CenterPoint().x - Formation::heading_height() * 6;
		button_rect_[5].right = button_rect_[5].left + (Formation::heading_height() * 4);

		button_rect_[6].left = button_rect_[5].right + Formation::spacing();
		button_rect_[6].right = button_rect_[6].left + (Formation::heading_height() * 4);

		button_rect_[1].left = button_rect_[6].right + Formation::spacing();
		button_rect_[1].right = button_rect_[1].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_YESNO) {

		button_rect_[5].left = client_rect_.CenterPoint().x - Formation::heading_height() * 4;
		button_rect_[5].right = client_rect_.CenterPoint().x - (Formation::spacing() / 2);

		button_rect_[6].left = button_rect_[5].right + Formation::spacing();
		button_rect_[6].right = button_rect_[6].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_RETRYCANCEL) {

		button_rect_[3].left = client_rect_.CenterPoint().x - Formation::heading_height() * 4;
		button_rect_[3].right = client_rect_.CenterPoint().x - (Formation::spacing() / 2);

		button_rect_[1].left = button_rect_[3].right + Formation::spacing();
		button_rect_[1].right = button_rect_[1].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_CANCELTRYCONTINUE) {

		button_rect_[1].left = client_rect_.CenterPoint().x - Formation::heading_height() * 6;
		button_rect_[1].right = button_rect_[1].left + (Formation::heading_height() * 4);

		button_rect_[9].left = button_rect_[1].right + Formation::spacing();
		button_rect_[9].right = button_rect_[9].left + (Formation::heading_height() * 4);

		button_rect_[10].left = button_rect_[9].right + Formation::spacing();
		button_rect_[10].right = button_rect_[10].left + (Formation::heading_height() * 4);

	} else if ((type_ & MB_TYPEMASK) == MB_RESET) {

		button_rect_[11].left = client_rect_.CenterPoint().x - Formation::heading_height() * 2;
		button_rect_[11].right = client_rect_.CenterPoint().x + Formation::heading_height() * 2;
	}
	
	client_rect_.right += GetSystemMetrics(SM_CXDLGFRAME);
	client_rect_.bottom += GetSystemMetrics(SM_CYDLGFRAME);
	int screen_width = GetSystemMetrics(SM_CXSCREEN); //Get the system metrics - VERT
	int screen_height = GetSystemMetrics(SM_CYSCREEN); //Get the system metrics - HORZ
	int window_width = client_rect_.Width();
	int window_height = client_rect_.Height();
	int offset_x = (screen_width - window_width) / 2;
	int offset_y = (screen_height - window_height) / 2;

	SetWindowPos(&CWnd::wndTop, client_rect_.left + offset_x, client_rect_.top + offset_y, window_width, window_height, SWP_SHOWWINDOW);

	return TRUE;
}

void MessageBoxDlg::SetButtonsText(CStringArray& button_text) {

	button_text_.RemoveAll();
	for (int i = 0; i < int(button_text.GetSize()); i++) {
		button_text_.Add(button_text[i]);
	}
}

int MessageBoxDlg::Open(CString text, UINT type, BOOL remain_popup) {

	text_ = text;
	type_ = type;
	button_index_ = -1;

	int result = IDCANCEL;

	if (remain_popup) {
		result = int(DoModal());
	} else {
		result = Formation::OpenDialog(this, type);
	}

	return result;
}

void MessageBoxDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(Formation::font(Formation::BIG_FONT));

	offdc.FillSolidRect(client_rect_, LABEL_COLOR3);
	offdc.FillSolidRect(title_rect_, BACKCOLOR1);
	offdc.SetTextColor(WHITE_COLOR);
	offdc.DrawText(L" SPAN", title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	
	HICON icon;
	if ((type_ & MB_ICONMASK) == MB_ICONEXCLAMATION) {
		icon = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_MESSAGEBOX_INFORMATION), IMAGE_ICON, Formation::icon_size(Formation::BIG_ICON), Formation::icon_size(Formation::BIG_ICON), LR_DEFAULTCOLOR);
	} else if ((type_ & MB_ICONMASK) == MB_ICONERROR) {
		icon = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_MESSAGEBOX_ERROR), IMAGE_ICON, Formation::icon_size(Formation::BIG_ICON), Formation::icon_size(Formation::BIG_ICON), LR_DEFAULTCOLOR);
	} else if ((type_ & MB_ICONMASK) == MB_ICONQUESTION) {
		icon = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_MESSAGEBOX_QUESTION), IMAGE_ICON, Formation::icon_size(Formation::BIG_ICON), Formation::icon_size(Formation::BIG_ICON), LR_DEFAULTCOLOR);
	} else {
		icon = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_MESSAGEBOX_INFORMATION), IMAGE_ICON, Formation::icon_size(Formation::BIG_ICON), Formation::icon_size(Formation::BIG_ICON), LR_DEFAULTCOLOR);
	}
	offdc.DrawState(icon_rect_.TopLeft(), CSize(icon_rect_.Width(), icon_rect_.Height()), icon, DSS_NORMAL, (CBrush*)NULL);
	DestroyIcon(icon);

	offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));
	offdc.SetTextColor(BLACK_COLOR);
	offdc.DrawText(text_, text_rect_, DT_LEFT | DT_TOP | DT_WORDBREAK);
	
	offdc.SetTextColor(WHITE_COLOR);
	for (int index = 0; index < int(button_rect_.GetSize()); index++) {
		if (button_rect_[index].Width() != 0) {
			offdc.FillSolidRect(button_rect_[index], BACKCOLOR1);
			offdc.DrawText(button_text_[index], button_rect_[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
	if (button_index_ != -1) {
		if (button_index_ < int(button_rect_.GetSize())) {
			offdc.FillSolidRect(button_rect_[button_index_], BACKCOLOR1_SEL);
			offdc.DrawText(button_text_[button_index_], button_rect_[button_index_], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
}

void MessageBoxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	button_index_ = -1;
	for (int index = 0; index < button_rect_.GetSize(); index++) {
		if (button_rect_[index].PtInRect(point)) {
			button_index_ = index;
			InvalidateRect(button_rect_[index], FALSE);
		}
	}
}

void MessageBoxDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (button_index_ != -1) {
		EndDialog(button_index_ + 1);
		button_index_ = -1;
	}
}