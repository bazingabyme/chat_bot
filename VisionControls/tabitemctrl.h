#pragma once

#ifndef INCLUDE_TABITEMCTRL
#define INCLUDE_TABITEMCTRL __declspec(dllimport)
#endif //INCLUDE_TABITEMCTRL

#define WM_TABITEMCTRL_SELCHANGE		WM_USER + 425
#define WM_TABITEMCTRL_DELETE			WM_USER + 426

// TabItemCtrl

class INCLUDE_TABITEMCTRL TabItemCtrl : public CStatic
{
	DECLARE_DYNAMIC(TabItemCtrl)

public:
	TabItemCtrl();
	virtual ~TabItemCtrl();

	bool Create(BOOL title);
	void Destroy();
	void ShowAddAndDelete(BOOL add_and_delete);
	void ShowSelectionBar(BOOL show);
	bool SetText(CString text, CString add_text);
	void SetSize(int size);
	bool AddItem(COLORREF fillcolor, COLORREF bordercolor);
	void SelectItem(int item_index, BOOL update);
	COLORREF GetColor(int item_index);
	void ChangeColor(int item_index, COLORREF fillcolor);
	void ChangeBorderColor(int item_index, COLORREF bordercolor);
	void SetItemData(int index, CString data);
	CString GetItemData(int index);
	void RemoveItem(int item_index);
	void RemoveAllItems();
	int item_index() { return item_index_; }
	int total_items() { return (int)item_info_list_.GetSize() - 1; }

protected:
	virtual void PreSubclassWindow();

private:
	struct ItemInfo {
		CRect rect;
		CString text;
		CString data;
		COLORREF fillcolor;
		COLORREF textcolor;
		COLORREF bordercolor;
		BOOL enable;
		BOOL visible;
	};

	CRect client_rect_;
	CRect caption_rect_;
	CString text_;
	CString add_text_;
	CString data_;
	
	int size_;
	int item_index_;
	BOOL item_selected_;
	BOOL show_selection_bar_;

	ItemInfo delete_button_;
	CPtrArray item_info_list_;

	HICON delete_icon_;
	CRect delete_icon_rect_;
	
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);

	DECLARE_MESSAGE_MAP()
};


