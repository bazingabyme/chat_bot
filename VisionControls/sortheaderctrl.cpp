#include "stdafx.h"
#include "sortheaderctrl.h"

SortHeaderCtrl::SortHeaderCtrl() : m_iSortColumn( -1 ), m_bSortAscending( TRUE ) {

	height_ = 40;
	m_iSpacing = 5;
	m_sizeArrow.cx = 10;
	m_sizeArrow.cy = 10;
	m_sizeImage.cx = 0;
	m_sizeImage.cy = 0;

	m_iHotIndex = -1;
	m_nClickFlags = 0;
	m_iSortColumn = -1;

	back_color_ = RGB(255, 255, 255);
	text_color_ = RGB(0, 0, 0);
}

SortHeaderCtrl::~SortHeaderCtrl() {

}

BEGIN_MESSAGE_MAP(SortHeaderCtrl, CHeaderCtrl)
	ON_MESSAGE(HDM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void SortHeaderCtrl::SetSortArrow( const int iSortColumn, const BOOL bSortAscending ) {

	m_iSortColumn = iSortColumn;
	m_bSortAscending = bSortAscending;

	// change the item to owner drawn.
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	VERIFY( GetItem( iSortColumn, &hditem ) );
	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY( SetItem( iSortColumn, &hditem ) );

	// invalidate the header control so it gets redrawn
	Invalidate();
}

BOOL SortHeaderCtrl::OnEraseBkgnd(CDC* pDC) {

	return TRUE;
}

void SortHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	CMemHeaderDC MemDC(&dc);
	DrawCtrl(&MemDC);
}

LRESULT SortHeaderCtrl::OnNcHitTest(CPoint point)
{
	m_hdhtiHotItem.pt = point;
	ScreenToClient(&m_hdhtiHotItem.pt);

	m_iHotIndex = (INT)SendMessage(HDM_HITTEST, 0, (LPARAM)(&m_hdhtiHotItem));

	return CHeaderCtrl::OnNcHitTest(point);
}

void SortHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nClickFlags = nFlags;
	
	if (m_iHotIndex >= 0) {
		if (m_hdhtiHotItem.flags & HHT_ONHEADER) {
			RECT rectItem;
			VERIFY(GetItemRect(m_iHotIndex, &rectItem));
			InvalidateRect(&rectItem);
		}
	}

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void SortHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nClickFlags = nFlags;

	if (m_iHotIndex >= 0) {
		if (m_hdhtiHotItem.flags & HHT_ONHEADER) {
			RECT rectItem;
			VERIFY(GetItemRect(m_iHotIndex, &rectItem));
			InvalidateRect(&rectItem);
		}
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

LRESULT SortHeaderCtrl::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	CImageList* pImageList;
	pImageList = CImageList::FromHandle((HIMAGELIST)lParam);

	IMAGEINFO info;
	if (pImageList->GetImageInfo(0, &info)) {
		m_sizeImage.cx = info.rcImage.right - info.rcImage.left;
		m_sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	return Default();
}

LRESULT SortHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);

	LPHDLAYOUT lphdlayout = (LPHDLAYOUT)lParam;
	//if (m_bStaticBorder)
	//	lphdlayout->prc->right += GetSystemMetrics(SM_CXBORDER);
	lphdlayout->prc->top = height_;
	lphdlayout->pwpos->cx -= 2;
	lphdlayout->pwpos->cy = height_;

	m_sizeArrow.cx = height_ / 4;
	m_sizeArrow.cy = height_ / 4;

	m_iSpacing = m_sizeArrow.cx / 2;

	return   lResult;
}

void SortHeaderCtrl::DrawCtrl(CDC* pDC)
{
	CRect rectClip;
	if (pDC->GetClipBox(&rectClip) == ERROR)
		return;

	CRect rectClient, rectItem;
	GetClientRect(&rectClient);
	pDC->FillSolidRect(rectClip, back_color_);

	INT iWidth = 0;
	INT iItems = GetItemCount();

	pDC->SelectObject(GetFont());
	pDC->SetTextColor(text_color_);
	
	for (INT i = 0; i<iItems; i++) {

		INT iItem = OrderToIndex(i);
		TCHAR szText[255];
		HDITEM hditem;
		hditem.mask = HDI_WIDTH | HDI_FORMAT | HDI_TEXT | HDI_IMAGE;
		hditem.pszText = szText;
		hditem.cchTextMax = sizeof(szText);
		VERIFY(GetItem(iItem, &hditem));
		VERIFY(GetItemRect(iItem, rectItem));

		if (rectItem.right >= rectClip.left || rectItem.left <= rectClip.right) {

			rectItem.DeflateRect(m_iSpacing, 0);
			DrawItem(pDC, rectItem, &hditem, iItem == m_iSortColumn, m_bSortAscending);
			rectItem.InflateRect(m_iSpacing, 0);

			if (m_nClickFlags & MK_LBUTTON && m_iHotIndex == iItem && m_hdhtiHotItem.flags & HHT_ONHEADER)
				pDC->InvertRect(rectItem);

			if (i != 0 && i != 1 && i != (iItems)) { //DRAW VERTICAL LINES BETWEEN HEADERS
				pDC->SelectObject(&Formation::disablecolor_pen());
				pDC->MoveTo(rectItem.left, rectItem.CenterPoint().y - Formation::spacing2());
				pDC->LineTo(rectItem.left, rectItem.CenterPoint().y + Formation::spacing2());
			}
		}
		iWidth += hditem.cxy;
	}

	if (iWidth > 0) {
		if (back_color_ == WHITE_COLOR) {
			pDC->SelectObject(&Formation::disablecolor_pen());
			rectClient.right = rectClient.left + iWidth;
			pDC->MoveTo(rectClient.left, rectClient.bottom - 1);
			pDC->LineTo(rectClient.right, rectClient.bottom - 1);
		}
	}
}

