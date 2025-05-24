#include "stdafx.h"

#define INCLUDE_EDITCTRL __declspec(dllexport)
#include "editctrl.h"

//EditCtrl

IMPLEMENT_DYNAMIC(EditCtrl, CStatic)

EditCtrl::EditCtrl()
{
	multiline_ = FALSE;
	text_ = _T("");
	text_align_[0] = DT_RIGHT | DT_SINGLELINE | DT_VCENTER;
	text_align_[1] = DT_LEFT | DT_SINGLELINE | DT_VCENTER;

	label_backcolor_ = WHITE_COLOR;
	label_bordercolor_ = BACKCOLOR1;
	edit_backcolor_ = BACKCOLOR1;
	edit_textcolor_ = WHITE_COLOR;
}

EditCtrl::~EditCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(EditCtrl, CStatic)
	ON_MESSAGE(WM_EDIT_CHARCHANGE, OnEditCharChangeMessage)
	ON_MESSAGE(WM_EDIT_ENTERPRESSED, OnEditEnterPressedMessage)
	ON_WM_PAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// EditCtrl message handlers

LRESULT EditCtrl::OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam) {

	Edit* edit = (Edit*)wparam;

	GetParent()->SendMessageW(WM_EDIT_CHARCHANGE, (WPARAM)this);

	return 0;
}

LRESULT EditCtrl::OnEditEnterPressedMessage(WPARAM wparam, LPARAM lparam) {

	Edit* edit = (Edit*)wparam;

	GetParent()->SendMessageW(WM_EDIT_ENTERPRESSED, (WPARAM)this, lparam);

	return 0;
}

void EditCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void EditCtrl::Create(int text_area, BOOL password_style, BOOL show_keyboard, BOOL multiline, int text_limit) {
		
	GetClientRect(client_rect_);
	
	multiline_ = multiline;

	CDC* dc = GetDC();
	CDC cdc;
	cdc.CreateCompatibleDC(dc);
	cdc.SelectObject(GetFont());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight + tm.tmExternalLeading;
	DeleteDC(cdc);
	ReleaseDC(dc);

	int text_width = client_rect_.Width() * text_area / 100;
	if (text_width < Formation::spacing()) {
		text_width = Formation::spacing();
	}
	int edit_width = client_rect_.right - text_width - Formation::spacing();

	text_rect_.SetRect(Formation::spacing(), client_rect_.top, text_width, client_rect_.bottom);
	int offset = Formation::spacing();
	if (!multiline) {
		offset = (client_rect_.Height() - font_height) / 2;
	}
	edit_rect_.SetRect(text_rect_.right, client_rect_.top + offset, text_rect_.right + edit_width, client_rect_.bottom - offset);

	edit_rect_topedge_.SetRect(edit_rect_.left, Formation::spacing(), edit_rect_.right, edit_rect_.top);
	edit_rect_bottomedge_.SetRect(edit_rect_.left, edit_rect_.bottom, edit_rect_.right, text_rect_.bottom - Formation::spacing());

	//DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT;
	DWORD style = WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_LEFT;
	if (password_style) {
		style |= ES_PASSWORD;
	}
	if (multiline) {
		style |= ES_MULTILINE;
	} else {
		style |= ES_AUTOHSCROLL;
	}
	ctrEdit.Create(style, edit_rect_, this, ID_UICONTROLS_EDIT);
	ctrEdit.SetFont(GetFont());
	ctrEdit.SetBackcolor(edit_backcolor_);
	ctrEdit.SetTextcolor(edit_textcolor_);
	ctrEdit.SetLimitText(text_limit);
	ctrEdit.ShowKeyboard(show_keyboard);
	if (show_keyboard) {
		ctrEdit.SetKeyBoardCaptionBarText(text_);
		ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_ALPHANUMERIC);
	}
	if (password_style) {
		ctrEdit.SetPasswordChar(0x25CF);
	}
}

void EditCtrl::Destroy() {

	ctrEdit.DestroyWindow();
}

