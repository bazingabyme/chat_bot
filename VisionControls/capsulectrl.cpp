#include "stdafx.h"

#define INCLUDE_CAPSULECTRL __declspec(dllexport)
#include "capsulectrl.h"

//CapsuleCtrl

IMPLEMENT_DYNAMIC(CapsuleCtrl, CStatic)

CapsuleCtrl::CapsuleCtrl()
{
	font_[0] = font_[1] = NULL;

	border_color_ = LABEL_COLOR;
	back_color_[0] = LABEL_COLOR;
	back_color_[1] = WHITE_COLOR;
	text_color_[0] = WHITE_COLOR;
	text_color_[1] = BLACK_COLOR;

	alignment_[0] = DT_SINGLELINE | DT_RIGHT;
	alignment_[1] = DT_SINGLELINE | DT_LEFT;

	capsule_shape_ = TRUE;
	start_y_ = 0;
	down_point_.SetPoint(0, 0);
	mouse_moved_ = FALSE;

	text_[0].Empty();
}

CapsuleCtrl::~CapsuleCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(CapsuleCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// CapsuleCtrl message handlers

void CapsuleCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void CapsuleCtrl::Create(int caption_area, BOOL capsule_shape, BOOL topbottomstyle) {
	
	capsule_shape_ = capsule_shape;
	topbottomstyle_ = topbottomstyle;

	start_y_ = 0;
	bounding_rect_.SetRectEmpty();
	down_point_.SetPoint(0, 0);
	mouse_moved_ = FALSE;

	GetClientRect(client_rect_);

	if (text_[0].IsEmpty()) {
		GetWindowText(text_[0]);
	}
	text_[0] = text_[0] + L" ";
	multiline_[0] = multiline_[1] = FALSE;

	int width = client_rect_.Width() * caption_area / 100;
	int height = client_rect_.Height() * caption_area / 100;

	if (capsule_shape_) {
		if (topbottomstyle_) {
			rect_[0].SetRect(client_rect_.left, client_rect_.top, client_rect_.right, height);
			rect_[1].SetRect(client_rect_.left, height, client_rect_.right, client_rect_.bottom);
		} else {
			rect_[0].SetRect(client_rect_.left + client_rect_.Height() / 2, client_rect_.top, width, client_rect_.bottom);
			rect_[1].SetRect(width, client_rect_.top, client_rect_.right - client_rect_.Height() / 2, client_rect_.bottom);
		}
	} else {
		if (topbottomstyle_) {
			rect_[0].SetRect(client_rect_.left, client_rect_.top, client_rect_.right, height);
			rect_[1].SetRect(client_rect_.left, height, client_rect_.right, client_rect_.bottom);
		} else {
			rect_[0].SetRect(client_rect_.left, client_rect_.top, width, client_rect_.bottom);
			rect_[1].SetRect(width, client_rect_.top, client_rect_.right, client_rect_.bottom);
		}
	}

	if (font_[0] == NULL) {
		font_[0] = GetFont();
	}
	if (font_[1] == NULL) {
		font_[1] = GetFont();
	}
	
	Invalidate();
}

void CapsuleCtrl::Destroy() {

}

void CapsuleCtrl::SetFontEx(CFont* font, BOOL rightside) {

	font_[rightside] = font;
}

void CapsuleCtrl::SetBorderColor(COLORREF color) {

	border_color_ = color;
}

void CapsuleCtrl::SetBackColor(COLORREF color, BOOL rightside) {

	back_color_[rightside] = color;
}

void CapsuleCtrl::SetTextColor(COLORREF color, BOOL rightside) {

	text_color_[rightside] = color;
}

void CapsuleCtrl::SetText(CString text, BOOL multiline, DWORD alignment, BOOL rightside) {

	text_[rightside] = text;
	multiline_[rightside] = multiline;
	if (!multiline_[rightside]) {
		if (alignment == DT_LEFT) {
			text_[rightside] = L" " + text_[rightside];
		} else {
			text_[rightside] = text_[rightside] + L" ";
		}
	}

	alignment_[rightside] = multiline_[rightside] ? (alignment | DT_WORDBREAK) : (DT_SINGLELINE | alignment);

	start_y_ = 0;

	Invalidate();
}

CString CapsuleCtrl::GetText(BOOL rightside) {

	return text_[rightside];
}

void CapsuleCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	if (!capsule_shape_ && topbottomstyle_) {

		//Draw Top Part as it is
		COfflineDC offdc1(&dc, &rect_[0]);
		offdc1.SetBkMode(TRANSPARENT);

		for (int i = 0; i < 1; i++) {
			offdc1.SetTextColor(text_color_[i]);

			if (back_color_[i] != WHITE_COLOR) {
				border_color_ = back_color_[i];
			}
			CPen pen(PS_SOLID, 1, border_color_);
			CPen* old_pen = offdc1.SelectObject(&pen);
			CBrush brush(back_color_[i]);
			CBrush* old_brush = offdc1.SelectObject(&brush);
			offdc1.Rectangle(rect_[i]);
			offdc1.SelectObject(old_brush);
			offdc1.SelectObject(old_pen);
			brush.DeleteObject();
			pen.DeleteObject();

			offdc1.SelectObject(font_[i]);

			if (!multiline_[i]) {
				offdc1.DrawText(Formation::PrepareString(offdc1, text_[i], rect_[i]), rect_[i], alignment_[i] | DT_VCENTER);
			} else {
				offdc1.DrawText(Formation::PrepareMultilineString(offdc1, text_[i], rect_[i]), rect_[i], alignment_[i] | DT_VCENTER);
			}
		}

		//Calculate bouding rect for text scrolling
		CRect temp_rect(0, 0, rect_[1].Width(), rect_[1].Height());
		bounding_rect_ = temp_rect;
		if (topbottomstyle_) {
			dc.SelectObject(font_[1]);
			if (!multiline_[1]) {
				dc.DrawText(Formation::PrepareString(offdc1, text_[1], bounding_rect_), bounding_rect_, DT_CALCRECT);
			} else {
				dc.DrawText(Formation::PrepareMultilineString(offdc1, text_[1], bounding_rect_), bounding_rect_, DT_CALCRECT);
			}
			//To consider height only for scrolling
			bounding_rect_.right = temp_rect.right;
			if (bounding_rect_.bottom < temp_rect.bottom) {
				bounding_rect_.bottom = temp_rect.bottom;
			}
		}

		//Draw Bottom Part with text scrolling
		COfflineDC offdc2(&dc, &rect_[1], &bounding_rect_, start_y_);
		offdc2.SetBkMode(TRANSPARENT);
		
		for (int i = 1; i < 2; i++) {
			offdc2.SetTextColor(text_color_[i]);

			if (back_color_[i] != WHITE_COLOR) {
				border_color_ = back_color_[i];
			}
			//Draw Bottom Rect as it is
			CRect text_rect(0, start_y_, rect_[1].Width(), start_y_ + rect_[1].Height());
			CPen pen(PS_SOLID, 1, border_color_);
			CPen* old_pen = offdc2.SelectObject(&pen);
			CBrush brush(back_color_[i]);
			CBrush* old_brush = offdc2.SelectObject(&brush);
			offdc2.Rectangle(text_rect);
			offdc2.SelectObject(old_brush);
			offdc2.SelectObject(old_pen);
			brush.DeleteObject();
			pen.DeleteObject();

			offdc2.SelectObject(font_[i]);
			if (!multiline_[i]) {
				offdc2.DrawText(Formation::PrepareString(offdc2, text_[i], bounding_rect_), bounding_rect_, alignment_[i] | DT_VCENTER);
			} else {
				CRect temp_rect(Formation::spacingHalf(), 1, bounding_rect_.Width(), bounding_rect_.Height());
				offdc2.DrawText(Formation::PrepareMultilineString(offdc2, text_[i], temp_rect), temp_rect, alignment_[i] | DT_VCENTER);
			}
		}

	} else {

		COfflineDC offdc(&dc, &client_rect_);
		offdc.SetBkMode(TRANSPARENT);

		if (capsule_shape_) {
			Gdiplus::Graphics graphics(offdc);
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			graphics.FillEllipse(Formation::labelcolor_brush_gdi(), Gdiplus::Rect(client_rect_.left, client_rect_.top, client_rect_.Height() - 1, client_rect_.Height() - 1));
			graphics.DrawEllipse(Formation::labelcolor_pen_gdi(), Gdiplus::Rect(client_rect_.right - client_rect_.Height(), client_rect_.top, client_rect_.Height() - 1, client_rect_.Height() - 1));
		}

		for (int i = 0; i < 2; i++) {
			offdc.SetTextColor(text_color_[i]);

			if (back_color_[i] != WHITE_COLOR) {
				border_color_ = back_color_[i];
			}
			CPen pen(PS_SOLID, 1, border_color_);
			CPen* old_pen = offdc.SelectObject(&pen);
			CBrush brush(back_color_[i]);
			CBrush* old_brush = offdc.SelectObject(&brush);
			offdc.Rectangle(rect_[i]);
			offdc.SelectObject(old_brush);
			offdc.SelectObject(old_pen);
			brush.DeleteObject();
			pen.DeleteObject();

			offdc.SelectObject(font_[i]);
			if (!multiline_[i]) {
				offdc.DrawText(Formation::PrepareString(offdc, text_[i], rect_[i]), rect_[i], alignment_[i] | DT_VCENTER);
			} else {
				offdc.DrawText(Formation::PrepareMultilineString(offdc, text_[i], rect_[i]), rect_[i], alignment_[i] | DT_VCENTER);
			}
		}

		if (capsule_shape_) {
			offdc.SelectObject(Formation::whitecolor_pen());
			offdc.MoveTo(rect_[1].right - 1, rect_[1].top + 1);
			offdc.LineTo(rect_[1].right - 1, rect_[1].bottom - 1);
		}
	}
}

void CapsuleCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	mouse_moved_ = FALSE;

	if (rect_[1].PtInRect(point)) {
		down_point_ = point;
		SetCapture();
	}
}

void CapsuleCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (!mouse_moved_ && rect_[0].PtInRect(point)) {
		GetParent()->SendMessageW(WM_CAPSULECTRL_HEADER_CLICKED, (WPARAM)this, (LPARAM)NULL);
	}

	mouse_moved_ = FALSE;
}

void CapsuleCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON && !bounding_rect_.IsRectEmpty()) {

		start_y_ += (down_point_.y - point.y);

		start_y_ = (start_y_ > (bounding_rect_.Height() - rect_[1].Height())) ? (bounding_rect_.Height() - rect_[1].Height()) : start_y_;
		start_y_ = start_y_ < 0 ? 0 : start_y_;
		
		down_point_ = point;
		mouse_moved_ = TRUE;

		InvalidateRect(rect_[1], FALSE);
	}
}

void CapsuleCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}

