#pragma once

#ifndef INCLUDE_LABELCTRL
#define INCLUDE_LABELCTRL __declspec(dllimport)
#endif //INCLUDE_LABELCTRL

// LabelCtrl

class INCLUDE_LABELCTRL LabelCtrl : public CStatic
{
	DECLARE_DYNAMIC(LabelCtrl)

public:
	LabelCtrl();
	virtual ~LabelCtrl();

	void Create(DWORD alignment, BOOL multiline = FALSE);
	void Destroy();
	void SetText(CString text);
	CString GetText() { return text_; }
	void SetBackColor(COLORREF color);
	void SetTextColor(COLORREF color);
	void SetBorderColor(COLORREF color);
	
protected:
	virtual void PreSubclassWindow();

private:
	int start_y_;
	CPoint down_point_;
	BOOL mouse_moved_;

	CRect client_rect_;
	CRect text_rect_;

	CString text_;
	DWORD alignment_;
	BOOL multiline_;

	COLORREF text_color_;
	COLORREF back_color_;
	COLORREF border_color_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};