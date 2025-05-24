// tabitemctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_TABITEMCTRL __declspec(dllexport)
#include "tabitemctrl.h"

// TabItemCtrl

IMPLEMENT_DYNAMIC(TabItemCtrl, CStatic)

TabItemCtrl::TabItemCtrl()
{
	size_ = 0;
	text_ = _T("");
	add_text_ = _T("+");
	delete_icon_ = NULL;
	show_selection_bar_ = TRUE;
}

TabItemCtrl::~TabItemCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(TabItemCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// TabItemCtrl message handlers

void TabItemCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void TabItemCtrl::Destroy() {

	if (delete_icon_ != NULL) {
		DestroyIcon(delete_icon_);
		delete_icon_ = NULL;
	}

	for (int index = 0; index < item_info_list_.GetSize(); index++) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		delete iteminfo;
	}
	item_info_list_.RemoveAll();
}

void TabItemCtrl::ShowAddAndDelete(BOOL add_and_delete) {

	ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
	iteminfo->visible = add_and_delete;
	delete_button_.visible = add_and_delete;
}

void TabItemCtrl::ShowSelectionBar(BOOL show) {

	show_selection_bar_ = show;
}

void TabItemCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	offdc.SetTextColor(WHITE_COLOR);

	//FOR SELECTED ITEM
	if (show_selection_bar_ && item_index_ < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(item_index_);
		IsWindowEnabled() ? offdc.SelectObject(Formation::labelcolor_pen()) : offdc.SelectObject(Formation::disablecolor_pen());
		offdc.MoveTo(client_rect_.left, client_rect_.bottom - 1);
		offdc.LineTo(client_rect_.right, client_rect_.bottom - 1);
		CRect rect(iteminfo->rect.left, iteminfo->rect.bottom + 1, iteminfo->rect.right, client_rect_.bottom);
		IsWindowEnabled() ? offdc.FillSolidRect(rect, BACKCOLOR1) : offdc.FillSolidRect(rect, DISABLE_COLOR);
	}

	//FOR CAPTION BAR
	IsWindowEnabled() ? offdc.FillSolidRect(caption_rect_, LABEL_COLOR) : offdc.FillSolidRect(caption_rect_, DISABLE_COLOR);
	offdc.DrawText(Formation::PrepareString(offdc, text_, caption_rect_), caption_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	//FOR DELETE BUTTON
	if (delete_button_.visible) {
		offdc.SelectObject(Formation::disablecolor_pen()); //Separator
		offdc.MoveTo(delete_button_.rect.left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0));
		offdc.LineTo(delete_button_.rect.left, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing()) : client_rect_.bottom);
		if (delete_button_.enable && IsWindowEnabled()) {
			offdc.DrawState(delete_icon_rect_.TopLeft(), CSize(delete_icon_rect_.Width(), delete_icon_rect_.Height()), delete_icon_, DSS_NORMAL, (CBrush*)NULL);
		} else {
			offdc.DrawState(delete_icon_rect_.TopLeft(), CSize(delete_icon_rect_.Width(), delete_icon_rect_.Height()), delete_icon_, DSS_DISABLED, (CBrush*)NULL);
		}
	}

	//FOR ADD BUTTON
	if (item_info_list_.GetSize() > 0) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		if (iteminfo->visible) {
			COLORREF color = (IsWindowEnabled() && iteminfo->enable) ? (item_index_ == 0 ? BACKCOLOR1_SEL : BACKCOLOR1) : DISABLE_COLOR;
			offdc.FillSolidRect(iteminfo->rect, color);
			offdc.DrawText(Formation::PrepareString(offdc, add_text_, iteminfo->rect), iteminfo->rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}

	//FOR ALL ITEMS
	for (int index = 1; index < item_info_list_.GetSize(); index++) {

		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		if (iteminfo->visible) {

			CPen pen(PS_SOLID, 1, iteminfo->bordercolor);
			CBrush brush(iteminfo->fillcolor);
			CPen* old_pen = offdc.SelectObject(&pen);
			CBrush* old_brush = offdc.SelectObject(&brush);
			offdc.Rectangle(iteminfo->rect);
			offdc.SelectObject(old_pen);
			offdc.SelectObject(old_brush);
			pen.DeleteObject();
			brush.DeleteObject();

			(item_index_ == index) ? offdc.SelectObject(Formation::font(Formation::LARGE_FONT)) : offdc.SelectObject(GetFont());

			COLORREF color = (!IsWindowEnabled() || !iteminfo->enable) ? DISABLE_COLOR : iteminfo->textcolor;
			offdc.SetTextColor(color);
			offdc.DrawText(iteminfo->text, iteminfo->rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
}

bool TabItemCtrl::Create(BOOL title) {

	GetClientRect(client_rect_);

	int title_height = 0;
	if (title) {
		title_height = (client_rect_.Height() * 50 / 100);
		if (title_height > Formation::heading_height()) {
			title_height = Formation::heading_height();
		}
	}
	int item_height = client_rect_.Height() - title_height - Formation::spacing2();
	if (item_height > Formation::control_height()) {
		item_height = Formation::control_height();
	}
	caption_rect_.SetRect(client_rect_.left, client_rect_.top, client_rect_.right, title_height);

	item_info_list_.RemoveAll();
	int left = client_rect_.left;
	CRect add_button_rect;
	ItemInfo* iteminfo = new ItemInfo();
	if (iteminfo != NULL) {
		iteminfo->text = add_text_;
		iteminfo->bordercolor = BACKCOLOR2;
		iteminfo->fillcolor = WHITE_COLOR;
		iteminfo->enable = TRUE;
		iteminfo->visible = TRUE;
		add_button_rect.SetRect(left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), left + item_height + item_height + item_height / 2, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);
		iteminfo->rect = add_button_rect;
		item_info_list_.Add(iteminfo);
	}

	delete_button_.bordercolor = BACKCOLOR1;
	delete_button_.fillcolor = BACKCOLOR1;
	delete_button_.enable = FALSE;
	delete_button_.visible = TRUE;
	delete_button_.rect.SetRect(client_rect_.right - item_height, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), client_rect_.right, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);

	int icon_size = Formation::icon_size(Formation::MEDIUM_ICON);
	delete_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_ITEMCTRL_DELETE), IMAGE_ICON, icon_size, icon_size, LR_DEFAULTCOLOR);
	delete_icon_rect_.left = delete_button_.rect.left + (delete_button_.rect.Width() - icon_size) / 2;
	delete_icon_rect_.right = delete_icon_rect_.left + icon_size;
	delete_icon_rect_.top = delete_button_.rect.top + (delete_button_.rect.Height() - icon_size) / 2;
	delete_icon_rect_.bottom = delete_icon_rect_.top + icon_size;

	item_index_ = 0;
	size_ = 0;

	return true;
}

