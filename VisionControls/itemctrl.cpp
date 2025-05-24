// itemctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_ITEMCTRL __declspec(dllexport)
#include "itemctrl.h"

// ItemCtrl

IMPLEMENT_DYNAMIC(ItemCtrl, CStatic)

ItemCtrl::ItemCtrl()
{
	text_ = _T("");
	delete_icon_ = NULL;
}

ItemCtrl::~ItemCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ItemCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// ItemCtrl message handlers

void ItemCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void ItemCtrl::Destroy() {

	font_.DeleteObject();

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

void ItemCtrl::ShowAddAndDelete(BOOL add_and_delete) {

	ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
	iteminfo->visible = add_and_delete;
	delete_button_.visible = add_and_delete;
}

void ItemCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);

		//cdc_.BitBlt(client_rect_.left, client_rect_.top, client_rect_.Width(), client_rect_.Height(), NULL, 0, 0, WHITENESS);
	offdc.SelectObject(GetFont());

	//FOR CLIENT AREA
	offdc.SelectObject(GetStockObject(WHITE_BRUSH));
	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_pen());
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
	}
	offdc.Rectangle(client_rect_);

	//FOR CAPTION BAR
	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_brush());
	} else {
		offdc.SelectObject(Formation::disablecolor_brush());
	}
	offdc.Rectangle(caption_rect_);
	offdc.SetTextColor(WHITE_COLOR);
	offdc.DrawText(Formation::PrepareString(offdc, text_, caption_rect_), caption_rect_, alignment_ | DT_SINGLELINE | DT_VCENTER);

	//FOR DELETE BUTTON
	if (delete_button_.visible) {
		if (delete_button_.enable) {
			offdc.DrawState(delete_icon_rect_.TopLeft(), CSize(delete_icon_rect_.Width(), delete_icon_rect_.Height()), delete_icon_, DSS_NORMAL, (CBrush*)NULL);
		} else {
			offdc.DrawState(delete_icon_rect_.TopLeft(), CSize(delete_icon_rect_.Width(), delete_icon_rect_.Height()), delete_icon_, DSS_DISABLED, (CBrush*)NULL);
		}
	}

	CFont* old_font = NULL;
	//FOR ADD BUTTON
	if (item_info_list_.GetSize() > 0) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
		if (iteminfo->visible) {
			
			offdc.SelectObject(Formation::blackcolor_pen());
			offdc.SelectObject(GetStockObject(NULL_BRUSH));
			if (item_index_ == 0) { //For selected item
				old_font = offdc.SelectObject(&font_);
			}
			offdc.Rectangle(iteminfo->rect);
			offdc.SetTextColor(BLACK_COLOR);
			offdc.DrawText(iteminfo->text, iteminfo->rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}

	//FOR ALL ITEMS
	for (int i = 1; i < item_info_list_.GetSize(); i++) {

		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
		if (iteminfo->visible) {

			CPen pen(PS_SOLID, 2, iteminfo->bordercolor);
			CBrush brush(iteminfo->fillcolor);
			CPen* old_pen = offdc.SelectObject(&pen);
			CBrush* old_brush = offdc.SelectObject(&brush);

			if (item_index_ == i) { //For selected item
				old_font = offdc.SelectObject(&font_);
			} else {
				offdc.SelectObject(GetFont());
			}
			offdc.Rectangle(iteminfo->rect);
			offdc.SetTextColor(iteminfo->textcolor);
			offdc.DrawText(iteminfo->text, iteminfo->rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

			offdc.SelectObject(old_pen);
			offdc.SelectObject(old_brush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
	}
	offdc.SelectObject(old_font);
}

bool ItemCtrl::Create(int text_area, int alignment) {

	alignment_ = alignment;

	GetClientRect(client_rect_);

	int title_height = (client_rect_.Height() * 50 / 100);
	if (title_height > Formation::control_height()) {
		title_height = Formation::control_height();
	}
	int item_height = client_rect_.Height() - title_height;
	int caption_width = client_rect_.Width() * text_area / 100;
			
	caption_rect_.SetRect(client_rect_.left, 0, caption_width, client_rect_.bottom);
	//client_rect_.SetRect(caption_rect_.right, 0, client_rect_.right, client_rect_.bottom);

	item_info_list_.RemoveAll();
	int left = caption_rect_.right + Formation::spacing();
	int item_width = client_rect_.Width() / 2;
	if (item_width > Formation::control_height()) {
		item_width = Formation::control_height();
	}

	CRect add_button_rect;
	ItemInfo* iteminfo = new ItemInfo();
	if (iteminfo != NULL) {
		iteminfo->text = L"+";
		iteminfo->bordercolor = BACKCOLOR2;
		iteminfo->fillcolor = WHITE_COLOR;
		iteminfo->enable = TRUE;
		iteminfo->visible = TRUE;
		add_button_rect.SetRect(left, client_rect_.top + Formation::spacing(), left + item_width, client_rect_.bottom - Formation::spacing());
		iteminfo->rect = add_button_rect;
		item_info_list_.Add(iteminfo);
	}

	font_.DeleteObject();
	font_.CreateFontW(-(add_button_rect.Width() * 75 / 100), 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial Unicode MS"));

	delete_button_.bordercolor = BACKCOLOR1;
	delete_button_.fillcolor = BACKCOLOR1;
	delete_button_.enable = FALSE;
	delete_button_.visible = TRUE;
	delete_button_.rect.SetRect(client_rect_.right - item_width - Formation::spacing(), client_rect_.top + Formation::spacing(), client_rect_.right - Formation::spacing(), client_rect_.bottom - Formation::spacing());

	int icon_size = Formation::icon_size(Formation::MEDIUM_ICON);
	delete_icon_ = (HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_ITEMCTRL_DELETE), IMAGE_ICON, icon_size, icon_size, LR_DEFAULTCOLOR);
	delete_icon_rect_.left = delete_button_.rect.left + (delete_button_.rect.Width() - icon_size) / 2;
	delete_icon_rect_.right = delete_icon_rect_.left + icon_size;
	delete_icon_rect_.top = delete_button_.rect.top + (delete_button_.rect.Height() - icon_size) / 2;
	delete_icon_rect_.bottom = delete_icon_rect_.top + icon_size;

	item_index_ = 0;

	return true;
}

bool ItemCtrl::SetText(CString text) {

	text_ = text;

	if (alignment_ == DT_LEFT) {
		text_ = L"  " + text_;
	} else if (alignment_ == DT_RIGHT) {
		text_ = text_ + L"  ";
	}

	Invalidate();

	return true;
}

bool ItemCtrl::AddItem(COLORREF fillcolor, COLORREF bordercolor) {

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

		//ARRANGE ITEMS
		int left = caption_rect_.right + Formation::spacing();
		int item_width = (client_rect_.Width() - caption_rect_.Width() - Formation::spacing() - ((int)(item_info_list_.GetSize() + 1) * Formation::spacing())) / (int)(item_info_list_.GetSize() + 1);
		if (item_width > Formation::control_height()) {
			item_width = Formation::control_height();
		}
		for (int i = 1; i < item_info_list_.GetSize(); i++) { //For items
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
			iteminfo->rect.SetRect(left, client_rect_.top + Formation::spacing(), left + item_width, client_rect_.bottom - Formation::spacing());
			left += item_width + Formation::spacing();
		}
		if (item_info_list_.GetSize() > 0) { //For + button
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
			iteminfo->rect.SetRect(left, client_rect_.top + Formation::spacing(), left + item_width, client_rect_.bottom - Formation::spacing());
		}

		delete_button_.enable = TRUE;
		delete_button_.rect.SetRect(client_rect_.right - item_width - Formation::spacing(), client_rect_.top + Formation::spacing(), client_rect_.right - Formation::spacing(), client_rect_.bottom - Formation::spacing());

		item_index_ = (int)item_info_list_.GetSize() - 1;

		Invalidate();

		return true;
	}
	return false;
}

void ItemCtrl::SelectItem(int item_index, BOOL update) {

	if (item_index >= 0 && item_index < item_info_list_.GetSize()) {

		item_index_ = item_index;
		if (item_index_ == 0) {
			delete_button_.enable = FALSE;
		} else {
			delete_button_.enable = TRUE;
		}

		if (update) {
			GetParent()->SendMessageW(WM_ITEMCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(item_index_));
		}

		InvalidateRect(client_rect_);
	}
}

void ItemCtrl::ChangeColor(int item_index, COLORREF fillcolor) {

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

void ItemCtrl::ChangeBorderColor(int item_index, COLORREF bordercolor) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(item_index);
		iteminfo->bordercolor = bordercolor;

		InvalidateRect(client_rect_);
	}
}

void ItemCtrl::RemoveItem(int item_index) {

	if (item_index > 0 && item_index < item_info_list_.GetSize()) {

		ItemInfo* iteminfo_temp = (ItemInfo*)item_info_list_.GetAt(item_index);
		delete iteminfo_temp;
		item_info_list_.RemoveAt(item_index);

		//ARRANGE ITEMS
		int left = caption_rect_.right + Formation::spacing();
		int item_width = (client_rect_.Width() - caption_rect_.Width() - Formation::spacing() - ((int)(item_info_list_.GetSize() + 1) * Formation::spacing())) / (int)(item_info_list_.GetSize() + 1);
		if (item_width > Formation::control_height()) {
			item_width = Formation::control_height();
		}
		for (int i = 1; i < item_info_list_.GetSize(); i++) { //For items
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
			iteminfo->text.Format(L"%d", i);
			iteminfo->rect.SetRect(left, client_rect_.top + Formation::spacing(), left + item_width, client_rect_.bottom - Formation::spacing());
			left += item_width + Formation::spacing();
		}
		if (item_info_list_.GetSize() > 0) { //For + button
			ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(0);
			iteminfo->rect.SetRect(left, client_rect_.top + Formation::spacing(), left + item_width, client_rect_.bottom - Formation::spacing());
		}

		item_index_ = (int)item_info_list_.GetSize() - 1;
		if (item_index_ == 0) {
			delete_button_.enable = FALSE;
		}
		Invalidate();
	}
}

void ItemCtrl::RemoveAllItems() {

	int size = (int)item_info_list_.GetSize();
	for (int i = 1; i < size; i++) {
		RemoveItem(1);
	}
}

void ItemCtrl::SetItemData(int index, CString data) {

	if (index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		iteminfo->data = data;
	}
}

CString ItemCtrl::GetItemData(int index) {
	
	if (index < item_info_list_.GetSize()) {
		ItemInfo* iteminfo = (ItemInfo*)item_info_list_.GetAt(index);
		 return iteminfo->data;
	}
	return L"";
}

void ItemCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

void ItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {

	}

	CStatic::OnMouseMove(nFlags, point);
}

void ItemCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ItemInfo* iteminfo = NULL;

	if (delete_button_.visible && delete_button_.rect.PtInRect(point)) { //Click on x Delete
		if (item_index_ > 0) {

			int index = item_index_;
			RemoveItem(item_index_);
			GetParent()->SendMessageW(WM_ITEMCTRL_DELETE, (WPARAM)this, (LPARAM)(index));
		}

	} else { //Selection change

		for (int i = 0; i < (int)item_info_list_.GetSize(); i++) {
			iteminfo = (ItemInfo*)item_info_list_.GetAt(i);
			if (iteminfo->visible && iteminfo->rect.PtInRect(point)) {
				int index = item_index_;
				item_index_ = i;
				if (item_index_  == 0) {
					delete_button_.enable = FALSE;
				} else {
					delete_button_.enable = TRUE;
				}
				GetParent()->SendMessageW(WM_ITEMCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(index));
				InvalidateRect(client_rect_);
				break;
			}
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void ItemCtrl::OnEnable(BOOL bEnable)
{
	Invalidate();

	//CStatic::OnEnable(bEnable);
}