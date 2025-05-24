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
	/*if (!multiline) {
		alignment_ |= DT_SINGLELINE;
	}*/

	text_rect_ = client_rect_;

	if (multiline_) {
		
		start_y_ = 0;
		down_point_.SetPoint(0, 0);
		mouse_moved_ = FALSE;

	//	SetText(text_);
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
	}

	CDC* dc = GetDC();
	dc->SelectObject(GetFont());
	if (multiline_) {
		text_ = Formation::PrepareMultilineString(*dc, text_, client_rect_);
		dc->DrawText(text_, &text_rect_, DT_CALCRECT);
	} else {
		text_ = Formation::PrepareString(*dc, text_, text_rect_);
	}

	Invalidate(FALSE);
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
		COfflineDC offdc2(& offdc, & client_rect_, & text_rect_, start_y_);
		offdc2.SetBkMode(TRANSPARENT);
		offdc2.SelectObject(GetFont());
		offdc2.SetTextColor(text_color_);
		offdc2.DrawText(text_, text_rect_, alignment_ | DT_WORDBREAK);
	} else {
		offdc.DrawText(text_, text_rect_, alignment_ | DT_VCENTER | DT_SINGLELINE);
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
		//	printf("HEIGHT %d %d START %d, %d\n", text_rect_.Height(), client_rect_.Height(), start_y_, distance_to_move_y);
			start_y_ = (start_y_ >= (text_rect_.Height() - client_rect_.Height())) ? (text_rect_.Height() - client_rect_.Height()) : start_y_;
			start_y_ = start_y_ < 0 ? 0 : start_y_;

			down_point_ = point;
			mouse_moved_ = TRUE;

			CRect rect = client_rect_;
			rect.DeflateRect(0, 1, 0, 1);
			InvalidateRect(client_rect_, FALSE);
		}
	}
}

void LabelCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}