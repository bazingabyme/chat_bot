#include "stdafx.h"

#define INCLUDE_TABCTRL __declspec(dllexport)
#include "tabctrl.h"

//TabCtrl

IMPLEMENT_DYNAMIC(TabCtrl, CStatic)

TabCtrl::TabCtrl()
{
	back_color_ = WHITE_COLOR;
	line_color_ = LABEL_COLOR;
	selection_color_ = BACKCOLOR1; //SCROLL_COLOR;
	text_color_ = BLACK_COLOR3;
	text_selection_color_ = BLACK_COLOR;
	button_style_ = FALSE;
	button_clicked_ = FALSE;
	auto_calculated_length_ = TRUE;
	tab_length_ = 0;
}

TabCtrl::~TabCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(TabCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// TabCtrl message handlers

void TabCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void TabCtrl::Create(TAB_ALIGNMENT tab_alignment, BOOL button_style) {

	button_style_ = button_style;
	tab_index_ = button_clicked_tab_index_ = 0;
	tab_alignment_ = tab_alignment;

	GetClientRect(client_rect_);
		
	if (tab_alignment_ == LEFT || tab_alignment_ == RIGHT) {
		LOGFONT lf = { 0 };
		Formation::font(Formation::MEDIUM_FONT).GetLogFont(&lf);
		lf.lfOrientation = -900;
		lf.lfEscapement = -900;
		font_.CreateFontIndirect(&lf);
	}
}

void TabCtrl::Destroy() {

	font_.DeleteObject();
	RemoveAllTabs();
}

void TabCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	int visible_tabs = 0;
	for (int i = 0; i < (int)tab_list_.GetSize(); i++) {
		if (tab_visible_[i]) {
			offdc.FillSolidRect(tab_rect_[i], back_color_);
			visible_tabs++;
		}
	}
	CPen pen(PS_SOLID, 1, line_color_);
	CPen* old_pen = offdc.SelectObject(&pen);
	if (tab_index_ < tab_rect_.GetSize()) {
		CRect button_clicked_tab_rect = tab_rect_[button_clicked_tab_index_];
		CRect tab_rect = tab_rect_[tab_index_];
		if (tab_alignment_ == LEFT) {
			offdc.MoveTo(client_rect_.right - 1, client_rect_.top);
			offdc.LineTo(client_rect_.right - 1, client_rect_.bottom);
			if (!button_style_) {
				button_clicked_tab_rect.left = button_clicked_tab_rect.right - Formation::spacing() - 1;
				tab_rect.left = tab_rect.right - Formation::spacing() - 1;
			}
		} else if (tab_alignment_ == RIGHT) {
			offdc.MoveTo(client_rect_.left, client_rect_.top);
			offdc.LineTo(client_rect_.left, client_rect_.bottom);
			if (!button_style_) {
				button_clicked_tab_rect.right = button_clicked_tab_rect.left + Formation::spacing() + 1;
				tab_rect.right = tab_rect.left + Formation::spacing() + 1;
			}
		} else if (tab_alignment_ == TOP) {
			offdc.MoveTo(client_rect_.left, client_rect_.bottom - 1);
			offdc.LineTo(client_rect_.right, client_rect_.bottom - 1);
			if (!button_style_) {
				button_clicked_tab_rect.top = button_clicked_tab_rect.bottom - Formation::spacing() - 1;
				tab_rect.top = tab_rect.bottom - Formation::spacing() - 1;
			}
		} else if (tab_alignment_ == BOTTOM) {
			offdc.MoveTo(client_rect_.left, client_rect_.top);
			offdc.LineTo(client_rect_.right, client_rect_.top);
			if (!button_style_) {
				button_clicked_tab_rect.bottom = button_clicked_tab_rect.top + Formation::spacing() + 1;
				tab_rect.bottom = tab_rect.top + Formation::spacing() + 1;
			}
		}

		if (!(back_color_ != WHITE_COLOR && visible_tabs == 1)) {
			offdc.FillSolidRect(tab_rect, selection_color_);
			if (button_clicked_) {
				offdc.FillSolidRect(button_clicked_tab_rect, BACKCOLOR1_SEL);
			}
		}
	}
	offdc.SelectObject(old_pen);
	pen.DeleteObject();

	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(text_color_);
	CFont* old_font = NULL;
	if (tab_alignment_ == LEFT || tab_alignment_ == RIGHT) {
		old_font = offdc.SelectObject(&font_);
	} else {
		old_font = offdc.SelectObject(GetFont());
	}
	int visible_tab_count = 0;
	for (int i = 0; i < (int)tab_list_.GetSize(); i++) {
		if (tab_visible_[i]) {
			tab_enable_[i] ? ((tab_index_ == i) ? offdc.SetTextColor(text_selection_color_) : offdc.SetTextColor(text_color_)) : offdc.SetTextColor(DISABLE_COLOR);
			CString text = (tab_data_changed_[i]) ? tab_list_[i] + L"*" : tab_list_[i];
			text = (back_color_ == selection_color_ && tab_index_ == i && (int)tab_list_.GetSize() > 1) ? L"● " + text : text; //► ▼ ▲ • ●
			if (tab_alignment_ == LEFT || tab_alignment_ == RIGHT) {
				if (!text.IsEmpty()) {
					CSize text_size = offdc.GetTextExtent(text); //Text width and height
					while (text_size.cx > tab_rect_[i].Height() - 1) {
						text = text.Mid(0, text.GetLength() - 1);
						if (text.IsEmpty()) {
							break;
						}
						text_size = offdc.GetTextExtent(text);
					}
				}
				CSize text_size = offdc.GetTextExtent(text); //Text width and height
				int x = text_size.cy + (tab_rect_[i].Width() - text_size.cy) / 2; //Text height + (rectangle width - text height) / 2
				if (!button_style_ && back_color_ != selection_color_ && (int)tab_list_.GetSize() > 1) {
					x = (tab_alignment_ == RIGHT) ? x + Formation::spacingHalf() : x - Formation::spacingHalf();
				}
				int y = tab_rect_[i].top + (tab_rect_[i].Height() - text_size.cx) / 2; //Top position + (rectangle height - text width) / 2
				offdc.TextOut(x, y, text);
			} else {
				CRect text_rect = tab_rect_[i];
				if (!button_style_ && back_color_ != selection_color_ && (int)tab_list_.GetSize() > 1) {
					text_rect.top = (tab_alignment_ == TOP) ? text_rect.top - Formation::spacingHalf() : text_rect.top + Formation::spacingHalf();
					text_rect.bottom = text_rect.top + tab_rect_[i].Height();
				}
				offdc.DrawText(Formation::PrepareString(offdc, text, text_rect), text_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
			++visible_tab_count;
		}
	}
	offdc.SelectObject(old_font);

	if(button_style_) {
		offdc.SelectObject(Formation::disablecolor_pen());
		for (int i = 1; i < (int)tab_list_.GetSize(); i++) {
			if(i != tab_index_ && i != tab_index_ + 1) {
				if (tab_visible_[i]) {
					if (tab_alignment_ == LEFT || tab_alignment_ == RIGHT) {
						offdc.MoveTo(tab_rect_[i].left + Formation::spacing(), tab_rect_[i].top);
						offdc.LineTo(tab_rect_[i].right - Formation::spacing(), tab_rect_[i].top);
					} else {
						offdc.MoveTo(tab_rect_[i].left, tab_rect_[i].top + Formation::spacing());
						offdc.LineTo(tab_rect_[i].left, tab_rect_[i].bottom - Formation::spacing());
					}
				}
			}
		}
	}
}

void TabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	button_clicked_ = FALSE;
	for (int index = 0; index < tab_rect_.GetSize(); index++) {
		if (tab_enable_[index] && tab_rect_[index].PtInRect(point)) {
			if (tab_index_ != index) {
				button_clicked_tab_index_ = index;
				button_clicked_ = TRUE;
				InvalidateRect(client_rect_);

				SetCapture();
			}
			break;
		}
	}
}

void TabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (button_clicked_) {
		button_clicked_ = FALSE;
		int prev_tab_index = tab_index_;
		tab_index_ = button_clicked_tab_index_;
		InvalidateRect(client_rect_);
		GetParent()->SendMessageW(WM_TABCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(prev_tab_index));
	}
}

