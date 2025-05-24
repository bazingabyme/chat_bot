// sliderctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_LISTCTRL __declspec(dllexport)
#include "listctrl.h"

// ListCtrl

IMPLEMENT_DYNAMIC(ListCtrl, CStatic)

ListCtrl::ListCtrl()
{
	allow_dragging_ = FALSE;
	list_item_height_ = 0;
	border_color_ = BACKCOLOR1;
	selection_color_ = BACKCOLOR1_SEL;
	selection_borderonly_ = TRUE;
	allow_selection_ = TRUE;
}

ListCtrl::~ListCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ListCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// ListCtrl message handlers

void ListCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

BOOL ListCtrl::PreTranslateMessage(MSG* pMsg)
{
	/*if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;*/

	return CStatic::PreTranslateMessage(pMsg);
}

void ListCtrl::Destroy() {

	drag_image_.DeleteImageList();

	allow_dragging_ = FALSE;

	for (int index = 0; index < items_list_.GetCount(); index++) {
		ItemInfo* item_info = items_list_.GetAt(index);
		delete item_info;
	}
	items_list_.RemoveAll();
}

void ListCtrl::Create(BOOL check_box_styled, BOOL grid_styled, BOOL border, COLORREF border_color) {

	check_box_styled_ = check_box_styled;
	grid_styled_ = grid_styled;
	border_color_ = border_color;

	GetClientRect(client_rect_);
	wnd_rect_ = client_rect_;
	wnd_rect_.DeflateRect(2, 2, 2, 2);
	//client_rect_.DeflateRect(2, 2, 2, 2);
	
	if (list_item_height_ <= 0) {
		list_item_height_ = Formation::control_height();
	}
	items_to_display_ = client_rect_.Height() / (list_item_height_);

	start_index_ = 0;
	selected_index_ = -1;
	point_down_.SetPoint(-1, -1);
	scroll_item_ = FALSE;
	drag_item_ = FALSE;

	border_ = border;

	scroll_section_ = 0;
	scroll_thumb_.SetRect(wnd_rect_.right - 2, 0, wnd_rect_.right, 0);
}

void ListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	int top = 0;
	int offset = list_item_height_ / 4;
	int grid_line_width = grid_styled_ ? 1 : 0;
	CRect button_rect(0, 0, 0, 0);
	if (check_box_styled_) {
		button_rect.SetRect(wnd_rect_.left, wnd_rect_.top, wnd_rect_.left + list_item_height_ - grid_line_width, wnd_rect_.top + list_item_height_ - grid_line_width);
	}
	CRect item_rect(button_rect.right + 1, wnd_rect_.top, wnd_rect_.right - Formation::spacing(), wnd_rect_.top + list_item_height_ - grid_line_width);

	for (int index = start_index_; index < (start_index_ + items_to_display_); index++, top++) {

		if(index >= (start_index_ + items_to_display_) - 3) {
			item_rect.bottom -= 1;
			button_rect.bottom -= 1;
		}

		if (index < items_list_.GetCount()) {
			ItemInfo* item_info = items_list_.GetAt(index);
			if (item_info != NULL) {

				if (check_box_styled_) {

					if (IsWindowEnabled() && item_info->enable) {
						CBrush brush(item_info->buttoncolor);
						CPen pen(PS_SOLID, 1, item_info->buttoncolor);
						CBrush* old_brush = offdc.SelectObject(&brush);
						CPen* old_pen = offdc.SelectObject(&pen);
						offdc.Rectangle(button_rect);
						offdc.SelectObject(old_brush);
						offdc.SelectObject(old_pen);
						brush.DeleteObject();
						pen.DeleteObject();
					} else {
						offdc.SelectObject(Formation::disablecolor_pen());
						offdc.SelectObject(Formation::disablecolor_brush());
						offdc.Rectangle(button_rect);
					}

					Gdiplus::Graphics graphics(offdc);
					graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
					CRect on_rect(button_rect.CenterPoint().x - offset, button_rect.CenterPoint().y - offset, button_rect.CenterPoint().x + offset, button_rect.CenterPoint().y + offset);
					graphics.DrawRectangle(Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
					if (item_info->check)  {
						CRect off_rect(on_rect.CenterPoint().x - offset / 2, on_rect.CenterPoint().y - offset / 2, on_rect.CenterPoint().x + offset / 2, on_rect.CenterPoint().y + offset / 2);
						graphics.FillRectangle(Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					}
				}

				if (allow_selection_ && index == selected_index_) {

					offdc.SetTextColor(item_info->textcolor);
					if (!IsWindowEnabled() || !item_info->enable) {
						offdc.SelectObject(Formation::disablecolor_pen());
						if (item_info->backcolor != WHITE_COLOR) {
							offdc.SelectObject(Formation::disablecolor_brush());
							offdc.SetTextColor(WHITE_COLOR);
						} else {
							offdc.SelectObject(GetStockObject(NULL_BRUSH));
							offdc.SetTextColor(DISABLE_COLOR);
						}
						offdc.Rectangle(item_rect);
					} else {
						CPen selection_pen(0, 1, selection_color_);
						CBrush selection_brush(selection_color_);
						CBrush brush(item_info->backcolor);
						CPen* old_selection_pen = offdc.SelectObject(&selection_pen);
						CBrush* old_selection_brush = NULL;
						if (!selection_borderonly_) {
							old_selection_brush = offdc.SelectObject(&selection_brush);
						} else {
							old_selection_brush = offdc.SelectObject(&brush);
						}
						offdc.Rectangle(item_rect);
						offdc.SelectObject(&old_selection_pen);
						offdc.SelectObject(&old_selection_brush);
						brush.DeleteObject();
						selection_pen.DeleteObject();
						selection_brush.DeleteObject();
					}

				} else {

					if (!IsWindowEnabled() || !item_info->enable) {
						offdc.SelectObject(GetStockObject(NULL_PEN));
						if (item_info->backcolor != WHITE_COLOR) {
							offdc.SelectObject(Formation::disablecolor_brush());
							offdc.SetTextColor(WHITE_COLOR);
						} else {
							offdc.SelectObject(GetStockObject(NULL_BRUSH));
							offdc.SetTextColor(DISABLE_COLOR);
						}
						offdc.Rectangle(item_rect);
					} else {
						offdc.SetTextColor(item_info->textcolor);
						CBrush brush(item_info->backcolor);
						CPen pen(PS_SOLID, 1, item_info->backcolor);
						CBrush* old_brush = offdc.SelectObject(&brush);
						CPen* old_pen = offdc.SelectObject(&pen);
						offdc.Rectangle(item_rect);
						offdc.SelectObject(old_brush);
						offdc.SelectObject(old_pen);
						pen.DeleteObject();
						brush.DeleteObject();
					}
				}
	
				CRect rect(item_rect.left + Formation::spacing(), item_rect.top, item_rect.right, item_rect.bottom);
				offdc.DrawText(Formation::PrepareString(offdc, item_info->text, rect), rect, item_info->textalignment | DT_SINGLELINE | DT_VCENTER);
			}
		}
		item_rect.top = item_rect.bottom + grid_line_width;
		item_rect.bottom = item_rect.top + list_item_height_ - grid_line_width;
		button_rect.top = button_rect.bottom + grid_line_width;
		button_rect.bottom = button_rect.top + list_item_height_ - grid_line_width;
	}
	if (!IsWindowEnabled()) {
		offdc.SelectObject(Formation::disablecolor_pen());
		offdc.Rectangle(scroll_thumb_);
	} else {
		CPen pen_slider(0, 2, SCROLL_COLOR);
		CPen* old_pen = offdc.SelectObject(&pen_slider);
		offdc.Rectangle(scroll_thumb_);
		offdc.SelectObject(&old_pen);
		pen_slider.DeleteObject();
	}

	if (border_) {
		offdc.SelectObject(GetStockObject(NULL_BRUSH));
		if (IsWindowEnabled()) {
			BOOL all_items_disabled = TRUE;
			for (int index = 0; index < items_list_.GetCount(); index++) {
				if (items_list_[index]->enable) {
					all_items_disabled = FALSE;
					break;
				}
			}
			if (all_items_disabled) {
				if (border_color_ != WHITE_COLOR) {
					offdc.SelectObject(Formation::disablecolor_pen());
					offdc.Rectangle(client_rect_);
				}
			} else {
				CPen pen_border(0, 1, border_color_);
				CPen* old_pen = offdc.SelectObject(&pen_border);
				offdc.Rectangle(client_rect_);
				offdc.SelectObject(&old_pen);
				pen_border.DeleteObject();
			}
		} else {
			if (border_color_ != WHITE_COLOR) {
				offdc.SelectObject(Formation::disablecolor_pen());
				offdc.Rectangle(client_rect_);
			}
		}
	}
}

void ListCtrl::SetBorderColor(COLORREF border_color) {

	border_color_ = border_color;
	Invalidate();
}

void ListCtrl::SetSelectionColor(COLORREF color, BOOL borderonly) {

	selection_borderonly_ = borderonly;
	selection_color_ = color;
}

void ListCtrl::SetCurSel(int index) {

	selected_index_ = index;
	if (index > 0) {
		if (index < items_to_display_) {
			start_index_ = 0;
		} else if(index > ((int)items_list_.GetSize() - items_to_display_)) {
			start_index_ = (int)items_list_.GetSize() - items_to_display_;
		} else {
			start_index_ = index;
		}
		SetScrollBarPos();
	}
	Invalidate();
}

void ListCtrl::AddItem(CString str, COLORREF backcolor, COLORREF textcolor, BOOL check, DWORD textalignment, BOOL header) {

	ItemInfo* item_info = new ItemInfo();

	item_info->text = str;
	item_info->backcolor = backcolor;
	item_info->textcolor = textcolor;
	item_info->textalignment = textalignment;
	item_info->header = header;

	if (check_box_styled_) {
		item_info->check = check;
	} else {
		item_info->check = TRUE;
	}

	items_list_.Add(item_info);
	
	if (items_list_.GetSize() > items_to_display_) {
		scroll_section_ = (float)((float)wnd_rect_.Height() / (int)items_list_.GetSize());
		int scroll_start = (int)((start_index_ * scroll_section_) + (wnd_rect_.top + 1));
		int grid_line_width = grid_styled_ ? 1 : 0;
		int scroll_end = (int)((scroll_section_ + grid_line_width) * items_to_display_);
		if (scroll_section_ > 0) {
			scroll_thumb_.SetRect(scroll_thumb_.left, scroll_start, scroll_thumb_.right, scroll_end);
		}
	} else {
		scroll_thumb_.SetRect(scroll_thumb_.left, 0, scroll_thumb_.right, 0);
	}
	Invalidate();
}

void ListCtrl::DeleteItem(int index) {

	start_index_ = 0;

	if (index < items_list_.GetSize() && index >= 0) {
		ItemInfo* item_info = items_list_.GetAt(index);
		delete item_info;
		items_list_.RemoveAt(index);
	}

	if (selected_index_ == index) {
		selected_index_ = -1;
	}
	
	if (items_list_.GetSize() > items_to_display_) {
		scroll_section_ = (float)((float)wnd_rect_.Height() / (int)items_list_.GetSize());
		int scroll_start = (int)((start_index_ * scroll_section_) + (wnd_rect_.top + 1));
		int grid_line_width = grid_styled_ ? 1 : 0;
		int scroll_end = (int)((scroll_section_ + grid_line_width) * items_to_display_);
		if (scroll_section_ > 0) {
			scroll_thumb_.SetRect(scroll_thumb_.left, scroll_start, scroll_thumb_.right, scroll_end);
		}
	} else {
		scroll_thumb_.SetRect(scroll_thumb_.left, 0, scroll_thumb_.right, 0);
	}

	Invalidate();
}

void ListCtrl::DeleteAllItems() {

	start_index_ = 0;

	for (int index = 0; index < items_list_.GetCount(); index++) {
		ItemInfo* item_info = items_list_.GetAt(index);
		delete item_info;
	}
	items_list_.RemoveAll();

	selected_index_ = -1;

	if (items_list_.GetSize() > items_to_display_) {
		scroll_section_ = (float)((float)wnd_rect_.Height() / (int)items_list_.GetSize());
		int scroll_start = (int)((start_index_ * scroll_section_) + (wnd_rect_.top + 1));
		int grid_line_width = grid_styled_ ? 1 : 0;
		int scroll_end = (int)((scroll_section_ + grid_line_width) * items_to_display_);
		if (scroll_section_ > 0) {
			scroll_thumb_.SetRect(scroll_thumb_.left, scroll_start, scroll_thumb_.right, scroll_end);
		}
	} else {
		scroll_thumb_.SetRect(scroll_thumb_.left, 0, scroll_thumb_.right, 0);
	}

	Invalidate();
}

CString ListCtrl::GetCurSelText() { 

	CString str;
	if (selected_index_ >= 0 && selected_index_ < items_list_.GetCount()) {
		str = items_list_.GetAt(selected_index_)->text;
	}
	return str;
}

CString ListCtrl::GetText(int index) {

	CString str;
	if (index < items_list_.GetCount()) {
		str = items_list_.GetAt(index)->text;
	}
	return str;
}

void ListCtrl::SetText(int index, CString text) {

	if (index < items_list_.GetSize() && index >= 0) {
		ItemInfo* item_info = items_list_.GetAt(index);
		item_info->text = text;
		Invalidate(FALSE);
	}
}

BOOL ListCtrl::IsItemHeader(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->header;
	}
	return FALSE;
}

