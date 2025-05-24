#pragma once

#ifndef INCLUDE_ONOFFCTRL
#define INCLUDE_ONOFFCTRL __declspec(dllimport)
#endif //INCLUDE_ONOFFCTRL

#define WM_ONOFFCTRL_CHECKONOFF		WM_USER + 408

// OnOffCtrl

class INCLUDE_ONOFFCTRL OnOffCtrl : public CStatic
{
	DECLARE_DYNAMIC(OnOffCtrl)

public:
	OnOffCtrl();
	virtual ~OnOffCtrl();

	void Create(int text_area = 50, DWORD text_aligment_ = DT_LEFT, BOOL highlight_changed_value = TRUE);
	void Destroy();
	void SetText(CString text, CString on_text = _T(""), CString off_text = _T(""));
	CString GetText(BOOL right);
	BOOL GetCheck();
	void SetCheck(BOOL check, BOOL update = TRUE, BOOL highlight = FALSE);

protected:
	virtual void PreSubclassWindow();

private:
	CRect client_rect_;
	CRect caption_rect_;
	CRect rect_[2];
	CPoint down_point_;
	DWORD text_aligment_;
	CString caption_text_;
	CString text_[2];
	BOOL checked_;
	BOOL prev_checked_;
	BOOL highlight_changed_value_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};