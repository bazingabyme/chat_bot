// sliderctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_SLIDERCTRL __declspec(dllexport)
#include "sliderctrl.h"

// SliderCtrl

IMPLEMENT_DYNAMIC(SliderCtrl, CStatic)

SliderCtrl::SliderCtrl()
{
	text_ = _T("");
	value_changed_ = FALSE;
	highlight_changed_value_ = FALSE;
}

SliderCtrl::~SliderCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(SliderCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// SliderCtrl message handlers

void SliderCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void SliderCtrl::Destroy() {

}

void SliderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_pen());
	} else {
		offdc.SelectObject(Formation::disablecolor_pen());
	}
	offdc.SelectObject(GetStockObject(NULL_BRUSH));
	offdc.Rectangle(client_rect_);

	if (IsWindowEnabled()) {
		offdc.SelectObject(Formation::spancolor1_brush());
	} else {
		offdc.SelectObject(Formation::disablecolor_brush());
	}

	if (!text_.IsEmpty()) {
		offdc.Rectangle(caption_rect_);
		offdc.SetTextColor(WHITE_COLOR);
		CString text = text_;
		if (value_changed_ && highlight_changed_value_) {
			text = L"*" + text;
		}
		offdc.DrawText(Formation::PrepareString(offdc, L" " + text, caption_rect_), caption_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	IsWindowEnabled() ? offdc.SetTextColor(BLACK_COLOR) : offdc.SetTextColor(DISABLE_COLOR);
	CString str;
	if (use_floating_) {
		str.Format(L"%.1f ", f_min_pos_);
	} else {
		str.Format(L"%d ", min_pos_);
	}
	offdc.DrawText(str, CRect(0, client_rect_.top, bar_rect_.left, (bar_rect_.bottom + (bar_rect_.top - client_rect_.top))), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (use_floating_) {
		str.Format(L"%.1f ", f_max_pos_);
	} else {
		str.Format(L"%d ", max_pos_);
	}
	offdc.DrawText(str, CRect(bar_rect_.right, client_rect_.top, client_rect_.right, (bar_rect_.bottom + (bar_rect_.top - client_rect_.top))), DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	if (gradient_color_ == RGB(0, 0, 0)) {
		
		offdc.Rectangle(bar_rect_);
		if (IsWindowEnabled()) {
			offdc.SelectObject(Formation::spancolor2_brush());
			offdc.SelectObject(Formation::spancolor2_pen());
		}
		offdc.Rectangle(thumb_rect_[1].right, bar_rect_.top, thumb_rect_[2].left, bar_rect_.bottom);

	} else {
		
		Gdiplus::LinearGradientBrush lgb(Gdiplus::Rect(bar_rect_.left, bar_rect_.top, bar_rect_.Width(), bar_rect_.Height()), Gdiplus::Color(0, 0, 0), Gdiplus::Color(GetRValue(gradient_color_), GetGValue(gradient_color_), GetBValue(gradient_color_)), Gdiplus::LinearGradientModeHorizontal);
		graphics.FillRectangle(&lgb, bar_rect_.left, bar_rect_.top, bar_rect_.Width(), bar_rect_.Height());
		graphics.DrawRectangle(Formation::blackcolor_pen_gdi(), bar_rect_.left, bar_rect_.top, bar_rect_.Width(), bar_rect_.Height());
	}

	/*cdc_.SelectObject(GetStockObject(NULL_BRUSH));
	cdc_.Rectangle(touch_rect_[0]);
	cdc_.Rectangle(touch_rect_[1]);
	cdc_.Rectangle(touch_rect_[2]);*/

	Gdiplus::Point points[3];
	for (int i = start_index_; i <= stop_index_; i++) {

		if (use_floating_) {
			str.Format(L"%.1f", f_cur_pos_[i]);
		} else {
			str.Format(L"%d", cur_pos_[i]);
		}
		offdc.DrawText(str, value_rect_[i], alignment_[i]);

		if (i == 0) {
			points[0].X = thumb_rect_[0].CenterPoint().x;
			points[0].Y = thumb_rect_[0].top;
			points[1].X = thumb_rect_[0].left + thumb_rect_[0].Width() / 4;
			points[1].Y = points[2].Y = thumb_rect_[0].bottom;
			points[2].X = thumb_rect_[0].right - thumb_rect_[0].Width() / 4;
		} else if (i == 1) {
			points[0].X = thumb_rect_[1].right;
			points[0].Y = thumb_rect_[1].top;
			points[1].X = thumb_rect_[1].right;
			points[1].Y = points[2].Y = thumb_rect_[1].bottom;
			points[2].X = thumb_rect_[1].left + (thumb_rect_[1].Width() / 2);
		} else if (i == 2) {
			points[0].X = thumb_rect_[2].left;
			points[0].Y = thumb_rect_[2].top;
			points[1].X = thumb_rect_[2].left;
			points[1].Y = points[2].Y = thumb_rect_[2].bottom;
			points[2].X = thumb_rect_[2].right - (thumb_rect_[2].Width() / 2);
		}

		if (IsWindowEnabled()) {
			if (button_down_[i]) {
				graphics.FillPolygon(Formation::spancolor1_brush_for_selection_gdi(), points, 3);
			} else {
				graphics.FillPolygon(Formation::spancolor1_brush_gdi(), points, 3);
			}
		} else {
			graphics.FillPolygon(Formation::disablecolor_brush_gdi(), points, 3);
		}
	}
}

void SliderCtrl::Create(int thumbs_to_display, BOOL title, COLORREF gradient_color, BOOL highlight_changed_value) {

	highlight_changed_value_ = highlight_changed_value;

	GetClientRect(client_rect_);

	thumb_rect_[0].SetRectEmpty();
	value_rect_[0].SetRectEmpty();
	touch_rect_[0].SetRectEmpty();
	thumb_rect_[1].SetRectEmpty();
	value_rect_[1].SetRectEmpty();
	touch_rect_[1].SetRectEmpty();
	thumb_rect_[2].SetRectEmpty();
	value_rect_[2].SetRectEmpty();
	touch_rect_[2].SetRectEmpty();

	use_floating_ = FALSE;
	button_down_[0] = button_down_[1] = button_down_[2] = FALSE;
	start_index_ = stop_index_ = 0;

	min_pos_ = 0;
	max_pos_ = 255;
	prev_pos_[0] = cur_pos_[0] = 0;
	prev_pos_[1] = cur_pos_[1] = 0;
	prev_pos_[2] = cur_pos_[2] = 255;
	total_pos_ = 1;
	f_min_pos_ = 0;
	f_max_pos_ = 255;
	f_prev_pos_[0] = f_cur_pos_[0] = 0.0;
	f_prev_pos_[1] = f_cur_pos_[1] = 0.0;
	f_prev_pos_[2] = f_cur_pos_[2] = 255.0;
	f_total_pos_ = 1.0;
	
	if (thumbs_to_display <= 0) {
		thumbs_to_display = 1;
	} else if (thumbs_to_display >= 3) {
		thumbs_to_display = 2;
	}
	if (thumbs_to_display == 2) {
		start_index_ = 1;
		stop_index_ = 2;
	}

	thumb_to_display_ = thumbs_to_display;
	gradient_color_ = gradient_color;
	thumb_index_ = 0;

	int title_height = (client_rect_.Height() * 25 / 100);
	int value_height = (client_rect_.Height() * 20 / 100);
	int bar_height = (client_rect_.Height() * 10 / 100);
	int thumb_height = (client_rect_.Height() * 40 / 100);
	int thumb_width = thumb_height + (thumb_height / 2);
	if (!title) { //Caption bar not required
		title_height = 0;
		value_height = (client_rect_.Height() * 28 / 100);
		bar_height = (client_rect_.Height() * 15 / 100);
		thumb_height = (client_rect_.Height() * 52 / 100);
		thumb_width = thumb_height + (thumb_height / 2);
	}

	caption_rect_.SetRect(client_rect_.left, 0, client_rect_.right, title_height);
	bar_rect_.SetRect(client_rect_.left + thumb_width, title_height + value_height, client_rect_.right - thumb_width, title_height + value_height + bar_height);

	if (thumb_to_display_ == 1) {

		value_rect_[0].SetRect(bar_rect_.left - thumb_width, title_height, bar_rect_.left, title_height + value_height);
		thumb_rect_[0].SetRect(value_rect_[0].left, bar_rect_.bottom, value_rect_[0].right, bar_rect_.bottom + thumb_height);
		touch_rect_[0].SetRect(value_rect_[0].left, value_rect_[0].top, thumb_rect_[0].right, thumb_rect_[0].bottom);

		//SET LEFT AND RIGHT BOUNDRY
		left_limit_[0] = bar_rect_.left - (thumb_rect_[0].Width() / 2);
		right_limit_[0] = bar_rect_.right - (thumb_rect_[0].Width() / 2);

		alignment_[0] = DT_CENTER | DT_VCENTER;

	} else {

		value_rect_[1].SetRect(bar_rect_.left - thumb_width, title_height, bar_rect_.left, title_height + value_height);
		thumb_rect_[1].SetRect(value_rect_[1].left, bar_rect_.bottom, value_rect_[1].right, bar_rect_.bottom + thumb_height);
		touch_rect_[1].SetRect(value_rect_[1].left, value_rect_[1].top, thumb_rect_[1].right, thumb_rect_[1].bottom);
		value_rect_[2].SetRect(bar_rect_.right, title_height, bar_rect_.right + thumb_width, title_height + value_height);
		thumb_rect_[2].SetRect(value_rect_[2].left, bar_rect_.bottom, value_rect_[2].right, bar_rect_.bottom + thumb_height);
		touch_rect_[2].SetRect(value_rect_[2].left, value_rect_[2].top, thumb_rect_[2].right, thumb_rect_[2].bottom);

		//SET LEFT AND RIGHT BOUNDRY
		left_limit_[1] = bar_rect_.left - 1 - (thumb_rect_[1].Width());
		right_limit_[1] = thumb_rect_[1].left - (thumb_rect_[1].Width());
		left_limit_[2] = thumb_rect_[1].right;
		right_limit_[2] = bar_rect_.right + 1;

		alignment_[1] = DT_RIGHT | DT_VCENTER;
		alignment_[2] = DT_LEFT | DT_VCENTER;
	}
}

void SliderCtrl::SetText(CString text) {

	text_ = text;
}

void SliderCtrl::ValidateIndex(int& index) {

	//TO SET INDEX 0 IF IT IS OUT OF BOUNDS
	if (thumb_to_display_ == 1 && index != 0) {
		index = 0;
	}
	if (thumb_to_display_ == 2) {
		if (index != 1 && index != 2) {
			index = 1;
		}
	}
}

void SliderCtrl::SetRange(int min, int max) {

	use_floating_ = FALSE;
	if (min == max) {
		max++;
	}
	min_pos_ = min;
	max_pos_ = max;
	if (min > max) {
		max_pos_ = min;
		min_pos_ = max;
	}
	prev_pos_[0] = prev_pos_[1] = cur_pos_[0] = cur_pos_[1] = min;
	prev_pos_[2] = cur_pos_[2] = max;
	total_pos_ = (max - min);
}

int SliderCtrl::GetPos(int thumb_index) {

	ValidateIndex(thumb_index);

	return cur_pos_[thumb_index];
}

void SliderCtrl::SetPosition(int pos, int thumb_index) {

	if (pos >= min_pos_ && pos <= max_pos_) {

		//TO SET INDEX 0 IF IT IS OUT OF BOUNDS
		ValidateIndex(thumb_index);

		//if ((thumb_index == 2 && pos <= cur_pos_[1]) || (thumb_index == 1 && pos >= cur_pos_[2])) {
		//	return;
		//}

		//CALCULATE POINTER POSITION
		cur_pos_[thumb_index] = pos;
		int left = bar_rect_.left + (bar_rect_.Width() * (cur_pos_[thumb_index] - min_pos_) / total_pos_); 

		if (thumb_index == 0) { //SHIFT HALF RECT TO ALIGN POINTER IN CENTER | /\ |
			left -= (thumb_rect_[thumb_index].Width() / 2);
		} else if (thumb_index == 1) { //SHIFT FULL RECT TO ALIGN POINTER IN RIGHT |  /|
			left -= (thumb_rect_[thumb_index].Width());
		} else if (thumb_index == 2) { //NO SHIFT REQUIRED |\  |
		}

		thumb_rect_[thumb_index].MoveToX(left);
		value_rect_[thumb_index].MoveToX(left);
		touch_rect_[thumb_index].MoveToX(left);

		//SET LEFT AND RIGHT BOUNDRY
		right_limit_[1] = thumb_rect_[2].left - thumb_rect_[1].Width();
		left_limit_[2] = thumb_rect_[1].right;

		InvalidateRect(client_rect_);
	}
}

void SliderCtrl::SetPos(int pos, BOOL highlight) {

	SetPosition(pos, 0);

	if (!highlight) {
		prev_pos_[0] = pos;
	}
}

void SliderCtrl::SetPos2(int left_pos, int right_pos, BOOL highlight) {

	if (left_pos >= min_pos_ && right_pos <= max_pos_) {

		if (left_pos > right_pos) {
			int temp = left_pos;
			left_pos = right_pos;
			right_pos = temp;
		}
		if (left_pos == right_pos) {
			if (right_pos + 1 <= max_pos_) {
				right_pos += 1;
			} else if (left_pos - 1 >= min_pos_) {
				left_pos -= 1;
			}
		}

		SetPosition(left_pos, 1);
		SetPosition(right_pos, 2);

		if (!highlight) {
			prev_pos_[1] = left_pos;
			prev_pos_[2] = right_pos;
		}
	}
}

void SliderCtrl::SetRangeF(double min, double max) {

	use_floating_ = TRUE;

	if (min == max) {
		max += 0.1;
	}
	f_min_pos_ = min;
	f_max_pos_ = max;
	if (min > max) {
		f_max_pos_ = min;
		f_min_pos_ = max;
	}
	f_prev_pos_[0] = f_prev_pos_[1] = f_cur_pos_[0] = f_cur_pos_[1] = min;
	f_prev_pos_[2] = f_cur_pos_[2] = max;
	f_total_pos_ = (max - min);
}

double SliderCtrl::GetPosF(int thumb_index) {

	//TO SET INDEX 0 IF IT IS OUT OF BOUNDS
	ValidateIndex(thumb_index);

	return f_cur_pos_[thumb_index];
}

void SliderCtrl::SetPositionF(double pos, int thumb_index) {

	if (pos >= f_min_pos_ && pos <= f_max_pos_) {

		//TO SET INDEX 0 IF IT IS OUT OF BOUNDS
		ValidateIndex(thumb_index);

		//CALCULATE POINTER POSITION
		f_cur_pos_[thumb_index] = pos;
		int left = (int)(bar_rect_.left + (bar_rect_.Width() * ((f_cur_pos_[thumb_index] - f_min_pos_) / f_total_pos_)));

		if (thumb_index == 0) { //SHIFT HALF RECT TO ALIGN POINTER IN CENTER | /\ |
			left -= (thumb_rect_[thumb_index].Width() / 2);
		} else if (thumb_index == 1) { //SHIFT FULL RECT TO ALIGN POINTER IN RIGHT |  /|
			left -= (thumb_rect_[thumb_index].Width());
		} else if (thumb_index == 2) { //NO SHIFT REQUIRED |\  |
		}

		thumb_rect_[thumb_index].MoveToX(left);
		value_rect_[thumb_index].MoveToX(left);
		touch_rect_[thumb_index].MoveToX(left);

		//SET LEFT AND RIGHT BOUNDRY
		right_limit_[1] = thumb_rect_[2].left - thumb_rect_[1].Width();
		left_limit_[2] = thumb_rect_[1].right;

		InvalidateRect(client_rect_);
	}
}

void SliderCtrl::SetPosF(double pos, BOOL highlight) {

	SetPositionF(pos, 0);

	if (!highlight) {
		f_prev_pos_[0] = pos;
	}
}

void SliderCtrl::SetPos2F(double left_pos, double right_pos, BOOL highlight) {

	if (left_pos >= f_min_pos_ && right_pos <= f_max_pos_) {

		if (left_pos > right_pos) {
			double temp = left_pos;
			left_pos = right_pos;
			right_pos = temp;
		}
		if (left_pos == right_pos) {
			if (right_pos + 0.1 <= f_max_pos_) {
				right_pos += 0.1;
			} else if (left_pos - 0.1 >= f_min_pos_) {
				left_pos -= 0.1;
			}
		}

		SetPositionF(left_pos, 1);
		SetPositionF(right_pos, 2);

		if (!highlight) {
			f_prev_pos_[1] = left_pos;
			f_prev_pos_[2] = right_pos;
		}
	}
}

void SliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	thumb_index_ = -1;

	for (int i = start_index_; i <= stop_index_; i++) {
		if (touch_rect_[i].PtInRect(point)) {
			thumb_index_ = i;
			break;
		}
	}
	if (thumb_index_ != -1) {
		point_down_ = point;
		button_down_[thumb_index_] = TRUE;
		InvalidateRect(thumb_rect_[thumb_index_]);

		SetCapture();
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void SliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {

		if (thumb_index_ != -1) {
			int pointx = thumb_rect_[thumb_index_].left + (point.x - point_down_.x);
			if (pointx > left_limit_[thumb_index_] && pointx <= right_limit_[thumb_index_]) {

				thumb_rect_[thumb_index_].MoveToX(pointx);
				value_rect_[thumb_index_].MoveToX(pointx);
				touch_rect_[thumb_index_].MoveToX(pointx);

				int pos = 0;
				if (thumb_index_ == 0) { //TO GET CENTER | /\ |
					pos = (thumb_rect_[thumb_index_].CenterPoint().x - bar_rect_.left);
				} else if (thumb_index_ == 1) { //TO GET RIGHT |  /|
					pos = (thumb_rect_[thumb_index_].right - bar_rect_.left);
				} else if (thumb_index_ == 2) { //TO GET LEFT |\  |
					pos = (thumb_rect_[thumb_index_].left - bar_rect_.left);
				}
				if (use_floating_) {
					f_cur_pos_[thumb_index_] = ((f_total_pos_ * pos) / bar_rect_.Width()) + f_min_pos_;
				} else {
					cur_pos_[thumb_index_] = ((total_pos_ * pos) / bar_rect_.Width()) + min_pos_;
				}

				//TO ESCAPE FROM SAME VALUE WHEN TWO THUMBS EXIST
				if (thumb_index_ == 1) {
					if (use_floating_) {
						f_cur_pos_[thumb_index_] = (f_cur_pos_[thumb_index_] == f_cur_pos_[2]) ? (f_cur_pos_[thumb_index_] - 0.1) : f_cur_pos_[thumb_index_];
					} else {
						cur_pos_[thumb_index_] = (cur_pos_[thumb_index_] == cur_pos_[2]) ? (cur_pos_[thumb_index_] - 1) : cur_pos_[thumb_index_];
					}
				} else if (thumb_index_ == 2) {
					if (use_floating_) {
						f_cur_pos_[thumb_index_] = (f_cur_pos_[thumb_index_] == f_cur_pos_[1]) ? (f_cur_pos_[thumb_index_] + 0.1) : f_cur_pos_[thumb_index_];
					} else {
						cur_pos_[thumb_index_] = (cur_pos_[thumb_index_] == cur_pos_[1]) ? (cur_pos_[thumb_index_] + 1) : cur_pos_[thumb_index_];
					}
				}
				
				//TO SET MIN OR MAX VALUE
				if (use_floating_) {
					if (f_cur_pos_[thumb_index_] < f_min_pos_) f_cur_pos_[thumb_index_] = f_min_pos_;
					if (f_cur_pos_[thumb_index_] > f_max_pos_) f_cur_pos_[thumb_index_] = f_max_pos_;
				} else {
					if (cur_pos_[thumb_index_] < min_pos_) cur_pos_[thumb_index_] = min_pos_;
					if (cur_pos_[thumb_index_] > max_pos_) cur_pos_[thumb_index_] = max_pos_;
				}

				//value_changed_ = TRUE;
				//TO PREVENT MULTIPLE TIMES GETTING SAME VALUE
				if (use_floating_ && f_prev_pos_[thumb_index_] != f_cur_pos_[thumb_index_]) {
					//f_prev_pos_[thumb_index_] = f_cur_pos_[thumb_index_];
					/*if (f_prev_pos_[thumb_index_] != f_cur_pos_[thumb_index_]) {
						value_changed_ = FALSE;
					}*/
					GetParent()->SendMessageW(WM_SLIDERCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(thumb_index_));
				} else if (!use_floating_ && prev_pos_[thumb_index_] != cur_pos_[thumb_index_]) {
					//prev_pos_[thumb_index_] = cur_pos_[thumb_index_];
					/*if (prev_pos_[thumb_index_] != cur_pos_[thumb_index_]) {
						value_changed_ = FALSE;
					}*/
					GetParent()->SendMessageW(WM_SLIDERCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(thumb_index_));
				}

				//SET LEFT AND RIGHT BOUNDRY
				right_limit_[1] = thumb_rect_[2].left - thumb_rect_[1].Width();
				left_limit_[2] = thumb_rect_[1].right;

				InvalidateRect(client_rect_);
			}
		}
		point_down_ = point;
	}

	CStatic::OnMouseMove(nFlags, point);
}

void SliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (thumb_index_ != -1) {
		button_down_[thumb_index_] = FALSE;
		InvalidateRect(thumb_rect_[thumb_index_]);
	
		ReleaseCapture();
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void SliderCtrl::OnEnable(BOOL bEnable)
{
	/*if (highlight_changed_value_) {
		if (use_floating_) {
			if (start_index_ == 0) {
				SetPosF(f_prev_pos_[0]);
			} else {
				SetPosF(f_prev_pos_[1], f_prev_pos_[2]);
			}
		} else {
			if (start_index_ == 0) {
				SetPos(prev_pos_[0]);
			} else {
				SetPos(prev_pos_[1], prev_pos_[2]);
			}
		}
	}*/

	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}