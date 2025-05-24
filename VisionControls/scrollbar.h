#pragma once

#ifndef INCLUDE_SCROLLBAR
#define INCLUDE_SCROLLBAR __declspec(dllimport)
#endif //INCLUDE_SCROLLBAR

class CSBMemDC : public CDC
{
public:

protected:

	BOOL		m_ValidFlag;		// TRUE if we can successfully release/copy the offscreen image to the screen
	CRect		m_SrcRect;			// Bounding rectangle for drawing
	CBitmap		m_MemBmp;			// Offscreen bitmap image
	CBitmap*	m_OldBmp;			// Previous bitmap in the offscreen DC
	CDC*		m_SrcDC;			// Source device context for final blit

public:

	void SetRect(CRect inRect);

	// Construction

	CSBMemDC();
	virtual ~CSBMemDC();

	// Access

	CPoint	ConvertPoint(CPoint inDrawPoint);
	CRect	ConvertRect(CRect inDrawRect);
	void	CopySourceImage();
	void	Create(CDC* inDC, CRect inSrcRect);

	void	Release(BOOL inCopyToSourceFlag = TRUE);

};

#define	STYLE_VERTICAL			0 
#define	STYLE_HORIZONTAL		1

#define	BUTTON_LEFT 0
#define	BUTTON_TOP	0
#define	BUTTON_RIGHT 1
#define	BUTTON_BOTTOM 1

#define	SCROLLAREARECT_ALL 0
#define	SCROLLAREARECT_LEFT 1
#define	SCROLLAREARECT_TOP 1
#define	SCROLLAREARECT_RIGHT 2
#define	SCROLLAREARECT_BOTTOM 2

#define	TIMER_MOUSECHECK 3
#define	TIMER_SCROLLBAR 5
#define	TIMER_FLASHFOCUS 6

//#define	WM_SCROLLBARPOSCHANGE WM_USER + 410

/////////////////////////////////////////////////////////////////////////////
// ScrollBar window

class INCLUDE_SCROLLBAR ScrollBar : public CWnd
{
// Construction
public:

	ScrollBar() ;

	virtual BOOL PreTranslateMessage(MSG* inMsg) ;

	//==========================================================================
	//	Enumerations
	//==========================================================================

	public:

		enum EScrollBarArea
		{
			SCROLLAREA_UNKNOWN = -1,			// this should never occur

			// horizontal scrollbar
			SCROLLAREA_LEFTBUTTON = 0,			// left scroll button
			SCROLLAREA_RIGHTBUTTON = 1,			// right scroll button
			SCROLLAREA_SCROLLAREALEFT = 2,		// left of thumbslider
			SCROLLAREA_SCROLLAREARIGHT = 3,		// right of thumbslider

			SCROLLAREA_THUMBSLIDER = 10,		// thumb slider

			SCROLLAREA_TOTAL = 20,				// the entire scroll area (between the scroll buttons)

			// vertical scrollbar
			SCROLLAREA_TOPBUTTON = 30,			// top scroll button
			SCROLLAREA_BOTTOMBUTTON = 31,		// bottom scroll button
			SCROLLAREA_SCROLLAREATOP = 32,		// top of thumbslider
			SCROLLAREA_SCROLLAREABOTTOM = 33,	// bottom of thumbslider
		};

		enum EScrollThumbStyle
		{
			SCROLLTHUMBSTYLE_PLAIN = 0,
			SCROLLTHUMBSTYLE_GRIPPER = 1,
			SCROLLTHUMBSTYLE_DOT = 2,
			SCROLLTHUMBSTYLE_LINE = 3,
			SCROLLTHUMBSTYLE_VALUE = 4
		};

		enum EThumbSliderSize
		{
			THUMBSLIDERSIZE_DEFAULT = 0,
			THUMBSLIDERSIZE_FIXEDPIXELS = 1,
			THUMBSLIDERSIZE_RELATIVEPCT = 2
		};

// Attributes
public:

	BOOL			EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH) ;
	void			EnableWindow(BOOL bEnable = TRUE) ;

	long			GetDefaultThumbSliderWidth() ;
	long			GetScrollPos() ;
	void			GetScrollRange(long& outMinPos, long& outMaxPos) ;
	long			GetThumbSliderWidth() ;
	
	void			SetBackgroundColor(COLORREF inBackgroundColor) ;
	void			SetHotTrackThumb(BOOL bHotTrackThumb) ;
	void			SetLineSize(long inLineSize) ;
	void			SetPageSize(long inPageSize) ;
	void			SetScrollPos(long inScrollPos, BOOL inRedraw = TRUE) ;
	void			SetScrollRange(long inMinPos, long inMaxPos, BOOL inRedraw = TRUE) ;
	void			SetThumbSliderColor(COLORREF inThumbSliderColor) ;
	void			SetThumbSliderTrackColor(COLORREF inThumbSliderTrackColor, COLORREF inThumbSliderTextColor, BOOL inTrackThumbSlider) ;
	void			SetThumbSliderSize(EThumbSliderSize inThumbSliderSize, short inRelativeSize = 50) ;
	void			SetThumbStyle(EScrollThumbStyle inThumbStyle) ;
					