BOOL ListCtrl::IsItemEnabled(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->enable;
	}
	return FALSE;
}

void ListCtrl::EnableItem(int index, BOOL enable) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->enable = enable;
	}
	Invalidate();
}

COLORREF ListCtrl::GetBackColor(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->backcolor;
	}
	return NULL;
}

void ListCtrl::SetBackColor(int index, COLORREF backcolor) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->backcolor = backcolor;
	}
	Invalidate();
}

COLORREF ListCtrl::GetButtonColor(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->buttoncolor;
	}
	return NULL;
}

void ListCtrl::SetButtonColor(int index, COLORREF buttoncolor) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->buttoncolor = buttoncolor;
	}
	Invalidate();
}

COLORREF ListCtrl::GetTextColor(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->textcolor;
	}
	return NULL;
}

void ListCtrl::SetTextColor(int index, COLORREF textcolor) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->textcolor = textcolor;
	}
	Invalidate();
}

void ListCtrl::SetCheck(int index, BOOL check) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->check = check;
	}
	Invalidate();
}

void ListCtrl::SetData(int index, BOOL data) {

	if (index < items_list_.GetCount()) {
		items_list_.GetAt(index)->data = data;
	}
	Invalidate();
}

BOOL ListCtrl::GetCheck(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->check;
	}
	return FALSE;
}

