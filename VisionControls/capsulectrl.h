#pragma once

#ifndef INCLUDE_CAPSULECTRL
#define INCLUDE_CAPSULECTRL __declspec(dllimport)
#endif //INCLUDE_CAPSULECTRL

#define WM_CAPSULECTRL_HEADER_CLICKED	WM_USER + 400

// CapsuleCtrl

class INCLUDE_CAPSULECTRL CapsuleCtrl : public CStatic
{
	DECLARE_DYNAMIC(CapsuleCtrl)

public:
	CapsuleCtrl();
	virtual ~CapsuleCtrl();

	void Create(int caption_area, BOOL capsule_shape, BOOL topbottomstyle);
	void Destroy();
	void SetBorderColor(COLORREF color);
	void SetFontEx(CFont* font, BOOL rightside);
	void SetText(CString text, BOOL multiline, DWORD alignment, BOOL rightside);
	CString GetText(BOOL rightside);
	void SetBackColor(COLORREF color, BOOL rightside);
	void SetTextColor(COLORREF color, BOOL rightside);
	
protected:
	virtual void PreSubclassWindow();

private:
	CFont* font_[2];

	CRect client_rect_;
	CRect rect_[2];
	CString text_[2];
	DWORD alignment_[2];

	COLORREF border_color_;
	COLORREF back_color_[2];
	COLORREF text_color_[2];
	BOOL multiline_[2];
	BOOL capsule_shape_;
	BOOL topbottomstyle_;

	int start_y_;
	CRect bounding_rect_;
	CPoint down_point_;
	BOOL mouse_moved_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};