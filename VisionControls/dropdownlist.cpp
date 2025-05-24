// dropdownlist.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_DROPDOWNLIST __declspec(dllexport)
#include "dropdownlist.h"

//DDList

IMPLEMENT_DYNAMIC(DDList, CStatic)

DDList::DDList()
{
}

DDList::~DDList()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(DDList, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// DDList message handlers

void DDList::Destroy() {
	
	items_list_.RemoveAll();
}

void DDList::Initialize(CRect parent_rect) {

	parent_rect_ = parent_rect;

	GetClientRect(client_rect_);

	items_to_display_ = 0;
	top_index_ = 0;
	selected_index_ = 0;
	prev_selected_index_ = 0;
	point_down_.SetPoint(-1, -1);
	mouse_moved_ = FALSE;
}

void DDList::ResetPosition(int offset_x) {

	MoveWindow(parent_rect_.left, parent_rect_.top + parent_rect_.Height() + offset_x, parent_rect_.Width(), (items_to_display_ * parent_rect_.Height()));
}

void DDList::Update(int items) {

	items_to_display_ = items;
	MoveWindow(parent_rect_.left, parent_rect_.top + parent_rect_.Height(), parent_rect_.Width(), (items_to_display_ * parent_rect_.Height()));
	GetClientRect(client_rect_);
	
	if (items_list_.GetSize() > items_to_display_) {
		int scroll_section = (int)items_list_.GetSize() - (items_to_display_ / 2) * 2; //TO FOCUS ON CENTER INDEX, TOTAL VIEWING ITEMS ARE DEVIDED BY TWO, TO REMOVE IT FROM TOP AND BOTTOM, MULTIPLICATION OF 2 IS DONE.
		if (scroll_section == 0) {
			scroll_section = 1;
		}
		scroll_thumb_.SetRect(client_rect_.right - (parent_rect_.Height() * 10 / 100), 0, client_rect_.right - (parent_rect_.Height() * 10 / 100) + 2, (client_rect_.Height() - 4) / scroll_section);
	}
}

void DDList::SetCurSel(int index, BOOL highlight) {

	selected_index_ = index;
	top_index_ = index;
	if (top_index_ >= (int)items_list_.GetSize() - items_to_display_) {
		top_index_ = (int)items_list_.GetSize() - items_to_display_;
	}

	if (!highlight) {
		prev_selected_index_ = index;
	}
}

int DDList::FindString(CString str) {

	for (int i = 0; i < items_list_.GetCount(); i++) {
		if (items_list_.GetAt(i) == str) {
			return i;
		}
	}
	return -1;
}

CString DDList::GetCurSelText() { 

	if (selected_index_ >= 0 && selected_index_ < (int)items_list_.GetSize()) {
		return items_list_.GetAt(selected_index_);
	}
	return _T("");
}

void DDList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	offdc.SetTextColor(WHITE_COLOR);

	/*offdc.SelectObject(Formation::spancolor1_pen_for_selection());
	offdc.SelectObject(Formation::spancolor1_brush());
	offdc.Rectangle(client_rect_);*/
	offdc.FillSolidRect(client_rect_, BACKCOLOR1);

	int top = 0;
	for (int index = top_index_; index < top_index_ + items_to_display_; index++, top++) {
		CRect rect(2, (top * parent_rect_.Height()) + 2, client_rect_.Width() - (parent_rect_.Height() * 25 / 100), ((top * parent_rect_.Height()) + parent_rect_.Height()) - 2);
		if (index == selected_index_) {
			offdc.SelectObject(Formation::spancolor1_pen_for_selection());
			offdc.SelectObject(Formation::spancolor1_brush_for_selection());
			offdc.Rectangle(rect);
		} else {
			offdc.SelectObject(Formation::spancolor1_brush());
		}
		offdc.DrawText(Formation::PrepareString(offdc, L" " + items_list_.GetAt(index), rect), rect, DT_SINGLELINE | DT_VCENTER);
				
		CPen slider_pen(0, 2, SCROLL_COLOR);
		CPen* old_pen = offdc.SelectObject(&slider_pen);
		scroll_thumb_.MoveToY(top_index_ * scroll_thumb_.Height() + 2);
		offdc.Rectangle(scroll_thumb_);
		offdc.SelectObject(old_pen);
		slider_pen.DeleteObject();
	}
}

