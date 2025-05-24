#include "StdAfx.h"
#include "win_shape.h"

void WinRect::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	//Gdiplus::Graphics gdi_dc(hdc_drawing);
	//gdi_dc.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

	//Gdiplus::Rect rect(left_top_points(full_image).x, left_top_points(full_image).y, (right_bottom_points(full_image).x - left_top_points(full_image).x), (right_bottom_points(full_image).y - left_top_points(full_image).y));

	//if (angle() != 0) {
	//	//gdi_dc.TranslateTransform(rect.X + (rect.Width / 2), rect.Y + (rect.Height / 2), Gdiplus::Matrix); // Fix position
	//	gdi_dc.RotateTransform(angle());
	//}

	//if (fill_color() == BLACK_COLOR) {
	//	Gdiplus::Pen gdi_pen(Gdiplus::Color(GetRValue(line_color()), GetGValue(line_color()), GetBValue(line_color())));
	//	gdi_dc.DrawRectangle(&gdi_pen, left_top_points(full_image).x, left_top_points(full_image).y, abs(right_bottom_points(full_image).x - left_top_points(full_image).x), abs(right_bottom_points(full_image).y - left_top_points(full_image).y));
	//} else {
	//	Gdiplus::SolidBrush gdi_brush(Gdiplus::Color(GetRValue(line_color()), GetGValue(line_color()), GetBValue(line_color())));
	//	gdi_dc.FillRectangle(&gdi_brush, left_top_points(full_image).x, left_top_points(full_image).y, abs(right_bottom_points(full_image).x - left_top_points(full_image).x), abs(right_bottom_points(full_image).y - left_top_points(full_image).y));
	//}

	//if (selected()) {

	//	if (lock(ROTATE) == FALSE) {
	//		//Ellipse(hdc_drawing, rect.CenterPoint().x - shape_selection_size(), rect.top - shape_selection_size(), rect.CenterPoint().x + shape_selection_size(), rect.top + shape_selection_size());
	//		Gdiplus::Rect rect(rect.X + (rect.Width / 2) - shape_selection_size(), rect.Y - shape_selection_size(), shape_selection_size() * 2, shape_selection_size() * 2);
	//		gdi_dc.FillEllipse(Formation::spancolor2_brush_gdi(), rect);
	//	}
	//	if (lock(RESIZE) == FALSE) {
	//		Gdiplus::SolidBrush gdi_brush(Gdiplus::Color(GetRValue(line_color()), GetGValue(line_color()), GetBValue(line_color())));
	//		Gdiplus::Rect rect1(rect.X - shape_selection_size(), rect.Y - shape_selection_size(), shape_selection_size() * 2, shape_selection_size() * 2);
	//		gdi_dc.FillRectangle(&gdi_brush, rect1);
	//		Gdiplus::Rect rect2(rect.X + rect.Width - shape_selection_size(), rect.Y - shape_selection_size(), shape_selection_size() * 2, shape_selection_size() * 2);
	//		gdi_dc.FillRectangle(&gdi_brush, rect2);
	//		Gdiplus::Rect rect3(rect.X + rect.Width - shape_selection_size(), rect.Y + rect.Height - shape_selection_size(), shape_selection_size() * 2, shape_selection_size() * 2);
	//		gdi_dc.FillRectangle(&gdi_brush, rect3);
	//		Gdiplus::Rect rect4(rect.X - shape_selection_size(), rect.Y + rect.Height - shape_selection_size(), shape_selection_size() * 2, shape_selection_size() * 2);
	//		gdi_dc.FillRectangle(&gdi_brush, rect4);
	//	}
	//	Gdiplus::Pen pen_for_selection(Gdiplus::Color(GetRValue(WHITE_COLOR), GetGValue(WHITE_COLOR), GetBValue(WHITE_COLOR)), 1);
	//	pen_for_selection.SetDashStyle(Gdiplus::DashStyleDot);
	//	gdi_dc.DrawRectangle(&pen_for_selection, rect);
	//}

	//if (angle() != 0) {
	//	gdi_dc.ResetTransform();
	//}

	if (!visible()) return;

	HPEN hpen = CreatePen(line_style(), line_width(), line_color());
	HBRUSH hbrush = (fill_color() == BLACK_COLOR) ? (HBRUSH)GetStockObject(NULL_BRUSH) : CreateSolidBrush(fill_color());
	HPEN old_hpen = (HPEN)SelectObject(hdc_drawing, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);
	
	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	CRect rect(left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
	Rectangle(hdc_drawing, left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
	
	SelectObject(hdc_drawing, old_hpen);
	SelectObject(hdc_drawing, old_hbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);

	if (selected()) {

		SelectObject(hdc_drawing, GetStockObject(NULL_PEN));
		if (lock(ROTATE) == FALSE) {
			SelectObject(hdc_drawing, Formation::spancolor2_brush());
			Ellipse(hdc_drawing, rect.CenterPoint().x - shape_selection_size(), rect.top - shape_selection_size(), rect.CenterPoint().x + shape_selection_size(), rect.top + shape_selection_size());
		}
		if (lock(RESIZE) == FALSE) {
			HBRUSH hbrush = CreateSolidBrush(line_color());
			HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);
			if (resize_from_left_right_only_) {
				Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.CenterPoint().y - shape_selection_size(), rect.left + shape_selection_size(), rect.CenterPoint().y + shape_selection_size());
				Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.CenterPoint().y - shape_selection_size(), rect.right + shape_selection_size(), rect.CenterPoint().y + shape_selection_size());
			} else {
				Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.top - shape_selection_size(), rect.left + shape_selection_size(), rect.top + shape_selection_size());
				Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.top - shape_selection_size(), rect.right + shape_selection_size(), rect.top + shape_selection_size());
				Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.bottom - shape_selection_size(), rect.right + shape_selection_size(), rect.bottom + shape_selection_size());
				Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.bottom - shape_selection_size(), rect.left + shape_selection_size(), rect.bottom + shape_selection_size());
			}
			SelectObject(hdc_drawing, old_hbrush);
			DeleteObject(hbrush);
		}

		HPEN hpen_for_selection = CreatePen(PS_DOT, 1, WHITE_COLOR);
		old_hpen = (HPEN)SelectObject(hdc_drawing, hpen_for_selection);
		SetROP2(hdc_drawing, R2_MERGEPEN);
		SelectObject(hdc_drawing, GetStockObject(NULL_BRUSH));
		Rectangle(hdc_drawing, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject(hdc_drawing, old_hpen);
		DeleteObject(hpen_for_selection);
	}

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);
}

