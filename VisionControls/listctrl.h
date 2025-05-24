#pragma once

#ifndef INCLUDE_LISTCTRL
#define INCLUDE_LISTCTRL __declspec(dllimport)
#endif //INCLUDE_LISTCTRL

#define WM_LISTCTRL_SELCHANGE		WM_USER + 405
#define WM_LISTCTRL_CHECKONOFF		WM_USER + 406
#define WM_LISTCTRL_ITEMDRAGGED		WM_USER + 407
#define WM_LISTCTRL_DOUBLECLICKED	WM_USER + 427

// ListCtrl

class INCLUDE_LISTCTRL ListCtrl : public CStatic
{
	DECLARE_DYNAMIC(ListCtrl)

public:
	struct ItemInfo {

		ItemInfo() {

			buttoncolor = BACKCOLOR1;
			header = FALSE;
			enable = TRUE;
			check = FALSE;
			data = FALSE;
		}

		CString text;
		COLORREF backcolor;
		COLORREF textcolor;
		COLORREF buttoncolor;
		DWORD textalignment;
		BOOL header;
		BOOL enable;
		BOOL check;
		BOOL data;
	};

	ListCtrl();
	virtual ~ListCtrl();

	void Create(BOOL check_box_styled, BOOL grid_styled = TRUE, BOOL border = TRUE, COLORREF border_color = BACKCOLOR1);
	void Destroy();
	void AddItem(CString str, COLORREF backcolor, COLORREF textcolor, BOOL check = TRUE, DWORD textalignment = DT_LEFT, BOOL header = FALSE);
	void DeleteItem(int index);
	void DeleteAllItems();
	int GetTotalItems() { return (int)items_list_.GetSize(); }
	CString GetCurSelText();
	void SetBorderColor(COLORREF border_color);
	void SetSelectionColor(COLORREF color, BOOL borderonly);
	void AllowSelection(BOOL allow) { allow_selection_ = allow; }
	void AllowDragging(BOOL allow) { allow_dragging_ = allow; }
	CPoint GetItemDroppedPoints() { return point_up_; }

	void SetListItemHeight(int height) { list_item_height_ = height; }
	CString GetText(int index);
	void SetText(int index, CString str);
	int GetCurSel() { return selected_index_; }
	void SetCurSel(int index);
	BOOL IsItemHeader(int index);
	BOOL IsItemEnabled(int index);
	void EnableItem(int index, BOOL enable);
	COLORREF GetBackColor(int index);
	void SetBackColor(int index, COLORREF backcolor);
	COLORREF GetButtonColor(int index);
	void SetButtonColor(int index, COLORREF buttoncolor);
	COLORREF GetTextColor(int index);
	void SetTextColor(int index, COLORREF textcolor);
	void SetCheck(int index, BOOL check);
	void SetData(int index, BOOL data);
	BOOL GetCheck(int index);
	BOOL GetData(int index);

	CImageList drag_image_;

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void SetScrollBarPos();

	CRect client_rect_;
	CRect wnd_rect_;
	BOOL border_;
	COLORREF border_color_;

	CArray<ItemInfo*> items_list_;

	COLORREF selection_color_;

	float scroll_section_;
	CRect scroll_thumb_;
	CPoint point_down_;
	BOOL scroll_item_;
	BOOL drag_item_;

	BOOL check_box_styled_;
	BOOL grid_styled_;
	int list_item_height_;
	int start_index_;
	int selected_index_;
	int items_to_display_;

	CPoint point_up_;
	BOOL allow_dragging_;

	BOOL selection_borderonly_;
	BOOL allow_selection_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};