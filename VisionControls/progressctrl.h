#pragma once

#ifndef INCLUDE_PROGRESSCTRL
#define INCLUDE_PROGRESSCTRL __declspec(dllimport)
#endif //INCLUDE_PROGRESSCTRL

// ProgressCtrl

class INCLUDE_PROGRESSCTRL ProgressCtrl : public CStatic
{
	DECLARE_DYNAMIC(ProgressCtrl)

public:
	ProgressCtrl();
	virtual ~ProgressCtrl();

	void Create(BOOL continuous, BOOL percentage);
	void Destroy();
	void SetBackColor(COLORREF backcolor) { backcolor_ = backcolor; }
	void SetTextColor(COLORREF textcolor) { textcolor_ = textcolor; }
	void SetText(CString text, CString text_at_100 = _T(""));
	void SetPos(double pos);
	void SetPos(long pos);
	void SetLimit(int limit);
	void SetValue(int value);
	void BlinkText(BOOL blink);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CRect client_rect_;
	CRect bar_rect_;
	double f_pos_;
	long pos_;
	int value_;
	int limit_;

	BOOL percentage_;
	CString text_;
	CString text_at_100_;
	BOOL continuous_;
	BOOL floating_;
	BOOL blink_text_flag_;
	BOOL timer_fired_;

	COLORREF backcolor_, textcolor_;

public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnable(BOOL bEnable);
};