void WinRoundRect::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	HPEN hpen = CreatePen(line_style(), line_width(), line_color());
	HBRUSH hbrush = (fill_color() == BLACK_COLOR) ? (HBRUSH)GetStockObject(NULL_BRUSH) : CreateSolidBrush(fill_color());
	HPEN old_hpen = (HPEN)SelectObject(hdc_drawing, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);

	int width = (right_bottom_points(full_image).x - left_top_points(full_image).x);
	int height = (right_bottom_points(full_image).y - left_top_points(full_image).y);
	if (width > height) {
		width = height;
	}

	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	CRect rect(left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
	RoundRect(hdc_drawing, left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y, width, width);

	SelectObject(hdc_drawing, old_hpen);
	SelectObject(hdc_drawing, old_hbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);

	if (selected()) {

		SelectObject(hdc_drawing, GetStockObject(NULL_PEN));
		if (lock(ROTATE) == FALSE) {
			SelectObject(hdc_drawing, Formation::spancolor2_brush());
			Ellipse(hdc_drawing, rect.CenterPoint().x - shape_selection_size(), rect.top - shape_selection_size(), rect.CenterPoint().x + shape_selection_size(), rect.top + shape_selection_size());
		}
		if (lock(RESIZE) == FALSE) {
			HBRUSH hbrush = CreateSolidBrush(line_color());
			HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);
			Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.top - shape_selection_size(), rect.left + shape_selection_size(), rect.top + shape_selection_size());
			Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.top - shape_selection_size(), rect.right + shape_selection_size(), rect.top + shape_selection_size());
			Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.bottom - shape_selection_size(), rect.right + shape_selection_size(), rect.bottom + shape_selection_size());
			Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.bottom - shape_selection_size(), rect.left + shape_selection_size(), rect.bottom + shape_selection_size());
			SelectObject(hdc_drawing, old_hbrush);
			DeleteObject(hbrush);
		}

		HPEN hpen_for_selection = CreatePen(PS_DOT, 1, WHITE_COLOR);
		old_hpen = (HPEN)SelectObject(hdc_drawing, hpen_for_selection);
		SetROP2(hdc_drawing, R2_MERGEPEN);
		SelectObject(hdc_drawing, GetStockObject(NULL_BRUSH));
		Rectangle(hdc_drawing, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject(hdc_drawing, old_hpen);
		DeleteObject(hpen_for_selection);
	}

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);
}

