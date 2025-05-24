/*----------------------------------------------------------------------
Copyright (C)2001 MJSoft. All Rights Reserved.
          This source may be used freely as long as it is not sold for
					profit and this copyright information is not altered or removed.
					Visit the web-site at www.mjsoft.co.uk
					e-mail comments to info@mjsoft.co.uk
File:     SortListCtrl.cpp
Purpose:  Provides a sortable list control, it will sort text, numbers
          and dates, ascending or descending, and will even draw the
					arrows just like windows explorer!
----------------------------------------------------------------------*/

#include "stdafx.h"

#define INCLUDE_SORTLISTCTRL __declspec(dllexport)
#include "SortListCtrl.h"

LPCTSTR g_pszSection = _T("ListCtrls");

struct ItemData
{
public:
	ItemData() : arrpsz( NULL ), arrpsz_sort(NULL), dwData( NULL ) {}

	LPTSTR* arrpsz;
	LPTSTR* arrpsz_sort;
	DWORD dwData;

private:
	// ban copying.
	ItemData( const ItemData& );
	ItemData& operator=( const ItemData& );
};

SortListCtrl::SortListCtrl() : m_iNumColumns( 0 ), m_iSortColumn( -1 ), m_bSortAscending( TRUE )
{ 
}

SortListCtrl::~SortListCtrl()
{
}

BEGIN_MESSAGE_MAP(SortListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SortListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void SortListCtrl::Create(CRect wnd_rect, BOOL allow_sorting) {

	allow_sorting_ = allow_sorting;
	date_format_ = STD_DATE_FORMAT;
	//CRect rect;
	//GetClientRect(rect);

	//TO HIDE BOTH SCROLL BARS
	CRect client_rect_ = wnd_rect;
	GetClientRect(client_rect_);
	int cxvs = GetSystemMetrics(SM_CXVSCROLL); //Get the system metrics - VERT
	int cyvs = GetSystemMetrics(SM_CYVSCROLL); //Get the system metrics - HORZ
	//Here we set the position of the window to the clientrect + the size of the scrollbars
	SetWindowPos(NULL, client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.bottom, SWP_NOMOVE | SWP_NOZORDER);

	/*HRGN client_rgn = CreateRectRgn(client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.bottom);
	SetWindowRgn(client_rgn, TRUE);
	DeleteObject(client_rgn);*/
}

void SortListCtrl::PreSubclassWindow()
{
	ASSERT( GetStyle() & LVS_REPORT );

	CListCtrl::PreSubclassWindow();
	VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );
}

BOOL SortListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch (((NMHDR*)lParam)->code)
	{
		case HDN_BEGINTRACKW:
		case HDN_BEGINTRACKA:
		{
			//HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;
			//if( pHDN->iItem == 0)            // Prevent only first (col# 0)
			*pResult = TRUE;                // disable tracking
		}
		return TRUE;
	}
	
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

BOOL SortListCtrl::SetHeadings(UINT uiStringID) {

	CString strHeadings;
	VERIFY(strHeadings.LoadString(uiStringID));
	return SetHeadings(strHeadings);
}

// the heading text is in the format column 1 text,column 1 width;column 2 text,column 3 width;etc.
BOOL SortListCtrl::SetHeadings( const CString& strHeadings ) {

	column_type_.clear();

	int iStart = 0;
	for( ;; ) {

		const int iComma = strHeadings.Find(_T(','), iStart); //TEXT
		if(iComma == -1)
			break;
		const CString strHeading = strHeadings.Mid(iStart, iComma - iStart);
		iStart = iComma + 1;

		int iSemiColon = strHeadings.Find(_T(';'), iStart); //WIDTH
		if(iSemiColon == -1)
			iSemiColon = strHeadings.GetLength();
		const int iWidth = _wtoi(strHeadings.Mid(iStart, iSemiColon - iStart));
		iStart = iSemiColon + 1;

		int iColon = strHeadings.Find(_T(':'), iStart); //TYPE
		if (iColon == -1)
			iColon = strHeadings.GetLength();
		const int column_align = _wtoi(strHeadings.Mid(iStart, iColon - iStart));
		iStart = iColon + 1;
		iStart++;

		if (strHeading.IsEmpty()) {
			LVCOLUMN lvCol;
			lvCol.mask = LVIF_IMAGE;
			lvCol.cx = iWidth;
			lvCol.iImage = 0;
			lvCol.iSubItem = m_iNumColumns;
			lvCol.fmt = LVCFMT_CENTER | HDF_EX_FIXEDWIDTH;
			InsertColumn(m_iNumColumns, &lvCol);
			column_type_.push_back(3);
		} else {
			InsertColumn(m_iNumColumns, strHeading, column_align | LVCFMT_FIXED_WIDTH, iWidth);
			column_type_.push_back(0);
		}

		m_iNumColumns++;
	}

	return TRUE;
}

