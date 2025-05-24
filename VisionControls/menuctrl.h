#pragma once

#ifndef INCLUDE_MENUCTRL
#define INCLUDE_MENUCTRL __declspec(dllimport)
#endif //INCLUDE_MENUCTRL

class INCLUDE_MENUCTRL MenuCtrl : public CMenu
{
public:
	MenuCtrl();
	virtual ~MenuCtrl();
	void Destroy();
	void SetFont(CFont* font);
	void SetTextAlignment(int alignment);
	void SetMenuItemHeight(int menu_item_height);
	void SetMenuItemWidth(int menu_item_width);
	CArray<HICON>& GetMenuItemIconList() { return icon_list_; }
	void AddMenuItemIcon(HICON icon);
	void ChangeToOwnerDraw(MenuCtrl* menuctrl);

private:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	CArray<HICON> icon_list_;
	CFont* font_;
	int alignment_;
	int menu_item_width_;
	int menu_item_height_;
};