void WinEllipse::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	HPEN hpen = CreatePen(line_style(), line_width(), line_color());
	HBRUSH hbrush = (fill_color() == BLACK_COLOR) ? (HBRUSH)GetStockObject(NULL_BRUSH) : CreateSolidBrush(fill_color());
	HPEN old_hpen = (HPEN)SelectObject(hdc_drawing, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);

	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	CRect rect(left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
	Ellipse(hdc_drawing, rect.left, rect.top, rect.right, rect.bottom);

	SelectObject(hdc_drawing, old_hpen);
	SelectObject(hdc_drawing, old_hbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);

	if (selected()) {

		SelectObject(hdc_drawing, GetStockObject(NULL_PEN));
		if (shape_type() == SHAPE_TYPE::ELLIPSE && lock(ROTATE) == FALSE) {
			SelectObject(hdc_drawing, Formation::spancolor2_brush());
			Ellipse(hdc_drawing, rect.CenterPoint().x - shape_selection_size(), rect.top - shape_selection_size(), rect.CenterPoint().x + shape_selection_size(), rect.top + shape_selection_size());
		}
		if (lock(RESIZE) == FALSE) {
			HBRUSH hbrush = CreateSolidBrush(line_color());
			HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);
			Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.top - shape_selection_size(), rect.left + shape_selection_size(), rect.top + shape_selection_size());
			Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.top - shape_selection_size(), rect.right + shape_selection_size(), rect.top + shape_selection_size());
			Rectangle(hdc_drawing, rect.right - shape_selection_size(), rect.bottom - shape_selection_size(), rect.right + shape_selection_size(), rect.bottom + shape_selection_size());
			Rectangle(hdc_drawing, rect.left - shape_selection_size(), rect.bottom - shape_selection_size(), rect.left + shape_selection_size(), rect.bottom + shape_selection_size());
			SelectObject(hdc_drawing, old_hbrush);
			DeleteObject(hbrush);
		}

		HPEN hpen_for_selection = CreatePen(PS_DOT, 0, WHITE_COLOR);
		old_hpen = (HPEN)SelectObject(hdc_drawing, hpen_for_selection);
		SetROP2(hdc_drawing, R2_MERGEPEN);
		SelectObject(hdc_drawing, GetStockObject(NULL_BRUSH));
		Rectangle(hdc_drawing, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject(hdc_drawing, old_hpen);
		DeleteObject(hpen_for_selection);
	}

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);
}

void WinLine::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	HPEN hpen = CreatePen(line_style(), line_width(), line_color());
	HBRUSH hbrush = (fill_color() == BLACK_COLOR) ? (HBRUSH)GetStockObject(NULL_BRUSH) : CreateSolidBrush(fill_color());
	HPEN old_hpen = (HPEN)SelectObject(hdc_drawing, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(hdc_drawing, hbrush);

	SetBkMode(hdc_drawing, TRANSPARENT);

	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	MoveToEx(hdc_drawing, left_top_points(full_image).x, left_top_points(full_image).y, NULL);
	LineTo(hdc_drawing, right_bottom_points(full_image).x, right_bottom_points(full_image).y);

	SelectObject(hdc_drawing, old_hpen);
	SelectObject(hdc_drawing, old_hbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);

	if (selected()) {
		HPEN hpen_for_selection = CreatePen(PS_DOT, 1, WHITE_COLOR);
		old_hpen = (HPEN)SelectObject(hdc_drawing, hpen_for_selection);
		SetROP2(hdc_drawing, R2_MERGEPEN);
		MoveToEx(hdc_drawing, left_top_points(full_image).x, left_top_points(full_image).y, NULL);
		LineTo(hdc_drawing, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
		SelectObject(hdc_drawing, old_hpen);
		DeleteObject(hpen_for_selection);
	}

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);
}