void SortListCtrl::SetColumnType(int column_index, int column_type) {

	if (column_index >= 0 && column_index < int(column_type_.size())) {
		column_type_[column_index] = column_type;
	}
}

int SortListCtrl::AddItem(LPCTSTR pszText, ...) {

	LVITEM lvItem = { 0 };
	lvItem.mask = LVIF_IMAGE;
	lvItem.iImage = 0;
	//lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	int iIndex = InsertItem(&lvItem);

	LPTSTR* arrpsz = new LPTSTR[m_iNumColumns];
	LPTSTR* arrpsz_sort = new LPTSTR[m_iNumColumns];

 	va_list list;
	va_start(list, pszText);

	arrpsz[0] = new TCHAR[lstrlen(pszText) + 1];
	(void)lstrcpy(arrpsz[0], pszText);
	arrpsz_sort[0] = new TCHAR[lstrlen(pszText) + 1];
	(void)lstrcpy(arrpsz_sort[0], pszText);

	for (int iColumn = 1; iColumn < m_iNumColumns; iColumn++) {

		pszText = va_arg(list, LPCTSTR);

		if (column_type_[iColumn] == 3) {
			VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_IMAGE, pszText, 1, 0, 0, 0));
		} else {
			VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0));
		}

		arrpsz[iColumn] = new TCHAR[lstrlen(pszText) + 1];
		(void)lstrcpy(arrpsz[iColumn], pszText);
		arrpsz_sort[iColumn] = new TCHAR[lstrlen(pszText) + 1];
		(void)lstrcpy(arrpsz_sort[iColumn], pszText);
	}

	va_end(list);

	ItemData* pid = new ItemData;
	pid->arrpsz = arrpsz;
	pid->arrpsz_sort = arrpsz_sort;
	VERIFY(CListCtrl::SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(pid)));

	return iIndex;
}

int SortListCtrl::AddItem2(LPCTSTR pszText, ...) {

	LVITEM lvItem = { 0 };
	lvItem.mask = LVIF_IMAGE;
	lvItem.iImage = 0;
	//lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	int iIndex = InsertItem(&lvItem);
	
	LPTSTR* arrpsz = new LPTSTR[m_iNumColumns];
	LPTSTR* arrpsz_sort = new LPTSTR[m_iNumColumns];

 	va_list list;
	va_start(list, pszText);

	arrpsz[0] = new TCHAR[lstrlen(pszText) + 1];
	(void)lstrcpy(arrpsz[0], pszText);

	pszText = va_arg(list, LPCTSTR);

	arrpsz_sort[0] = new TCHAR[lstrlen(pszText) + 1];
	(void)lstrcpy(arrpsz_sort[0], pszText);

	for (int iColumn = 1; iColumn < m_iNumColumns; iColumn++) {

		pszText = va_arg(list, LPCTSTR);

		if (column_type_[iColumn] == 3) {
			VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_IMAGE, pszText, 1, 0, 0, 0));
		} else {
			VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0));
		}

		arrpsz[iColumn] = new TCHAR[lstrlen(pszText) + 1];
		(void)lstrcpy(arrpsz[iColumn], pszText);

		pszText = va_arg(list, LPCTSTR);

		arrpsz_sort[iColumn] = new TCHAR[lstrlen(pszText) + 1];
		(void)lstrcpy(arrpsz_sort[iColumn], pszText);
	}

	va_end(list);

	ItemData* pid = new ItemData;
	pid->arrpsz = arrpsz;
	pid->arrpsz_sort = arrpsz_sort;
	VERIFY(CListCtrl::SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(pid)));

	return iIndex;
}