bool TabItemCtrl::SetText(CString text, CString add_text) {

	text_ = L" " + text;
	add_text_ = add_text;

	Invalidate();

	return true;
}

void TabItemCtrl::SetSize(int size) {

	size_ = size;
}

bool TabItemCtrl::AddItem(COLORREF fillcolor, COLORREF bordercolor) {

	ItemInfo* iteminfo = new ItemInfo();
	if (iteminfo != NULL) {

		iteminfo->text.Format(L"%d", item_info_list_.GetSize());
		iteminfo->bordercolor = bordercolor;
		iteminfo->fillcolor = fillcolor;
		
		if (GetRValue(iteminfo->fillcolor) > 127 && GetGValue(iteminfo->fillcolor) > 127 && GetBValue(iteminfo->fillcolor) > 127) {
			iteminfo->textcolor = BLACK_COLOR;
		} else {
			iteminfo->textcolor = WHITE_COLOR;
		}
		iteminfo->enable = TRUE;
		iteminfo->visible = TRUE;
		item_info_list_.Add(iteminfo);

		ItemInfo* add_iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		int add_button_width = add_iteminfo->rect.Width();
		
		//ARRANGE ITEMS
		int left = client_rect_.left;
		int item_width = (client_rect_.Width() - Formation::spacing() - ((int)item_info_list_.GetSize() * Formation::spacing()) - add_button_width) / (int)(item_info_list_.GetSize());
		if (item_width > Formation::control_height()) {
			item_width = Formation::control_height();
		}
		for (int i = 1; i < item_info_list_.GetSize(); i++) { //For items
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
			iteminfo->rect.SetRect(left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), left + item_width, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);
			left += item_width + Formation::spacing();
		}
		
		//For + button
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		iteminfo->rect.SetRect(left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), left + add_button_width, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);
		if (item_info_list_.GetSize() > size_ && size_ > 0) {
			iteminfo->enable = FALSE;
		} else {
			iteminfo->enable = TRUE;
		}
		
		delete_button_.enable = TRUE;
		//delete_button_.rect.SetRect(client_rect_.right - item_width, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), client_rect_.right, client_rect_.bottom - Formation::spacing());

		item_index_ = (int)item_info_list_.GetSize() - 1;

		Invalidate();

		return true;
	}
	return false;
}