void WinPolygon::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	SetBkMode(hdc_drawing, TRANSPARENT);
	SelectObject(hdc_drawing, font_);
	SetTextColor(hdc_drawing, text_color_);

	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	std::vector<cv::Point> cv_points;
	for (int index = 0; index < points_[full_image].GetSize(); index++) {
		cv::Point cv_point;
		cv_point.x = points_[full_image][index].x;
		cv_point.y = points_[full_image][index].y;
		cv_points.push_back(cv_point);
	}
	cv::Rect cv_bounding_rect = cv::boundingRect(cv_points);

	Gdiplus::Graphics graphics(hdc_drawing);
	CArray<Gdiplus::Point> gdi_points;
	for (int index = 0; index < points_[full_image].GetSize(); index++) {
		Gdiplus::Point gdi_point;
		gdi_point.X = points_[full_image][index].x;
		gdi_point.Y = points_[full_image][index].y;
		gdi_points.Add(gdi_point);
	}
	Gdiplus::Pen pen(Gdiplus::Color(GetRValue(line_color()), GetGValue(line_color()), GetBValue(line_color())));
	if (fill_color() != BLACK_COLOR) {
		Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(fill_color()), GetGValue(fill_color()), GetBValue(fill_color())));
		graphics.FillPolygon(&brush, &gdi_points[0], (int)gdi_points.GetSize());
		if (fill_color() != line_color()) {
			graphics.DrawPolygon(&pen, &gdi_points[0], (int)gdi_points.GetSize());
		}
	} else {
		graphics.DrawPolygon(&pen, &gdi_points[0], (int)gdi_points.GetSize());
	}

	CRect rect(cv_bounding_rect.x, cv_bounding_rect.y, cv_bounding_rect.x + cv_bounding_rect.width, cv_bounding_rect.y + cv_bounding_rect.height);
	DrawText(hdc_drawing, text_, text_.GetLength(), rect, format_);

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);
}

void WinText::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	if (backmode_ != 50) {
		SetBkMode(hdc_drawing, backmode_);
	}
	SetBkColor(hdc_drawing, fill_color());
	SetTextColor(hdc_drawing, line_color());

	BOOL font_null = (font_ == NULL) ? TRUE : FALSE;
	if (font_null) {
		font_ = CreateFont(-font_height_[full_image], 0, 0, 0, font_weight_, font_italic_, font_underline_, 0, 0, 0, 0, 0, 0, font_facename_);
	}
	HFONT old_font = (HFONT)SelectObject(hdc_drawing, font_);

	XFORM xform;
	int nGraphicsMode = SetGraphicsMode(hdc_drawing, GM_ADVANCED);
	if (angle() != 0) {
		double fangle = (double)angle() / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(center(full_image).x - cos(fangle)*center(full_image).x + sin(fangle)*center(full_image).y);
		xform.eDy = (float)(center(full_image).y - cos(fangle)*center(full_image).y - sin(fangle)*center(full_image).x);

		SetWorldTransform(hdc_drawing, &xform);
	}

	if (backmode_ == 50) {
		CRect rect;
		DrawText(hdc_drawing, text_, text_.GetLength() + 1, &rect, DT_CALCRECT);
		Gdiplus::Graphics gr(hdc_drawing);
		Gdiplus::SolidBrush br(Gdiplus::Color(127, GetRValue(fill_color()), GetGValue(fill_color()), GetBValue(fill_color())));    // Alpha, Red, Blue, Green
		gr.FillRectangle(&br, left_top_points(full_image).x, left_top_points(full_image).y, rect.Width(), rect.Height());
		Gdiplus::SolidBrush semiTransBrush(Gdiplus::Color(127, GetRValue(line_color()), GetGValue(line_color()), GetBValue(line_color())));
		gr.DrawString(text_, text_.GetLength(), &Gdiplus::Font(hdc_drawing, font_), Gdiplus::PointF(left_top_points(full_image).x, left_top_points(full_image).y), &semiTransBrush);
	} else {
		CRect rect(left_top_points(full_image).x, left_top_points(full_image).y, right_bottom_points(full_image).x, right_bottom_points(full_image).y);
		DrawText(hdc_drawing, text_, text_.GetLength(), rect, format_);
	} 

	if (angle() != 0) {
		xform.eM11 = (float)1.0;
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx = (float)0;
		xform.eDy = (float)0;

		SetWorldTransform(hdc_drawing, &xform);
	}
	SetGraphicsMode(hdc_drawing, nGraphicsMode);

	SelectObject(hdc_drawing, old_font);

	if (font_null) {
		DeleteObject(font_);
		font_ = NULL;
	}
}

void WinPoint::draw(HDC hdc_drawing, BOOL full_image)
{
	if (!visible()) return;

	for (int index = 0; index < points_[full_image].GetSize(); index++) {
		SetPixel(hdc_drawing, points_[full_image][index].x, points_[full_image][index].y, color_);
	}
}