void SortListCtrl::FreeItemMemory( const int iItem )
{
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );

	LPTSTR* arrpsz = pid->arrpsz;
	LPTSTR* arrpsz_sort = pid->arrpsz_sort;

	for (int i = 0; i < m_iNumColumns; i++) {
		delete[] arrpsz[i];
		delete[] arrpsz_sort[i];
	}

	delete[] arrpsz;
	delete[] arrpsz_sort;
	delete pid;

	VERIFY( CListCtrl::SetItemData( iItem, NULL ) );
}

BOOL SortListCtrl::DeleteItem( int iItem )
{
	FreeItemMemory(iItem);
	return CListCtrl::DeleteItem( iItem );
}

BOOL SortListCtrl::DeleteAllItems()
{
	for (int iItem = 0; iItem < GetItemCount(); iItem++)
		FreeItemMemory(iItem);

	return CListCtrl::DeleteAllItems();
}

bool IsNumber( LPCTSTR pszText )
{
	for( int i = 0; i < lstrlen( pszText ); i++ )
		if( !_istdigit( pszText[ i ] ) )
			return false;

	return true;
}

int NumberCompare( LPCTSTR pszNumber1, LPCTSTR pszNumber2 )
{
	const int iNumber1 = _wtoi(pszNumber1);
	const int iNumber2 = _wtoi(pszNumber2);
	if( iNumber1 < iNumber2 )
		return -1;
	if( iNumber1 > iNumber2 )
		return 1;

	return 0;
}

int FloatNumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2)
{
	const float iNumber1 = float(_wtof(pszNumber1));
	const float iNumber2 = float(_wtof(pszNumber2));
	if (iNumber1 < iNumber2)
		return -1;
	if (iNumber1 > iNumber2)
		return 1;

	return 0;
}

bool IsDate( LPCTSTR pszText )
{
	// format should be 99/99/9999.

	if( lstrlen( pszText ) != 20 )
		return false;

	return _istdigit( pszText[ 0 ] )
		&& _istdigit( pszText[ 1 ] )
		&& pszText[ 2 ] == _T('/')
		&& _istdigit( pszText[ 3 ] )
		&& _istdigit( pszText[ 4 ] )
		&& pszText[ 5 ] == _T('/')
		&& _istdigit( pszText[ 6 ] )
		&& _istdigit( pszText[ 7 ] )
		&& _istdigit( pszText[ 8 ] )
		&& _istdigit( pszText[ 9 ] );
}

int DateCompare( const CString& strDate1, const CString& strDate2 )
{
	const int iYear1 = _wtoi(strDate1.Mid(6, 4));
	const int iYear2 = _wtoi(strDate2.Mid(6, 4));
	if( iYear1 < iYear2 )
		return -1;
	if( iYear1 > iYear2 )
		return 1;

	const int iMonth1 = _wtoi(strDate1.Mid(3, 2));
	const int iMonth2 = _wtoi(strDate2.Mid(3, 2));
	if( iMonth1 < iMonth2 )
		return -1;
	if( iMonth1 > iMonth2 )
		return 1;

	const int iDay1 = _wtoi(strDate1.Mid(0, 2));
	const int iDay2 = _wtoi(strDate2.Mid(0, 2));
	if( iDay1 < iDay2 )
		return -1;
	if( iDay1 > iDay2 )
		return 1;

	const int iHour1 = _wtoi(strDate1.Mid(strDate1.GetLength() - 8, 2));
	const int iHour2 = _wtoi(strDate2.Mid(strDate2.GetLength() - 8, 2));
	if (iHour1 < iHour2)
		return -1;
	if (iHour1 > iHour2)
		return 1;

	const int iMinute1 = _wtoi(strDate1.Mid(strDate1.GetLength() - 5, 2));
	const int iMinute2 = _wtoi(strDate2.Mid(strDate2.GetLength() - 5, 2));
	if (iMinute1 < iMinute2)
		return -1;
	if (iMinute1 > iMinute2)
		return 1;

	const int iSecond1 = _wtoi(strDate1.Mid(strDate1.GetLength() - 2, 2));
	const int iSecond2 = _wtoi(strDate2.Mid(strDate2.GetLength() - 2, 2));
	if (iSecond1 < iSecond2)
		return -1;
	if (iSecond1 > iSecond2)
		return 1;

	return 0;
}

