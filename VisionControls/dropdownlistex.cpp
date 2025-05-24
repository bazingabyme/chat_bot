#include "stdafx.h"
#include "keyboard_dlg.h"

#define INCLUDE_DROPDOWNLISTEX __declspec(dllexport)
#include "dropdownlistex.h"

// ListCtrlEx

IMPLEMENT_DYNAMIC(ListCtrlEx, CStatic)

ListCtrlEx::ListCtrlEx()
{
	row_height_ = 0;

	document_height_ = 0;

	back_color_ = BACKCOLOR1;

	item_list_text_.RemoveAll();
	item_list_rect_.RemoveAll();

	client_rect_.SetRectEmpty();
	wnd_rect_.SetRectEmpty();
}

ListCtrlEx::~ListCtrlEx()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ListCtrlEx, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// GridCtrlEx message handlers

void ListCtrlEx::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	row_height_ = Formation::control_height();

	CStatic::PreSubclassWindow();
}

BOOL ListCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd() && pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CStatic::PreTranslateMessage(pMsg);
}

void ListCtrlEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	int top = -GetScrollPos(SB_VERT);
	//int left = -GetScrollPos(SB_HORZ);
	
	int prev_row_no = -1;
	for (int row_no = 0; row_no < item_list_rect_.GetSize(); row_no++) {

		//Find top
		if (prev_row_no != -1) {
			if (row_no != prev_row_no) {
				if (!item_list_rect_[row_no].IsRectEmpty()) {
					top += row_height_;
					top += spacing_;
				}
			}
		}
		item_list_rect_[row_no].MoveToY(top);

		offdc.FillSolidRect(item_list_rect_[row_no], (selected_index_ == row_no) ? BACKCOLOR1_SEL : back_color_);
		CString text = item_list_text_[row_no];
		if (alignment_ == DT_LEFT) {
			text = L" " + text;
		} else if (alignment_ == DT_RIGHT) {
			text = text + L" ";
		}
		offdc.SetTextColor((selected_index_ == row_no) ? WHITE_COLOR : BLACK_COLOR);
		offdc.DrawText(Formation::PrepareString(offdc, text, item_list_rect_[row_no]), item_list_rect_[row_no], alignment_ | DT_SINGLELINE | DT_VCENTER);
		
		if (!item_list_rect_[row_no].IsRectEmpty()) {
			prev_row_no = row_no;
		}
	}
}

void ListCtrlEx::ResetScrollBar() {

	SCROLLINFO	si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = client_rect_.Height();
	si.nPos = si.nMax = document_height_;// +8;
	si.nMin = 0;
	SetScrollInfo(SB_VERT, &si);
	if (client_rect_.Height() >= document_height_) {
		ShowScrollBar(SB_VERT, FALSE);
	} else {
		SetScrollPos(SB_VERT, selected_index_ == -1 ? 0 : (selected_index_ * row_height_));
		EnableScrollBarCtrl(SB_VERT, TRUE);
	}
}

void ListCtrlEx::Create(CStringArray& items_list, int selected_index, int spacing, DWORD alignment) {

	selected_index_ = selected_index;
	spacing_ = spacing;
	alignment_ = alignment;
	document_height_ = 0;

	GetClientRect(client_rect_);

	item_list_text_.RemoveAll();
	for (int index = 0; index < int(items_list.GetSize()); index++) {
		item_list_text_.Add(items_list[index]);
	}
	
	item_list_rect_.RemoveAll();

	CRect rect = client_rect_;
	int prev_row_no = -1;
	for (int row_no = 0; row_no < item_list_text_.GetSize(); row_no++) {

		if (prev_row_no != -1) {
			if (row_no != prev_row_no) {
				rect.top += row_height_;
				rect.top += spacing_;
			}
		}
		rect.bottom = rect.top + row_height_;

		item_list_rect_.Add(rect);

		prev_row_no = row_no;
	}

	document_height_ = rect.bottom;

	if (document_height_ > client_rect_.Height() /*If vertical scroll bar exist*/) {
		client_rect_.right -= GetSystemMetrics(SM_CXVSCROLL);
		for (int row_no = 0; row_no < item_list_rect_.GetSize(); row_no++) {
			item_list_rect_[row_no].right -= GetSystemMetrics(SM_CXVSCROLL);
		}
	} else {
		for (int row_no = 0; row_no < item_list_rect_.GetSize(); row_no++) {
			item_list_rect_[row_no].right = client_rect_.right;
		}
	}

	//selected_index_ = -1;

	ResetScrollBar();

	Invalidate(FALSE);
}

