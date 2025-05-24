
#include "stdafx.h"

#define INCLUDE_MENUCTRL __declspec(dllexport)
#include "menuctrl.h"

MenuCtrl::MenuCtrl()
{
	font_ = NULL;
	alignment_ = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	menu_item_height_ = 40;
	menu_item_width_ = 60;
}

MenuCtrl::~MenuCtrl()
{

}

void MenuCtrl::Destroy() {

	for (int i = 0; i < icon_list_.GetSize(); i++) {
		DestroyIcon(icon_list_[i]);
	}
	icon_list_.RemoveAll();

	DestroyMenu();

	font_ = NULL;
}

void MenuCtrl::SetFont(CFont* font) {

	font_ = font;
}

void MenuCtrl::SetTextAlignment(int alignment) {

	alignment_ = alignment;
}

void MenuCtrl::SetMenuItemHeight(int menu_item_height) {

	menu_item_height_ = menu_item_height;
}

void MenuCtrl::SetMenuItemWidth(int menu_item_width) {

	menu_item_width_ = menu_item_width;
}

void MenuCtrl::AddMenuItemIcon(HICON icon) {

	icon_list_.Add(icon);
}

void MenuCtrl::ChangeToOwnerDraw(MenuCtrl *menuctrl)
{
	//CString str;                  //use to hold the caption temporarily
	//MenuCtrl* pMenu;               //use to hold the sub menu
	//							  //get the number of the menu items of the parent menu
	//int iMenuCount = menuctrl->GetMenuItemCount();
	//UINT nID; //use to hold the identifier of the menu items
	//for (int i = 0; i<iMenuCount; i++)
	//{
	//	//if (menuctrl->GetSubMenu(i)) //if the parent menu has sub menu
	//	//{
	//	//	menuctrl->ModifyMenu(i,
	//	//		MF_BYPOSITION | MF_OWNERDRAW,
	//	//		0,
	//	//		(LPCTSTR)NULL);
	//	//}
	//	//else
	//	//{
	//		nID = menuctrl->GetMenuItemID(i);
	//		menuctrl->ModifyMenu(i,
	//			MF_BYPOSITION | MF_OWNERDRAW,
	//			(UINT)nID,
	//			(LPCTSTR)NULL);
	//	//}
	//}
	//for (int i = 0; i<iMenuCount; i++)
	//{
	//	pMenu = 0;       //reset pointer for safety

	//	if (menuctrl->GetSubMenu(i)) //if the parent menu has sub menu
	//	{
	//		pMenu = new MenuCtrl;
	//		HMENU hMenu = menuctrl->GetSubMenu(i)->GetSafeHmenu();
	//		pMenu->Attach(hMenu);
	//		//ChangeToOwnerDraw(pMenu);
	//		for (int i = 0; i<2; i++)
	//		{
	//			nID = pMenu->GetMenuItemID(i);
	//			pMenu->ModifyMenu(i,
	//				MF_BYPOSITION | MF_OWNERDRAW,
	//				(UINT)nID,
	//				(LPCTSTR)NULL);
	//		}
	//	}
	//}
	//get the number of the menu items of the parent menu
	int iMenuCount = menuctrl->GetMenuItemCount();
	UINT nID;	//use to hold the identifier of the menu items
	for (int i = 0; i < iMenuCount; i++) {
		if (menuctrl->GetSubMenu(i)) { //if the parent menu has sub menu

			menuctrl->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, (UINT)0, (LPCTSTR)NULL);

			MenuCtrl* pMenu = new MenuCtrl;
			pMenu->Attach(menuctrl->GetSubMenu(i)->GetSafeHmenu());
			pMenu->SetFont(&Formation::font(Formation::FONT_SIZE::MEDIUM_FONT));
			pMenu->SetMenuItemHeight(menu_item_height_ - Formation::spacing4());
			pMenu->SetMenuItemWidth(menu_item_width_ - Formation::control_height() * 2);
			ChangeToOwnerDraw(pMenu);

		} else {
			nID = menuctrl->GetMenuItemID(i);
			menuctrl->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, (UINT)nID, (LPCTSTR)NULL);
		}
	}
}

void MenuCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//assign the height of the menu item
	lpMeasureItemStruct->itemHeight = menu_item_height_;
	//assign the width of the menu item
	lpMeasureItemStruct->itemWidth = menu_item_width_; //str.GetLength() * 7;
}

void MenuCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	////draw an orange background
	//pDC->FillSolidRect(&lpDrawItemStruct->rcItem,
	//	RGB(255, 150, 0));

	////if the menu item is selected
	//if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
	//	(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	//{
	//	//draw a blue background
	//	pDC->FillSolidRect(&lpDrawItemStruct->rcItem,
	//		RGB(0, 150, 255));
	//}

	//CString str;
	//GetMenuString(lpDrawItemStruct->itemID, str, MF_BYCOMMAND);

	////draw the caption of the menu item
	//pDC->TextOut(lpDrawItemStruct->rcItem.left,
	//	lpDrawItemStruct->rcItem.top, str);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (pDC != NULL) {
		
		CString str;
		GetMenuString(lpDrawItemStruct->itemID, str, MF_BYCOMMAND);

		if (font_ != NULL) {
			pDC->SelectObject(font_);
		}
		CRect rect(lpDrawItemStruct->rcItem);
		rect.DeflateRect(0, 0, 0, 1);
		
		//draw an orange background
		if (lpDrawItemStruct->itemState & ODS_DISABLED) {
			pDC->FillSolidRect(rect, DISABLE_COLOR);
		} else if (str.Trim() == Language::GetString(IDSTRINGT_PRINT_SCREEN)) {
			pDC->FillSolidRect(rect, LABEL_COLOR);
		} else if (str.Trim() == L"SPAN") {
			pDC->FillSolidRect(rect, BACKCOLOR1);
		} else {
			pDC->FillSolidRect(rect, BACKCOLOR1);
		}
		pDC->SetTextColor(WHITE_COLOR);

		//if the menu item is selected
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) && (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
	
			if (lpDrawItemStruct->itemState & ODS_DISABLED) {
				pDC->FillSolidRect(rect, DISABLE_COLOR);
			} else {
				pDC->FillSolidRect(rect, BACKCOLOR1_SEL);
			}
		}
	
		int icon_index = ::GetMenuPosFromID(GetSafeHmenu(), lpDrawItemStruct->itemID);
		if (icon_index < icon_list_.GetSize() && icon_list_.GetSize() > 0) {

			HICON icon = icon_list_[icon_index];

			ICONINFO iconinfo;
			::GetIconInfo(icon, &iconinfo);
			DWORD dw_width = (DWORD)(iconinfo.xHotspot * 2);
			DWORD dw_height = (DWORD)(iconinfo.yHotspot * 2);
			::DeleteObject(iconinfo.hbmMask);
			::DeleteObject(iconinfo.hbmColor);

			CRect icon_rect(lpDrawItemStruct->rcItem);
			icon_rect.top = icon_rect.CenterPoint().y - (dw_height / 2);
			icon_rect.bottom = icon_rect.top + dw_height;
			icon_rect.left += (icon_rect.top - lpDrawItemStruct->rcItem.top);
			icon_rect.right = icon_rect.left + dw_width;

			//DrawIconEx(pDC->GetSafeHdc(), icon_rect.left, icon_rect.top, icon, dw_width, dw_height, 0, NULL, DI_NORMAL);
			pDC->DrawState(icon_rect.TopLeft(), CSize(icon_rect.Width(), icon_rect.Height()), icon, (lpDrawItemStruct->itemState & ODS_DISABLED ? DSS_DISABLED : DSS_NORMAL), (CBrush*)NULL);

			rect.left = icon_rect.right + (icon_rect.top - lpDrawItemStruct->rcItem.top);

		} else {

			rect.left += Formation::spacing2();
		}

		//str = L" " + str + L" ";
		pDC->DrawText(Formation::PrepareString(*pDC, str, rect), rect, alignment_);
	}
}