int CALLBACK SortListCtrl::CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	SortListCtrl* pListCtrl = (SortListCtrl*)lParamData;
	ASSERT( pListCtrl->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );

	ItemData* pid1 = (ItemData*)lParam1;
	ItemData* pid2 = (ItemData*)lParam2;
	
	ASSERT( pid1 );
	ASSERT( pid2 );

	LPCTSTR pszText1 = pid1->arrpsz_sort[ pListCtrl->m_iSortColumn ];
	LPCTSTR pszText2 = pid2->arrpsz_sort[ pListCtrl->m_iSortColumn ];

	if (pListCtrl->column_type_[pListCtrl->m_iSortColumn] == 0) { //TEXT
		return pListCtrl->m_bSortAscending ? lstrcmp(pszText1, pszText2) : lstrcmp(pszText2, pszText1);
	} else if (pListCtrl->column_type_[pListCtrl->m_iSortColumn] == 1) { //INT-NUMBER
		return pListCtrl->m_bSortAscending ? NumberCompare(pszText1, pszText2) : NumberCompare(pszText2, pszText1);
	} else if (pListCtrl->column_type_[pListCtrl->m_iSortColumn] == 2) { //DATE/TIME
		/*CString strDate1(pszText1);
		CString strDate2(pszText2);
		CString str_date1 = Formation::GetDate(strDate1.Mid(0, strDate1.GetLength() - 8).Trim(), Formation::DateFormat(), pListCtrl->date_format_);
		CString str_date2 = Formation::GetDate(strDate2.Mid(0, strDate2.GetLength() - 8).Trim(), Formation::DateFormat(), pListCtrl->date_format_);
		strDate1 = str_date1 + L" " + strDate1.Mid(strDate1.GetLength() - 8, 8);
		strDate2 = str_date2 + L" " + strDate2.Mid(strDate2.GetLength() - 8, 8);
		return pListCtrl->m_bSortAscending ? DateCompare(strDate1, strDate2) : DateCompare(strDate2, strDate1);*/
		return pListCtrl->m_bSortAscending ? DateCompare(pszText1, pszText2) : DateCompare(pszText2, pszText1);
	} else if (pListCtrl->column_type_[pListCtrl->m_iSortColumn] == 4) { //FLOAT-NUMBER
		return pListCtrl->m_bSortAscending ? FloatNumberCompare(pszText1, pszText2) : FloatNumberCompare(pszText2, pszText1);
	}

	/*if( IsNumber( pszText1 ) )
		return pListCtrl->m_bSortAscending ? NumberCompare( pszText1, pszText2 ) : NumberCompare( pszText2, pszText1 );
	else if( IsDate( pszText1 ) )
		return pListCtrl->m_bSortAscending ? DateCompare( pszText1, pszText2 ) : DateCompare( pszText2, pszText1 );
	else
		return pListCtrl->m_bSortAscending ? lstrcmp( pszText1, pszText2 ) : lstrcmp( pszText2, pszText1 );*/

	return 0;
}

void SortListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;

	if (column_type_[iColumn] != 3) {
		// if it's a second click on the same column then reverse the sort order,
		// otherwise sort the new column in ascending order.
		Sort(iColumn, iColumn == m_iSortColumn ? !m_bSortAscending : TRUE);
	}

	*pResult = 0;
}

void SortListCtrl::Sort( int iColumn, BOOL bAscending )
{
	m_iSortColumn = iColumn;
	m_bSortAscending = bAscending;

	// show the appropriate arrow in the header control.
	m_ctlHeader.SetSortArrow( m_iSortColumn, m_bSortAscending );
	VERIFY( SortItems( CompareFunction, reinterpret_cast<DWORD_PTR>( this ) ) );
}

void SortListCtrl::LoadColumnInfo()
{
	// you must call this after setting the column headings.
	ASSERT( m_iNumColumns > 0 );

	CString strKey;
	strKey.Format( _T("%d"), GetDlgCtrlID() );

	UINT nBytes = 0;
	BYTE* buf = NULL;
	if( AfxGetApp()->GetProfileBinary( g_pszSection, strKey, &buf, &nBytes ) )
	{
		if( nBytes > 0 )
		{
			CMemFile memFile( buf, nBytes );
			CArchive ar( &memFile, CArchive::load );
			m_ctlHeader.Serialize( ar );
			ar.Close();

			m_ctlHeader.Invalidate();
		}

		delete[] buf;
	}
}