void SortHeaderCtrl::DrawItem(CDC* pDC, CRect rect, LPHDITEM lphdi, BOOL bSort, BOOL bSortAscending)
{
	ASSERT(lphdi->mask & HDI_FORMAT);

	INT iWidth = 0;

	CBitmap* pBitmap = NULL;
	BITMAP BitmapInfo;
	if (lphdi->fmt & HDF_BITMAP)
	{
		ASSERT(lphdi->mask & HDI_BITMAP);
		ASSERT(lphdi->hbm);

		pBitmap = CBitmap::FromHandle(lphdi->hbm);
		if (pBitmap)
			VERIFY(pBitmap->GetObject(sizeof(BITMAP), &BitmapInfo));
	}

	switch (lphdi->fmt&HDF_JUSTIFYMASK)
	{
	case HDF_LEFT:
		rect.left += (iWidth = DrawImage(pDC, rect, lphdi, FALSE)) ? iWidth + m_iSpacing : 0;
		if (lphdi->fmt & HDF_IMAGE && !iWidth)
			break;
		//rect.right -= bSort ? m_iSpacing + m_sizeArrow.cx : 0;
		//rect.left += (iWidth = DrawText(pDC, rect, lphdi)) ? iWidth + m_iSpacing : 0;
		if (bSort) {
			//rect.right += m_iSpacing + m_sizeArrow.cx;
			//rect.left += DrawArrow(pDC, rect, bSortAscending, FALSE) + m_iSpacing;
			DrawArrow(pDC, rect, bSortAscending, TRUE);
			rect.right -= m_sizeArrow.cx;
		}
		DrawText(pDC, rect, lphdi);
		//DrawBitmap(pDC, rect, lphdi, pBitmap, &BitmapInfo, TRUE);
		break;

	case HDF_CENTER:
		rect.left += (iWidth = DrawImage(pDC, rect, lphdi, FALSE)) ? iWidth + m_iSpacing : 0;
		if (lphdi->fmt & HDF_IMAGE && !iWidth)
			break;

		//rect.left += bSort ? m_iSpacing + m_sizeArrow.cx : 0;
		//rect.right -= (iWidth = DrawBitmap(pDC, rect, lphdi, pBitmap, &BitmapInfo, TRUE)) ? iWidth + m_iSpacing : 0;
		if (bSort) {
			//rect.left -= m_iSpacing + m_sizeArrow.cx;
			//rect.right -= DrawArrow(pDC, rect, bSortAscending, TRUE) + 2 * m_iSpacing;
			DrawArrow(pDC, rect, bSortAscending, TRUE);
			rect.right -= m_sizeArrow.cx;
		}
		DrawText(pDC, rect, lphdi);
		break;

	case HDF_RIGHT:
		//if (!(lphdi->fmt & HDF_BITMAP_ON_RIGHT))
		//	rect.left += (iWidth = DrawBitmap(pDC, rect, lphdi, pBitmap, &BitmapInfo, FALSE)) ? iWidth + m_iSpacing : 0;

		rect.left += (iWidth = DrawImage(pDC, rect, lphdi, FALSE)) ? iWidth + m_iSpacing : 0;
		if (lphdi->fmt & HDF_IMAGE && !iWidth)
			break;

		//rect.left += bSort && (lphdi->fmt&HDF_BITMAP_ON_RIGHT) ? m_iSpacing + m_sizeArrow.cx : 0;
		//if (lphdi->fmt&HDF_BITMAP_ON_RIGHT)
		//	rect.right -= (iWidth = DrawBitmap(pDC, rect, lphdi, pBitmap, &BitmapInfo, TRUE)) ? iWidth + m_iSpacing : 0;
		if (bSort) {
			//rect.left -= (lphdi->fmt&HDF_BITMAP_ON_RIGHT) ? m_iSpacing + m_sizeArrow.cx : 0;
			//rect.right -= DrawArrow(pDC, rect, bSortAscending, TRUE) + 2 * m_iSpacing;
			DrawArrow(pDC, rect, bSortAscending, TRUE);
			rect.right -= m_sizeArrow.cx;
		}
		DrawText(pDC, rect, lphdi);
		break;
	}
}

