// spinctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_SPINCTRL __declspec(dllexport)
#include "spinctrl.h"

// SpinCtrl

IMPLEMENT_DYNAMIC(SpinCtrl, CStatic)

SpinCtrl::SpinCtrl()
{
	alignment_ = DT_LEFT;
	client_rect_.SetRectEmpty();

	text_ = _T("");
	value_changed_ = FALSE;
	highlight_changed_value_ = FALSE;

	double_precesion_ = FALSE;
}

SpinCtrl::~SpinCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(SpinCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

void SpinCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

BOOL SpinCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd() && pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	if (pMsg->hwnd == ctrEdit.GetSafeHwnd() && pMsg->message == WM_LBUTTONDOWN) {
				
		ctrEdit.SetKeyBoardCaptionBarText(text_);
		ctrEdit.OpenKeyBoard();
		CString edit_text;
		ctrEdit.GetWindowTextW(edit_text);
		
		timer_executed_ = 1;
		value_changed_ = TRUE;
		if (use_floating_) {
			SetPositionF(_wtof(edit_text), FALSE);
			if (f_prev_pos_ == f_cur_pos_) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
				value_changed_ = FALSE;
			}
			GetParent()->SendMessageW(WM_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPosF());
		} else {
			SetPosition(_wtoi(edit_text), FALSE);
			if (prev_pos_ == cur_pos_) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
				value_changed_ = FALSE;
			}
			GetParent()->SendMessageW(WM_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPos());
		}
		timer_executed_ = 0;

		InvalidateRect(spin_rect_[0], FALSE); //To update arrow color
		InvalidateRect(spin_rect_[1], FALSE); //To update arrow color

		//TO UPDATE COLORS OF TOP AND BOTTOM SIDES OF EDIT PORTION
		InvalidateRect(edit_rect_topedge_, FALSE);
		InvalidateRect(edit_rect_bottomedge_, FALSE);
		
		//TO UPDATE COLOR OF EDIT CONTROL OR RESET L"*" IN TEXT
		if (value_changed_ && highlight_changed_value_) {
			text_.IsEmpty() ? ctrEdit.SetBackcolor(BACKCOLOR2) : InvalidateRect(text_rect_, FALSE); //To update text
		} else {
			text_.IsEmpty() ? ctrEdit.SetBackcolor(BACKCOLOR1) : InvalidateRect(text_rect_, FALSE); //To update text
		}

		return TRUE;
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void SpinCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	
	COfflineDC offdc(&dc, &client_rect_);

	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	offdc.SelectObject(GetStockObject(NULL_BRUSH));

	Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_pen());
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
	}
	offdc.Rectangle(text_rect_);
	offdc.SetTextColor(IsWindowEnabled() ? BLACK_COLOR : DISABLE_COLOR);
	CRect temp_text_rect(text_rect_);
	temp_text_rect.right -= Formation::spacing();
	if (show_balloon_) { //To prevent text and balloon value overlapping
		temp_text_rect.right -= (Formation::control_height() + Formation::spacing());
	}
	CString text = text_;
	if (value_changed_ && highlight_changed_value_ && !text.IsEmpty()) {
		text.Trim();
		if (alignment_ == DT_LEFT) {
			text = L" *" + text;
		} else {
			text = L"*" + text + L" ";
		}
	}
	offdc.DrawText(Formation::PrepareString(offdc, text, temp_text_rect), temp_text_rect, alignment_ | DT_SINGLELINE | DT_VCENTER);

	//Draw left and right text
	offdc.DrawText(left_text_, left_text_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	offdc.DrawText(right_text_, right_text_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	//Draw ellipse and text to represent balloon data 
	if (show_balloon_) {
		CString str;
		//if (IsWindowEnabled()) {
			if (use_floating_) {
				double_precesion_ ? str.Format(L"%.2f", f_balloon_data_) : str.Format(L"%.1f", f_balloon_data_);
			} else {
				str.Format(L"%d", balloon_data_);
			}
			offdc.DrawText(str, balloon_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//}
		//if (IsWindowEnabled()) {
			graphics.DrawEllipse(&Gdiplus::Pen(Gdiplus::Color(GetRValue(balloon_color_), GetGValue(balloon_color_), GetBValue(balloon_color_)), 1), balloon_rect_.left, balloon_rect_.top, balloon_rect_.Width(), balloon_rect_.Height()); \
		//}
	}

	//Draw outer lines <=>
	if (IsWindowEnabled()) {
		graphics.DrawPolygon(Formation::spancolor1_pen_gdi(), point_for_spin_btn_, 6);
	} else {
		graphics.DrawPolygon(Formation::disablecolor_pen_gdi(), point_for_spin_btn_, 6);
	}
	
	//Draw left and right arrow near to edit control
	for (int i = 0; i < 2; i++)	{
		if (IsWindowEnabled()) {
			bool update = true;
			if (use_floating_) {
				if ((i == 0 && f_cur_pos_ == f_min_pos_) || (i == 1 && f_cur_pos_ == f_max_pos_)) {
					graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow_[i], 3);
					update = false;
				}
			} else {
				if ((i == 0 && cur_pos_ == min_pos_) || (i == 1 && cur_pos_ == max_pos_)) {
					graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow_[i], 3);
					update = false;
				}
			}
			if (update) {
				if (button_down_[i]) {
					graphics.FillPolygon(Formation::spancolor1_brush_for_selection_gdi(), point_for_arrow_[i], 3);
				} else {
					graphics.FillPolygon(Formation::spancolor1_brush_gdi(), point_for_arrow_[i], 3);
				}
			}
		} else {
			graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow_[i], 3);
		}
	}

	//Draw above and below sides of edit control
	if (IsWindowEnabled()) {
		if (value_changed_ && highlight_changed_value_ && text_.IsEmpty()) {
			offdc.SelectObject(Formation::spancolor2_pen());
			offdc.SelectObject(Formation::spancolor2_brush());
		} else {
			offdc.SelectObject(Formation::spancolor1_pen());
			offdc.SelectObject(Formation::spancolor1_brush());
		}
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
		offdc.SelectObject(Formation::disablecolor_brush());
	}
	offdc.Rectangle(edit_rect_topedge_);
	offdc.Rectangle(edit_rect_bottomedge_);
}

