
#include "stdafx.h"

#define INCLUDE_SCROLLBAR __declspec(dllexport)
#include "scrollbar.h"

// CSBMemDC class

CSBMemDC::CSBMemDC()
{
	m_ValidFlag = FALSE;
	m_OldBmp = NULL;
}

CSBMemDC::~CSBMemDC()
{
	// Automatically release and draw on exit
	Release();
}

/////////////////////////////////////////////////////////////////////////////
//
//	Create: Creates the memory DC and image bitmap
//
/////////////////////////////////////////////////////////////////////////////

void CSBMemDC::Create(CDC* inDC, CRect inSrcRect)
{
	ASSERT(inDC != NULL);

	if (!m_ValidFlag)
	{
		// create the memory DC
		CreateCompatibleDC(inDC);

		// save the source DC
		m_SrcDC = inDC;

		// keep track of the destination rectangle
		m_SrcRect.CopyRect(inSrcRect);

		// create a bitmap for the memory bitmap image
		m_MemBmp.CreateCompatibleBitmap(inDC, inSrcRect.Width(), inSrcRect.Height());

		// select the memory image into the memory DC
		m_OldBmp = SelectObject(&m_MemBmp);

		m_ValidFlag = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	Release: Releases the memory DC and image bitmap and optionally copies the image
//
/////////////////////////////////////////////////////////////////////////////

void CSBMemDC::Release(BOOL inCopyToSourceFlag)
{
	// copy the offscreen buffer to the sourceDC passed in Create()

	if (m_ValidFlag)
	{
		// blit to source DC to the m_SrcRect
		if ((inCopyToSourceFlag) && (m_SrcDC != NULL))
			m_SrcDC->BitBlt(m_SrcRect.left, m_SrcRect.top, m_SrcRect.Width(), m_SrcRect.Height(), this, 0, 0, SRCCOPY);

		// de-select the memory image from the DC
		SelectObject(m_OldBmp);

		// delete the memory bitmap image
		m_MemBmp.DeleteObject();

		// delete the memory DC
		DeleteDC();

		m_ValidFlag = FALSE;
		m_OldBmp = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	CopySourceImage: Copies the source image from the m_SrcRect into the memory DC/image.
//
/////////////////////////////////////////////////////////////////////////////

void CSBMemDC::CopySourceImage()
{
	if (m_ValidFlag)
	{
		// copy the image from the source rectangle to the offscreen image
		if (m_SrcDC != NULL)
			this->BitBlt(0, 0, m_SrcRect.Width(), m_SrcRect.Height(), m_SrcDC, m_SrcRect.left, m_SrcRect.top, SRCCOPY);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	ConvertRect: Converts a rectangle based on the source coordinates 
//				 to one based on the memory image's coordinates.
//
/////////////////////////////////////////////////////////////////////////////

CRect CSBMemDC::ConvertRect(CRect inDrawRect)
{
	CRect	theRect;

	theRect.CopyRect(inDrawRect);
	theRect.OffsetRect(-m_SrcRect.left, -m_SrcRect.top);

	return (theRect);
}

/////////////////////////////////////////////////////////////////////////////
//
//	ConvertPoint: Converts a point based on the source coordinates to 
//				  one based on the memory image's coordinates.
//
/////////////////////////////////////////////////////////////////////////////

CPoint CSBMemDC::ConvertPoint(CPoint inDrawPoint)
{
	CPoint	thePoint;

	// Convert the point relative to this DC
	thePoint = inDrawPoint;
	thePoint.x -= m_SrcRect.left;
	thePoint.y -= m_SrcRect.top;

	return (thePoint);
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetRect: Sets the drawing/output bounding rectangle.
//
/////////////////////////////////////////////////////////////////////////////

void CSBMemDC::SetRect(CRect inRect)
{
	m_SrcRect = inRect;
}

// ScrollBar class

ScrollBar::ScrollBar()
{
	// Set up default values
	m_BackgroundColor = GetSysColor(COLOR_SCROLLBAR);
	m_ThumbSliderColor = GetSysColor(COLOR_3DFACE);
	m_ThumbSliderTrackColor = GetSysColor(COLOR_3DSHADOW);
	m_Style = -1;

	m_ScrollPos = 0;
	m_ScrollMinPos = 0;
	m_ScrollMaxPos = 100;

	m_bMouseOver = FALSE;
	m_bHotTrackThumb = FALSE;

	m_ThumbStyle = SCROLLTHUMBSTYLE_GRIPPER;

	// Neither scroll button is down to start
	m_bScrollButtonDownFlag[BUTTON_LEFT] = FALSE;
	m_bScrollButtonDownFlag[BUTTON_RIGHT] = FALSE;

	m_bThumbSliderDownFlag = FALSE;
	m_bLeftButtonDownFlag = FALSE;
	m_bTrackScrollAreaFlag = FALSE;
	m_bTrackThumbSliderFlag = FALSE;

	m_PageSize = 10;
	m_LineSize = 1;

	m_bFlashFocus = FALSE;
	m_bFlashState = TRUE;

	// Create the fonts

	LOGFONT	theLogFont;
	CFont*	theStockFont;

	theStockFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	theStockFont->GetLogFont(&theLogFont);
	m_Font[0].CreateFontIndirect(&theLogFont);
	theLogFont.lfWeight = FW_BOLD;
	m_Font[1].CreateFontIndirect(&theLogFont);

	m_ThumbSliderRectOrig.SetRectEmpty();
	m_ThumbSliderRectActive.SetRectEmpty();

	// Both scroll buttons are enabled by default
	m_bScrollButtonEnable[0] = TRUE;
	m_bScrollButtonEnable[1] = TRUE;
}

ScrollBar::~ScrollBar()
{

}

/////////////////////////////////////////////////////////////////////////////
//	Message map

BEGIN_MESSAGE_MAP(ScrollBar, CWnd)
	//{{AFX_MSG_MAP(ScrollBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//	SetThumbSliderSize
//
//	Sets the thumb slider size (see the EThumbSliderSize enum).
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetThumbSliderSize(EThumbSliderSize inThumbSliderSize, short inThumbSliderSizeValue)
{
	m_ThumbSliderSize = inThumbSliderSize;
	m_ThumbSliderSizeValue = inThumbSliderSizeValue;

	// For m_ThumbSliderSize:

	//	THUMBSLIDERSIZE_DEFAULT, inThumbSliderSizeValue = IGNORED
	//	THUMBSLIDERSIZE_FIXEDPIXELS, inThumbSliderSizeValue = pixels
	//	THUMBSLIDERSIZE_RELATIVEPCT, inThumbSliderSizeValue = percent of total scrollbar physical size
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetThumbStyle
//
//	Sets the thumb style (see the EScrollThumbStyle enum).
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetThumbStyle(EScrollThumbStyle inThumbStyle)
{
	m_ThumbStyle = inThumbStyle;
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetBackgroundColor
//
//	Sets the background color of the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetBackgroundColor(COLORREF inBackgroundColor)
{
	m_BackgroundColor = inBackgroundColor;

	if (IsWindow(m_hWnd))
		Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetHotTrackThumb
//
//	Sets the hot-tracking ability for the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetHotTrackThumb(BOOL bHotTrackThumb)
{
	m_bHotTrackThumb = bHotTrackThumb;
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetLineSize
//
//	Sets the line size for scrolling (with the arrows)
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetLineSize(long inLineSize)
{
	m_LineSize = inLineSize;
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetPageSize
//
//	Sets the page size for scrolling.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetPageSize(long inPageSize)
{
	m_PageSize = inPageSize;
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetThumbSliderColor
//
//	Sets the color of the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetThumbSliderColor(COLORREF inThumbSliderColor)
{
	// Set the thumb slider color
	m_ThumbSliderColor = inThumbSliderColor;

	if (IsWindow(m_hWnd))
		Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetThumbSliderTrackColor
//
//	Sets the color of the thumb slider when being tracked (moved) over with the mouse.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetThumbSliderTrackColor(COLORREF inThumbSliderTrackColor, COLORREF inThumbSliderTextColor, BOOL inTrackThumbSlider)
{
	m_ThumbSliderTrackColor = inThumbSliderTrackColor;
	m_ThumbSliderTextColor = inThumbSliderTextColor;
	m_bTrackThumbSliderFlag = inTrackThumbSlider;
}

/////////////////////////////////////////////////////////////////////////////
//
//	EnableScrollBar
//
//	Enable/disable scroll buttons at either end of the scrollbar control.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::EnableScrollBar(UINT inArrowFlags)
{
	BOOL	bEnable = FALSE;

	switch(inArrowFlags)
	{
		case ESB_DISABLE_BOTH:
			m_bScrollButtonEnable[0] = FALSE;
			m_bScrollButtonEnable[1] = FALSE;
			bEnable = TRUE;
			break;

		case ESB_DISABLE_RTDN:	// same as ESB_DISABLE_RIGHT or ESB_DISABLE_DOWN
			m_bScrollButtonEnable[1] = FALSE;
			bEnable = TRUE;
			break;

		case ESB_DISABLE_LTUP:	// same as ESB_DISABLE_LEFT or ESB_DISABLE_UP
			m_bScrollButtonEnable[0] = FALSE;
			bEnable = TRUE;
			break;

		case ESB_ENABLE_BOTH:
			m_bScrollButtonEnable[0] = TRUE;
			m_bScrollButtonEnable[1] = TRUE;
			bEnable = TRUE;
			break;

		default:
			// invalid parameter
			bEnable = FALSE;
			break;
	}

	// Update by redrawing
	if (IsWindow(GetSafeHwnd()))
		Draw();

	return (bEnable);
}

/////////////////////////////////////////////////////////////////////////////
//
//	EnableWindow
//
//	Enable/disable the entire scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::EnableWindow(BOOL bEnable)
{
	// Call the base class to handle the enable/disable
	CWnd::EnableWindow(bEnable);

	// Redraw to show any changes in the enabled state
	Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	Create: Simplified Create() method.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::Create(DWORD inStyle, const RECT& inRect, CWnd* inParentWnd, UINT inID)
{
	CRect	theScrollClientRect;
	BOOL	theWndBorderFlag = ((inStyle & WS_BORDER) == WS_BORDER);

	theScrollClientRect.CopyRect(&inRect);

	// Modify the create size based on whether the scrollbar is horizontal or vertical

	if (inStyle & WS_VSCROLL)
	{
		// Vertical scrollbar
		theScrollClientRect.right = theScrollClientRect.left + GetSystemMetrics(SM_CXVSCROLL) + 2;
		theScrollClientRect.right += (theWndBorderFlag * 2);
		m_Style = STYLE_VERTICAL;
	}
	else
	{
		// Assume horizontal scrollbar
		theScrollClientRect.bottom = theScrollClientRect.top + GetSystemMetrics(SM_CYVSCROLL) + 2;
		theScrollClientRect.bottom += (theWndBorderFlag * 2);
		m_Style = STYLE_HORIZONTAL;
	}

	// Turn off "true" scrollbar window flags
	inStyle &= ~WS_VSCROLL;
	inStyle &= ~WS_HSCROLL;

	return CWnd::Create(AfxRegisterWndClass(0), L"ScrollBarEx", inStyle, theScrollClientRect, inParentWnd, inID);
}

/////////////////////////////////////////////////////////////////////////////
// ScrollBar message handlers

/////////////////////////////////////////////////////////////////////////////
//
//	OnPaint : WM_PAINT handler
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	Draw()
//
//	Handles drawing of the scrollbar
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::Draw()
{
	CDC*	theDC;
	CSBMemDC	theMemDC;
	CRect	theClientRect, theDrawRect;

	// Draw the scrollbar here
	GetClientRect(theClientRect);

	theDC = GetDC();
	theMemDC.Create(theDC, theClientRect);
	theDrawRect = theMemDC.ConvertRect(theClientRect);

	// Draw the components of the scrollbar
	DrawBackground(&theMemDC, theDrawRect);
	DrawScrollAreas(&theMemDC, theDrawRect);
	DrawButtons(&theMemDC, theDrawRect);
	DrawThumbSlider(&theMemDC, theDrawRect);

	theMemDC.Release();
	ReleaseDC(theDC);
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawBackground
//
//	Handles drawing the background of the scrollbar control
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawBackground(CDC* inDC, CRect inDrawRect)
{
	ASSERT(inDC != NULL);
	
	// Is the window enabled?
	if (IsWindowEnabled())
	{
		// Override this to draw the background
		inDC->FillSolidRect(inDrawRect, m_BackgroundColor);
	}
	else
	{
		// If the window is not enabled, draw using a pattern brush.
		CBrush		theDitherBrush;
		CBitmap*	thePatternBitmap;

		thePatternBitmap = CreatePatternBitmap(inDC, m_BackgroundColor);
		if (thePatternBitmap != NULL)
		{
			theDitherBrush.CreatePatternBrush(thePatternBitmap);
			
			// make it darker?
			inDC->FillRect(inDrawRect, &theDitherBrush);

			thePatternBitmap->DeleteObject();
			delete thePatternBitmap;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	CreatePatternBitmap
//
//	Used to create the background pattern bitmap when disabled.
//
/////////////////////////////////////////////////////////////////////////////

CBitmap* ScrollBar::CreatePatternBitmap(CDC* inDC, COLORREF inBackgroundColor)
{
	CBitmap*	thePatternBmp = NULL;
	CDC			theMemDC;
	CBitmap*	theOldBmp;
	short		theXLoop, theYLoop;
	BOOL		theColorFlag = FALSE;
	COLORREF	theDitherColor = RGB(0x00, 0x00, 0x00);

	ASSERT(inDC != NULL);

	theMemDC.CreateCompatibleDC(inDC);

	// Create a pattern bitmap using the background color and a dark gray
	thePatternBmp = new CBitmap();
	thePatternBmp->CreateCompatibleBitmap(inDC, 8, 8);
	theOldBmp = theMemDC.SelectObject(thePatternBmp);

	// Set the dither color to dark gray
	theDitherColor = RGB(0x7F, 0x7F, 0x7F);

	// Draw in thePatternBmp, alternating the background color and theDitherColor
	for(theYLoop=0; theYLoop<8; theYLoop++)
	{
		for(theXLoop=0; theXLoop<8; theXLoop++)
		{
			theColorFlag = !theColorFlag;
			theMemDC.SetPixel(theXLoop, theYLoop, theColorFlag ? theDitherColor : inBackgroundColor);
		}
		theColorFlag = !theColorFlag;
	}

	theMemDC.SelectObject(theOldBmp);
	theMemDC.DeleteDC();

	return (thePatternBmp);
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawScrolAreas
//
//	Draws the various scroll areas for the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawScrollAreas(CDC* inDC, CRect inDrawRect)
{
	CRect	theScrollAreaRect = CRect(0, 0, 0, 0);

	ASSERT(inDC != NULL);

	if (m_bTrackScrollAreaFlag)
	{
		switch(m_ScrollBarClickArea)
		{
			case SCROLLAREA_SCROLLAREALEFT:
				GetScrollAreaRect(SCROLLAREARECT_LEFT, theScrollAreaRect);
				break;

			case SCROLLAREA_SCROLLAREARIGHT:
				GetScrollAreaRect(SCROLLAREARECT_RIGHT, theScrollAreaRect);
				break;

			case SCROLLAREA_SCROLLAREATOP:
				GetScrollAreaRect(SCROLLAREARECT_TOP, theScrollAreaRect);
				break;

			case SCROLLAREA_SCROLLAREABOTTOM:
				GetScrollAreaRect(SCROLLAREARECT_BOTTOM, theScrollAreaRect);
				break;
		}
	}

	if (!theScrollAreaRect.IsRectEmpty())
		inDC->InvertRect(theScrollAreaRect);
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawButtons
//
//	Draws the buttons at either end of the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawButtons(CDC* inDC, CRect inDrawRect)
{
	CRect	theButtonRect;

	ASSERT(inDC != NULL);

	// Override this to draw the scrollbar buttons

	if (m_Style == STYLE_VERTICAL)
	{
		// Vertical scrollbar

		// Draw the top scrollbar button
		theButtonRect.CopyRect(inDrawRect);
		theButtonRect.bottom = theButtonRect.top + GetSystemMetrics(SM_CYHSCROLL);
		inDC->DrawFrameControl(theButtonRect, DFC_SCROLL, DFCS_SCROLLUP | (DFCS_PUSHED * m_bScrollButtonDownFlag[0]) | (!IsWindowEnabled() * DFCS_INACTIVE) | (!m_bScrollButtonEnable[0] * DFCS_INACTIVE));

		// Draw the bottom scrollbar button
		theButtonRect.CopyRect(inDrawRect);
		theButtonRect.top = theButtonRect.bottom - GetSystemMetrics(SM_CYHSCROLL);
		inDC->DrawFrameControl(theButtonRect, DFC_SCROLL, DFCS_SCROLLDOWN | (DFCS_PUSHED * m_bScrollButtonDownFlag[1] | (!IsWindowEnabled() * DFCS_INACTIVE) | (!m_bScrollButtonEnable[1] * DFCS_INACTIVE)));
	}
	else
	{
		// Horizontal scrollbar

		// Draw the left scrollbar button
		theButtonRect.CopyRect(inDrawRect);
		theButtonRect.right = theButtonRect.left + GetSystemMetrics(SM_CXVSCROLL);
		inDC->DrawFrameControl(theButtonRect, DFC_SCROLL, DFCS_SCROLLLEFT | (DFCS_PUSHED * m_bScrollButtonDownFlag[0] | (!IsWindowEnabled() * DFCS_INACTIVE) | (!m_bScrollButtonEnable[0] * DFCS_INACTIVE)));

		// Draw the right scrollbar button
		theButtonRect.CopyRect(inDrawRect);
		theButtonRect.left = theButtonRect.right - GetSystemMetrics(SM_CXVSCROLL);
		inDC->DrawFrameControl(theButtonRect, DFC_SCROLL, DFCS_SCROLLRIGHT | (DFCS_PUSHED * m_bScrollButtonDownFlag[1] | (!IsWindowEnabled() * DFCS_INACTIVE) | (!m_bScrollButtonEnable[1] * DFCS_INACTIVE)));
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawThumbSlider
//
//	Draws the thumb slider in the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawThumbSlider(CDC* inDC, CRect inDrawRect)
{
	// Override this to draw the thumb slider

	CRect		theThumbSliderRect;
	COLORREF	theThumbSliderColor = GetSysColor(COLOR_3DFACE);
	COLORREF	theThumbSliderTextColor = GetSysColor(COLOR_WINDOWTEXT);

	ASSERT(inDC != NULL);

	theThumbSliderColor = m_ThumbSliderColor;

	// If the mouse is down on the thumb slider and the
	// mouse is actively tracking the thumb slider, update
	// the thumb and text colors.
	if (m_bThumbSliderDownFlag && m_bTrackThumbSliderFlag)
	{
		theThumbSliderColor = m_ThumbSliderTrackColor;
		theThumbSliderTextColor = m_ThumbSliderTextColor;
	}
	else
	if (m_bHotTrackThumb)
	{
		// Handle this if the hot tracking of the thumb is enabled
		if (m_bThumbSliderDownFlag && m_bMouseOver)
		{
			theThumbSliderColor = GetSysColor(COLOR_3DSHADOW);
		}
		else
		if ((m_bMouseOver && IsMouseOverThumbSlider()) || (m_bThumbSliderDownFlag))
		{
			theThumbSliderColor = GetSysColor(COLOR_3DHIGHLIGHT);
		}
	}
	else
	if (m_bThumbSliderDownFlag)
	{

	}
	else
	if (m_bFlashFocus)
	{
		if (m_bFlashState)
			theThumbSliderColor = GetSysColor(COLOR_3DSHADOW);
		else
			theThumbSliderColor = GetSysColor(COLOR_3DFACE);
	}

	// Get the bounding rectangle for the thumb slider
	GetThumbSliderRect(theThumbSliderRect);

	// Fill the background of the thumb slider
	inDC->FillSolidRect(theThumbSliderRect, theThumbSliderColor);

	// Frame in a 3D rectangle
	//inDC->Draw3dRect(theThumbSliderRect, theThumbSliderColor, RGB(0x00, 0x00, 0x00));

	// Inflate again
	theThumbSliderRect.InflateRect(-1, -1);

	// Draw the inset 3D raised thumb
	inDC->Draw3dRect(theThumbSliderRect, GetHighlightColor(), GetShadowColor());

	// Draw the scrollbar thumb slider custom, based on the thumb style
	if (m_ThumbStyle == SCROLLTHUMBSTYLE_GRIPPER)
		DrawThumbGripperMark(inDC, theThumbSliderRect);
	else
	if (m_ThumbStyle == SCROLLTHUMBSTYLE_DOT)
		DrawThumbDot(inDC, theThumbSliderRect);
	else
	if (m_ThumbStyle == SCROLLTHUMBSTYLE_LINE)
		DrawThumbLine(inDC, theThumbSliderRect);
	else
	if (m_ThumbStyle == SCROLLTHUMBSTYLE_VALUE)
		DrawThumbValue(inDC, theThumbSliderRect, theThumbSliderTextColor);
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawThumbGripperMark
//
//	Draws the thumb gripper style on the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawThumbGripperMark(CDC* inDC, CRect inThumbSliderRect)
{
	CRect	theGripperRect;
	short	theMidYPos, theMidXPos;

	ASSERT(inDC != NULL);

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		theMidYPos = (short)(inThumbSliderRect.top + inThumbSliderRect.Height() / 2);

		theGripperRect.SetRect(inThumbSliderRect.left + 2, theMidYPos - 3, inThumbSliderRect.right - 2, theMidYPos - 1);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(inThumbSliderRect.left + 2, theMidYPos - 6, inThumbSliderRect.right - 2, theMidYPos - 4);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(inThumbSliderRect.left + 2, theMidYPos + 0, inThumbSliderRect.right - 2, theMidYPos + 2);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(inThumbSliderRect.left + 2, theMidYPos + 3, inThumbSliderRect.right - 2, theMidYPos + 5);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());
	}
	else
	{
		// horizontal scrollbar
		theMidXPos = (short)(inThumbSliderRect.left + inThumbSliderRect.Width() / 2);

		// draw the gripper marks
		theGripperRect.SetRect(theMidXPos - 3, inThumbSliderRect.top + 2, theMidXPos - 1, inThumbSliderRect.bottom - 2);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(theMidXPos - 6, inThumbSliderRect.top + 2, theMidXPos - 4, inThumbSliderRect.bottom - 2);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(theMidXPos + 0, inThumbSliderRect.top + 2, theMidXPos + 2, inThumbSliderRect.bottom - 2);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());

		theGripperRect.SetRect(theMidXPos + 3, inThumbSliderRect.top + 2, theMidXPos + 5, inThumbSliderRect.bottom - 2);
		inDC->Draw3dRect(theGripperRect, GetHighlightColor(), GetShadowColor());
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawThumbDot
//
//	Draws the thumb dot style on the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawThumbDot(CDC* inDC, CRect inThumbSliderRect)
{
	COLORREF	theHighlightColor, theShadowColor;
	int			iThick = 1;
	CRect		theDrawRect;
	CPen*		theOldPen;
	CPoint		theStartPt, theEndPt;
	CPoint		theCenterPt;
	short		theDotRadius;
	CBrush		theFillBrush;
	CBrush*		theOldBrush;

	ASSERT(inDC != NULL);

	// Draw a recessed dot on the thumb slider
	theHighlightColor = GetHighlightColor();
	theShadowColor = GetShadowColor();

	CPen		theHighlightPen(PS_SOLID, iThick, theHighlightColor);
	CPen		theShadowPen(PS_SOLID, iThick, theShadowColor);

	theCenterPt.x = (inThumbSliderRect.left + inThumbSliderRect.Width() / 2);
	theCenterPt.y = (inThumbSliderRect.top + inThumbSliderRect.Height() / 2);

	theDrawRect.CopyRect(inThumbSliderRect);
	theDotRadius = (GetSystemMetrics(SM_CXVSCROLL) - 10) / 2;

	theStartPt.x = theCenterPt.x - theDotRadius;
	theStartPt.y = theCenterPt.y - theDotRadius;
	theEndPt.x = theCenterPt.x + theDotRadius;
	theEndPt.y = theCenterPt.y + theDotRadius;

	theDrawRect.left += 2;
	theDrawRect.top += 2;
	theDrawRect.right -= 2;
	theDrawRect.bottom -= 2;

	theDrawRect.SetRect(theStartPt.x, theStartPt.y, theEndPt.x, theEndPt.y);

	theFillBrush.CreateSolidBrush(m_ThumbSliderColor);
	theOldBrush = inDC->SelectObject(&theFillBrush);

	theOldPen = inDC->SelectObject(&theHighlightPen);
	inDC->Ellipse(theDrawRect);
	inDC->Arc(&theDrawRect, theStartPt, theEndPt);
	inDC->SelectObject(theOldPen);
	theHighlightPen.DeleteObject();

	theOldPen = inDC->SelectObject(&theShadowPen);
	inDC->Arc(&theDrawRect, theEndPt, theStartPt);
	inDC->SelectObject(theOldPen);
	theShadowPen.DeleteObject();

	inDC->SelectObject(theOldBrush);
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawThumbLine
//
//	Draws the thumb line on the thumb style.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawThumbLine(CDC* inDC, CRect inThumbSliderRect)
{
	CRect	theLineRect;
	short	theMidYPos, theMidXPos;

	ASSERT(inDC != NULL);

	// Draw a single line through the center of the thumb

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		theMidYPos = (short)(inThumbSliderRect.top + inThumbSliderRect.Height() / 2);

		theLineRect.SetRect(inThumbSliderRect.left + 1, theMidYPos - 1, inThumbSliderRect.right - 1, theMidYPos + 1);
	}
	else
	{
		// horizontal scrollbar
		theMidXPos = (short)(inThumbSliderRect.left + inThumbSliderRect.Width() / 2);

		theLineRect.SetRect(theMidXPos - 1, inThumbSliderRect.top + 1, theMidXPos + 1, inThumbSliderRect.bottom - 1);
	}

	inDC->Draw3dRect(theLineRect, GetShadowColor(), GetHighlightColor());
}

/////////////////////////////////////////////////////////////////////////////
//
//	DrawThumbValue
//
//	Draws the value of the scrollbar on the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::DrawThumbValue(CDC* inDC, CRect inThumbSliderRect, COLORREF inThumbSliderTextColor)
{
	CString	theScrollValue;
	CRect	theDrawRect;
	CFont*	theOldFont;

	ASSERT(inDC != NULL);

	// Draw the current scroll position on the thumb slider
	theDrawRect.CopyRect(inThumbSliderRect);
	theDrawRect.InflateRect(-1, -1);

	theOldFont = inDC->SelectObject(GetScrollThumbFont(m_bThumbSliderDownFlag));
	theScrollValue.Format(L"%d", m_ScrollPos);
	inDC->SetTextColor(inThumbSliderTextColor);
	inDC->SetBkMode(TRANSPARENT);
	inDC->DrawText(theScrollValue, theDrawRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	inDC->SelectObject(theOldFont);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetScrollThumbFont
//
//	Get the font used to display the scroll value on the thumb slider,
//	depending on the value of inThumbSliderScrollingFlag.  (TRUE returns
//	a bold font.)
//
/////////////////////////////////////////////////////////////////////////////

CFont* ScrollBar::GetScrollThumbFont(BOOL inThumbSliderScrollingFlag)
{
	return (&m_Font[inThumbSliderScrollingFlag]);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetScrollPos
//
//	Gets the current scroll position.
//
/////////////////////////////////////////////////////////////////////////////

long ScrollBar::GetScrollPos()
{ 
	return (m_ScrollPos);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetCurrentPosPercent
//
//	Gets the current position as a percent of the total range.
//
/////////////////////////////////////////////////////////////////////////////

float ScrollBar::GetCurrentPosPercent()
{
	float	thePosPercent = 0.0f;
	long	theCurPos = m_ScrollPos - m_ScrollMinPos;

	// Compute the current position percent scrolled

	float	theRange = (float) (m_ScrollMaxPos - m_ScrollMinPos);

	thePosPercent = theCurPos / theRange;

	return (thePosPercent);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetScrollRange
//
//	Gets the current scroll range.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::GetScrollRange(long& outMinPos, long& outMaxPos)
{
	outMinPos = m_ScrollMinPos;
	outMaxPos = m_ScrollMaxPos;
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetScrollPos
//
//	Gets the current scroll position.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetScrollPos(long inScrollPos, BOOL inRedrawFlag)
{
	m_ScrollPos = inScrollPos;

	// Ensure the scroll pos is in range
	m_ScrollPos = std::max(std::min(m_ScrollPos, m_ScrollMaxPos), m_ScrollMinPos);

	if (inRedrawFlag)
		Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	SetScrollRange
//
//	Sets the current scroll range.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::SetScrollRange(long inMinPos, long inMaxPos, BOOL inRedrawFlag)
{
	m_ScrollMinPos = inMinPos;
	m_ScrollMaxPos = inMaxPos;

	if (inRedrawFlag)
		Draw();
}

void ScrollBar::EnableScrollTimer(BOOL inEnableTimerFlag, BOOL inFastTimerFlag)
{
	if (inEnableTimerFlag)
	{
		m_bFastScrollFlag = inFastTimerFlag;
		SetTimer(TIMER_SCROLLBAR, inFastTimerFlag ? 80 : 700, NULL);

		SetTimer(TIMER_MOUSECHECK, 50, NULL);
	}
	else
	{
		KillTimer(TIMER_SCROLLBAR);
		KillTimer(TIMER_MOUSECHECK);
		m_bFastScrollFlag = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	IsMouseOverScrollBar
//
//	Returns TRUE if the mouse is currently within the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::IsMouseOverScrollBar(CPoint inMousePoint)
{
	CRect	theClientRect;

	GetClientRect(theClientRect);

	return (theClientRect.PtInRect(inMousePoint));
}

/////////////////////////////////////////////////////////////////////////////
//
//	IsMouseOverThumbSlider
//
//	Returns TRUE if the mouse is currently within the thumb slider rectangle.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::IsMouseOverThumbSlider()
{
	CPoint	ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(&ptMouse);

	return (IsMouseOverThumbSlider(ptMouse));
}

/////////////////////////////////////////////////////////////////////////////
//
//	IsMouseOverThumbSlider
//
//	Returns TRUE if the mouse is currently over the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::IsMouseOverThumbSlider(CPoint inMousePoint)
{
	CRect	theThumbSliderRect;

	GetThumbSliderRect(theThumbSliderRect);

	return (theThumbSliderRect.PtInRect(inMousePoint));
}

/////////////////////////////////////////////////////////////////////////////
//
//	IsMouseOverScrollButton
//
//	Returns TRUE if the mouse is currently over the specified scroll button.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::IsMouseOverScrollButton(short inScrollButtonIndex, CPoint inMousePoint)
{
	CRect	theScrollButtonRect;

	GetScrollButtonRect(inScrollButtonIndex, theScrollButtonRect);

	return (theScrollButtonRect.PtInRect(inMousePoint));
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetScrollButtonRect
//
//	Returns the bounding rectangle for the specified scroll bar button.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::GetScrollButtonRect(short inScrollButtonIndex, CRect& outScrollButtonRect)
{
	CRect	theScrollButtonRect;

	GetClientRect(theScrollButtonRect);

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		if (inScrollButtonIndex == BUTTON_TOP)
		{
			// top button
			theScrollButtonRect.bottom = theScrollButtonRect.top + GetSystemMetrics(SM_CXVSCROLL);
		}
		else
		{
			// bottom button
			theScrollButtonRect.top = theScrollButtonRect.bottom - GetSystemMetrics(SM_CXVSCROLL);
		}
	}
	else
	{
		// horizontal scrollbar
		if (inScrollButtonIndex == BUTTON_LEFT)
		{
			// left button
			theScrollButtonRect.right = theScrollButtonRect.left + GetSystemMetrics(SM_CXVSCROLL);
		}
		else
		{
			// right button
			theScrollButtonRect.left = theScrollButtonRect.right - GetSystemMetrics(SM_CXVSCROLL);
		}
	}

	outScrollButtonRect.CopyRect(theScrollButtonRect);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetScrollAreaRect
//
//	Returns the bounding rectangle for the specified scroll area.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::GetScrollAreaRect(short inScrollArea, CRect& outScrollAreaRect)
{
	CRect	theScrollAreaRect, theThumbSliderRect;

	GetClientRect(theScrollAreaRect);

	GetThumbSliderRect(theThumbSliderRect);

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		theScrollAreaRect.top += GetSystemMetrics(SM_CXVSCROLL);
		theScrollAreaRect.bottom -= GetSystemMetrics(SM_CXVSCROLL);

		if (inScrollArea == SCROLLAREARECT_ALL)
		{

		}
		else
		if (inScrollArea == SCROLLAREARECT_TOP)
		{
			// Get the scrollbar area "above" the thumb slider
			theScrollAreaRect.bottom = theThumbSliderRect.top;
		}
		else
		if (inScrollArea == SCROLLAREARECT_BOTTOM)
		{
			// Get the scrollbar area "below" the thumb slider
			theScrollAreaRect.top = theThumbSliderRect.bottom;
		}
	}
	else
	{
		// horizontal scrollbar
		theScrollAreaRect.left += GetSystemMetrics(SM_CXVSCROLL);
		theScrollAreaRect.right -= GetSystemMetrics(SM_CXVSCROLL);

		if (inScrollArea == SCROLLAREARECT_ALL)
		{

		}
		else
		if (inScrollArea == SCROLLAREARECT_LEFT)
		{
			// Get the scrollbar area to the left of the thumb slider
			theScrollAreaRect.right = theThumbSliderRect.left;
		}
		else
		if (inScrollArea == SCROLLAREARECT_RIGHT)
		{
			// Get the scrollbar area to the right of the thumb slider
			theScrollAreaRect.left = theThumbSliderRect.right;
		}
	}

	outScrollAreaRect.CopyRect(theScrollAreaRect);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetThumbSliderRect
//
//	Returns the bounding rectangle for the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::GetThumbSliderRect(CRect& outThumbSliderRect, BOOL inDrawRectArea)
{
	CRect	theThumbSliderRect;
	long	theWidth;
	float	thePctScroll;
	long	theThumbSliderWidth;

	// Compute the position of the thumb slider rectangle
	GetClientRect(theThumbSliderRect);

	// Get the percentage of the scrollbar position
	thePctScroll = GetCurrentPosPercent();

	theThumbSliderWidth = GetThumbSliderWidth();

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		theThumbSliderRect.top += GetSystemMetrics(SM_CXVSCROLL);
		theThumbSliderRect.bottom -= GetSystemMetrics(SM_CXVSCROLL);

		theWidth = theThumbSliderRect.Height() - theThumbSliderWidth;

		theThumbSliderRect.top += (long) ((float) theWidth * thePctScroll);
		theThumbSliderRect.bottom = theThumbSliderRect.top + theThumbSliderWidth;
	}
	else
	{
		// horizontal scrollbar
		theThumbSliderRect.left += GetSystemMetrics(SM_CXVSCROLL);
		theThumbSliderRect.right -= GetSystemMetrics(SM_CXVSCROLL);

		theWidth = theThumbSliderRect.Width() - theThumbSliderWidth;

		theThumbSliderRect.left += (long) ((float) theWidth * thePctScroll);
		theThumbSliderRect.right = theThumbSliderRect.left + theThumbSliderWidth;
	}

	outThumbSliderRect.CopyRect(theThumbSliderRect);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetDefaultThumbSliderWidth
//
//	Returns the default thumb slider width as set by the control panel display settings.
//
/////////////////////////////////////////////////////////////////////////////

long ScrollBar::GetDefaultThumbSliderWidth()
{
	return (GetSystemMetrics(SM_CXHTHUMB));
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetThumbSliderWidth
//
//	Returns the current thumb slider width based on the properties of the thumb slider.
//
/////////////////////////////////////////////////////////////////////////////

long ScrollBar::GetThumbSliderWidth()
{
	// Determine how the thumb slider is sized

	long	theThumbSize;
	CRect	theClientRect;
	CRect	theScrollAreaRect;

	GetClientRect(theClientRect);

	theScrollAreaRect.CopyRect(theClientRect);

	if (m_Style == STYLE_VERTICAL)
	{
		// vertical scrollbar
		theScrollAreaRect.top += GetSystemMetrics(SM_CXVSCROLL);
		theScrollAreaRect.bottom -= GetSystemMetrics(SM_CXVSCROLL);
	}
	else
	{
		// horizontal scrollbar
		theScrollAreaRect.left += GetSystemMetrics(SM_CXVSCROLL);
		theScrollAreaRect.right -= GetSystemMetrics(SM_CXVSCROLL);
	}

	// 1) Standard size = GetSystemMetrics(SM_CXHTHUMB);
	// 2) Fixed size (pixels) to display current scroll value
	// 2) Relative size = based on scrollbar physical size

	// For m_ThumbSliderSize:

	//	THUMBSLIDERSIZE_DEFAULT, m_ThumbSliderSizeValue = IGNORED
	//	THUMBSLIDERSIZE_FIXEDPIXELS, m_ThumbSliderSizeValue = pixels
	//	THUMBSLIDERSIZE_RELATIVEPCT, m_ThumbSliderSizeValue = percent of total scrollbar physical size

	switch(m_ThumbSliderSize)
	{
		case THUMBSLIDERSIZE_DEFAULT:
		default:
			theThumbSize = GetDefaultThumbSliderWidth();
			break;

		case THUMBSLIDERSIZE_FIXEDPIXELS:
			theThumbSize = m_ThumbSliderSizeValue;
			break;

		case THUMBSLIDERSIZE_RELATIVEPCT:
			if (m_Style == STYLE_VERTICAL)
			{
				// vertical scrollbar
				theThumbSize = (long) ((float) m_ThumbSliderSizeValue * (float) theScrollAreaRect.Height() / 100.0f);
			}
			else
			{
				// horizontal scrollbar
				theThumbSize = (long) ((float) m_ThumbSliderSizeValue * (float) theScrollAreaRect.Width() / 100.0f);
			}
			break;
	}

	return (theThumbSize);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetMouseArea
//
//	Returns the scroll bar area where the mouse is currently hovering.
//
/////////////////////////////////////////////////////////////////////////////

ScrollBar::EScrollBarArea ScrollBar::GetMouseArea(CPoint inMousePt)
{
	CRect			theScrollButtonRect[2];
	CRect			theThumbSliderRect;
	CRect			theScrollAreaRect;
	EScrollBarArea	theMouseArea = SCROLLAREA_UNKNOWN;

	GetScrollButtonRect(BUTTON_LEFT, theScrollButtonRect[BUTTON_LEFT]);
	GetScrollButtonRect(BUTTON_RIGHT, theScrollButtonRect[BUTTON_RIGHT]);
	GetThumbSliderRect(theThumbSliderRect);
	GetScrollAreaRect(SCROLLAREARECT_ALL, theScrollAreaRect);

	if (m_Style == STYLE_VERTICAL)
	{
		if (theScrollButtonRect[BUTTON_TOP].PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_TOPBUTTON;
		}
		else
		if (theScrollButtonRect[BUTTON_BOTTOM].PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_BOTTOMBUTTON;
		}
		else
		if (theThumbSliderRect.PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_THUMBSLIDER;
		}
		else
		if (theScrollAreaRect.PtInRect(inMousePt))
		{
			if (inMousePt.y < theThumbSliderRect.top)
				theMouseArea = SCROLLAREA_SCROLLAREATOP;
			else
				theMouseArea = SCROLLAREA_SCROLLAREABOTTOM;
		}
	}
	else
	{
		if (theScrollButtonRect[BUTTON_LEFT].PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_LEFTBUTTON;
		}
		else
		if (theScrollButtonRect[BUTTON_RIGHT].PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_RIGHTBUTTON;
		}
		else
		if (theThumbSliderRect.PtInRect(inMousePt))
		{
			theMouseArea = SCROLLAREA_THUMBSLIDER;
		}
		else
		if (theScrollAreaRect.PtInRect(inMousePt))
		{
			if (inMousePt.x < theThumbSliderRect.left)
				theMouseArea = SCROLLAREA_SCROLLAREALEFT;
			else
				theMouseArea = SCROLLAREA_SCROLLAREARIGHT;
		}
	}

	return (theMouseArea);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnLButtonDown
//
//	Handle the WM_LBUTTONDOWN message.
//
//	Begin tracking the left mouse handling an action on the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnLButtonDown(UINT inFlags, CPoint inPoint) 
{
	// If the window is disabled, ignore the left mouse button.
	if (!IsWindowEnabled())
		return;

	CWnd::OnLButtonDown(inFlags, inPoint);

	SetFocus();

	EScrollBarArea	theScrollBarArea;
	BOOL			bSetTimerFlag = FALSE;
	CRect			theThumbRect;

	theScrollBarArea = GetMouseArea(inPoint);

	GetThumbSliderRect(theThumbRect);
	m_ScrollThumbClickOffset = 0;

	switch(theScrollBarArea)
	{
		case SCROLLAREA_LEFTBUTTON:
			// left scroll button
		case SCROLLAREA_TOPBUTTON:
			// top scroll button
			if (m_bScrollButtonEnable[0])
			{
				m_bLeftButtonDownFlag = TRUE;
				bSetTimerFlag = TRUE;
				m_bScrollButtonDownFlag[0] = TRUE;
			}
			break;

		case SCROLLAREA_RIGHTBUTTON:
			// right scroll button
		case SCROLLAREA_BOTTOMBUTTON:
			// bottom scroll button
			if (m_bScrollButtonEnable[1])
			{
				m_bLeftButtonDownFlag = TRUE;
				bSetTimerFlag = TRUE;
				m_bScrollButtonDownFlag[1] = TRUE;
			}
			break;

		case SCROLLAREA_SCROLLAREALEFT:
			// left of thumbslider
		case SCROLLAREA_SCROLLAREATOP:
			// top of thumbslider
			m_bLeftButtonDownFlag = TRUE;
			m_bTrackScrollAreaFlag = TRUE;
			bSetTimerFlag = TRUE;
			break;

		case SCROLLAREA_SCROLLAREARIGHT:
			// right of thumbslider
		case SCROLLAREA_SCROLLAREABOTTOM:
			// bottom of thumbslider
			m_bLeftButtonDownFlag = TRUE;
			m_bTrackScrollAreaFlag = TRUE;
			bSetTimerFlag = TRUE;
			break;

		case SCROLLAREA_THUMBSLIDER:
			// thumb slider
			m_bThumbSliderDownFlag = TRUE;
			m_bLeftButtonDownFlag = TRUE;
			break;

		default:
		case SCROLLAREA_UNKNOWN:
			break;
	}

	if (m_bLeftButtonDownFlag)
	{
		m_ScrollBarClickArea = theScrollBarArea;

		SetCapture();

		if (bSetTimerFlag)
			EnableScrollTimer(TRUE);

		if (!m_bThumbSliderDownFlag)
		{
			HandleScrollRepeat(TRUE);
		}
		else
		{
			// Determine the click point in the thumb scroll area
			if (m_Style == STYLE_VERTICAL)
			{
				// vertical scrollbar
				m_ScrollThumbClickOffset = (short) (inPoint.y - theThumbRect.top);
			}
			else
			{
				// horizontal scrollbar
				m_ScrollThumbClickOffset = (short) (inPoint.x - theThumbRect.left);
			}
		}

		m_bMouseOverScrollBarFlag = TRUE;

		Draw();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnLButtonUp
//
//	Handle the WM_LBUTTONUP message.
//
//	Stop tracking the mouse drag on the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnLButtonUp(UINT inFlags, CPoint inPoint) 
{
	if (m_bLeftButtonDownFlag)
	{
		m_ScrollBarClickArea = EScrollBarArea::SCROLLAREA_UNKNOWN;
		m_bLeftButtonDownFlag = FALSE;

		ReleaseCapture();

		EnableScrollTimer(FALSE);

		m_bScrollButtonDownFlag[BUTTON_LEFT] = FALSE;
		m_bScrollButtonDownFlag[BUTTON_RIGHT] = FALSE;

		m_bThumbSliderDownFlag = FALSE;
		m_bTrackScrollAreaFlag = FALSE;

		Draw();
	}

	CWnd::OnLButtonUp(inFlags, inPoint);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnMouseMove
//
//	Handle the WM_MOUSEMOVE message.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnMouseMove(UINT inFlags, CPoint inPoint) 
{
	if (m_bLeftButtonDownFlag)
	{
		if (m_bThumbSliderDownFlag)
		{
			// Dragging the thumb slider
			HandleDragThumbSlider(inPoint);
		}
		else
		if (m_bScrollButtonDownFlag[0])
		{
			// left or top scroll button down
		}
		else
		if (m_bScrollButtonDownFlag[1])
		{
			// right or bottom scroll button down
		}
		else
		{
			// Check to see if the mouse is over the scrollbar or not
			if (IsMouseOverScrollBar(inPoint))
			{
				if (!m_bMouseOverScrollBarFlag)
				{
					m_bMouseOverScrollBarFlag = TRUE;
					Draw();
				}
			}
			else
			{
				if (m_bMouseOverScrollBarFlag)
				{
					m_bMouseOverScrollBarFlag = FALSE;
					Draw();
				}
			}
		}
	}
	else
	{
		TrackMouse(TRUE);
	}
	
	CWnd::OnMouseMove(inFlags, inPoint);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnMouseLeave
//
//	Handle the WM_MOUSELEAVE message when the mouse exits the control.
//
/////////////////////////////////////////////////////////////////////////////

LRESULT ScrollBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	TrackMouse(FALSE);

	return (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
//	TrackMouse
//
//	Start or stop tracking the mouse.  This is used to ensure the WM_MOUSELEAVE
//	message is sent when the mouse leaves the scrollbar control.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::TrackMouse(BOOL bTrack)
{
	if ((bTrack && !m_bMouseOver) || (!bTrack && m_bMouseOver))
	{
		TRACKMOUSEEVENT	tme;

		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));

		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;

		if (!bTrack)
		{
			tme.dwFlags = TME_CANCEL;
		}

		m_bMouseOver = bTrack;

		BOOL	bRet = _TrackMouseEvent(&tme);

		Draw();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnTimer
//
//	Handle the WM_TIMER message.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnTimer(UINT_PTR inIDEvent)
{
	switch(inIDEvent)
	{
		case TIMER_SCROLLBAR:
			// repeatedly scroll
			HandleScrollRepeat(m_bFastScrollFlag);
			EnableScrollTimer(TRUE, TRUE);
			break;

		case TIMER_MOUSECHECK:
			HandleScrollRepeat(FALSE);
			break;

		case TIMER_FLASHFOCUS:
			HandleFlashFocus();
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	HandleScrollRepeat
//
//	Handle automatic repeat scrolling when the mouse click is held down on
//	an area of the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::HandleScrollRepeat(BOOL inAlterScrollPos)
{
	CPoint	theMousePoint;
	long	thePrevScrollPos = m_ScrollPos;

	GetCursorPos(&theMousePoint);
	ScreenToClient(&theMousePoint);

	EScrollBarArea	theScrollBarArea = GetMouseArea(theMousePoint);

	// Determine which button is scrolling and check to see whether it IS down

	switch(m_ScrollBarClickArea)
	{
		// horizontal scrollbar
		case SCROLLAREA_LEFTBUTTON:
			if (theScrollBarArea == SCROLLAREA_LEFTBUTTON)
			{
				// Mouse is in the left scrollbar button
				m_bScrollButtonDownFlag[0] = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(-m_LineSize);
			}
			else
			{
				// Mouse is out of the left scrollbar button
				m_bScrollButtonDownFlag[0] = FALSE;
			}
			break;

		case SCROLLAREA_RIGHTBUTTON:
			if (theScrollBarArea == SCROLLAREA_RIGHTBUTTON)
			{
				// Mouse is in the right scrollbar button
				m_bScrollButtonDownFlag[1] = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(+m_LineSize);
			}
			else
			{
				// Mouse is out of the right scrollbar button
				m_bScrollButtonDownFlag[1] = FALSE;
			}
			break;

		case SCROLLAREA_SCROLLAREALEFT:
			if (theScrollBarArea == SCROLLAREA_SCROLLAREALEFT)
			{
				// Mouse is in the left scroll area
				m_bTrackScrollAreaFlag = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(-m_PageSize);
			}
			else
			{
				// Mouse is out of the left scroll area
				m_bTrackScrollAreaFlag = FALSE;
			}
			break;

		case SCROLLAREA_SCROLLAREARIGHT:
			if (theScrollBarArea == SCROLLAREA_SCROLLAREARIGHT)
			{
				// Mouse is in the right scroll area
				m_bTrackScrollAreaFlag = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(m_PageSize);
			}
			else
			{
				// Mouse is out of the right scroll area
				m_bTrackScrollAreaFlag = FALSE;
			}
			break;

		// vertical scrollbar
		case SCROLLAREA_TOPBUTTON:
			if (theScrollBarArea == SCROLLAREA_TOPBUTTON)
			{
				// Mouse is in the top scrollbar button
				m_bScrollButtonDownFlag[0] = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(-m_LineSize);
			}
			else
			{
				// Mouse is out of the top scrollbar button
				m_bScrollButtonDownFlag[0] = FALSE;
			}
			break;

		case SCROLLAREA_BOTTOMBUTTON:
			if (theScrollBarArea == SCROLLAREA_BOTTOMBUTTON)
			{
				// Mouse is in the bottom scrollbar button
				m_bScrollButtonDownFlag[1] = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(+m_LineSize);
			}
			else
			{
				// Mouse is out of the bottom scrollbar button
				m_bScrollButtonDownFlag[1] = FALSE;
			}
			break;

		case SCROLLAREA_SCROLLAREATOP:
			if (theScrollBarArea == SCROLLAREA_SCROLLAREATOP)
			{
				// Mouse is in the top scroll area
				m_bTrackScrollAreaFlag = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(-m_PageSize);
			}
			else
			{
				// Mouse is out of the top scroll area
				m_bTrackScrollAreaFlag = FALSE;
			}
			break;

		case SCROLLAREA_SCROLLAREABOTTOM:
			if (theScrollBarArea == SCROLLAREA_SCROLLAREABOTTOM)
			{
				// Mouse is in the bottom scroll area
				m_bTrackScrollAreaFlag = TRUE;
				if (inAlterScrollPos)
					ChangeScrollPos(m_PageSize);
			}
			else
			{
				// Mouse is out of the bottom scroll area
				m_bTrackScrollAreaFlag = FALSE;
			}
			break;
	}

//	if (thePrevScrollPos != m_ScrollPos)
	{
		Draw();
		NotifyParentScrollPosChange();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	ChangeScrollPos
//
//	Change the scroll position by the specified delta.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::ChangeScrollPos(long inDeltaPos)
{
	m_ScrollPos += inDeltaPos;

	m_ScrollPos = std::max(std::min(m_ScrollPos, m_ScrollMaxPos), m_ScrollMinPos);

	Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	HandleDragThumbSlider
//
//	Handles dragging the thumb slider with the mouse for a WM_MOUSEMOVE message.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::HandleDragThumbSlider(CPoint inPoint)
{
	// Get the current mouse position relative to the entire scroll area

	CRect	theScrollAreaRect;
	long	theNewPos;
	long	theScrollRange = m_ScrollMaxPos - m_ScrollMinPos;
	CRect	theButtonRect;

	GetScrollAreaRect(SCROLLAREARECT_ALL, theScrollAreaRect);
	GetScrollButtonRect(0, theButtonRect);

	if (m_Style == STYLE_VERTICAL)
	{
		theScrollAreaRect.bottom -= GetThumbSliderWidth();

		// vertical scrollbar
		theNewPos = (long) (((float) (inPoint.y - m_ScrollThumbClickOffset - theButtonRect.Height()) / (float) theScrollAreaRect.Height()) * theScrollRange) + m_ScrollMinPos;
	}
	else
	{
		theScrollAreaRect.right -= GetThumbSliderWidth();

		// horizontal scrollbarz
		theNewPos = (long) (((float) (inPoint.x - m_ScrollThumbClickOffset - theButtonRect.Width()) / (float) theScrollAreaRect.Width()) * theScrollRange) + m_ScrollMinPos;
	}

	// Check to ensure that the scrollbar is within the min/max range
	theNewPos = std::max(std::min(theNewPos, m_ScrollMaxPos), m_ScrollMinPos);
	SetScrollPos(theNewPos, TRUE);

	// Notify the parent window of the position change.
	NotifyParentScrollPosChange();
}

/////////////////////////////////////////////////////////////////////////////
//
//	NotifyParentScrollPosChange
//
//	Notifies the scrollbar's parent window about a position change
//	by sending the WM_SCROLLBARPOSCHANGE message.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::NotifyParentScrollPosChange()
{
	CWnd*	theScrollBarParent;
	CString	theValueTooltip;

	theScrollBarParent = GetParent();

	// Format the tooltip with the current scroll value
	theValueTooltip.Format(L"%ld", m_ScrollPos);
	m_tooltip.AddTool(this, theValueTooltip);

	m_tooltip.Activate(TRUE);

	if (theScrollBarParent != NULL)
	{
		// Notify the parent that the scroll position changed with a WM_SCROLLBARPOSCHANGE message
		//
		//	wParam = scrollbar window ID
		//	lParam = new scrollbar position

		//theScrollBarParent->SendMessage(WM_SCROLLBARPOSCHANGE, (WPARAM)GetDlgCtrlID(), (LPARAM)m_ScrollPos);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnSetCursor
//
//	Handle the WM_SETCURSOR message to change the mouse cursor shape.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::OnSetCursor(CWnd* inWnd, UINT inHitTest, UINT inMessage) 
{
	CPoint	theMousePoint;
	HCURSOR	theCursor = NULL;

	GetCursorPos(&theMousePoint);
	ScreenToClient(&theMousePoint);

	if (theCursor == NULL)
		theCursor = LoadCursor(NULL, IDC_ARROW);

	if (theCursor != NULL)
	{
		SetCursor(theCursor);
		return (TRUE);
	}

	return CWnd::OnSetCursor(inWnd, inHitTest, inMessage);
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetHighlightColor
//
//	Returns the 3D highlight color for drawing.
//
/////////////////////////////////////////////////////////////////////////////

COLORREF ScrollBar::GetHighlightColor()
{
	return (GetSysColor(COLOR_3DHIGHLIGHT));
}

/////////////////////////////////////////////////////////////////////////////
//
//	GetShadowColor
//
//	Returns the 3D shadow color for drawing.
//
/////////////////////////////////////////////////////////////////////////////

COLORREF ScrollBar::GetShadowColor()
{
	return (GetSysColor(COLOR_3DSHADOW));
}

int ScrollBar::OnCreate(LPCREATESTRUCT ioCreateStruct) 
{
	if (CWnd::OnCreate(ioCreateStruct) == -1)
		return (-1);
	
	// Create the ToolTip control.
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	
	return (0);
}

BOOL ScrollBar::PreTranslateMessage(MSG* inMsg)
{
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(inMsg);

	return CWnd::PreTranslateMessage(inMsg);
}

BOOL ScrollBar::IsMouseScrolling()
{ 
	BOOL	theMouseScrollingFlag;

	theMouseScrollingFlag = m_bThumbSliderDownFlag;

	return (theMouseScrollingFlag);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnMouseWheel
//
//	Handle the WM_MOUSEWHEEL message to allow the mouse wheel to affect
//	the position of the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ScrollBar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int		theScrollAmt = m_PageSize;

	if (zDelta < 0)
	{
		theScrollAmt = -m_PageSize;
	}

	SetScrollPos(GetScrollPos() + theScrollAmt, TRUE);
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnSetFocus
//
//	Handle the WM_SETFOCUS message to enable the flashing of the thumb slider
//	when the scrollbar gets focus.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnSetFocus(CWnd* pOldWnd)
{
	//SetTimer(TIMER_FLASHFOCUS, 500, NULL);

	m_bFlashFocus = TRUE;
	m_bFlashState = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnKillFocus
//
//	Handle the WM_KILLFOCUS message to stop flashing of the thumb slider
//	when the scrollbar loses focus.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnKillFocus(CWnd* pNewWnd)
{
	KillTimer(TIMER_FLASHFOCUS);

	m_bFlashFocus = FALSE;

	Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	HandleFlashFocus
//
//	Toggle the flash state when the scrollbar has focus.  This is done to
//	flash the thumb slider and show the scrollbar has focus.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::HandleFlashFocus()
{
	// Toggle the flash state flag
	m_bFlashState = !m_bFlashState;

	// Redraw to show the flash state change.
	Draw();
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnGetDlgCode
//
//	Handle the WM_GETDLGCODE to allow the scrollbar control to handle key
//	presses when the scrollbar exists in a dialog.
//
/////////////////////////////////////////////////////////////////////////////

UINT ScrollBar::OnGetDlgCode()
{
	UINT	uiDlgCode = DLGC_WANTARROWS;

	if (GetStyle() & WS_TABSTOP)
		uiDlgCode |= DLGC_WANTTAB;

	return (uiDlgCode);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnKeyDown
//
//	Handle the WM_KEYDOWN message to allow keyboard handling for the scrollbar.
//
/////////////////////////////////////////////////////////////////////////////

void ScrollBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

	// Handle WM_KEYDOWN keys

	long	thePrevScrollPos = m_ScrollPos;

	switch(nChar)
	{
		case VK_LEFT:
		case VK_UP:
			ChangeScrollPos(-m_LineSize);
			break;

		case VK_RIGHT:
		case VK_DOWN:
			ChangeScrollPos(m_LineSize);
			break;

		case VK_PRIOR:
			ChangeScrollPos(-m_PageSize);
			break;

		case VK_NEXT:
			ChangeScrollPos(m_PageSize);
			break;

		case VK_HOME:
			m_ScrollPos = m_ScrollMinPos;
			break;

		case VK_END:
			m_ScrollPos = m_ScrollMaxPos;
			break;
	}

	if (m_ScrollPos != thePrevScrollPos)
	{
		Draw();
		NotifyParentScrollPosChange();
	}
}

void ScrollBar::OnCaptureChanged(CWnd *pWnd) 
{
	CWnd::OnCaptureChanged(pWnd);

	Draw();
}