INT SortHeaderCtrl::DrawImage(CDC* pDC, CRect rect, LPHDITEM lphdi, BOOL bRight)
{
	CImageList* pImageList = GetImageList();
	INT iWidth = 0;

	if (lphdi->mask & HDI_IMAGE && lphdi->fmt & HDF_IMAGE)
	{
		ASSERT(pImageList);
		ASSERT(lphdi->iImage >= 0 && lphdi->iImage<pImageList->GetImageCount());

		if (rect.Width()>0) {

			POINT point;
			point.y = rect.CenterPoint().y - (m_sizeImage.cy >> 1);
			if (bRight)
				point.x = rect.right - m_sizeImage.cx;
			else
				point.x = rect.left;

			SIZE size;
			size.cx = rect.Width()<m_sizeImage.cx ? rect.Width() : m_sizeImage.cx;
			size.cy = m_sizeImage.cy;
			pImageList->DrawIndirect(pDC, lphdi->iImage, point, size, CPoint(0, 0));

			iWidth = m_sizeImage.cx;
		}
	}

	return iWidth;
}

INT SortHeaderCtrl::DrawBitmap(CDC* pDC, CRect rect, LPHDITEM lphdi, CBitmap* pBitmap, BITMAP* pBitmapInfo, BOOL bRight)
{
	INT iWidth = 0;

	if (pBitmap) {
		iWidth = pBitmapInfo->bmWidth;
		if (iWidth <= rect.Width() && rect.Width()>0) {
			POINT point;
			point.y = rect.CenterPoint().y - (pBitmapInfo->bmHeight >> 1);
			if (bRight)
				point.x = rect.right - iWidth;
			else
				point.x = rect.left;
			CDC dc;
			if (dc.CreateCompatibleDC(pDC) == TRUE) {
				VERIFY(dc.SelectObject(pBitmap));
				iWidth = pDC->BitBlt(point.x, point.y, pBitmapInfo->bmWidth, pBitmapInfo->bmHeight, &dc, 0, 0, SRCCOPY) ? iWidth : 0;
			}
			else iWidth = 0;
		}
		else
			iWidth = 0;
	}

	return iWidth;
}

INT SortHeaderCtrl::DrawText(CDC* pDC, CRect rect, LPHDITEM lphdi)
{
	CSize size;

	if (rect.Width() > 0 && lphdi->mask & HDI_TEXT && lphdi->fmt & HDF_STRING) {

		size = pDC->GetTextExtent(lphdi->pszText);

		switch (lphdi->fmt & HDF_JUSTIFYMASK)
		{
		case HDF_LEFT:
		case HDF_LEFT | HDF_RTLREADING:
			pDC->DrawText(lphdi->pszText, -1, rect, DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
			break;
		case HDF_CENTER:
		case HDF_CENTER | HDF_RTLREADING:
			pDC->DrawText(lphdi->pszText, -1, rect, DT_CENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
			break;
		case HDF_RIGHT:
		case HDF_RIGHT | HDF_RTLREADING:
			pDC->DrawText(lphdi->pszText, -1, rect, DT_RIGHT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
			break;
		}
	}

	size.cx = rect.Width()>size.cx ? size.cx : rect.Width();
	return size.cx>0 ? size.cx : 0;
}

INT SortHeaderCtrl::DrawArrow(CDC* pDC, CRect rect, BOOL bSortAscending, BOOL bRight)
{
	INT iWidth = 0;

	if (rect.Width() > 0 && m_sizeArrow.cx <= rect.Width()) {

		iWidth = m_sizeArrow.cx;

		rect.top += (rect.CenterPoint().y - (m_sizeArrow.cy / 2));
		rect.bottom = rect.top + m_sizeArrow.cy;

		CRect rectClient;
		GetClientRect(&rectClient);
		if (rect.right >= rectClient.right) { //For last column
			if (bRight) {
				rect.left = rect.right - m_sizeArrow.cx * 2;
				if (GetParent()->GetStyle() & WS_VSCROLL) { //If vertical scrollbar visible
					rect.left -= GetSystemMetrics(SM_CXVSCROLL);
				}
			}
		} else {
			rect.left = bRight ? rect.right - m_sizeArrow.cx : rect.left;
		}		
		rect.right = rect.left + m_sizeArrow.cx;
		if (rect.Width() % 2 != 0) {
			rect.left -= 1;
		}
		
		Gdiplus::Point point[3];
		point[0].X = rect.left;// -rect.Height() * 80 / 100;
		point[1].X = rect.CenterPoint().x;// -rect.Height() * 45 / 100;
		point[2].X = rect.right;// -rect.Height() * 1 / 100;
		if (bSortAscending) {
			point[0].Y = point[2].Y = rect.top;// +rect.Height() * 45 / 100;
			point[1].Y = rect.bottom;
		} else {
			point[1].Y = rect.top;// +rect.Height() * 45 / 100;
			point[0].Y = point[2].Y = rect.bottom;
		}
		Gdiplus::Graphics graphics(pDC->GetSafeHdc());
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::Pen pen(Gdiplus::Color(GetRValue(text_color_), GetGValue(text_color_), GetBValue(text_color_)));
		graphics.DrawPolygon(&pen, point, 3);
	}

	return iWidth;
}