void SpinCtrl::Destroy() {

	client_rect_.SetRectEmpty();

	ctrEdit.DestroyWindow();
}

void SpinCtrl::Create(BOOL highlight_changed_value, int text_area) {

	GetClientRect(client_rect_);

	highlight_changed_value_ = highlight_changed_value;

	balloon_color_ = ACCEPT_COLOR;
	show_balloon_ = FALSE;
	use_floating_ = FALSE;
	double_precesion_ = FALSE;
	value_changed_ = FALSE;
	button_down_[0] = button_down_[1] = FALSE;
	timer_executed_ = 0;
	spin_index_ = -1;
	min_pos_ = 0;
	max_pos_ = 255;
	cur_pos_ = 0;
	prev_pos_ = 0;
	balloon_data_ = 0;
	steps_[0] = 1;
	steps_[1] = -1;
	f_min_pos_ = 0.0;
	f_max_pos_ = 255.0;
	f_cur_pos_ = 0.0;
	f_prev_pos_ = 0.0;
	f_balloon_data_ = 0.0;
	f_steps_[0] = 0.1;
	f_steps_[1] = -0.1;

	CDC* dc = GetDC();
	CDC cdc;
	cdc.CreateCompatibleDC(dc);
	cdc.SelectObject(GetFont());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight + tm.tmExternalLeading;
	ReleaseDC(dc);
	DeleteDC(cdc);

	int text_width = client_rect_.Width() * text_area / 100;
	int remain_width = client_rect_.Width() - text_width;

	int edit_width = remain_width * 24 / 100;
	int spin_width = remain_width * 38 / 100;
	int balloon_width = text_width * 38 / 100;

	text_rect_.SetRect(0, client_rect_.top, text_width, client_rect_.bottom);
	balloon_rect_.SetRect(text_rect_.right - balloon_width, client_rect_.top, text_rect_.right, client_rect_.bottom);
	balloon_rect_.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
	spin_rect_[0].SetRect(text_rect_.right, client_rect_.top, text_rect_.right + spin_width, client_rect_.bottom);
	int offset = (client_rect_.Height() - font_height) / 2;
	edit_rect_.SetRect(spin_rect_[0].right, client_rect_.top + offset, spin_rect_[0].right + edit_width, client_rect_.bottom - offset);
	edit_rect_topedge_ = CRect(edit_rect_.left, client_rect_.top, edit_rect_.right, edit_rect_.top);
	edit_rect_bottomedge_ = CRect(edit_rect_.left, edit_rect_.bottom, edit_rect_.right, client_rect_.bottom);
	spin_rect_[1].SetRect(edit_rect_.right, client_rect_.top, edit_rect_.right + spin_width, client_rect_.bottom);

	ctrEdit.Create(WS_CHILD | WS_VISIBLE | ES_CENTER | ES_READONLY, edit_rect_, this, ID_UICONTROLS_EDIT);
	ctrEdit.SetFont(GetFont());
	ctrEdit.SetTextcolor(WHITE_COLOR);
	ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC);

	left_text_.Format(L"%d", min_pos_);
	left_text_rect_ = spin_rect_[0];
	left_text_rect_.DeflateRect(0, 0, spin_rect_[0].Height() * 30 / 100, 0);
	right_text_.Format(L"%d", max_pos_);
	right_text_rect_ = spin_rect_[1];
	right_text_rect_.DeflateRect(spin_rect_[1].Height() * 30 / 100, 0, 0, 0);

	point_for_spin_btn_[0].X = point_for_spin_btn_[2].X = spin_rect_[0].left + spin_rect_[0].Height() * 30 / 100;
	point_for_spin_btn_[1].X = spin_rect_[0].left;
	point_for_spin_btn_[3].X = point_for_spin_btn_[5].X = spin_rect_[1].right - spin_rect_[1].Height() * 30 / 100;
	point_for_spin_btn_[4].X = spin_rect_[1].right - 1;
	point_for_spin_btn_[0].Y = point_for_spin_btn_[5].Y = spin_rect_[0].bottom - 1;
	point_for_spin_btn_[2].Y = point_for_spin_btn_[3].Y = spin_rect_[0].top;
	point_for_spin_btn_[1].Y = point_for_spin_btn_[4].Y = spin_rect_[0].Height() / 2;

	point_for_arrow_[0][0].X = point_for_arrow_[0][2].X = spin_rect_[0].right - 1 - spin_rect_[0].Height() * 20 / 100;
	point_for_arrow_[0][1].X = spin_rect_[0].right - 1 - spin_rect_[0].Height() * 45 / 100;
	point_for_arrow_[0][0].Y = spin_rect_[0].top + spin_rect_[0].Height() * 20 / 100;
	point_for_arrow_[0][1].Y = spin_rect_[0].top + spin_rect_[0].Height() * 50 / 100;
	point_for_arrow_[0][2].Y = spin_rect_[0].top + spin_rect_[0].Height() * 80 / 100;

	point_for_arrow_[1][0].X = point_for_arrow_[1][2].X = spin_rect_[1].left + spin_rect_[0].Height() * 20 / 100;
	point_for_arrow_[1][1].X = spin_rect_[1].left + spin_rect_[1].Height() * 45 / 100;
	point_for_arrow_[1][0].Y = spin_rect_[1].top + spin_rect_[1].Height() * 20 / 100;
	point_for_arrow_[1][1].Y = spin_rect_[1].top + spin_rect_[1].Height() * 50 / 100;
	point_for_arrow_[1][2].Y = spin_rect_[1].top + spin_rect_[1].Height() * 80 / 100;
}

