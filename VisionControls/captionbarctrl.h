#pragma once

#ifndef INCLUDE_CAPTIONBARCTRL
#define INCLUDE_CAPTIONBARCTRL __declspec(dllimport)
#endif //INCLUDE_CAPTIONBARCTRL

// CaptionBarCtrl

class INCLUDE_CAPTIONBARCTRL CaptionBarCtrl : public CStatic
{
	DECLARE_DYNAMIC(CaptionBarCtrl)

public:
	enum LED_STATUS { LED_STATUS_NORMAL, LED_STATUS_ALARM, LED_STATUS_ERROR, LED_STATUS_RUNNING };

	CaptionBarCtrl();
	virtual ~CaptionBarCtrl();

	void Create(BOOL show_toggle_button, int left_text_area = 100, int right_text_area = 0);
	void Destroy();
	void SetLeftText(CString text, COLORREF text_color) {
		left_text_ = text;
		left_text_color_ = text_color;
		InvalidateRect(left_text_rect_);
	}
	void SetRightText(CString text, COLORREF text_color, BOOL check_on) {
		right_text_[check_on] = text;
		right_text_color_ = text_color;
		InvalidateRect(right_text_rect_);
	}
	void ShowLeftText(BOOL show) {
		left_visible_text_ = show;
		InvalidateRect(left_text_rect_);
	}
	void ShowRightText(BOOL show) {
		right_visible_text_ = show;
		InvalidateRect(right_text_rect_);
	}
	void SetColorCode(COLORREF color) {
		color_code_ = color;
		InvalidateRect(color_code_rect_);
	}
	COLORREF GetColorCode() {
		return color_code_;
	}
	void ShowColorCode(BOOL show) {
		visible_color_code_ = show;
		InvalidateRect(color_code_rect_);
	}
	
private:
	CRect client_rect_;
	CRect color_code_rect_;
	CRect left_text_rect_;
	CRect right_text_rect_;
	CRect right_text_button_rect_;
	BOOL right_text_button_clicked_;
	BOOL right_text_button_status_;
	
	CString	left_text_;
	CString	right_text_[2];
	COLORREF left_text_color_;
	COLORREF right_text_color_;
	BOOL left_visible_text_;
	BOOL right_visible_text_;
	BOOL show_toggle_button_;
	COLORREF color_code_;
	BOOL visible_color_code_;

	DECLARE_MESSAGE_MAP()

public:
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};