void TabCtrl::SetTabLength(int tab_length) {

	if (tab_length != -1) {
		auto_calculated_length_ = FALSE;
		tab_length_ = tab_length;
	} else {
		auto_calculated_length_ = TRUE;
	}
	RefreshTabs();
}

void TabCtrl::RefreshTabs() {

	tab_rect_.RemoveAll();
	int item_count = (int)tab_list_.GetCount();
	
	int tab_length = tab_length_;
	int total_items = item_count;
	for (int i = 0; i < item_count; i++) {
		if (!tab_visible_[i]) {
			--total_items;
		}
	}
	if (total_items > 0) {
		int calc_tab_length = 0;
		if (tab_alignment_ == TOP || tab_alignment_ == BOTTOM) {
			calc_tab_length = client_rect_.Width() / total_items;
		} else {
			calc_tab_length = client_rect_.Height() / total_items;
		}
		if (auto_calculated_length_) {
			tab_length = calc_tab_length;
		} else {
			if (tab_length_ > calc_tab_length) {
				tab_length = calc_tab_length;
			}
		}
	}

	int offset = 0;
	for (int i = 0; i < item_count; i++) {
		if (tab_visible_[i]) {
			if (i == item_count - 1) {
				if (tab_alignment_ == TOP || tab_alignment_ == BOTTOM) {
					if (auto_calculated_length_) {
						tab_rect_.Add(CRect(offset, 0, client_rect_.right, Formation::control_height()));
					} else {
						tab_rect_.Add(CRect(offset, 0, offset + tab_length, Formation::control_height()));
					}
				} else {
					if (auto_calculated_length_) {
						tab_rect_.Add(CRect(client_rect_.right - Formation::control_height(), offset, client_rect_.right, client_rect_.bottom));
					} else {
						tab_rect_.Add(CRect(client_rect_.right - Formation::control_height(), offset, client_rect_.right, offset + tab_length));
					}
				}
			} else {
				if (tab_alignment_ == TOP || tab_alignment_ == BOTTOM) {
					tab_rect_.Add(CRect(offset, 0, offset + tab_length, Formation::control_height()));
				} else {
					tab_rect_.Add(CRect(client_rect_.right - Formation::control_height(), offset, client_rect_.right, offset + tab_length));
				}
			}
			offset += tab_length;
		} else {
			tab_rect_.Add(CRect());
		}
	}

	InvalidateRect(client_rect_, FALSE);
}

