#pragma once

// CMemHeaderDC

class CMemHeaderDC : public CDC
{
public:
	// constructor sets up the memory DC
	CMemHeaderDC(CDC* pDC) : CDC() {
		ASSERT(pDC != NULL);

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC)    // Create a Memory DC
		{
			pDC->GetClipBox(&m_rect);
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else        // Make a copy of the relevent parts of the current DC for printing
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}
	// Destructor copies the contents of the mem DC to the original DC
	~CMemHeaderDC() {
		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);
			//Swap back the original bitmap.
			SelectObject(m_pOldBitmap);
			m_bitmap.DeleteObject();
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CMemHeaderDC* operator->() { return this; }

	// Allow usage as a pointer
	operator CMemHeaderDC*() { return this; }

private:
	CBitmap  m_bitmap;      // Offscreen bitmap
	CBitmap* m_pOldBitmap;  // bitmap originally found in CMemHeaderDC
	CDC*     m_pDC;         // Saves CDC passed in constructor
	CRect    m_rect;        // Rectangle of drawing area.
	BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

#define HDF_EX_AUTOWIDTH	0x0001
#define HDF_EX_INCLUDESORT	0x0002
#define HDF_EX_FIXEDWIDTH	0x0004

typedef struct _HDITEMEX
{
	UINT	nStyle;
	INT		iMinWidth;
	INT		iMaxWidth;

	_HDITEMEX() : nStyle(0), iMinWidth(0), iMaxWidth(-1) {};

} HDITEMEX, FAR* LPHDITEMEX;

class SortHeaderCtrl : public CHeaderCtrl
{
public:
	SortHeaderCtrl();
	virtual ~SortHeaderCtrl();

	void SetSortArrow( const int iColumn, const BOOL bAscending );
	void DrawCtrl(CDC* pDC);
	INT DrawImage(CDC* pDC, CRect rect, LPHDITEM hdi, BOOL bRight);
	INT DrawBitmap(CDC* pDC, CRect rect, LPHDITEM hdi, CBitmap* pBitmap, BITMAP* pBitmapInfo, BOOL bRight);
	INT DrawText(CDC* pDC, CRect rect, LPHDITEM lphdi);
	INT DrawArrow(CDC* pDC, CRect rect, BOOL bSortAscending, BOOL bRight);
	void SetHeight(int height) { height_ = height; }
	void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
	void SetTextColor(COLORREF text_color) { text_color_ = text_color; }

private:
	virtual void DrawItem(CDC* pDC, CRect rect, LPHDITEM lphdi, BOOL bSort, BOOL bSortAscending);
	afx_msg LRESULT OnSetImageList(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLayout(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	INT m_iHotIndex;
	HDHITTESTINFO m_hdhtiHotItem;
	UINT m_nClickFlags;
	int m_iSortColumn;
	BOOL m_bSortAscending;
	INT m_iSpacing;
	SIZE m_sizeImage;
	SIZE m_sizeArrow;
	int height_;

	COLORREF text_color_, back_color_;

	DECLARE_MESSAGE_MAP()
};