void TabItemCtrl::SelectItem(int item_index, BOOL update) {

	if (item_index >= 0 && item_index < item_info_list_.GetSize()) {

		item_index_ = item_index;
		if (item_index_ == 0) {
			delete_button_.enable = FALSE;
		} else {
			delete_button_.enable = TRUE;
		}

		if (update) {
			GetParent()->SendMessageW(WM_TABITEMCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(item_index_));
		}

		InvalidateRect(client_rect_);
	}
}

COLORREF TabItemCtrl::GetColor(int item_index) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(item_index);
		return iteminfo->fillcolor;
	}

	return WHITE_COLOR;
}

void TabItemCtrl::ChangeColor(int item_index, COLORREF fillcolor) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(item_index);
		iteminfo->fillcolor = fillcolor;
		
		if (GetRValue(iteminfo->fillcolor) > 127 && GetGValue(iteminfo->fillcolor) > 127 && GetBValue(iteminfo->fillcolor) > 127) {
			iteminfo->textcolor = BLACK_COLOR;
		} else {
			iteminfo->textcolor = WHITE_COLOR;
		}

		InvalidateRect(client_rect_);
	}
}

void TabItemCtrl::ChangeBorderColor(int item_index, COLORREF bordercolor) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(item_index);
		iteminfo->bordercolor = bordercolor;

		InvalidateRect(client_rect_);
	}
}

void TabItemCtrl::RemoveItem(int item_index) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {

		ItemInfo* iteminfo_temp = (ItemInfo*)item_info_list_.GetAt(item_index);
		delete iteminfo_temp;
		item_info_list_.RemoveAt(item_index);

		ItemInfo* add_iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		int add_button_width = add_iteminfo->rect.Width();

		//ARRANGE ITEMS
		int left = client_rect_.left;
		int item_width = (client_rect_.Width() - Formation::spacing() - ((int)item_info_list_.GetSize() * Formation::spacing()) - add_button_width) / (int)(item_info_list_.GetSize());
		if (item_width > Formation::control_height()) {
			item_width = Formation::control_height();
		}
		for (int i = 1; i < item_info_list_.GetSize(); i++) { //For items
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
			iteminfo->text.Format(L"%d", i);
			iteminfo->rect.SetRect(left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), left + item_width, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);
			left += item_width + Formation::spacing();
		}
		
		//For + button
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		iteminfo->rect.SetRect(left, caption_rect_.bottom + (caption_rect_.bottom  > 0 ? Formation::spacing() : 0), left + add_button_width, show_selection_bar_ ? (client_rect_.bottom - Formation::spacing() - Formation::spacing() / 2) : client_rect_.bottom);
		if (item_info_list_.GetSize() > size_ && size_ > 0) {
			iteminfo->enable = FALSE;
		} else {
			iteminfo->enable = TRUE;
		}

		item_index_ = (int)item_info_list_.GetSize() - 1;
		if (item_index_ == 0) {
			delete_button_.enable = FALSE;
		}
		Invalidate();
	}
}

void TabItemCtrl::RemoveAllItems() {

	int size = (int)item_info_list_.GetSize();
	for (int i = 1; i < size; i++) {
		RemoveItem(1);
	}
}

void TabItemCtrl::SetItemData(int index, CString data) {

	if (index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		iteminfo->data = data;
	}
}

CString TabItemCtrl::GetItemData(int index) {
	
	if (index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		 return iteminfo->data;
	}
	return L"";
}

void TabItemCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

void TabItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {

	}

	CStatic::OnMouseMove(nFlags, point);
}

void TabItemCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ItemInfo* iteminfo = NULL;

	if (delete_button_.visible && delete_button_.rect.PtInRect(point)) { //Click on x Delete
		if (item_index_ > 0) {

			int index = item_index_;
			RemoveItem(item_index_);
			GetParent()->SendMessageW(WM_TABITEMCTRL_DELETE, (WPARAM)this, (LPARAM)(index));
		}

	} else { //Selection change

		for (int index = 0; index < (int)item_info_list_.GetSize(); index++) {
			iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
			if (iteminfo->enable && iteminfo->visible && iteminfo->rect.PtInRect(point)) {
				int prev_item_index = item_index_;
				item_index_ = index;
				if (item_index_  == 0) {
					delete_button_.enable = FALSE;
				} else {
					delete_button_.enable = TRUE;
				}
				GetParent()->SendMessageW(WM_TABITEMCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(prev_item_index));
				InvalidateRect(client_rect_);
				break;
			}
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void TabItemCtrl::OnEnable(BOOL bEnable)
{
	Invalidate();

	//CStatic::OnEnable(bEnable);
}