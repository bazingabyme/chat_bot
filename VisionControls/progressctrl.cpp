// progressctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_PROGRESSCTRL __declspec(dllexport)
#include "progressctrl.h"

// ProgressCtrl

IMPLEMENT_DYNAMIC(ProgressCtrl, CStatic)

ProgressCtrl::ProgressCtrl()
{
	text_ = _T("");
	text_at_100_ = _T("");
	percentage_ = FALSE;
	blink_text_flag_ = FALSE;
	timer_fired_ = FALSE;
	limit_ = 0;
	value_ = 0;

	backcolor_ = WHITE_COLOR;
	textcolor_ = BLACK_COLOR;
}

ProgressCtrl::~ProgressCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ProgressCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// ProgressCtrl message handlers

void ProgressCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	
	offdc.FillSolidRect(client_rect_, backcolor_);
	
	offdc.FillSolidRect(bar_rect_, IsWindowEnabled() ? LABEL_COLOR2 : DISABLE_COLOR);
	CRect progress_rect = bar_rect_;
	int bar_rect_width = client_rect_.Width();// client_rect_.right - client_rect_.left - Formation::spacing2();
	if (percentage_) {
		if (floating_) {
			progress_rect.right = progress_rect.left + (int)(bar_rect_width * f_pos_ / 100);
		} else {
			progress_rect.right = progress_rect.left + (int)(bar_rect_width * pos_ / 100);
		}
	} else {
		progress_rect.right = progress_rect.left + value_ * bar_rect_width / limit_;
	}
	if (progress_rect.right > client_rect_.right) {
		progress_rect.right = client_rect_.right;
	}
	/*if (progress_rect.right > (client_rect_.right - Formation::spacing())) {
		progress_rect.right = (client_rect_.right - Formation::spacing());
	}*/
	offdc.FillSolidRect(progress_rect, BACKCOLOR1_SEL);

	if (timer_fired_ && IsWindowEnabled()) {
		blink_text_flag_ ? offdc.SetTextColor(RED_COLOR) : offdc.SetTextColor(textcolor_);
	} else {
		offdc.SetTextColor(IsWindowEnabled() ? textcolor_ : WHITE_COLOR);
	}
	CString str;
	if (percentage_) {
		str = text_ + L"0%";
		if (!text_at_100_.IsEmpty()) {
			if (floating_) {
				if (f_pos_ == 100) {
					str = text_at_100_;
				}
			} else {
				if (pos_ == 100) {
					str = text_at_100_;
				}
			}
		}
		if (floating_) {
			if (f_pos_ != 0 && f_pos_ != 100)  {
				str.Format(L"%s%.2f%%", text_, f_pos_);
			}
		} else {
			if (pos_ != 0 && pos_ != 100)  {
				str.Format(L"%s%d", text_, pos_);
			}
		}
	} else {
		str.Format(L"%d/%d %s", value_, limit_, text_);
	}
	offdc.DrawText(str, client_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void ProgressCtrl::Create(BOOL continuous, BOOL percentage) {

	GetClientRect(client_rect_);
	bar_rect_ = client_rect_;
	//bar_rect_.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
	bar_rect_.top = bar_rect_.bottom - Formation::spacing();
	f_pos_ = pos_ = 0;
	timer_fired_ = FALSE;
	blink_text_flag_ = FALSE;
	percentage_ = percentage;
	continuous_ = continuous;
}

void ProgressCtrl::Destroy() {

}

void ProgressCtrl::SetText(CString text, CString text_at_100) {

	text_ = text;
	if (!text_.IsEmpty() && percentage_) {
		text_ = text_ + _T(": ");
	}
	text_at_100_ = text_at_100;
}

void ProgressCtrl::SetPos(double pos) {
	
	floating_ = TRUE;

	f_pos_ = pos;
	if (f_pos_ > 100) {
		f_pos_ = 100;
	}

	if (!text_at_100_.IsEmpty()) {
		if (f_pos_ == 100) {
			SetTimer(1, 1000, NULL);
			timer_fired_ = TRUE;
		}
		if (f_pos_ == 0) {
			KillTimer(1);
			timer_fired_ = FALSE;
		}
	}

	Invalidate(FALSE);
}

void ProgressCtrl::SetPos(long pos) {

	floating_ = FALSE;

	pos_ = pos;
	if (pos_ > 100) {
		pos_ = 100;
	}

	if (!text_at_100_.IsEmpty()) {
		if (pos_ == 100) {
			SetTimer(1, 1000, NULL);
			timer_fired_ = TRUE;
		}
		if (pos_ == 0) {
			timer_fired_ = FALSE;
			KillTimer(1);
		}
	}

	Invalidate(FALSE);
}

void ProgressCtrl::SetLimit(int limit) {

	limit_ = limit;

	Invalidate(FALSE);
}

void ProgressCtrl::SetValue(int value) {

	value_ = value;

	if (value_ >= limit_) {
		value_ = limit_;
		BlinkText(TRUE);
	}
	if (value_ == 0) {
		BlinkText(FALSE);
	}

	Invalidate(FALSE);
}

void ProgressCtrl::BlinkText(BOOL blink) {

	if (blink) {
		SetTimer(1, 1000, NULL);
		timer_fired_ = TRUE;
	} else {
		timer_fired_ = FALSE;
		KillTimer(1);
	}
}

void ProgressCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		blink_text_flag_ = !blink_text_flag_;
		Invalidate(FALSE);
	}

	CStatic::OnTimer(nIDEvent);
}

void ProgressCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}