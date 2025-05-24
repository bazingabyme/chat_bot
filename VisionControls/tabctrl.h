#pragma once

#ifndef INCLUDE_TABCTRL
#define INCLUDE_TABCTRL __declspec(dllimport)
#endif //INCLUDE_TABCTRL

#define WM_TABCTRL_SELCHANGE	WM_USER + 413

// TabCtrl

class INCLUDE_TABCTRL TabCtrl : public CStatic
{
	DECLARE_DYNAMIC(TabCtrl)

public:
	enum TAB_ALIGNMENT { TOP, BOTTOM, LEFT, RIGHT };

	TabCtrl();
	virtual ~TabCtrl();

	void Create(TAB_ALIGNMENT tab_alignment, BOOL button_style = FALSE);
	void Destroy();
	void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
	void SetLineColor(COLORREF line_color) { line_color_ = line_color; }
	void SetSelectionColor(COLORREF selection_color) { selection_color_ = selection_color; }
	void SetTextColor(COLORREF text_color) { text_color_ = text_color; }
	void SetTextSelectionColor(COLORREF text_selection_color) { text_selection_color_ = text_selection_color; }
	void SetText(int tab_index, CString text);
	void SetTabLength(int tab_length);
	void SelectTab(int tab_index, BOOL update = TRUE);
	int GetTabIndex() { return tab_index_; }
	void AddTab(CString str);
	void SetTabData(int tab_index, LPVOID lpvoid);
	LPVOID GetTabData(int tab_index);
	void EnableTab(int tab_index, BOOL enable);
	BOOL IsEnableTab(int tab_index);
	void VisibleTab(int tab_index, BOOL visible);
	BOOL IsVisibleTab(int tab_index);
	void RemoveTab(int tab_index);
	void RemoveAllTabs();
	void TabDataChanged(int tab_index, BOOL changed);
	BOOL IsTabDataChanged(int tab_index);
	int GetTabCount() { return (int)tab_list_.GetSize(); }

protected:
	virtual void PreSubclassWindow();

private:
	void RefreshTabs();

	CRect client_rect_;
	COLORREF back_color_;
	COLORREF line_color_;
	COLORREF selection_color_;
	COLORREF text_color_;
	COLORREF text_selection_color_;
	BOOL button_style_;
	BOOL button_clicked_;
	CFont font_;
	BOOL auto_calculated_length_;
	int tab_length_;
	int tab_index_;
	int button_clicked_tab_index_;
	CStringArray tab_list_;
	CArray<CRect> tab_rect_;
	CArray<BOOL> tab_enable_;
	CArray<BOOL> tab_visible_;
	CArray<BOOL> tab_data_changed_;
	CArray<LPVOID> tab_data_;

	TAB_ALIGNMENT tab_alignment_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};