void DDList::LButtonDown(UINT nFlags, CPoint point)
{
	point_down_ = point;
	mouse_moved_ = FALSE;
}

void DDList::LButtonUp(UINT nFlags, CPoint point)
{
	if (!mouse_moved_) {
		int index = (point.y / parent_rect_.Height());
		selected_index_ = top_index_ + index;
		ShowWindow(SW_HIDE);
	}

	point_down_.SetPoint(-1, -1);
	mouse_moved_ = FALSE;
}

void DDList::MouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON && (point_down_.x != -1 || point_down_.y != -1) && (items_list_.GetSize() > 5)) {
		if ((point_down_.y - point.y) >= parent_rect_.Height()) {
			++top_index_;
			if (top_index_ > ((int)items_list_.GetSize() - 5)) top_index_ = (int)items_list_.GetSize() - 5;
			point_down_ = point;
			mouse_moved_ = TRUE;
			Invalidate(FALSE);
		} else if ((point_down_.y - point.y) < -parent_rect_.Height()) {
			--top_index_;
			if (top_index_ < 0) top_index_ = 0;
			point_down_ = point;
			mouse_moved_ = TRUE;
			Invalidate(FALSE);
		}
	}
}

// DropDownList

IMPLEMENT_DYNAMIC(DropDownList, CStatic)

DropDownList::DropDownList()
{
	list_window_rect_.SetRectEmpty();
	client_rect_.SetRectEmpty();
	text_rect_.SetRectEmpty();
	dropdown_rect_.SetRectEmpty();
	str_text_ = L"";
	highlight_changed_value_ = FALSE;
}

DropDownList::~DropDownList()
{
}

