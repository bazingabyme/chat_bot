#pragma once

#ifndef INCLUDE_SPINCTRL
#define INCLUDE_SPINCTRL __declspec(dllimport)
#endif //INCLUDE_SPINCTRL

#define WM_SPINCTRL_SELCHANGE		WM_USER + 412

// SpinCtrl

#include "edit.h"

class INCLUDE_SPINCTRL SpinCtrl : public CStatic
{
	DECLARE_DYNAMIC(SpinCtrl)

public:
	SpinCtrl();
	virtual ~SpinCtrl();

	void Create(BOOL highlight_changed_value = TRUE, int text_area = 50);
	void Destroy();
	void SetText(CString text);
	CString GetText() { return text_; }

	void SetRange(int min, int max, int freq);
	void SetPos(int pos, BOOL highlight = FALSE);
	int GetPos();
	int GetMin() { return min_pos_; }
	int GetMax() { return max_pos_; }
	void SetRangeF(double min, double max, double freq, BOOL double_precision = FALSE);
	void SetPosF(double pos, BOOL highlight = FALSE);
	double GetPosF();
	double GetMinF() { return f_min_pos_; }
	double GetMaxF() { return f_max_pos_; }

	void SetBallonData(double data, COLORREF color);
	void SetBallonData(int data, COLORREF color);
	void ShowBalloon(BOOL show);

	void SetTextAlignment(DWORD align);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

private:
	void SetPosition(int pos, BOOL highlight);
	void SetPositionF(double pos, BOOL highlight);
	void ValueChanged();

	Edit ctrEdit;
	COLORREF balloon_color_;

	DWORD alignment_;

	CString text_;
	CString left_text_;
	CString right_text_;
	CRect client_rect_;
	CRect text_rect_;
	CRect balloon_rect_;
	CRect left_text_rect_;
	CRect right_text_rect_;
	CRect edit_rect_;
	CRect edit_rect_topedge_;
	CRect edit_rect_bottomedge_;
	CRect spin_rect_[2];
	Gdiplus::Point point_for_spin_btn_[6];
	Gdiplus::Point point_for_arrow_[2][3];

	BOOL use_floating_;
	BOOL double_precesion_;
	BOOL button_down_[2];
	BOOL value_changed_;
	BOOL show_balloon_;
	BOOL highlight_changed_value_;
	int timer_executed_;
	int min_pos_;
	int max_pos_;
	int cur_pos_;
	int prev_pos_;
	int balloon_data_;
	int steps_[2];
	int spin_index_;
	double f_min_pos_;
	double f_max_pos_;
	double f_cur_pos_;
	double f_prev_pos_;
	double f_balloon_data_;
	double f_steps_[2];
	
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnable(BOOL bEnable);
};