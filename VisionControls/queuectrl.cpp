// queuectrl.cpp : implementation file
//

#include "stdafx.h"
//#include "..\VisionStudio\find_inner_rect.h"

#define INCLUDE_QUEUECTRL __declspec(dllexport)
#include "queuectrl.h"

// QueueCtrl

IMPLEMENT_DYNAMIC(QueueCtrl, CStatic)

QueueCtrl::QueueCtrl()
{
	cs_.OwningThread = NULL;
	set_as_tiny_page_ = FALSE;
	count_ = 0;
}

QueueCtrl::~QueueCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(QueueCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_QUEUECTRL_ADDITEM, Add)
END_MESSAGE_MAP()

// QueueCtrl message handlers

void QueueCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	if (cdc_.GetSafeHdc() != NULL) {

		int left = queue_rect_.right - cell_rect_.Width() - 1;
		int top = queue_rect_.bottom - cell_rect_.Height() - 1;
		int index = -1;
		BOOL right_to_left_ = TRUE;
		for (int i = 0; i < count_; i++) {
			if (++index == cols_) {
				index = 0;
				top -= cell_rect_.Height();
				right_to_left_ = !right_to_left_;
			}
			if (index != 0 && index != (cols_)) {
				left = right_to_left_ ? (left -= cell_rect_.Width()) : (left += cell_rect_.Width());
			}
			cell_rect_.MoveToXY(left, top);
		
			if (queue_array_.GetAt(i) == 0) {
				cdc_.SelectObject(Formation::reject_brush());
			} else if (queue_array_.GetAt(i) == 1) {
				cdc_.SelectObject(Formation::accept_brush());
			}
			cdc_.Rectangle(cell_rect_.left + 2, cell_rect_.top, cell_rect_.right - 1, cell_rect_.bottom - 3);
		}
		Gdiplus::Graphics graphics(cdc_);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		//graphics.DrawPolygon(Formation::blackcolor_pen_gdi(), points_for_in_, 3);
		graphics.DrawPolygon(Formation::blackcolor_pen_gdi(), points_for_out_, 3);

		if (status_changed_) {
			graphics.FillPolygon(Formation::spancolor2_brush_gdi(), points_for_in_, 3);
			//graphics.FillEllipse(Formation::spancolor2_brush_gdi(), title_rect_.right - (INT)(Formation::vertical_spacing() * 1.5), title_rect_.top + Formation::vertical_spacing() / 2, Formation::vertical_spacing(), Formation::vertical_spacing());
		} else {
			graphics.FillPolygon(Formation::spancolor1_brush_for_selection_gdi(), points_for_in_, 3);
			//graphics.FillEllipse(Formation::yellowcolor_brush_gdi(), title_rect_.right - (INT)(Formation::vertical_spacing() * 1.5), title_rect_.top + Formation::vertical_spacing() / 2, Formation::vertical_spacing(), Formation::vertical_spacing());
		}

		dc.BitBlt(client_rect_.left, client_rect_.top, client_rect_.Width(), client_rect_.Height(), &cdc_, 0, 0, SRCCOPY);
	}
}

void QueueCtrl::Create(CString title, int count) {


	count_ = count;
	text_ = L" " + title;

	InitializeCriticalSection(&cs_);
	
	Update();

	status_changed_ = FALSE;

	queue_array_.RemoveAll();
	for (int i = 0; i < count_; i++) {
		Add(0, NULL);
	}
}

void QueueCtrl::Destroy() {

	if (cs_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_);
	}

	queue_array_.RemoveAll();
	
	cdc_.DeleteDC();
	bitmap_.DeleteObject();
}

void QueueCtrl::Reset() {

	queue_array_.RemoveAll();
	for (int i = 0; i < count_; i++) {
		Add(0, NULL);
	}

	Invalidate();
}

