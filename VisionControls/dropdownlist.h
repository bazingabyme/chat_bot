#pragma once

#ifndef INCLUDE_DROPDOWNLIST
#define INCLUDE_DROPDOWNLIST __declspec(dllimport)
#endif //INCLUDE_DROPDOWNLIST

#define WM_DROPDOWNLIST_SELCHANGE	WM_USER + 401

// DDList

class DDList : public CStatic
{
	DECLARE_DYNAMIC(DDList)

public:
	DDList();
	virtual ~DDList();

	void Initialize(CRect parent_rect);
	void ResetPosition(int offset_x);
	void Destroy();
	void Update(int items);
	void SetCurSel(int index, BOOL highlight);
	int GetCurSel() { return selected_index_; }
	int GetPrevSel() { return prev_selected_index_; }
	int FindString(CString str);
	CString GetCurSelText();
	CStringArray& get_items_list() { return items_list_; }

private:
	CRect parent_rect_;
	CRect client_rect_;

	CStringArray items_list_;
	int selected_index_;
	int prev_selected_index_;
	CRect scroll_thumb_;
	CRect scroll_rect_;
	CPoint point_down_;
	BOOL mouse_moved_;
	int top_index_;
	int items_to_display_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	void LButtonDown(UINT nFlags, CPoint point);
	void LButtonUp(UINT nFlags, CPoint point);
	void MouseMove(UINT nFlags, CPoint point);
};

// DropDownList

class INCLUDE_DROPDOWNLIST DropDownList : public CStatic
{
	DECLARE_DYNAMIC(DropDownList)

public:
	DropDownList();
	virtual ~DropDownList();

	void Create(CWnd* parent, CRect parent_rect, int text_area, BOOL highlight_changed_value = TRUE);
	void Destroy();
	void SetText(CString text);
	void SetText(int index, CString text);
	CString GetText(int index);
	void AddItem(CString str);
	void DeleteItem(int index);
	void ResetContents();
	void SetCurSel(int index, BOOL highlight = FALSE);
	int GetCurSel();
	CString GetCurSelText() { return DDList_.GetCurSelText(); }
	int FindString(CString str) { return DDList_.FindString(str); }
	int GetItemCount() { return (int)DDList_.get_items_list().GetCount(); }

protected:
	virtual void PreSubclassWindow();

private:
	DDList DDList_;

	Gdiplus::Point points_for_open_arrow_[3], points_for_close_arrow_[3];

	CRect client_rect_;
	CRect list_window_rect_;
	CRect text_rect_;
	CRect dropdown_rect_;
	CString str_text_;
	int items_to_display_;
	BOOL highlight_changed_value_;

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);

protected:
	DECLARE_MESSAGE_MAP()
};
