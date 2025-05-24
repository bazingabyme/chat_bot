#include "stdafx.h"

#define INCLUDE_CAPTIONBARCTRL __declspec(dllexport)
#include "captionbarctrl.h"

//CaptionBarCtrl

IMPLEMENT_DYNAMIC(CaptionBarCtrl, CStatic)

CaptionBarCtrl::CaptionBarCtrl()
{

}

CaptionBarCtrl::~CaptionBarCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(CaptionBarCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CaptionBarCtrl message handlers

void CaptionBarCtrl::Create(BOOL show_toggle_button, int left_text_area, int right_text_area) {
	
	GetClientRect(client_rect_);

	show_toggle_button_ = show_toggle_button;

	right_text_button_status_ = FALSE;
	right_text_button_clicked_ = FALSE;

	left_visible_text_ = TRUE;
	right_visible_text_ = TRUE;
	left_text_ = right_text_[0] = right_text_[1] = _T("");
	left_text_color_ = right_text_color_ = WHITE_COLOR;
	color_code_ = LABEL_COLOR;
	visible_color_code_ = TRUE;

	color_code_rect_ = left_text_rect_ = right_text_rect_ = right_text_button_rect_ = client_rect_;
	color_code_rect_.left = Formation::spacing2();
	color_code_rect_.top = Formation::spacing2();
	color_code_rect_.right = Formation::spacing4() + Formation::spacing2();
	color_code_rect_.bottom -= Formation::spacing2();

	left_text_rect_.left = color_code_rect_.right + Formation::spacing2();
	left_text_rect_.right = left_text_rect_.left + client_rect_.Width() * left_text_area / 100;

	right_text_rect_.left = left_text_rect_.right + Formation::spacing();
	if (show_toggle_button_) {
		right_text_rect_.right = client_rect_.right - (Formation::heading_height() - Formation::spacing());
		right_text_button_rect_.left = right_text_rect_.right;
	} else {
		right_text_rect_.right = client_rect_.right - Formation::spacing2();
		right_text_button_rect_.SetRectEmpty();
	}
}

void CaptionBarCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void CaptionBarCtrl::Destroy() {

}

void CaptionBarCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SelectObject(GetFont());
	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(WHITE_COLOR);

	if (IsWindowEnabled()) {
		offdc.FillSolidRect(client_rect_, LABEL_COLOR);
	} else {
		offdc.FillSolidRect(client_rect_, DISABLE_COLOR);
	}

	if (visible_color_code_) {
		Gdiplus::Graphics graphics(offdc);
		graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		Gdiplus::Point poly_points[3];
		poly_points[0].X = color_code_rect_.CenterPoint().x - Formation::spacing() - Formation::spacing();
		poly_points[1].X = color_code_rect_.CenterPoint().x;
		poly_points[2].X = color_code_rect_.CenterPoint().x + Formation::spacing() + Formation::spacing();
		poly_points[0].Y = poly_points[2].Y = color_code_rect_.CenterPoint().y + Formation::spacing() + Formation::spacing() / 2;
		poly_points[1].Y = color_code_rect_.CenterPoint().y - Formation::spacing() - Formation::spacing() / 2;
		Gdiplus::SolidBrush gdi_brush(Gdiplus::Color(GetRValue(color_code_), GetGValue(color_code_), GetBValue(color_code_)));
		graphics.FillPolygon(&gdi_brush, poly_points, 3);
	}
	if (left_visible_text_) {
		offdc.SetTextColor(left_text_color_);
		CRect rect = left_text_rect_;
		if (!right_visible_text_) {
			rect.right += right_text_rect_.Width();
		}
		offdc.DrawText(left_text_, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	if (right_visible_text_) {
		if(!right_text_button_rect_.IsRectEmpty()) {
			if (right_text_button_clicked_) {
				offdc.FillSolidRect(right_text_button_rect_, BACKCOLOR1_SEL);
			} else {
				offdc.FillSolidRect(right_text_button_rect_, LABEL_COLOR);
			}
			CRect rect_show_region = right_text_button_rect_;
			rect_show_region.left = right_text_button_rect_.CenterPoint().x - Formation::spacingHalf();
			rect_show_region.right = right_text_button_rect_.CenterPoint().x + Formation::spacingHalf() + 1;
			rect_show_region.top = right_text_button_rect_.CenterPoint().y - Formation::spacingHalf();
			rect_show_region.bottom = right_text_button_rect_.CenterPoint().y + Formation::spacingHalf() + 1;
			if (right_text_button_status_) {
				offdc.FillSolidRect(rect_show_region, WHITE_COLOR3);
				offdc.SelectObject(Formation::font(Formation::SMALL_FONT));
			} else {
				offdc.FillSolidRect(rect_show_region, BLACK_COLOR3);
				offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));
			}
		}
		offdc.SetTextColor(right_text_color_);
		offdc.DrawText(right_text_[right_text_button_status_], right_text_rect_, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	}
}

BOOL CaptionBarCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;// CStatic::OnEraseBkgnd(pDC);
}

void CaptionBarCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}

void CaptionBarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	right_text_button_clicked_ = FALSE;
	if (right_visible_text_ && right_text_button_rect_.PtInRect(point)) {
		right_text_button_clicked_ = TRUE;
		InvalidateRect(right_text_button_rect_);

		SetCapture();
	}
}

void CaptionBarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (right_text_button_clicked_) {
		right_text_button_status_ = !right_text_button_status_;

		right_text_button_clicked_ = FALSE;
		InvalidateRect(right_text_button_rect_);
		InvalidateRect(right_text_rect_);
	}

	ReleaseCapture();
}