void QueueCtrl::Update() {

	GetClientRect(client_rect_);

	CDC* dc = GetDC();
	cdc_.DeleteDC();
	bitmap_.DeleteObject();
	cdc_.CreateCompatibleDC(dc);
	bitmap_.CreateCompatibleBitmap(dc, client_rect_.Width(), client_rect_.Height());
	cdc_.SelectObject(bitmap_);
	cdc_.BitBlt(client_rect_.left, client_rect_.top, client_rect_.Width(), client_rect_.Height(), NULL, 0, 0, WHITENESS);
	cdc_.SelectObject(GetFont());
	cdc_.SetBkMode(TRANSPARENT);
	cdc_.SetTextColor(WHITE_COLOR);
	cdc_.SelectObject(Formation::font(Formation::BIG_FONT));
	ReleaseDC(dc);

	if (count_ == 0) return;

	if (set_as_tiny_page_) {
		title_rect_.SetRectEmpty();
	} else {
		title_rect_.SetRectEmpty();
		//title_rect_.SetRect(client_rect_.left, client_rect_.top, client_rect_.right, Formation::control_height());
	}
	queue_rect_.SetRect(client_rect_.left, title_rect_.bottom + Formation::spacing(), client_rect_.right, client_rect_.bottom);
	
	int rect_width = Formation::DivideInSquare(queue_rect_.Width() - 4, queue_rect_.Height() - 4, count_, rows_, cols_);
	if (rect_width > Formation::control_height()) {
		rect_width = Formation::control_height();
	}
	cols_ = client_rect_.Width() / rect_width;
	rows_ = count_ / cols_;
	rows_ = (rows_ == 0) ? rows_ + 1 : (count_ % (rows_ * cols_)) != 0 ? rows_ + 1 : rows_;
	
	int bottom = rect_width * rows_;
	queue_rect_.SetRect(queue_rect_.left, queue_rect_.top, queue_rect_.right, queue_rect_.top + bottom);

	cell_rect_.SetRect(queue_rect_.right - rect_width, queue_rect_.bottom - rect_width, queue_rect_.right, queue_rect_.bottom);
	border_rect_.SetRect(queue_rect_.right - (cols_ * rect_width) - 1, queue_rect_.bottom - (rows_ * rect_width) - 1, queue_rect_.right - 1, queue_rect_.bottom - 1);

	push_from_left_ = (rows_ % 2 == 0) ? FALSE : TRUE;

	//if (set_as_tiny_page) {
	//	title_rect_.SetRectEmpty();
	//} else {
	//	title_rect_.SetRect(border_rect_.left, title_rect_.top, title_rect_.right, title_rect_.bottom);
	//}

	cdc_.SelectObject(Formation::blackcolor_pen2());
	cdc_.MoveTo(border_rect_.right, border_rect_.top - 2);
	cdc_.LineTo(border_rect_.left, border_rect_.top - 2);												//TOP BORDER LINE
	cdc_.MoveTo(border_rect_.right, border_rect_.bottom - 2);
	cdc_.LineTo(border_rect_.left, border_rect_.bottom - 2);											//BOTTOM BORDER LINE
	if (push_from_left_) {
		cdc_.LineTo(border_rect_.left, border_rect_.bottom - (rows_ - 1) * cell_rect_.Height() - 2);	//LEFT BORDER LINE
		cdc_.MoveTo(border_rect_.right, border_rect_.bottom - cell_rect_.Height() - 2);
		cdc_.LineTo(border_rect_.right, border_rect_.top - 2);											//RIGHT BORDER LINE
	} else {
		cdc_.LineTo(border_rect_.left, border_rect_.top - 2);											//LEFT BORDER LINE
		cdc_.MoveTo(border_rect_.right, border_rect_.bottom - cell_rect_.Height() - 2);
		cdc_.LineTo(border_rect_.right, border_rect_.bottom - (rows_ - 1) * cell_rect_.Height() - 2);	//RIGHT BORDER LINE
	}
	for (int i = 1; i < rows_; i++) {
		if (i % 2 != 0) {
			cdc_.MoveTo(border_rect_.right, border_rect_.bottom - i*cell_rect_.Height() - 2);
			cdc_.LineTo(border_rect_.left + cell_rect_.Width(), border_rect_.bottom - i*cell_rect_.Height() - 2);
		} else {
			cdc_.MoveTo(border_rect_.left, border_rect_.bottom - i*cell_rect_.Height() - 2);
			cdc_.LineTo(border_rect_.right - cell_rect_.Width(), border_rect_.bottom - i*cell_rect_.Height() - 2);
		}
	}

	cdc_.SelectObject(&Formation::labelcolor_brush());
	cdc_.SelectObject(GetStockObject(NULL_PEN));
	cdc_.Rectangle(title_rect_);
	cdc_.DrawText(text_, title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	cdc_.SelectObject(GetStockObject(NULL_PEN));
	cdc_.SelectObject(GetStockObject(NULL_BRUSH));

	if (push_from_left_) {
		points_for_in_[0].X = points_for_in_[2].X = border_rect_.left + rect_width * 30 / 100;
		points_for_in_[1].X = border_rect_.left + rect_width * 70 / 100;
		points_for_in_[0].Y = border_rect_.top + rect_width * 20 / 100;
		points_for_in_[1].Y = border_rect_.top + rect_width * 45 / 100;
		points_for_in_[2].Y = border_rect_.top + rect_width * 70 / 100;
	} else {
		points_for_in_[0].X = points_for_in_[2].X = border_rect_.right - rect_width * 30 / 100;
		points_for_in_[1].X = border_rect_.right - rect_width * 70 / 100;
		points_for_in_[0].Y = border_rect_.top + rect_width * 20 / 100;
		points_for_in_[1].Y = border_rect_.top + rect_width * 45 / 100;
		points_for_in_[2].Y = border_rect_.top + rect_width * 70 / 100;
	}
	points_for_out_[0].X = points_for_out_[2].X = border_rect_.right - rect_width * 70 / 100;
	points_for_out_[1].X = border_rect_.right - rect_width * 30 / 100;
	points_for_out_[0].Y = border_rect_.bottom - rect_width * 30 / 100;
	points_for_out_[1].Y = border_rect_.bottom - rect_width * 55 / 100;
	points_for_out_[2].Y = border_rect_.bottom - rect_width * 80 / 100;
}

LRESULT QueueCtrl::Add(WPARAM wparam, LPARAM lparam) {

	if (count_ == 0) return 0;

	EnterCriticalSection(&cs_);

	if (count_ == queue_array_.GetSize()) {
		queue_array_.RemoveAt(0);
	}
	queue_array_.Add((int)wparam);

	if (IsWindowVisible()) {
		status_changed_ = !status_changed_;
		InvalidateRect(client_rect_, TRUE);
	}

	LeaveCriticalSection(&cs_);

	return 0;
}

void QueueCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{

	CStatic::OnLButtonDown(nFlags, point);
}

void QueueCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {

	}

	CStatic::OnMouseMove(nFlags, point);
}

void QueueCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

	CStatic::OnLButtonUp(nFlags, point);
}