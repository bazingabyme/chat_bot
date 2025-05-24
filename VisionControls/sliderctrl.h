#pragma once

#ifndef INCLUDE_SLIDERCTRL
#define INCLUDE_SLIDERCTRL __declspec(dllimport)
#endif //INCLUDE_SLIDERCTRL

#define WM_SLIDERCTRL_SELCHANGE		WM_USER + 411

// SliderCtrl

class INCLUDE_SLIDERCTRL SliderCtrl : public CStatic
{
	DECLARE_DYNAMIC(SliderCtrl)

public:
	SliderCtrl();
	virtual ~SliderCtrl();

	void Create(int thumbs_to_display, BOOL title, COLORREF gradient_color = RGB(0, 0, 0), BOOL highlight_changed_value = TRUE);
	void Destroy();
	void SetText(CString text);
	void SetRange(int min, int max);
	void SetPos(int pos, BOOL highlight = FALSE);
	void SetPos2(int left_pos, int right_pos, BOOL highlight = FALSE);
	int GetPos(int thumb_index);
	void SetRangeF(double min, double max);
	void SetPosF(double pos, BOOL highlight = FALSE);
	void SetPos2F(double left_pos, double right_pos, BOOL highlight = FALSE);
	double GetPosF(int thumb_index);

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

private:
	void SetPosition(int pos, int thumb_index);
	void SetPositionF(double pos, int thumb_index);

	COLORREF gradient_color_;

	CString text_;
	int thumb_to_display_;
	CRect client_rect_;
	CRect caption_rect_;
	CRect bar_rect_;
	CRect thumb_rect_[3];
	CRect value_rect_[3];
	CRect touch_rect_[3];
	int left_limit_[3];
	int right_limit_[3];
	int alignment_[3];

	BOOL highlight_changed_value_;
	BOOL value_changed_;
	BOOL use_floating_;
	BOOL button_down_[3];
	int thumb_index_;
	int start_index_;
	int stop_index_;
	CPoint point_down_;

	int min_pos_;
	int max_pos_;
	int cur_pos_[3];
	int prev_pos_[3];
	int total_pos_;

	double f_min_pos_;
	double f_max_pos_;
	double f_cur_pos_[3];
	double f_prev_pos_[3];
	double f_total_pos_;

	void ValidateIndex(int& index);

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};