protected:			
					
	EThumbSliderSize m_ThumbSliderSize ;
	BOOL			m_bHotTrackThumb ;
	short			m_ThumbSliderSizeValue ;
	COLORREF		m_BackgroundColor ;
	COLORREF		m_ThumbSliderColor ;
	COLORREF		m_ThumbSliderTrackColor ;
	COLORREF		m_ThumbSliderTextColor ;
	BOOL			m_bTrackThumbSliderFlag ;
	BOOL			m_bTrackScrollAreaFlag ;
	short			m_Style ;
	long			m_PageSize, m_LineSize ;
	CFont			m_Font[2] ;
	
	long			m_ScrollMinPos, m_ScrollMaxPos ;
	long			m_ScrollPos ;
	BOOL			m_bScrollButtonEnable[2] ;
	BOOL			m_bScrollButtonDownFlag[2];
	BOOL			m_bThumbSliderDownFlag;
	BOOL			m_bLeftButtonDownFlag;
	BOOL			m_bMouseOver ;
	BOOL			m_bMouseOverScrollBarFlag ;
	EScrollBarArea	m_ScrollBarClickArea ;
	short			m_ScrollThumbClickOffset ;
	BOOL			m_bFastScrollFlag ;
	EScrollThumbStyle m_ThumbStyle ;
	CToolTipCtrl	m_tooltip ;
	CRect			m_ThumbSliderRectOrig ;
	CRect			m_ThumbSliderRectActive ;

	BOOL			m_bFlashFocus ;
	BOOL			m_bFlashState ;

	float			GetCurrentPosPercent() ;
	void			GetScrollAreaRect(short inScrollArea, CRect& outScrollAreaRect) ;
	void			GetScrollButtonRect(short inScrollButtonIndex, CRect& outScrollButtonRect) ;
	void			GetThumbSliderRect(CRect& outThumbSliderRect, BOOL inDrawRectArea = FALSE) ;
					
// Operations		
public:				

	BOOL			Create(DWORD inStyle, const RECT& inRect, CWnd* inParentWnd, UINT inID) ;

protected:

	void			ChangeScrollPos(long inDeltaPos) ;
	CBitmap*		CreatePatternBitmap(CDC* inDC, COLORREF inBackgroundColor) ;

	void			Draw() ;
	void			DrawThumbDot(CDC* inDC, CRect inThumbSliderRect) ;
	void			DrawThumbGripperMark(CDC* inDC, CRect inThumbSliderRect) ;
	void			DrawThumbLine(CDC* inDC, CRect inThumbSliderRect) ;
	void			DrawThumbValue(CDC* inDC, CRect inThumbSliderRect, COLORREF inThumbSliderTextColor) ;

	virtual void	DrawBackground(CDC* inDC, CRect inDrawRect) ;
	virtual	void	DrawButtons(CDC* inDC, CRect inDrawRect) ;
	virtual void	DrawScrollAreas(CDC* inDC, CRect inDrawRect) ;
	virtual void	DrawThumbSlider(CDC* inDC, CRect inDrawRect) ;

	void			EnableScrollTimer(BOOL inEnableTimerFlag, BOOL inFastTimerFlag = FALSE) ;

	EScrollBarArea	GetMouseArea(CPoint inMousePt) ;
	COLORREF		GetHighlightColor() ;
	CFont*			GetScrollThumbFont(BOOL inThumbSliderScrollingFlag) ;
	COLORREF		GetShadowColor() ;

	void			HandleDragThumbSlider(CPoint inPoint) ;
	void			HandleFlashFocus() ;
	void			HandleScrollRepeat(BOOL inAlterScrollPos) ;

	BOOL			IsMouseOverScrollBar(CPoint inMousePoint) ;
	BOOL			IsMouseOverScrollButton(short inScrollButtonIndex, CPoint inMousePoint) ;
	BOOL			IsMouseOverThumbSlider() ;
	BOOL			IsMouseOverThumbSlider(CPoint inMousePoint) ;
	BOOL			IsMouseScrolling() ;

	void			NotifyParentScrollPosChange() ;

	void			TrackMouse(BOOL bTrack) ;

public:
	virtual ~ScrollBar() ;

	// Generated message map functions
protected:
	//{{AFX_MSG(ScrollBar)
	afx_msg void OnPaint() ;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point) ;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point) ;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point) ;
	afx_msg void OnTimer(UINT_PTR nIDEvent) ;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) ;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