void TabCtrl::AddTab(CString text) {

	tab_list_.Add(text);
	tab_enable_.Add(TRUE);
	tab_visible_.Add(TRUE);
	tab_data_changed_.Add(FALSE);
	tab_data_.Add(NULL);

	tab_index_ = button_clicked_tab_index_ = 0;

	RefreshTabs();
}

void TabCtrl::SelectTab(int tab_index, BOOL update) {

	//if (tab_index != tab_index_) {
		if (tab_index < tab_list_.GetSize() && tab_enable_[tab_index] && tab_visible_[tab_index]) {
			int prev_tab_index = tab_index_;
			tab_index_ = tab_index;
			InvalidateRect(client_rect_, FALSE);
			if (update) {
				GetParent()->SendMessageW(WM_TABCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(prev_tab_index));
			}
		}
	//}
}

void TabCtrl::SetText(int tab_index, CString text) {

	if (tab_index < tab_list_.GetSize()) {
		tab_list_.SetAt(tab_index, text);
		InvalidateRect(client_rect_, FALSE);
	}
}

void TabCtrl::SetTabData(int tab_index, LPVOID lpvoid) {

	if (tab_index < tab_list_.GetSize()) {
		tab_data_.SetAt(tab_index, lpvoid);
	}
}

LPVOID TabCtrl::GetTabData(int tab_index) {

	if (tab_index < tab_list_.GetSize()) {
		return tab_data_.GetAt(tab_index);
	}
	return NULL;
}

void TabCtrl::EnableTab(int tab_index, BOOL enable) {

	if (tab_index < tab_list_.GetSize() && tab_visible_[tab_index]) {
		tab_enable_.SetAt(tab_index, enable);
		InvalidateRect(client_rect_, FALSE);
	}
}

BOOL TabCtrl::IsEnableTab(int tab_index) {

	if (tab_index < tab_list_.GetSize()) {
		return tab_enable_[tab_index];
	}
	return FALSE;
}

void TabCtrl::VisibleTab(int tab_index, BOOL visible) {

	if (tab_index < tab_list_.GetSize()) {
		if (visible != tab_visible_.GetAt(tab_index)) {
			tab_visible_.SetAt(tab_index, visible);
			if (!visible && tab_index_ == tab_index) {
				tab_index_ = button_clicked_tab_index_ = 0;
			}
			RefreshTabs();
		}
	}
}

BOOL TabCtrl::IsVisibleTab(int tab_index) {

	if (tab_index < tab_list_.GetSize()) {
		return tab_visible_[tab_index];
	}
	return FALSE;
}

void TabCtrl::RemoveTab(int tab_index) {

	if (tab_index < tab_list_.GetSize()) {
		tab_list_.RemoveAt(tab_index);
		tab_rect_.RemoveAt(tab_index);
		tab_enable_.RemoveAt(tab_index);
		tab_visible_.RemoveAt(tab_index);
	}
}

void TabCtrl::RemoveAllTabs() {

	tab_index_ = button_clicked_tab_index_ = 0;
	tab_list_.RemoveAll();
	tab_rect_.RemoveAll();
	tab_enable_.RemoveAll();
	tab_visible_.RemoveAll();
	tab_data_changed_.RemoveAll();
	tab_data_.RemoveAll();
}

void TabCtrl::OnEnable(BOOL bEnable)
{
	for (int i = 0; i < tab_enable_.GetSize(); i++) {
		tab_enable_[i] = bEnable;
	}
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}

void TabCtrl::TabDataChanged(int tab_index, BOOL changed) {

	if (tab_index < tab_list_.GetSize()) {
		tab_data_changed_[tab_index] = changed;
		InvalidateRect(client_rect_, FALSE);
	}
}

BOOL TabCtrl::IsTabDataChanged(int tab_index) {

	if (tab_index < tab_list_.GetSize()) {
		return tab_data_changed_[tab_index];
	}
	return FALSE;
}