void ListCtrlEx::Destroy() {

	item_list_text_.RemoveAll();
	item_list_rect_.RemoveAll();
}

void ListCtrlEx::Refresh(CString str) {

	str.MakeLower();

	item_list_visible_.RemoveAll();
	item_list_rect_.RemoveAll();

	GetClientRect(client_rect_);

	CRect rect = client_rect_;
	int prev_row_no = -1;
	for (int row_no = 0; row_no < item_list_text_.GetSize(); row_no++) {

		CString item_text = item_list_text_[row_no];
		item_text.MakeLower();

		BOOL visible = TRUE;
		if (str != L"" && item_text.Find(str) == -1) {
			visible = FALSE;
		}

		if (prev_row_no != -1) {
			if (row_no != prev_row_no && visible) {
				rect.top += row_height_;
				rect.top += spacing_;
			}
		}
		rect.bottom = rect.top;
		if (visible) {
			rect.bottom += row_height_;
		}
		
		item_list_visible_.Add(visible);
		item_list_rect_.Add(rect);

		if (visible) {
			prev_row_no = row_no;
		}
	}

	document_height_ = rect.bottom;

	if (document_height_ > client_rect_.Height() /*If vertical scroll bar exist*/) {
		client_rect_.right -= GetSystemMetrics(SM_CXVSCROLL);
		for (int row_no = 0; row_no < item_list_rect_.GetSize(); row_no++) {
			item_list_rect_[row_no].right -= GetSystemMetrics(SM_CXVSCROLL);
		}
	} else {
		for (int row_no = 0; row_no < item_list_rect_.GetSize(); row_no++) {
			item_list_rect_[row_no].right = client_rect_.right;
		}
	}

	ResetScrollBar();

	Invalidate(FALSE);
}

void ListCtrlEx::DisableScrollbar(UINT sbar, BOOL disable) {

	if (sbar == SB_VERT) {
		if (client_rect_.Height() < document_height_) {
			SetScrollPos(SB_VERT, document_height_);
			Invalidate(FALSE);
		}
	}

	EnableScrollBar(sbar, disable ? ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);
}

void ListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	down_point_.SetPoint(-1, -1);
	for (int index = 0; index < int(item_list_rect_.GetSize()); index++) {
		if (item_list_rect_[index].PtInRect(point)) {
			down_point_ = point;
			selected_index_ = index;
			Invalidate(FALSE);
			break;
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void ListCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CStatic::OnMouseMove(nFlags, point);
}

void ListCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	for (int index = 0; index < int(item_list_rect_.GetSize()); index++) {
		if (item_list_rect_[index].PtInRect(down_point_)) {
			DDListEx* ddlistex = (DDListEx*)GetParent();
			ddlistex->GetParentWnd()->PostMessageW(WM_LISTCTRLEX_SELCHANGE, (WPARAM)ddlistex, (LPARAM)(selected_index_));
			ddlistex->ShowWindow(SW_HIDE);
			break;
		}
	}
	down_point_.SetPoint(-1, -1);

	CStatic::OnLButtonUp(nFlags, point);
}

void ListCtrlEx::OnTimer(UINT_PTR nIDEvent)
{
	CStatic::OnTimer(nIDEvent);
}

void ListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iScrollBarPos = GetScrollPos( SB_VERT );

	switch( nSBCode )
	{
		case SB_LINEUP:
			iScrollBarPos = std::max(iScrollBarPos - Formation::spacing(), 0);
		break;
		case SB_LINEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + Formation::spacing(), GetScrollLimit(SB_VERT) );
		break;
		case SB_PAGEUP:
			iScrollBarPos = std::max(iScrollBarPos - (client_rect_.Height()), 0);
		break;
		case SB_PAGEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + (client_rect_.Height()), GetScrollLimit(SB_VERT));
		break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			SCROLLINFO si;
			ZeroMemory( &si, sizeof(SCROLLINFO) );
			si.cbSize	= sizeof(SCROLLINFO);
			si.fMask	= SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS)) {
				iScrollBarPos = si.nTrackPos;
			} else {
				iScrollBarPos = (UINT)nPos;
			}
			break;
		}
	}		
	
	SetScrollPos(SB_VERT, iScrollBarPos);
	Invalidate(FALSE);
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT ListCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_NCHITTEST || message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK)
		return ::DefWindowProc(m_hWnd, message, wParam, lParam);
	
	return CStatic::WindowProc(message, wParam, lParam);
}

