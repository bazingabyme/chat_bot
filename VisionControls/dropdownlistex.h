#pragma once

#ifndef INCLUDE_DROPDOWNLISTEX
#define INCLUDE_DROPDOWNLISTEX __declspec(dllimport)
#endif //INCLUDE_DROPDOWNLISTEX

#include "sortlistctrl.h"

#define WM_LISTCTRLEX_SELCHANGE					WM_USER + 447
#define WM_DROPDOWNLISTEX_SELCHANGE				WM_USER + 449

// ListCtrlEx

class ListCtrlEx : public CStatic
{
	DECLARE_DYNAMIC(ListCtrlEx)

public:

	ListCtrlEx();
	virtual ~ListCtrlEx();

	void Create(CStringArray& items_list, int selected_index, int spacing, DWORD align);
	void Destroy();
	void Refresh(CString str);
	void DisableScrollbar(UINT sbar, BOOL disable);
	void SetRowHeight(int height) { row_height_ = height; }
	void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
	COLORREF GetBackColor() { return back_color_; }
	int GetSelectedIndex() { return selected_index_; }

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void ResetScrollBar();

	CRect wnd_rect_;
	CRect client_rect_;
	CPoint down_point_;
	int	document_height_;
	int selected_index_;
	int spacing_;
	int row_height_;

	BOOL border_;
	COLORREF back_color_;
	DWORD alignment_;

	CArray<CString> item_list_text_;
	CArray<CRect> item_list_rect_;
	CArray<BOOL> item_list_visible_;

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};

// DDListEx

class DDListEx : public CDialogEx
{
	DECLARE_DYNAMIC(DDListEx)

public:
	enum { IDD = IDD_DDLISTEX_DLG };

	DDListEx(CWnd* pParent = NULL);
	virtual ~DDListEx();

	int Open(CWnd* parent_wnd, CRect parent_rect, CString text, CStringArray& items_list, int selected_index, CFont* font, DWORD alignment);
	void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
	CWnd* GetParentWnd() { return parent_wnd_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CWnd* parent_wnd_;
	CRect client_rect_;
	CRect parent_rect_;
	CRect title_rect_;
	CRect search_rect_;
	CRect search_icon_rect_;
	HICON search_icon_;
	BOOL search_button_clicked_;
	//CRect close_rect_;
	//CRect close_icon_rect_;
	//HICON close_icon_;
	//BOOL close_button_clicked_;
	COLORREF back_color_;

	ListCtrlEx ctrList;
	CStringArray items_list_;
	CString text_;
	CString search_text_;
	int selected_index_;
	CFont* font_;
	DWORD alignment_;

public:
	LRESULT OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

// DropDownListEx

class INCLUDE_DROPDOWNLISTEX DropDownListEx : public CStatic
{
	DECLARE_DYNAMIC(DropDownListEx)

public:
	DropDownListEx();
	virtual ~DropDownListEx();

	void Create(int text_area, DWORD alignment = DT_LEFT);
	void Destroy();
	void SetText(CString text);
	void SetText(int index, CString text);
	CString GetText(int index);
	void AddItem(CString str);
	void DeleteItem(int index);
	void ResetContents();
	void SetCurSel(int index);
	int GetCurSel();
	CString GetCurSelText();
	int FindString(CString str);
	int GetItemCount() { return int(items_list_.GetCount()); }
	void SetBackColor(COLORREF color);
	void SetTextColor(COLORREF color);
	void SetBorderColor(COLORREF color);
	void SetTextAlignment(DWORD text_alignment) { text_alignment_ = text_alignment; }
	
protected:
	virtual void PreSubclassWindow();

private:
	DDListEx ddlistex_;
	int cdc_height_;
	int font_height_;
	int start_y_;
	CPoint down_point_;

	CRect client_rect_;
	CRect text_rect_;
	CRect dropdown_rect_;

	Gdiplus::Point points_for_open_arrow_[3], points_for_close_arrow_[3];

	CString text_;
	DWORD text_alignment_;
	DWORD alignment_;
	CStringArray items_list_;
	int selected_index_;
	int prev_selected_index_;
	BOOL highlight_changed_value_;

	COLORREF text_color_;
	COLORREF back_color_;
	COLORREF border_color_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	LRESULT OnListCtrlExSelChangeMessage(WPARAM wparam, LPARAM lparam);
};