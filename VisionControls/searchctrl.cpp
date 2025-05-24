#include "stdafx.h"
#include "keyboard_dlg.h"

#define INCLUDE_SEARCHCTRL __declspec(dllexport)
#include "searchctrl.h"

//SearchCtrl

IMPLEMENT_DYNAMIC(SearchCtrl, CStatic)

SearchCtrl::SearchCtrl()
{
	backcolor_ = WHITE_COLOR;
	bordercolor_ = BACKCOLOR1;
	textcolor_ = BLACK_COLOR;

	search_icon_ = clear_icon_ = NULL;

	text_.Empty();
}

SearchCtrl::~SearchCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(SearchCtrl, CStatic)
	ON_MESSAGE(WM_KEYBOARD_EDIT_CHARCHANGE, OnKeyboardEditCharChangeMessage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// SearchCtrl message handlers

LRESULT SearchCtrl::OnKeyboardEditCharChangeMessage(WPARAM wparam, LPARAM lparam) {

	KeyBoardEdit* keyboard_edit = (KeyBoardEdit*)wparam;
	if (keyboard_edit) {
		keyboard_edit->GetWindowText(search_text_);
	}
	InvalidateRect(client_rect_, FALSE);

	GetParent()->SendMessageW(WM_SEARCH_EDIT_CHARCHANGE, (WPARAM)this);

	return 0;
}

void SearchCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void SearchCtrl::Create() {
		
	GetClientRect(client_rect_);
	
	CDC* dc = GetDC();
	CDC cdc;
	cdc.CreateCompatibleDC(dc);
	cdc.SelectObject(GetFont());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight + tm.tmExternalLeading;
	DeleteDC(cdc);
	ReleaseDC(dc);

	int offset = (client_rect_.Height() - font_height) / 2;
	search_rect_.SetRect(client_rect_.left + Formation::spacing(), client_rect_.top + offset, client_rect_.right - client_rect_.Height(), client_rect_.bottom - offset);
	search_rect_bottomedge_.SetRect(search_rect_.left, search_rect_.bottom, search_rect_.right, search_rect_.bottom + Formation::spacing());

	button_rect_ = client_rect_;
	button_rect_.left = search_rect_.right;

	search_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_DDLISTEX_SEARCH), IMAGE_ICON, Formation::icon_size(Formation::SMALL_ICON), Formation::icon_size(Formation::SMALL_ICON), LR_DEFAULTCOLOR);
	clear_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_DDLISTEX_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);

	search_text_.Empty();
	search_clicked_ = clear_clicked_ = FALSE;
}

void SearchCtrl::Destroy() {

	if (search_icon_ != NULL) {
		DestroyIcon(search_icon_);
		search_icon_ = NULL;
	}
	if (clear_icon_ != NULL) {
		DestroyIcon(clear_icon_);
		clear_icon_ = NULL;
	}
}

void SearchCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	
	//if (IsWindowEnabled()) {
		CBrush brush(search_clicked_ ? BACKCOLOR1_SEL : backcolor_);
		CPen pen(PS_SOLID, 1, bordercolor_);
		CBrush* old_brush = offdc.SelectObject(&brush);
		CPen* old_pen = offdc.SelectObject(&pen);
		offdc.Rectangle(client_rect_);
		offdc.SelectObject(old_brush);
		offdc.SelectObject(old_pen);
		brush.DeleteObject();
		pen.DeleteObject();
	//} else {
	//	offdc.SelectObject(Formation::disablecolor_pen());
	//	offdc.SelectObject(GetStockObject(NULL_BRUSH));
	//	offdc.Rectangle(client_rect_);
	//}

	int icon_size = search_text_.IsEmpty() ? Formation::icon_size(Formation::SMALL_ICON) : Formation::icon_size(Formation::MEDIUM_ICON);
	button_icon_rect_.left = button_rect_.left + (button_rect_.Width() - icon_size) / 2;
	button_icon_rect_.right = button_icon_rect_.left + icon_size;
	button_icon_rect_.top = button_rect_.top + (button_rect_.Height() - icon_size) / 2;
	button_icon_rect_.bottom = button_icon_rect_.top + icon_size;

	offdc.DrawState(button_icon_rect_.TopLeft(), CSize(button_icon_rect_.Width(), button_icon_rect_.Height()), search_text_.IsEmpty() ? search_icon_ : clear_icon_, IsWindowEnabled() ? DSS_NORMAL : DSS_DISABLED, (CBrush*)NULL);

	offdc.SetTextColor(IsWindowEnabled() ? textcolor_ : DISABLE_COLOR);
	offdc.DrawText(Formation::PrepareString(offdc, (!search_text_.IsEmpty() ? search_text_ : text_), search_rect_), search_rect_, (search_text_.IsEmpty() ?  DT_RIGHT : DT_LEFT) | DT_SINGLELINE | DT_VCENTER);
	if (!search_text_.IsEmpty()) {
		//int cx = offdc.GetTextExtent(L"W").cx;
		//int cy = offdc.GetTextExtent(L"W").cy;
		//search_rect_bottomedge_.right = ((cx * 30) > (search_rect_.Width() - cx)) ? (search_rect_.Width() - cx) : (cx * 30);
		//offdc.SelectObject(Formation::blackcolor_pen());
		offdc.SelectObject(Formation::blackcolor_pen());
		offdc.MoveTo(search_rect_bottomedge_.left, search_rect_bottomedge_.bottom);
		offdc.LineTo(search_rect_bottomedge_.right, search_rect_bottomedge_.bottom);
	}
}

CString SearchCtrl::GetSearchText() {

	return search_text_;
}

void SearchCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	search_clicked_ = clear_clicked_ = FALSE;
	
	if (!IsWindowEnabled()) {
		return;
	}

	if (search_rect_.PtInRect(point)) {

		search_clicked_ = TRUE;
		SetCapture();
		InvalidateRect(client_rect_, FALSE);

	} else if (button_rect_.PtInRect(point)) {

		search_text_.IsEmpty() ? search_clicked_ = TRUE : clear_clicked_ = TRUE;
		SetCapture();
		InvalidateRect(client_rect_, FALSE);
	}
}

void SearchCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (search_clicked_) {
				
		KeyboardDlg keyboard_dlg;
		keyboard_dlg.SetKeyboardType(KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC);
		keyboard_dlg.SetLimitText(10);
		keyboard_dlg.SetParent(this);

		if (keyboard_dlg.OpenKeyboard(text_, search_text_, FALSE, FALSE, FALSE) == IDOK) {

			search_text_ = keyboard_dlg.GetEditText();

			GetParent()->SendMessageW(WM_SEARCH_EDIT_CHARCHANGE, (WPARAM)this);
		}

		search_clicked_ = FALSE;
		InvalidateRect(client_rect_, FALSE);
	}

	if (clear_clicked_) {

		search_text_.Empty();

		GetParent()->SendMessageW(WM_SEARCH_EDIT_CHARCHANGE, (WPARAM)this);

		clear_clicked_ = FALSE;
		InvalidateRect(client_rect_, FALSE);
	}
}

void SearchCtrl::OnEnable(BOOL bEnable)
{
	if (!bEnable) {
		search_text_.Empty();
		clear_clicked_ = FALSE;
	}

	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}