// DDListEx

IMPLEMENT_DYNAMIC(DDListEx, CDialogEx)

DDListEx::DDListEx(CWnd* pParent /*=NULL*/)
	: CDialogEx(DDListEx::IDD, pParent)
{
	back_color_ = BACKCOLOR1;
	search_text_ = L"";
}

DDListEx::~DDListEx()
{
	ctrList.Destroy();
}

void DDListEx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDLC_LIST, ctrList);
}

BEGIN_MESSAGE_MAP(DDListEx, CDialogEx)
	ON_MESSAGE(WM_KEYBOARD_EDIT_CHARCHANGE, OnEditCharChangeMessage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// DDListEx message handlers

int DDListEx::Open(CWnd* parent_wnd, CRect parent_rect, CString text, CStringArray& items_list, int selected_index, CFont* font, DWORD alignment) {

	items_list_.RemoveAll();
	for (int index = 0; index < int(items_list.GetSize()); index++) {
		items_list_.Add(items_list[index]);
	}

	parent_wnd_ = parent_wnd;
	parent_rect_ = parent_rect;
	text_ = text;
	selected_index_ = selected_index;
	font_ = font;
	alignment_ = alignment;

	int screen_width = GetSystemMetrics(SM_CXSCREEN); //Get the system metrics - VERT
	int screen_height = GetSystemMetrics(SM_CYSCREEN); //Get the system metrics - HORZ

	parent_rect_.top += parent_rect_.Height();
	int margin = screen_height - parent_rect_.top;
	int min_items_to_display = int(margin / Formation::control_height());
	if (min_items_to_display < 5) {
		min_items_to_display = 10;
	}

	int items_to_display = int(items_list_.GetSize()) < min_items_to_display ? int(items_list_.GetSize()) : min_items_to_display;
	GetClientRect(client_rect_);
	client_rect_.right = client_rect_.left + parent_rect_.Width();

	CRect control_rect = client_rect_;
	control_rect.bottom = control_rect.top + Formation::control_height();
	title_rect_ = control_rect;
	search_rect_ = control_rect;
	//close_rect_ = control_rect;
	//close_rect_.left = close_rect_.right - Formation::control_height();
	//search_rect_.right = close_rect_.left;
	search_rect_.left = search_rect_.right - Formation::control_height();
	control_rect.top = title_rect_.bottom + 1;// Formation::spacing();
	control_rect.bottom = control_rect.top + (items_to_display * Formation::control_height() + ((items_to_display - 1) * 1));
	CRect list_rect = control_rect;
	ctrList.SetFont(font_);
	ctrList.MoveWindow(list_rect);
	ctrList.SetBackColor(LABEL_COLOR3);
	ctrList.Create(items_list_, selected_index_, 1, alignment_);

	search_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_DDLISTEX_SEARCH), IMAGE_ICON, Formation::icon_size(Formation::SMALL_ICON), Formation::icon_size(Formation::SMALL_ICON), LR_DEFAULTCOLOR);
	search_icon_rect_.left = search_rect_.left + (search_rect_.Width() - Formation::icon_size(Formation::SMALL_ICON)) / 2;
	search_icon_rect_.right = search_icon_rect_.left + Formation::icon_size(Formation::SMALL_ICON);
	search_icon_rect_.top = search_rect_.top + (search_rect_.Height() - Formation::icon_size(Formation::SMALL_ICON)) / 2;
	search_icon_rect_.bottom = search_icon_rect_.top + Formation::icon_size(Formation::SMALL_ICON);
	/*close_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_DDLISTEX_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::BIG_ICON), Formation::icon_size(Formation::BIG_ICON), LR_DEFAULTCOLOR);
	close_icon_rect_.left = close_rect_.left + (close_rect_.Width() - Formation::icon_size(Formation::BIG_ICON)) / 2;
	close_icon_rect_.right = close_icon_rect_.left + Formation::icon_size(Formation::BIG_ICON);
	close_icon_rect_.top = close_rect_.top + (close_rect_.Height() - Formation::icon_size(Formation::BIG_ICON)) / 2;
	close_icon_rect_.bottom = close_icon_rect_.top + Formation::icon_size(Formation::BIG_ICON);*/

	if (items_to_display == int(items_list_.GetSize())) {
		search_rect_.SetRectEmpty();
		search_icon_rect_.SetRectEmpty();
	}

	client_rect_.bottom = control_rect.bottom + Formation::spacing();

	client_rect_.right += GetSystemMetrics(SM_CXDLGFRAME);
	client_rect_.bottom += GetSystemMetrics(SM_CYDLGFRAME);
	int window_width = client_rect_.Width();
	int window_height = client_rect_.Height();

	if ((parent_rect_.top + window_height) > screen_height) {
		parent_rect_.top = screen_height - window_height;
	}

	search_button_clicked_ = FALSE;
	//close_button_clicked_ = FALSE;

	SetWindowPos(&CWnd::wndTopMost, parent_rect_.left, parent_rect_.top - 1, window_width, window_height, SWP_SHOWWINDOW);

	return selected_index_;
}