void SpinCtrl::SetText(CString text) {

	text_ = L" " + text + L" ";
}

void SpinCtrl::SetTextAlignment(DWORD align) {

	alignment_ = align;
}

void SpinCtrl::SetRange(int min, int max, int freq) {

	use_floating_ = FALSE;
	if (min < 0) {
		ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC);
	} else {
		ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_NUMERIC);
	}
	

	min_pos_ = min;
	max_pos_ = max;
	if (min > max) {
		max_pos_ = min;
		min_pos_ = max;
	}
	steps_[0] = freq;
	steps_[1] = -freq;

	left_text_.Format(L"%d", min_pos_);
	right_text_.Format(L"%d", max_pos_);

	InvalidateRect(left_text_rect_, FALSE);
	InvalidateRect(right_text_rect_, FALSE);
}

void SpinCtrl::SetRangeF(double min, double max, double freq, BOOL double_precesion) {

	use_floating_ = TRUE;
	if (min < 0) {
		ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC);
	} else {
		ctrEdit.SetKeyBoardType(Edit::KEYBOARD_TYPE::KB_FLOATING_NUMERIC);
	}

	double_precesion_ = double_precesion;

	f_min_pos_ = min;
	f_max_pos_ = max;
	if (min > max) {
		f_max_pos_ = min;
		f_min_pos_ = max;
	}
	f_steps_[0] = freq;
	f_steps_[1] = -freq;

	if (f_min_pos_ == 0) {
		left_text_ = L"0";
	} else {
		double_precesion_ ? left_text_.Format(L"%.2f", f_min_pos_) : left_text_.Format(L"%.1f", f_min_pos_);
	}

	int temp_max = (int)f_max_pos_;
	if (((f_max_pos_ * 100) / temp_max) == 100) {
		right_text_.Format(L"%.0f", f_max_pos_);
	} else {
		double_precesion_ ? right_text_.Format(L"%.2f", f_max_pos_) : right_text_.Format(L"%.1f", f_max_pos_);
	}

	InvalidateRect(left_text_rect_, FALSE);
	InvalidateRect(right_text_rect_, FALSE);
}