void SortListCtrl::SaveColumnInfo()
{
	ASSERT( m_iNumColumns > 0 );

	CString strKey;
	strKey.Format( _T("%d"), GetDlgCtrlID() );

	CMemFile memFile;

	CArchive ar( &memFile, CArchive::store );
	m_ctlHeader.Serialize( ar );
	ar.Close();

	DWORD dwLen = (DWORD)memFile.GetLength();
	BYTE* buf = memFile.Detach();	

	VERIFY( AfxGetApp()->WriteProfileBinary( g_pszSection, strKey, buf, dwLen ) );

	free( buf );
}

void SortListCtrl::OnDestroy() 
{
	DeleteAllItems();

	CListCtrl::OnDestroy();
}

BOOL SortListCtrl::SetText( int nItem, int nSubItem, LPCTSTR lpszText )
{
	if (nItem >= GetItemCount())
		return FALSE;

	if( !CListCtrl::SetItemText( nItem, nSubItem, lpszText ) )
		return FALSE;

	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	LPTSTR pszText = pid->arrpsz[nSubItem];
	delete[] pszText;
	pszText = new TCHAR[lstrlen(lpszText) + 1];
	(void)lstrcpy(pszText, lpszText);
	pid->arrpsz[nSubItem] = pszText;

	return TRUE;
}

BOOL SortListCtrl::SetSortText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	if (nItem >= GetItemCount())
		return FALSE;
	
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	LPTSTR pszText = pid->arrpsz_sort[nSubItem];
	delete[] pszText;
	pszText = new TCHAR[lstrlen(lpszText) + 1];
	(void)lstrcpy(pszText, lpszText);
	pid->arrpsz_sort[nSubItem] = pszText;
	
	return TRUE;
}

CString SortListCtrl::GetSortText(int nItem, int nSubItem) {

	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	return (pid->arrpsz_sort[nSubItem]);
}

BOOL SortListCtrl::SetItemData( int nItem, DWORD dwData )
{
	if( nItem >= GetItemCount() )
		return FALSE;

	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( nItem ) );
	pid->dwData = dwData;

	return TRUE;
}

DWORD SortListCtrl::GetItemData( int nItem ) const
{
	ASSERT( nItem < GetItemCount() );

	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( nItem ) );
	ASSERT( pid );
	return pid->dwData;
}

void SortListCtrl::SetHighlightTextColor(COLORREF Color)
{
	this->m_highlighttext = Color;
}

void SortListCtrl::SetHighlightColor(COLORREF Color)
{
	this->m_highlight = Color;
}