BOOL DDListEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
	if (pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		CPoint p;
		GetCursorPos(&p);
		CRect r;
		GetWindowRect(&r);
		CRect t = r;
		t.bottom = t.top + Formation::heading_height();
		t.right -= (Formation::control_height() * 2);
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

BOOL DDListEx::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(WHITE_COLOR);
	
	ModifyStyle(WS_CAPTION, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT DDListEx::OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam) {

	KeyBoardEdit* keyboard_edit = (KeyBoardEdit*)wparam;
	if (keyboard_edit) {
		keyboard_edit->GetWindowText(search_text_);
		ctrList.Refresh(search_text_);
		InvalidateRect(title_rect_, FALSE);
	}

	return 0;
}

void DDListEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(WHITE_COLOR);

	offdc.SelectObject(font_);
	offdc.FillSolidRect(title_rect_, back_color_);
	//offdc.FillSolidRect(close_rect_, close_button_clicked_ ? BACKCOLOR1_SEL : back_color_);

	if (!search_rect_.IsRectEmpty()) {
		offdc.FillSolidRect(search_rect_, search_button_clicked_ ? BACKCOLOR1_SEL : back_color_);
		offdc.DrawState(search_icon_rect_.TopLeft(), CSize(search_icon_rect_.Width(), search_icon_rect_.Height()), search_icon_, DSS_NORMAL, (CBrush*)NULL);
		offdc.SelectObject(Formation::disablecolor_pen());
		offdc.MoveTo(search_rect_.right - 1, search_icon_rect_.top);
		offdc.LineTo(search_rect_.right - 1, search_icon_rect_.bottom);
	}

	//offdc.DrawState(close_icon_rect_.TopLeft(), CSize(close_icon_rect_.Width(), close_icon_rect_.Height()), close_icon_, DSS_NORMAL, (CBrush*)NULL);

	int cx = offdc.GetTextExtent(L" ").cx;
	CRect text_rect = title_rect_;
	text_rect.left += cx;
	offdc.DrawText(Formation::PrepareString(offdc, (search_text_.IsEmpty() ? text_ : search_text_), text_rect), text_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	if (!search_text_.IsEmpty()) {
		int cy = offdc.GetTextExtent(L" ").cy;
		text_rect.right = ((cx * 30) > (text_rect.Width() - cx)) ? (text_rect.Width() - cx) : (cx * 30);
		offdc.SelectObject(Formation::whitecolor_pen());
		offdc.MoveTo(text_rect.left, text_rect.bottom - (text_rect.Height() - cy) / 2);
		offdc.LineTo(text_rect.right, text_rect.bottom - (text_rect.Height() - cy) / 2);
	}
}

void DDListEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	search_button_clicked_ = FALSE;
	//close_button_clicked_ = FALSE;
	if (search_rect_.PtInRect(point)) {
		search_button_clicked_ = TRUE;
		SetCapture();
		InvalidateRect(search_rect_, FALSE);
	/*} else if (close_rect_.PtInRect(point)) {
		close_button_clicked_ = TRUE;
		SetCapture();
		InvalidateRect(close_rect_, FALSE);*/
	}
}

void DDListEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (search_button_clicked_) {

		KeyboardDlg keyboard_dlg;
		keyboard_dlg.SetKeyboardType(KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC);
		keyboard_dlg.SetLimitText(10);
		keyboard_dlg.SetParent(this);

		if (keyboard_dlg.OpenKeyboard(text_, search_text_, FALSE, FALSE, FALSE) == IDOK) {

			CString edit_text = keyboard_dlg.GetEditText();
		}

		InvalidateRect(search_rect_, FALSE);
		search_button_clicked_ = FALSE;
	}

	//if (close_button_clicked_) {
	//	close_button_clicked_ = FALSE;
	//	ShowWindow(SW_HIDE);
	//	//EndDialog(selected_index_);
	//}
}

void DDListEx::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	if (WA_INACTIVE == nState && IsWindowVisible() && !search_button_clicked_) {

		GetParentWnd()->PostMessageW(WM_LISTCTRLEX_SELCHANGE, (WPARAM)this, (LPARAM)(-1));
		ShowWindow(SW_HIDE);
	}
}

//DropDownListEx

IMPLEMENT_DYNAMIC(DropDownListEx, CStatic)

DropDownListEx::DropDownListEx()
{
	text_color_ = WHITE_COLOR;
	back_color_ = BACKCOLOR1;
	border_color_ = BACKCOLOR1;

	text_ = _T("");
	text_alignment_ = DT_RIGHT;
	alignment_ = DT_LEFT;

	selected_index_ = prev_selected_index_ = 0;
}