void EditCtrl::SetText(CString text) {

	text_ = text;// +L" ";
	ctrEdit.SetKeyBoardCaptionBarText(text_);

	InvalidateRect(text_rect_, FALSE);
}

void EditCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	
	if (IsWindowEnabled()) {
		CBrush brush(label_backcolor_);
		CPen pen(PS_SOLID, 1, label_bordercolor_);
		CBrush* old_brush = offdc.SelectObject(&brush);
		CPen* old_pen = offdc.SelectObject(&pen);
		offdc.Rectangle(client_rect_);
		offdc.SelectObject(old_brush);
		offdc.SelectObject(old_pen);
		brush.DeleteObject();
		pen.DeleteObject();
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
		offdc.SelectObject(GetStockObject(NULL_BRUSH));
		offdc.Rectangle(client_rect_);
	}

	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_pen());
		offdc.SelectObject(Formation::spancolor1_brush());
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
		offdc.SelectObject(Formation::disablecolor_brush());
	}
	offdc.Rectangle(edit_rect_topedge_);
	offdc.Rectangle(edit_rect_bottomedge_);
	
	offdc.SetTextColor(IsWindowEnabled()? BLACK_COLOR : DISABLE_COLOR);
	CRect text_rect(text_rect_);
	text_rect.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());

	offdc.DrawText(Formation::PrepareString(offdc, text_, text_rect), text_rect, text_align_[0]);
}

void EditCtrl::SetTextAlignment(DWORD align, BOOL rightside) {

	text_align_[rightside] = align;
	if (rightside && ctrEdit.GetSafeHwnd() != NULL) {
		ctrEdit.ModifyStyle(0, align);
	}
}

void EditCtrl::SetKeyBoardType(Edit::KEYBOARD_TYPE keyboard_type) {
	
	ctrEdit.SetKeyBoardType(keyboard_type);
}

void EditCtrl::ShowMultilingualKeyboard(BOOL show) {

	ctrEdit.ShowMultilingualKeyboard(show);
}

void EditCtrl::SetHolder(CString holder) {

	ctrEdit.SetHolder(holder);
}

void EditCtrl::SetFocus() {

	ctrEdit.SetFocus();
}

void EditCtrl::SetEditBackColor(COLORREF edit_backcolor) {

	edit_backcolor_ = edit_backcolor;
}

void EditCtrl::SetEditTextColor(COLORREF edit_textcolor) { 

	edit_textcolor_ = edit_textcolor;
}

void EditCtrl::SetTextLimit(int limit) {

	ctrEdit.SetLimitText(limit);
}

void EditCtrl::SetReadOnly(BOOL readonly) {

	if (readonly) {
		ctrEdit.SetTextcolor(multiline_ ? BLACK_COLOR3 : DISABLE_COLOR);
	} else {
		ctrEdit.SetTextcolor(WHITE_COLOR);
	}
	
	ctrEdit.SendMessage(EM_SETREADONLY, readonly, 0);
}

BOOL EditCtrl::IsReadOnly() {

	if ((ctrEdit.GetStyle() & ES_READONLY) == ES_READONLY) {
		return TRUE;
	}
	return FALSE;
}

CString EditCtrl::GetEditText() {

	CString str;
	ctrEdit.GetWindowTextW(str);

	return str;
}

LONG EditCtrl::GetEditValue() {

	CString str = GetEditText();

	return _wtol(str);
}

void EditCtrl::SetEditText(CString text) {

	ctrEdit.SetWindowTextW(text);
	ctrEdit.SetSel(-1, text.GetLength());
}

void EditCtrl::SetEditValue(LONG value) {

	CString text;
	text.Format(L"%d", value);

	SetEditText(text);
}

void EditCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	if (bEnable) {
		ctrEdit.SetBackcolor(BACKCOLOR1);
	} else {
		ctrEdit.SetBackcolor(DISABLE_COLOR);
	}
	
	//CStatic::OnEnable(bEnable);
}