BEGIN_MESSAGE_MAP(DropDownList, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// DropDownList message handlers

void DropDownList::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void DropDownList::Destroy() {

	DDList_.DestroyWindow();
}

void DropDownList::Create(CWnd* parent, CRect parent_rect, int text_area, BOOL highlight_changed_value) {

	highlight_changed_value_ = highlight_changed_value;

	GetClientRect(client_rect_);
	text_rect_ = client_rect_;
	dropdown_rect_ = client_rect_;
	dropdown_rect_.left = text_rect_.right = (text_rect_.Width() * text_area / 100);
	
	items_to_display_ = 5;

	parent_rect.left = parent_rect.left + dropdown_rect_.left;
	DDList_.Create(_T("GroupBox"), WS_CHILD | SS_NOTIFY, CRect(parent_rect.left, parent_rect.bottom, parent_rect.right, parent_rect.bottom), parent);
	DDList_.SetFont(GetFont());
	DDList_.Initialize(parent_rect);

	//DRAW ARROW ACCESS OF MODEL MANAGE PAGE
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

	Invalidate(FALSE);
}

void DropDownList::SetText(CString text) {

	str_text_ = text + L" ";
}

void DropDownList::SetText(int index, CString text) {

	if (index < GetItemCount() && index >= 0) {
		DDList_.get_items_list().SetAt(index, text);
		DDList_.Invalidate(FALSE);
	}
}

CString DropDownList::GetText(int index) {

	if (index < GetItemCount() && index >= 0) {
		return DDList_.get_items_list().GetAt(index);
	}
	return L"";
}

void DropDownList::AddItem(CString str) {

	DDList_.get_items_list().Add(str);
	items_to_display_ = (int)(DDList_.get_items_list().GetSize() < 5) ? (int)DDList_.get_items_list().GetSize() : 5;

	if (items_to_display_ <= 5) {
		DDList_.Update(items_to_display_);

		DDList_.GetWindowRect(list_window_rect_);
		ScreenToClient(list_window_rect_);
	}
}

void DropDownList::DeleteItem(int index) {

	if (index < DDList_.get_items_list().GetSize() && index >= 0) {
		DDList_.get_items_list().RemoveAt(index);
	}
}

void DropDownList::ResetContents() {

	DDList_.get_items_list().RemoveAll();
}

void DropDownList::SetCurSel(int index, BOOL highlight) {

	if (index >= 0 && index < DDList_.get_items_list().GetSize()) {
		DDList_.SetCurSel(index, highlight);
	}
	InvalidateRect(dropdown_rect_);

	//TO RESET L"*" IN TEXT
	InvalidateRect(text_rect_, FALSE);
}

int DropDownList::GetCurSel() {

	return DDList_.GetCurSel();
}

void DropDownList::OnPaint()
{
	CPaintDC dc(this); // device context for painting}

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
	CString text = str_text_;
	if (highlight_changed_value_ && DDList_.GetCurSel() != DDList_.GetPrevSel()) {
		text = L"*" + text;
	}
	offdc.Rectangle(text_rect_);
	offdc.DrawText(Formation::PrepareString(offdc, text, text_rect_), text_rect_, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	if (DDList_.GetSafeHwnd()) {

		if (IsWindowEnabled()) {
			offdc.SelectObject(Formation::spancolor1_brush());
			if (DDList_.IsWindowVisible()) {
				offdc.SelectObject(Formation::spancolor1_pen_for_selection());
				offdc.SelectObject(Formation::spancolor1_brush_for_selection());
			}
		} else {
			offdc.SelectObject(Formation::disablecolor_brush());
		}
		offdc.Rectangle(dropdown_rect_);

		if (DDList_.GetCurSel() != -1 && DDList_.GetCurSel() < DDList_.get_items_list().GetSize()) {
			offdc.SetTextColor(WHITE_COLOR);
			CRect rect(dropdown_rect_);
			rect.DeflateRect(0, 0, (dropdown_rect_.Height() * 75 / 100), 0); //REMOVE ARRROW PART
			offdc.DrawText(Formation::PrepareString(offdc, L" " + DDList_.get_items_list().GetAt(DDList_.GetCurSel()), rect), rect, DT_SINGLELINE | DT_VCENTER);
		}

		Gdiplus::Graphics graphics(offdc);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		if (DDList_.IsWindowVisible()) {
			graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_close_arrow_, 3);
		} else {
			graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_open_arrow_, 3);
		}
	}
}

void DropDownList::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (DDList_.IsWindowVisible()) {
		if (list_window_rect_.PtInRect(point)) {
			point.Offset(0, -client_rect_.Height());
			DDList_.LButtonDown(nFlags, point);
		} else {
			InvalidateRect(dropdown_rect_);
			DDList_.ShowWindow(SW_HIDE);
		}
	} else {
		if (dropdown_rect_.PtInRect(point)) {
			InvalidateRect(dropdown_rect_);
			DDList_.ShowWindow(SW_SHOW);
			SetCapture();
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void DropDownList::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (list_window_rect_.PtInRect(point)) {
		point.Offset(0, -client_rect_.Height());
		DDList_.LButtonUp(nFlags, point);
		InvalidateRect(dropdown_rect_);
		if (!DDList_.IsWindowVisible()) {
			GetParent()->SendMessageW(WM_DROPDOWNLIST_SELCHANGE, (WPARAM)this, (LPARAM)DDList_.GetCurSel());
			if (highlight_changed_value_) {
				InvalidateRect(text_rect_, FALSE);
			}
		}
	}
	if (!DDList_.IsWindowVisible()) {
		ReleaseCapture();
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void DropDownList::OnMouseMove(UINT nFlags, CPoint point)
{
	if (list_window_rect_.PtInRect(point)) {
		point.Offset(0, -client_rect_.Height());
		DDList_.MouseMove(nFlags, point);
	}

	CStatic::OnMouseMove(nFlags, point);
}

void DropDownList::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}