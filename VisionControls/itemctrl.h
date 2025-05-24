#pragma once

#ifndef INCLUDE_ITEMCTRL
#define INCLUDE_ITEMCTRL __declspec(dllimport)
#endif //INCLUDE_ITEMCTRL

#define WM_ITEMCTRL_SELCHANGE		WM_USER + 403
#define WM_ITEMCTRL_DELETE			WM_USER + 404

// ItemCtrl

class INCLUDE_ITEMCTRL ItemCtrl : public CStatic
{
	DECLARE_DYNAMIC(ItemCtrl)

public:
	ItemCtrl();
	virtual ~ItemCtrl();

	bool Create(int text_area, int alignment);
	void Destroy();
	void ShowAddAndDelete(BOOL add_and_delete);
	bool SetText(CString text);
	bool AddItem(COLORREF fillcolor, COLORREF bordercolor);
	void SelectItem(int item_index, BOOL update);
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

	CFont font_;
	CRect client_rect_;
	CRect caption_rect_;
	int alignment_;
	CString text_;
	CString data_;
	
	int item_index_;
	BOOL item_selected_;
	
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