DropDownListEx::~DropDownListEx()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(DropDownListEx, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
	ON_MESSAGE(WM_LISTCTRLEX_SELCHANGE, OnListCtrlExSelChangeMessage)
END_MESSAGE_MAP()

// DropDownListEx message handlers

void DropDownListEx::Destroy() {

	items_list_.RemoveAll();
}

void DropDownListEx::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void DropDownListEx::Create(int text_area, DWORD alignment) {
		
	GetClientRect(client_rect_);

	alignment_ = alignment;

	text_rect_ = client_rect_;
	dropdown_rect_ = client_rect_;
	dropdown_rect_.left = text_rect_.right = (text_rect_.Width() * text_area / 100);

	points_for_open_arrow_[0].X = dropdown_rect_.right - dropdown_rect_.Height() * 60 / 100;
	points_for_open_arrow_[1].X = dropdown_rect_.right - dropdown_rect_.Height() * 40 / 100;
	points_for_open_arrow_[2].X = dropdown_rect_.right - dropdown_rect_.Height() * 20 / 100;
	points_for_open_arrow_[0].Y = points_for_open_arrow_[2].Y = dropdown_rect_.top + dropdown_rect_.Height() * 35 / 100;
	points_for_open_arrow_[1].Y = dropdown_rect_.bottom - dropdown_rect_.Height() * 35 / 100;
	points_for_close_arrow_[0].X = points_for_close_arrow_[2].X = dropdown_rect_.right - dropdown_rect_.Height() * 20 / 100;
	points_for_close_arrow_[1].X = dropdown_rect_.right - dropdown_rect_.Height() * 50 / 100;
	points_for_close_arrow_[0].Y = dropdown_rect_.top + dropdown_rect_.Height() * 30 / 100;
	points_for_close_arrow_[1].Y = dropdown_rect_.top + dropdown_rect_.Height() * 50 / 100;
	points_for_close_arrow_[2].Y = dropdown_rect_.top + dropdown_rect_.Height() * 70 / 100;

	items_list_.RemoveAll();

	down_point_.SetPoint(-1, -1);

	//selected_index_ = prev_selected_index_ = -1;

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	ddlistex_.Create(IDD_DDLISTEX_DLG);
	AfxSetResourceHandle(old_resource_handle);
}

void DropDownListEx::SetText(CString text) {

	text_ = text;

	InvalidateRect(text_rect_, FALSE);
}

void DropDownListEx::SetText(int index, CString text) {

	if (index < GetItemCount() && index >= 0) {
		items_list_.SetAt(index, text);
	}
}

CString DropDownListEx::GetText(int index) {

	if (index < GetItemCount() && index >= 0) {
		return items_list_.GetAt(index);
	}
	return L"";
}

void DropDownListEx::AddItem(CString str) {

	items_list_.Add(str);
}

void DropDownListEx::DeleteItem(int index) {

	items_list_.RemoveAt(index);
}

void DropDownListEx::ResetContents() {

	items_list_.RemoveAll();
}

void DropDownListEx::SetCurSel(int index) {

	prev_selected_index_ = selected_index_ = index;

	InvalidateRect(dropdown_rect_, FALSE);
}

int DropDownListEx::GetCurSel() {

	return selected_index_;
}

CString DropDownListEx::GetCurSelText() {

	if (selected_index_ >= 0 && selected_index_ < int(items_list_.GetSize())) {
		return items_list_[selected_index_];
	}
	return L"";
}

int DropDownListEx::FindString(CString str) {

	for (int index = 0; index < int(items_list_.GetSize()); index++) {
		if (items_list_.GetAt(index) == str) {
			return index;
		}
	}
	return -1;
}

void DropDownListEx::SetBackColor(COLORREF color) {

	back_color_ = color;
}

void DropDownListEx::SetTextColor(COLORREF color) {

	text_color_ = color;
}

void DropDownListEx::SetBorderColor(COLORREF color) {

	border_color_ = color;
}

void DropDownListEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SelectObject(GetFont());
	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(WHITE_COLOR);
	
	offdc.SelectObject(GetStockObject(NULL_BRUSH));
	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_pen());
		offdc.SetTextColor(BLACK_COLOR);
	} else {
		offdc.SetTextColor(DISABLE_COLOR);
		offdc.SelectObject(Formation::disablecolor_pen());
	}
	CString text = L" " + text_ + L" ";
	if (highlight_changed_value_ && selected_index_ != prev_selected_index_) {
		text = L"*" + text;
	}
	offdc.Rectangle(text_rect_);
	offdc.DrawText(Formation::PrepareString(offdc, text, text_rect_), text_rect_, text_alignment_ | DT_SINGLELINE | DT_VCENTER);

	if (IsWindowEnabled()) {
		offdc.FillSolidRect(dropdown_rect_, ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? BACKCOLOR1_SEL : BACKCOLOR1);
	} else {
		offdc.FillSolidRect(dropdown_rect_, DISABLE_COLOR);
	}
	CRect dd_text_rect(dropdown_rect_);
	dd_text_rect.right -= (Formation::control_height() - Formation::spacing3());
	CRect option_rect(dropdown_rect_);
	option_rect.left = dd_text_rect.right;

	if (selected_index_ >= 0 && selected_index_ < int(items_list_.GetSize())) {
		offdc.SetTextColor(WHITE_COLOR);
		offdc.DrawText(Formation::PrepareString(offdc, L" " + items_list_[selected_index_], dd_text_rect), dd_text_rect, alignment_ | DT_SINGLELINE | DT_VCENTER);
	}

	/*Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	if (down_point_.x == -1) {
		graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_open_arrow_, 3);
	} else {
		graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_close_arrow_, 3);
	}*/
	CPoint point1, point2;
	point1.x = option_rect.left + Formation::spacing2();
	point2.x = option_rect.right - Formation::spacing2();
	point1.y = option_rect.CenterPoint().y - Formation::spacing() - 1;
	point2.y = option_rect.CenterPoint().y - Formation::spacing();
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
	point1.y = option_rect.CenterPoint().y - 1;
	point2.y = option_rect.CenterPoint().y + 1;
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
	point1.y = option_rect.CenterPoint().y + Formation::spacing();
	point2.y = option_rect.CenterPoint().y + Formation::spacing() + 1;
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
}

void DropDownListEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (dropdown_rect_.PtInRect(point)) {
		down_point_ = point;
		Invalidate(FALSE);
		SetCapture();
	}
}

void DropDownListEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (dropdown_rect_.PtInRect(point)) {

		CRect wnd_rect = client_rect_;
		ClientToScreen(&wnd_rect);

		ddlistex_.Open(this, wnd_rect, text_, items_list_, selected_index_, GetFont(), alignment_);
	}
	down_point_.SetPoint(-1, -1);
}

void DropDownListEx::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}

LRESULT DropDownListEx::OnListCtrlExSelChangeMessage(WPARAM wparam, LPARAM lparam) {

	int selected_index = int(lparam);

	if (selected_index != -1) {
		selected_index_ = selected_index;
		GetParent()->SendMessage(WM_DROPDOWNLISTEX_SELCHANGE, (WPARAM)this, (LPARAM)selected_index_);
	}

	InvalidateRect(dropdown_rect_, FALSE);

	return 0;
}