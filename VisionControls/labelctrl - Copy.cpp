#include "stdafx.h"

#define INCLUDE_LABELCTRL __declspec(dllexport)
#include "labelctrl.h"

//LabelCtrl

IMPLEMENT_DYNAMIC(LabelCtrl, CStatic)

LabelCtrl::LabelCtrl()
{
	text_color_ = WHITE_COLOR;
	back_color_ = LABEL_COLOR;
	border_color_ = LABEL_COLOR;

	multiline_ = FALSE;
	text_ = _T("");
	alignment_ = DT_RIGHT;
}

LabelCtrl::~LabelCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(LabelCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// LabelCtrl message handlers

void LabelCtrl::Destroy() {

}

void LabelCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void LabelCtrl::Create(DWORD alignment, BOOL multiline) {
		
	GetClientRect(client_rect_);

	alignment_ = alignment;
	multiline_ = multiline;
	if (!multiline) {
		alignment_ |= DT_SINGLELINE;
	}

	text_rect_ = client_rect_;

	if (multiline_) {
		
		start_y_ = 0;
		down_point_.SetPoint(0, 0);
		mouse_moved_ = FALSE;

		SetText(text_);
	}
}

void PrintGdiCount() {
	printf("GDI OBJECTS %d\n", ::GetGuiResources(::GetCurrentProcess(), GR_GDIOBJECTS));
}

void LabelCtrl::SetText(CString text) {

	if (!multiline_) {
		
		text_ = L" " + text + L" ";

	} else {

		text_ = text;

		start_y_ = 0;
		down_point_.SetPoint(0, 0);
		mouse_moved_ = FALSE;

		/*int total_lines = 0;
		int index = 0;
		while ((index = text_.Find(L"\n", index)) != -1) {
			total_lines++;
			index++;
		}
		total_lines++;*/
	}

	CDC* dc = GetDC();
	dc->SelectObject(GetFont());
	text_ = Formation::PrepareMultilineString(*dc, text_, text_rect_);
	CRect calc_rect;
	dc->DrawText(text_, &calc_rect, DT_CALCRECT);
	cdc_height_ = calc_rect.Height();

	text_dc_.CreateCompatibleDC(dc);
	text_bitmap_.CreateCompatibleBitmap(dc, calc_rect.Width(), calc_rect.Height());
	text_dc_.SelectObject(&text_bitmap_);
	text_dc_.BitBlt(0, 0, calc_rect.Width(), calc_rect.Height(), NULL, 0, 0, WHITENESS);
	text_dc_.SetBkMode(TRANSPARENT);
	text_dc_.SelectObject(GetFont());
	text_dc_.SetTextColor(text_color_);
	text_dc_.DrawText(text_, calc_rect, alignment_ | DT_WORDBREAK);
	//CRect rect(client_rect_.left + Formation::spacing(), client_rect_.top + Formation::spacing(), client_rect_.right - Formation::spacing(), client_rect_.bottom - Formation::spacing());
	//InvalidateRect(rect, FALSE);

	Invalidate(FALSE);
	//InvalidateRect(text_rect_);
}

void LabelCtrl::SetBackColor(COLORREF color) {

	back_color_ = color;
}

void LabelCtrl::SetTextColor(COLORREF color) {

	text_color_ = color;
}

void LabelCtrl::SetBorderColor(COLORREF color) {

	border_color_ = color;
}

void LabelCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	COfflineDC offdc(&dc, &client_rect_);

	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	if (IsWindowEnabled()) {
		offdc.SetTextColor(text_color_);
		CBrush brush(back_color_);
		CPen pen(PS_SOLID, 1, border_color_);
		CBrush* old_brush = offdc.SelectObject(&brush);
		CPen* old_pen = offdc.SelectObject(&pen);
		offdc.Rectangle(client_rect_);
		offdc.SelectObject(old_brush);
		offdc.SelectObject(old_pen);
		brush.DeleteObject();
		pen.DeleteObject();
	} else {
		if (border_color_ == WHITE_COLOR) {
			offdc.SelectObject((HPEN)GetStockObject(NULL_PEN));
		} else {
			offdc.SelectObject(&Formation::disablecolor_pen());
		}
		if (back_color_ == WHITE_COLOR) {
			offdc.SetTextColor(DISABLE_COLOR);
			offdc.SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
		} else {
			offdc.SetTextColor(WHITE_COLOR);
			offdc.SelectObject(&Formation::disablecolor_brush());
		}
		offdc.Rectangle(client_rect_);
	}

	if (multiline_) {
		//CRect calc_rect;
		//text_dc_.DrawText(text_, &calc_rect, DT_CALCRECT);
		//CBitmap    m_bitmap;        // Offscreen bitmap
		//CBitmap* m_oldBitmap;		// bitmap originally found in CMemDC
		//CDC m_pDC;           // Saves CDC passed in constructor
		//m_pDC.CreateCompatibleDC(&offdc);
		//m_bitmap.CreateCompatibleBitmap(&m_pDC, calc_rect.Width(), calc_rect.Height());
		//m_oldBitmap = m_pDC.SelectObject(&m_bitmap);
		//m_pDC.BitBlt(0, 0, calc_rect.Width(), calc_rect.Height(), NULL, 0, 0, WHITENESS);
		//m_pDC.SetBkMode(TRANSPARENT);
		//m_pDC.SelectObject(GetFont());
		//m_pDC.SetTextColor(text_color_);

		//m_pDC.DrawText(text_, calc_rect, alignment_ | DT_WORDBREAK);
		//offdc.BitBlt(0, 0, client_rect_.Width(), client_rect_.Height(), &m_pDC, 0, start_y_, SRCCOPY);

		//m_pDC.SelectObject(m_oldBitmap);
		//m_bitmap.DeleteObject();

		offdc.BitBlt(0, 0, client_rect_.Width(), client_rect_.Height(), &text_dc_, 0, start_y_, SRCCOPY);
		/*
		text_dc_.DrawText(text_, text_rect_, alignment_ | DT_WORDBREAK);
		offdc.BitBlt(0, 0, client_rect_.Width(), client_rect_.Height(), &text_dc_, 0, start_y_, SRCCOPY);*/
		//offdc.StretchBlt(0, 0, client_rect_.Width(), client_rect_.Height(), &text_dc_, 0, start_y_, client_rect_.Width(), client_rect_.Height(), SRCCOPY);
		//CString text = text_;
		//CRect text_rect = client_rect_;
		////text = Formation::PrepareMultilineString(offdc2, text, text_rect);
		//printf("HEIGHT %d : %d\n", calc_rect.Height(), client_rect_.Height());
		//CString str;
		//str.Format(L"HEIGHT %d : %d\n", calc_rect.Height(), client_rect_.Height());
		////AfxMessageBox(str);
		//if (calc_rect.Height() < client_rect_.Height()) {
		//	if (alignment_ == DT_CENTER) {
		//		text_rect.top = (client_rect_.top + (client_rect_.Height() - calc_rect.Height()) / 2);
		//	}
		//}
		//if (calc_rect.Width() < client_rect_.Width()) {
		//	if (alignment_ == DT_CENTER) {
		//		text = Formation::PrepareString(offdc, text, text_rect);
		//		CRect calc_rect;
		//		offdc.DrawText(text, &calc_rect, DT_CALCRECT);
		//		text_rect.MoveToX(client_rect_.left + (client_rect_.Width() - calc_rect.Width()) / 2);
		//	}
		//}
		//offdc.DrawText(text, calc_rect, alignment_ | DT_WORDBREAK);
	} else {
		offdc.DrawText(Formation::PrepareString(offdc, text_, text_rect_), text_rect_, alignment_ | DT_VCENTER | DT_SINGLELINE);
	}
}

void LabelCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (multiline_) {
		down_point_ = point;
		mouse_moved_ = FALSE;

		SetCapture();
	}
}

void LabelCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
}

void LabelCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {
		if (multiline_) {

			int distance_to_move_y = (down_point_.y - point.y);

			start_y_ += distance_to_move_y;
			printf("HEIGHT %d %d START %d, %d\n", cdc_height_, text_rect_.Height(), start_y_, distance_to_move_y);
			start_y_ = (start_y_ >= (cdc_height_ - text_rect_.Height())) ? (cdc_height_ - text_rect_.Height()) : start_y_;
			start_y_ = start_y_ < 0 ? 0 : start_y_;

			down_point_ = point;
			mouse_moved_ = TRUE;

			CRect rect = text_rect_;
			rect.DeflateRect(0, 1, 0, 1);
			InvalidateRect(text_rect_, FALSE);
		}
	}
}

void LabelCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}