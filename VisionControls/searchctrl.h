#pragma once

#ifndef INCLUDE_SEARCHCTRL
#define INCLUDE_SEARCHCTRL __declspec(dllimport)
#endif //INCLUDE_SEARCHCTRL

#define WM_SEARCH_EDIT_CHARCHANGE		WM_USER + 452
#define WM_SEARCH_EDIT_ENTERPRESSED		WM_USER + 453

// SearchCtrl

class INCLUDE_SEARCHCTRL SearchCtrl : public CStatic
{
	DECLARE_DYNAMIC(SearchCtrl)

public:
	SearchCtrl();
	virtual ~SearchCtrl();

	void Create();
	void Destroy();
	CString GetSearchText();
	void SetText(CString text) { text_ = text; }
	void SetBackColor(COLORREF backcolor) { backcolor_ = backcolor; }
	void SetBorderColor(COLORREF bordercolor) { bordercolor_ = bordercolor; }
	void SetTextColor(COLORREF textcolor) { textcolor_ = textcolor; }
	void SetDisableTextColor(COLORREF disable_textcolor) { disable_textcolor_ = disable_textcolor; }

protected:
	virtual void PreSubclassWindow();

private:
	CRect client_rect_;
	CRect button_rect_;
	CRect button_icon_rect_;
	CRect search_rect_;
	CRect search_rect_bottomedge_;

	CString text_;
	CString search_text_;

	BOOL search_clicked_, clear_clicked_;

	HICON search_icon_, clear_icon_;

	COLORREF backcolor_, bordercolor_, textcolor_, disable_textcolor_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnKeyboardEditCharChangeMessage(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};