int SpinCtrl::GetPos() {

	return cur_pos_;
}

double SpinCtrl::GetPosF() {

	return f_cur_pos_;
}

void SpinCtrl::SetPosition(int pos, BOOL highlight) {

	if (pos < min_pos_) {
		pos = min_pos_;
	}
	if (pos > max_pos_) {
		pos = max_pos_;
	}

	cur_pos_ = pos;
	if (timer_executed_ == 0 && !highlight) {
		prev_pos_ = cur_pos_;
	}

	CString str;
	str.Format(L"%d", cur_pos_);
	ctrEdit.SetWindowTextW(str);
}

void SpinCtrl::SetPos(int pos, BOOL highlight) {

	SetPosition(pos, highlight);

	if (highlight) {
		if (prev_pos_ == cur_pos_) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
			value_changed_ = FALSE;
		} else {
			value_changed_ = TRUE;
		}
		InvalidateRect(text_rect_, FALSE);
	} else {
		ValueChanged();
	}
}

void SpinCtrl::SetPositionF(double pos, BOOL highlight) {

	if (pos < f_min_pos_) {
		pos = f_min_pos_;
	}
	if (pos > f_max_pos_) {
		pos = f_max_pos_;
	}
	
	f_cur_pos_ = round(pos * 100) / 100.0; //To set edit value unchanged when match with original value. It does not change if value is changed by keyboard bcoz of precision digits
	if (timer_executed_ == 0 && !highlight) {
		f_prev_pos_ = f_cur_pos_;
	}

	CString str;
	if (f_cur_pos_ == 0) {
		str.Format(L"%d", f_cur_pos_);
	} else {
		double_precesion_ ? str.Format(L"%.2f", f_cur_pos_) : str.Format(L"%.1f", f_cur_pos_);
	}
	ctrEdit.SetWindowTextW(str);
}

void SpinCtrl::SetPosF(double pos, BOOL highlight) {

	SetPositionF(pos, highlight);
	
	if (highlight) {
		CString str_prev, str_cur;
		double_precesion_ ? str_prev.Format(L"%.2f", f_prev_pos_) : str_prev.Format(L"%.1f", f_prev_pos_);
		double_precesion_ ? str_cur.Format(L"%.2f", f_cur_pos_) : str_cur.Format(L"%.1f", f_cur_pos_);
		if (str_prev == str_cur) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
			value_changed_ = FALSE;
		} else {
			value_changed_ = TRUE;
		}
		InvalidateRect(text_rect_, FALSE);
	} else {
		ValueChanged();
	} 
}

void SpinCtrl::SetBallonData(double data, COLORREF color) {

	if (data > f_max_pos_) {
		data = f_max_pos_;
	}
	if (data < f_min_pos_) {
		data = f_min_pos_;
	}

	balloon_color_ = color;
	f_balloon_data_ = data;

	ShowBalloon(TRUE);
}

void SpinCtrl::SetBallonData(int data, COLORREF color) {

	if (data > max_pos_) {
		data = max_pos_;
	}
	if (data < min_pos_) {
		data = min_pos_;
	}

	balloon_color_ = color;
	balloon_data_ = data;

	ShowBalloon(TRUE);
}

void SpinCtrl::ShowBalloon(BOOL show) {

	show_balloon_ = show;
	InvalidateRect(text_rect_, FALSE);
}

void SpinCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	spin_index_ = -1;
	if (spin_rect_[0].PtInRect(point)) {
		if (use_floating_) {
			if (f_cur_pos_ == f_min_pos_) {
				return;
			}
		} else {
			if (cur_pos_ == min_pos_) {
				return;
			}
		}
		spin_index_ = 0;
	} else if (spin_rect_[1].PtInRect(point)) {
		if (use_floating_) {
			if (f_cur_pos_ == f_max_pos_) {
				return;
			}
		} else {
			if (cur_pos_ == max_pos_) {
				return;
			}
		}
		spin_index_ = 1;
	} else {
		return;
	}

	button_down_[spin_index_] = TRUE;
	//TO UPDATE ARROW COLORS
	InvalidateRect(spin_rect_[spin_index_], FALSE);

	timer_executed_ = 0;
	SetTimer(spin_index_, 100, NULL);

	SetCapture();

	CStatic::OnLButtonDown(nFlags, point);
}

void SpinCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (spin_index_ == 0 || spin_index_ == 1) {

		button_down_[spin_index_] = FALSE;
		//TO UPDATE ARROW COLORS
		InvalidateRect(spin_rect_[spin_index_], FALSE);

		ReleaseCapture();
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void SpinCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (button_down_[spin_index_] || timer_executed_ == 0) {
		++timer_executed_;

		value_changed_ = TRUE;
		if (use_floating_) {
			SetPositionF(f_cur_pos_ - f_steps_[spin_index_], FALSE);
			CString str_prev, str_cur;
			double_precesion_ ? str_prev.Format(L"%.2f", f_prev_pos_) : str_prev.Format(L"%.1f", f_prev_pos_);
			double_precesion_ ? str_cur.Format(L"%.2f", f_cur_pos_) : str_cur.Format(L"%.1f", f_cur_pos_);
			if (str_prev == str_cur) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
				value_changed_ = FALSE;
			}
			GetParent()->SendMessageW(WM_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPosF());
		} else {
			SetPosition(cur_pos_ - steps_[spin_index_], FALSE);
			if (prev_pos_ == cur_pos_) { //TO IDENTIFY WHETHER VALUE GETS CHANGED TO UPDATE THE COLOR OF EDIT PORTION AND CONTROL
				value_changed_ = FALSE;
			}
			GetParent()->SendMessageW(WM_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPos());
		}
		
		InvalidateRect(spin_rect_[0], FALSE); //To update arrow color
		InvalidateRect(spin_rect_[1], FALSE); //To update arrow color

		//TO UPDATE COLORS OF TOP AND BOTTOM SIDES OF EDIT PORTION
		InvalidateRect(edit_rect_topedge_, FALSE);
		InvalidateRect(edit_rect_bottomedge_, FALSE);
		//TO UPDATE COLOR OF EDIT CONTROL OR RESET L"*" IN TEXT
		if (value_changed_ && highlight_changed_value_) {
			text_.IsEmpty() ? ctrEdit.SetBackcolor(BACKCOLOR2) : InvalidateRect(text_rect_, FALSE); //To update text
		} else {
			text_.IsEmpty() ? ctrEdit.SetBackcolor(BACKCOLOR1) : InvalidateRect(text_rect_, FALSE); //To update text
		}
	}
	if (!button_down_[spin_index_]) {

		KillTimer(spin_index_);
	}

	CStatic::OnTimer(nIDEvent);
}

void SpinCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	if (IsWindowEnabled()) {
		ctrEdit.SetBackcolor(BACKCOLOR1);
	} else {
		ctrEdit.SetBackcolor(DISABLE_COLOR);
	}
	ctrEdit.Invalidate(FALSE);

	/*if (highlight_changed_value_) {
		if (use_floating_) {
			SetPosF(f_prev_pos_);
		} else {
			SetPos(prev_pos_);
		}
	}*/
	//CStatic::OnEnable(bEnable);
}

void SpinCtrl::ValueChanged() {

	value_changed_ = FALSE;
	f_prev_pos_ = f_cur_pos_;
	prev_pos_ = cur_pos_;

	//TO UPDATE COLORS OF TOP AND BOTTOM SIDES OF EDIT PORTION
	InvalidateRect(edit_rect_topedge_, FALSE);
	InvalidateRect(edit_rect_bottomedge_, FALSE);

	InvalidateRect(spin_rect_[0], FALSE); //To update arrow color
	InvalidateRect(spin_rect_[1], FALSE); //To update arrow color

	InvalidateRect(text_rect_, FALSE); //To update text

	if (IsWindowEnabled()) {
		ctrEdit.SetBackcolor(BACKCOLOR1);
	} else {
		ctrEdit.SetBackcolor(DISABLE_COLOR);
	}
	ctrEdit.Invalidate(FALSE);
}