void WinPixelInfo::ShowPixelInfo(HDC hdc_drawing, BYTE R, BYTE G, BYTE B)
{
	int PixInfoWidth  = 172; /*Width of pix info box*/
	int PixInfoHeight = 62;  /*Height of pix info box*/
	int CursorOffsetX = 25;  /*Pointer Offset for X*/
	int CursorOffsetY = 25;  /*Pointer Offset for Y*/
	int row_height = 20;

	SelectObject(hdc_drawing, Formation::labelcolor_pen());
	SelectObject(hdc_drawing, Formation::labelcolor_brush());
	SelectObject(hdc_drawing, Formation::font(Formation::SMALL_FONT));
	SetBkMode(hdc_drawing, TRANSPARENT);
	SetTextColor(hdc_drawing, WHITE_COLOR);

	CRect rect(point_.x + CursorOffsetX, point_.y + CursorOffsetY, point_.x + PixInfoWidth + CursorOffsetX, point_.y + PixInfoHeight + CursorOffsetY);

	CRect caption_rect(rect.left, rect.top, rect.left + 50, rect.top + row_height);
	Rectangle(hdc_drawing, caption_rect.left, caption_rect.top, caption_rect.right, caption_rect.bottom);
	DrawText(hdc_drawing, L"XY ", 3, caption_rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	caption_rect.top = caption_rect.bottom + 1;
	caption_rect.bottom = caption_rect.top + row_height;
	Rectangle(hdc_drawing, caption_rect.left, caption_rect.top, caption_rect.right, caption_rect.bottom);
	DrawText(hdc_drawing, L"RGB ", 4, caption_rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	caption_rect.top = caption_rect.bottom + 1;
	caption_rect.bottom = caption_rect.top + row_height;
	Rectangle(hdc_drawing, caption_rect.left, caption_rect.top, caption_rect.right, caption_rect.bottom);
	DrawText(hdc_drawing, L"CPW ", 4, caption_rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	CString str;
	CRect value_rect(caption_rect.right + 1, rect.top, rect.right, rect.top + row_height);
	str.Format(L" %d,%d", point_.x, point_.y);
	Rectangle(hdc_drawing, value_rect.left, value_rect.top, value_rect.right, value_rect.bottom);
	DrawText(hdc_drawing, str, str.GetLength(), value_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	value_rect.right = caption_rect.right + 1 + 50 * 2;
	value_rect.top = value_rect.bottom + 1;
	value_rect.bottom = value_rect.top + row_height;
	str.Format(L" %d,%d,%d", R, G, B);
	Rectangle(hdc_drawing, value_rect.left, value_rect.top, value_rect.right, value_rect.bottom);
	DrawText(hdc_drawing, str, str.GetLength(), value_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	CRect color_rect = value_rect;
	color_rect.left = value_rect.right + 1;
	color_rect.right = rect.right;
	if (pixel_color_ == RGB(0, 0, 0)) pixel_color_ = RGB(1, 0, 0);
	HBRUSH brush = CreateSolidBrush(pixel_color_);
	HBRUSH old_brush = (HBRUSH)SelectObject(hdc_drawing, brush);
	Rectangle(hdc_drawing, color_rect.left, color_rect.top, color_rect.right, color_rect.bottom);
	SelectObject(hdc_drawing, old_brush);
	DeleteObject(brush);

	value_rect.top = value_rect.bottom + 1;
	value_rect.bottom = value_rect.top + row_height;
	cv::Mat rgb(1, 1, CV_8UC3);
	cv::Mat hsl(1, 1, CV_8UC3);
	rgb.at<cv::Vec3b>(0, 0) = cv::Vec3b(B, G, R);
	cv::cvtColor(rgb, hsl, cv::COLOR_BGR2HSV);
	cv::Vec3b hsl_color = hsl.at<cv::Vec3b>(0, 0);
	COLORREF hsl_clr = hsl_color[0] << 16 | hsl_color[1] << 8 | hsl_color[2];
	rgb.release();
	hsl.release();
	str.Format(L" %d,%d,%d", hsl_color[0], hsl_color[1], hsl_color[2]);
	Rectangle(hdc_drawing, value_rect.left, value_rect.top, value_rect.right, value_rect.bottom);
	DrawText(hdc_drawing, str, str.GetLength(), value_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	color_rect = value_rect;
	color_rect.left = value_rect.right + 1;
	color_rect.right = rect.right;
	if (hsl_clr == RGB(0, 0, 0)) hsl_clr = RGB(1, 0, 0);
	brush = CreateSolidBrush(hsl_clr);
	old_brush = (HBRUSH)SelectObject(hdc_drawing, brush);
	Rectangle(hdc_drawing, color_rect.left, color_rect.top, color_rect.right, color_rect.bottom);
	SelectObject(hdc_drawing, old_brush);
	DeleteObject(brush);
}