/*----------------------------------------------------------------------
Copyright (C)2001 MJSoft. All Rights Reserved.
          This source may be used freely as long as it is not sold for
					profit and this copyright information is not altered or removed.
					Visit the web-site at www.mjsoft.co.uk
					e-mail comments to info@mjsoft.co.uk
File:     SortListCtrl.h
Purpose:  Provides a sortable list control, it will sort text, numbers
          and dates, ascending or descending, and will even draw the
					arrows just like windows explorer!
----------------------------------------------------------------------*/

#pragma once

#include "sortheaderctrl.h"

#ifndef INCLUDE_SORTLISTCTRL
#define INCLUDE_SORTLISTCTRL __declspec(dllimport)
#endif //INCLUDE_SORTLISTCTRL

class INCLUDE_SORTLISTCTRL SortListCtrl : public CListCtrl
{
public:
	SortListCtrl();
	virtual ~SortListCtrl();

	BOOL SetHeadings(UINT uiStringID);
	BOOL SetHeadings( const CString& strHeadings );

	int AddItem(LPCTSTR pszText, ... );
	int AddItem2(LPCTSTR pszText, ...);
	BOOL DeleteItem( int iItem );
	BOOL DeleteAllItems();
	void LoadColumnInfo();
	void SaveColumnInfo();
	BOOL SetText( int nItem, int nSubItem, LPCTSTR lpszText );
	BOOL SetSortText(int nItem, int nSubItem, LPCTSTR lpszText);
	CString GetSortText(int nItem, int nSubItem);
	void Sort( int iColumn, BOOL bAscending );
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;
	void SetColumnType(int column_index, int column_type); //0-Text, 1-IntNumber, 2-Date/Time, 3-Image, 4-FloatNumber
	void SetDateFormat(CString date_format) { date_format_ = date_format; }
	void Create(CRect wnd_rect, BOOL allow_sorting);
	void SetHighlightTextColor(COLORREF Color);
	void SetHighlightColor(COLORREF Color);
	
protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

	static int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);
	void FreeItemMemory( const int iItem );

private:
	COLORREF m_highlight;
	COLORREF m_highlighttext;

	SortHeaderCtrl m_ctlHeader;
	BOOL allow_sorting_;
	std::vector<DWORD> column_type_; //0-Text, 1-Number, 2-Date/Time, 3-Image
	CString date_format_;

	bool m_item_selected;
	int m_iNumColumns;
	int m_iSortColumn;
	BOOL m_bSortAscending;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