BOOL ListCtrl::GetData(int index) {

	if (index < items_list_.GetCount()) {
		return items_list_.GetAt(index)->data;
	}
	return FALSE;
}

void ListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	point_down_ = point;
	scroll_item_ = FALSE;
	drag_item_ = FALSE;
	point_up_.SetPoint(0, 0);

	SetCapture();
}

void ListCtrl::SetScrollBarPos() {

	scroll_thumb_.SetRect(scroll_thumb_.left, (int)(start_index_ * scroll_section_) + (wnd_rect_.top + 1), scroll_thumb_.right, (int)(start_index_ * scroll_section_) + (wnd_rect_.top + 1) + scroll_thumb_.Height());
	if (start_index_ == (items_list_.GetCount() - items_to_display_)) {
		scroll_thumb_.bottom = wnd_rect_.bottom;
	}
}

void ListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (drag_item_) {

		ClientToScreen(&point);
		point_up_ = point;

		drag_image_.DragLeave(NULL);
		drag_image_.EndDrag();
		drag_image_.DeleteImageList();

		GetParent()->SendMessage(WM_LISTCTRL_ITEMDRAGGED, (WPARAM)this, (LPARAM)0);

	} else {

		if (!scroll_item_ && allow_selection_) {
			int index = start_index_ + (point.y / list_item_height_);
			if (index >= 0 && index < (int)items_list_.GetCount()) {
				if ((index - start_index_) == items_to_display_) { //To show last clipped item
					++start_index_;
					SetScrollBarPos();
				}
				ItemInfo* item_info = items_list_.GetAt(index);
				if (item_info != NULL && item_info->enable && !item_info->header) {
					selected_index_ = index;
					if (point.x < list_item_height_) {
						if (check_box_styled_) {
							item_info->check = !item_info->check;
							GetParent()->SendMessageW(WM_LISTCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(index));
						}
					}
					GetParent()->SendMessageW(WM_LISTCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(selected_index_));
				}
			}
			Invalidate(FALSE);
		}
	}

	point_down_.SetPoint(-1, -1);
	scroll_item_ = FALSE;
	drag_item_ = FALSE;
}

void ListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) {

	if(!scroll_item_ && allow_selection_) {
		int index = start_index_ + (point.y / list_item_height_);
		if ((index - start_index_) == items_to_display_ && index < (int)items_list_.GetSize()) { //To show last clipped item
			++start_index_;
			SetScrollBarPos();
		}
		if (index < items_list_.GetCount()) {
			ItemInfo* item_info = items_list_.GetAt(index);
			if (item_info != NULL && item_info->enable && !item_info->header) {
				selected_index_ = index;
				if (point.x > list_item_height_) {
					GetParent()->SendMessageW(WM_LISTCTRL_DOUBLECLICKED, (WPARAM)this, (LPARAM)(selected_index_));
				}
			}
		}
		Invalidate(FALSE);
	}
}

void ListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON && (point_down_.x != -1 && point_down_.y != -1)) {
		
		if (!drag_item_) {
			
			if (items_list_.GetSize() > items_to_display_) {

				if ((point_down_.y - point.y) >= list_item_height_ / 2) {

					++start_index_;
					if (start_index_ > ((int)items_list_.GetSize() - items_to_display_)) start_index_ = (int)items_list_.GetSize() - items_to_display_;

					SetScrollBarPos();

					point_down_ = point;
					scroll_item_ = TRUE;
					Invalidate(FALSE);

				} else if ((point_down_.y - point.y) < -list_item_height_ / 2) {

					--start_index_;
					if (start_index_ < 0) start_index_ = 0;

					SetScrollBarPos();

					point_down_ = point;
					scroll_item_ = TRUE;
					Invalidate(FALSE);
				}
			}
		}

		if (!scroll_item_ && allow_dragging_) {

			if (!drag_item_) {

				if (abs(point_down_.x - point.x) > list_item_height_) {

					if (!scroll_item_ && allow_selection_) {
						int index = start_index_ + (point_down_.y / list_item_height_);
						if ((index - start_index_) == items_to_display_ && index < (int)items_list_.GetSize()) { //To show last clipped item
							++start_index_;
							SetScrollBarPos();
						}
						if (index < items_list_.GetCount()) {
							ItemInfo* item_info = items_list_.GetAt(index);
							if (item_info != NULL) {
								selected_index_ = index;
								GetParent()->SendMessageW(WM_LISTCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(selected_index_));
							}
						}
						Invalidate(FALSE);
					}

					drag_image_.Create(Formation::control_height(), Formation::control_height(), ILC_MASK | ILC_COLOR32, 0, 0);

					HICON icon_delete = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_LISTCTRL_CAMERA), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
					drag_image_.Add(icon_delete);
					//drag_image_.Add(&bitmap_, RGB(0, 0, 0));

					drag_image_.BeginDrag(0, CPoint(-Formation::spacing() * 3, -Formation::spacing()));

					ClientToScreen(&point);
					drag_image_.DragEnter(GetDesktopWindow(), point);

					drag_item_ = TRUE;
				}

			} else {

				ClientToScreen(&point);
				drag_image_.DragMove(point);
			}
		}
	}
}

void ListCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}