void SortListCtrl::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	// first, lets extract data from
	// the message for ease of use later
	NMLVCUSTOMDRAW* pNMLVCUSTOMDRAW = (NMLVCUSTOMDRAW*)pNMHDR;

	// we'll copy the device context into hdc
	// but wont convert it to a pDC* until (and if)
	// we need it as this requires a bit of work
	// internally for MFC to create temporary CDC
	// objects
	HDC hdc = pNMLVCUSTOMDRAW->nmcd.hdc;
	CDC* pDC = NULL;

	// here is the item info
	// note that we don't get the subitem
	// number here, as this may not be
	// valid data except when we are
	// handling a sub item notification
	// so we'll do that separately in
	// the appropriate case statements
	// below.
	int nItem = (int)pNMLVCUSTOMDRAW->nmcd.dwItemSpec;
	UINT nState = pNMLVCUSTOMDRAW->nmcd.uItemState;
	LPARAM lParam = pNMLVCUSTOMDRAW->nmcd.lItemlParam;

	// next we set up flags that will control
	// the return value for *pResult
	bool bNotifyPostPaint = false;
	bool bNotifyItemDraw = false;
	bool bNotifySubItemDraw = false;
	bool bSkipDefault = false;

	// what we do next depends on the
	// drawing stage we are processing
	switch (pNMLVCUSTOMDRAW->nmcd.dwDrawStage) {
		case CDDS_PREPAINT:
		{
			// PrePaint
			bNotifyPostPaint = false;
			bNotifyItemDraw = true;
			pNMLVCUSTOMDRAW->nmcd.rc;
		}
		break;
		case CDDS_ITEMPREPAINT:
		{
			// Item PrePaint
			//
			// set up a different font to use, if any
			if (!pDC) pDC = CDC::FromHandle(hdc);

			bNotifyPostPaint = false;
			bNotifySubItemDraw = true;

			m_item_selected = false;
			m_item_selected = this->GetItemState(nItem, LVIS_SELECTED) != 0;
			if (m_item_selected)
			{
				this->SetItemState(nItem, NULL, LVIS_SELECTED);
				bNotifyPostPaint = true;
			}
		}
		break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{	
			// Sub Item PrePaint
			// set sub item number (data will be valid now)
			
			int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
			bNotifyPostPaint = false;
			pNMLVCUSTOMDRAW->clrFace = BLACK_COLOR;
			// set up a different font to use, if any
			if (!pDC) pDC = CDC::FromHandle(hdc);
			//         CFont* pNewFont = FontForItem(nItem,nSubItem,lParam,pNMLVCUSTOMDRAW);
			bool set = false;
			if (!m_item_selected)
				pNMLVCUSTOMDRAW->clrTextBk = CListCtrl::GetTextBkColor();
			else
				pNMLVCUSTOMDRAW->clrTextBk = m_highlight;
			if (m_item_selected)
				pNMLVCUSTOMDRAW->clrText = m_highlighttext;
			else
				pNMLVCUSTOMDRAW->clrText = CListCtrl::GetTextColor();

			//Not working
			/*CRect rc;
			CString itemText = GetItemText(pNMLVCUSTOMDRAW->nmcd.dwItemSpec, pNMLVCUSTOMDRAW->iSubItem);
			GetSubItemRect(pNMLVCUSTOMDRAW->nmcd.dwItemSpec, pNMLVCUSTOMDRAW->iSubItem, LVIR_BOUNDS, rc);
			pDC->FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
			GetSubItemRect(pNMLVCUSTOMDRAW->nmcd.dwItemSpec, pNMLVCUSTOMDRAW->iSubItem, LVIR_LABEL, rc);
			pDC->DrawText(itemText, rc, DT_LEFT);*/

			bNotifyPostPaint = true;    // need to restore old font
		}
		break;
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{
			// Sub Item PostPaint
			// set sub item number (data will be valid now)
			int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
			// restore old font if any
			if (!pDC) pDC = CDC::FromHandle(hdc);
		}
		break;
		case CDDS_ITEMPOSTPAINT:
		{
			// Item PostPaint
			// restore old font if any
			if (!pDC) pDC = CDC::FromHandle(hdc);
			if (m_item_selected)
			{
				m_item_selected = false;
				this->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
		break;
	}

	ASSERT(CDRF_DODEFAULT == 0);
	*pResult = 0;
	if (bNotifyPostPaint) {
		*pResult |= CDRF_NOTIFYPOSTPAINT;
	}
	if (bNotifyItemDraw) {
		*pResult |= CDRF_NOTIFYITEMDRAW;
	}
	if (bNotifySubItemDraw) {
		*pResult |= CDRF_NOTIFYSUBITEMDRAW;
	}
	if (bSkipDefault) {
		*pResult |= CDRF_SKIPDEFAULT;
	}
	if (*pResult == 0) {
		// redundant as CDRF_DODEFAULT==0 anyway
		// but shouldn't depend on this in our code
		*pResult = CDRF_DODEFAULT;
	}
}

void SortListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{	
	POSITION pos = GetFirstSelectedItemPosition();
	int prev_item_index = GetNextSelectedItem(pos);

	CListCtrl::OnLButtonDown(nFlags, point);

	pos = GetFirstSelectedItemPosition();
	int item_index = GetNextSelectedItem(pos);

	if (prev_item_index != -1 && item_index == -1) {
		SetItemState(prev_item_index, LVIS_SELECTED, LVIS_SELECTED);
	}
}

BOOL SortListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONUP || pMsg->message == WM_RBUTTONDBLCLK)
		pMsg->message = NULL;

	return CListCtrl::PreTranslateMessage(pMsg);
}