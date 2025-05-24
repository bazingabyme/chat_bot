// thumblistctrlpro.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_THUMBLISTCTRLPRO __declspec(dllexport)
#include "thumblistctrlpro.h"

// ThumbListCtrlPro

class ThumbListCtrl;

IMPLEMENT_DYNAMIC(ThumbListCtrlPro, CStatic)

ThumbListCtrlPro::ThumbListCtrlPro()
{
	max_items_ = 0;
	selected_item_index_ = -1;
	allow_selection_ = TRUE;
	allow_dragging_ = TRUE;
	point_down_.SetPoint(-1, -1);
	thumb_list_filled_ = FALSE;
	back_color_ = WHITE_COLOR;
	text_color_ = LABEL_COLOR;

	thumb_image_height_ = thumb_image_width_ = 0;
	thumb_title_height_ = 0;
	thumb_title_align_ = DT_LEFT;

	bitmap_info_[COLOR_24BIT] = NULL;
	bitmap_info_[GRAY_8BIT] = NULL;
}

ThumbListCtrlPro::~ThumbListCtrlPro()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ThumbListCtrlPro, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_THUMBLISTCTRLPRO_SETSCROLLBARPOS, SetScrollBarPosition)
END_MESSAGE_MAP()

// ThumbListCtrlPro message handlers

void ThumbListCtrlPro::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

BOOL ThumbListCtrlPro::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd() && pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;
	
	return CStatic::PreTranslateMessage(pMsg);
}

void ThumbListCtrlPro::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COfflineDC offdc(&dc, &client_rect_);
	offdc.FillSolidRect(client_rect_, back_color_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	int top = -GetScrollPos(SB_VERT);

	int prev_index = -1;
	for (int index = 0; index < int(item_info_list_.size()); index++) {

		if (!rect_list_[index].IsRectEmpty()) {

			if (prev_index != -1) {
				top += (thumb_image_height_ + spacing_);
			}
			prev_index = index;

			if (item_info_list_[index].buffer.data != NULL) {
				rect_list_[index].MoveToY(top);

				if (rect_list_[index].bottom > (client_rect_.top - thumb_image_height_ * 2) && rect_list_[index].top < (client_rect_.bottom + thumb_image_height_ * 2)) {

					StretchDIBits(offdc.GetSafeHdc(), rect_list_[index].left, rect_list_[index].top, thumb_image_width_, thumb_image_height_, 0, 0, thumb_image_width_, thumb_image_height_, item_info_list_[index].buffer.data, bitmap_info_[image_type_], DIB_RGB_COLORS, SRCCOPY);

					if (thumb_title_height_ > 0) {

						CRect thumb_title_rect = rect_list_[index];
						thumb_title_rect.bottom = rect_list_[index].top + thumb_title_height_;

						if (item_info_list_[index].color_code != BLACK_COLOR) {
							CRect color_code_rect;
							color_code_rect.left = thumb_title_rect.left + Formation::spacing2();
							color_code_rect.top = thumb_title_rect.top + Formation::spacing2();
							color_code_rect.right = color_code_rect.left + Formation::spacing4();
							color_code_rect.bottom = thumb_title_rect.bottom - Formation::spacing2();

							Gdiplus::Graphics graphics(offdc);
							Gdiplus::Point poly_points[3];
							poly_points[0].X = color_code_rect.CenterPoint().x - Formation::spacing() - Formation::spacing();
							poly_points[1].X = color_code_rect.CenterPoint().x;
							poly_points[2].X = color_code_rect.CenterPoint().x + Formation::spacing() + Formation::spacing();
							poly_points[0].Y = poly_points[2].Y = color_code_rect.CenterPoint().y + Formation::spacing() + Formation::spacing() / 2;
							poly_points[1].Y = color_code_rect.CenterPoint().y - Formation::spacing() - Formation::spacing() / 2;
							Gdiplus::SolidBrush gdi_brush(Gdiplus::Color(GetRValue(item_info_list_[index].color_code), GetGValue(item_info_list_[index].color_code), GetBValue(item_info_list_[index].color_code)));
							graphics.FillPolygon(&gdi_brush, poly_points, 3);

							thumb_title_rect.left = color_code_rect.right + Formation::spacing();
						}

						offdc.SetTextColor(item_info_list_[index].text_color);
						offdc.DrawText(item_info_list_[index].title_text, thumb_title_rect, thumb_title_align_ | DT_VCENTER | DT_SINGLELINE);
					}

					if (item_info_list_[index].status1 != THUMBPRO_BORDER_COLOR::NO_BORDER) {
						CRect rect1 = CRect(rect_list_[index].left + Formation::spacing(), rect_list_[index].top + Formation::spacing(), rect_list_[index].left + Formation::spacing3(), rect_list_[index].top + Formation::spacing3());
						offdc.FillSolidRect(rect1, item_info_list_[index].status1 == THUMBPRO_BORDER_COLOR::GREEN_BORDER ? ACCEPT_COLOR : REJECT_COLOR);
					}
					if (item_info_list_[index].status2 != THUMBPRO_BORDER_COLOR::NO_BORDER) {
						CRect rect2 = CRect(rect_list_[index].right - Formation::spacing3(), rect_list_[index].top + Formation::spacing(), rect_list_[index].right - Formation::spacing(), rect_list_[index].top + Formation::spacing3());
						offdc.FillSolidRect(rect2, item_info_list_[index].status2 == THUMBPRO_BORDER_COLOR::GREEN_BORDER ? ACCEPT_COLOR : REJECT_COLOR);
					}

					offdc.SetTextColor(text_color_);
					CRect rect_text1 = CRect(rect_list_[index].left + Formation::spacing(), rect_list_[index].top, rect_list_[index].right - Formation::spacing(), rect_list_[index].bottom - 1);
					offdc.DrawText(item_info_list_[index].text1, rect_text1, DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
					CRect rect_text2 = CRect(rect_list_[index].left + Formation::spacing(), rect_list_[index].top, rect_list_[index].right - Formation::spacing(), rect_list_[index].bottom - 1);
					offdc.DrawText(item_info_list_[index].text2, rect_text2, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
					CRect rect_text3 = CRect(rect_list_[index].left + Formation::spacing(), rect_list_[index].top, rect_list_[index].right - Formation::spacing(), rect_list_[index].bottom - 1);
					offdc.DrawText(item_info_list_[index].text3, rect_text3, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
					if (selected_item_index_ == index && allow_selection_) {
						Gdiplus::SolidBrush brush(Gdiplus::Color(72, 0, 0, 255));
						Gdiplus::Graphics gr_photo(offdc);
						gr_photo.FillRectangle(&brush, rect_list_[index].left, rect_list_[index].top, thumb_image_width_, thumb_image_height_);
					}
				}
			}
		}
	}
}

LRESULT ThumbListCtrlPro::SetScrollBarPosition(WPARAM wparam, LPARAM lparam) {

	int document_height = 0;
	for (int index = 0; index < int(item_info_list_.size()); index++) {
		if (!rect_list_[index].IsRectEmpty()) {
			document_height += (thumb_image_height_ + spacing_);
		}
	}

	if(client_rect_.Height() >= document_height) {
		SCROLLINFO	si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = 0;
		si.nMax = 0;
		si.nMin = 0;
		SetScrollInfo(SB_VERT, &si);
		SetScrollPos(SB_VERT, 0);
	} else {
		SCROLLINFO	si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = client_rect_.Height();
		si.nMax = document_height;
		si.nMin = 0;
		SetScrollInfo(SB_VERT, &si);
		SetScrollPos(SB_VERT, document_height);
		//PostMessage(WM_VSCROLL, SB_BOTTOM);
	}

	return 0;
}

void ThumbListCtrlPro::Create(int max_items, int image_width, int image_height, int spacing, THUMBPRO_IMAGE_TYPE image_type) {

	if (max_items <= 0) {
		return;
	}

	max_items_ = max_items;
	source_image_width_ = image_width;
	source_image_height_ = image_height;
	spacing_ = spacing;
	image_type_ = image_type;
	thumb_list_filled_ = FALSE;
	selected_item_index_ = -1;
	thumb_index_ = 0;
	point_down_.SetPoint(-1, -1);

	GetClientRect(client_rect_);
	client_rect_.right -= (GetSystemMetrics(SM_CYVSCROLL) + Formation::spacing());

	item_info_list_.clear();
	rect_list_.RemoveAll();

	thumb_image_width_ = client_rect_.Width();
	thumb_image_width_ -= (thumb_image_width_ % 4);
	thumb_image_height_ = (thumb_image_width_ * image_height / image_width);
			
	CRect rect;
	rect.top = rect.left = 0;
	rect.right = thumb_image_width_;
	for (int cell_index = 0; cell_index < max_items_; cell_index++) {
		rect.bottom = rect.top + thumb_image_height_;
		rect_list_.Add(rect);
		rect.top = rect.bottom + spacing_;
	}
	
	SetScrollBarPosition();

	//GENERATE BITMAP INFO HEADER FOR 24BIT COLOR
	long BmpSize = sizeof(BITMAPINFO);
	bitmap_info_[COLOR_24BIT] = (BITMAPINFO *)calloc(1, BmpSize);
	bitmap_info_[COLOR_24BIT]->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_[COLOR_24BIT]->bmiHeader.biWidth = thumb_image_width_;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biHeight = -(thumb_image_height_);
	bitmap_info_[COLOR_24BIT]->bmiHeader.biPlanes = 1;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biBitCount = 24;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biCompression = BI_RGB;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (thumb_image_width_ * (thumb_image_height_) * 3);

	//GENERATE BITMAP INFO HEADER FOR FOR 8BIT GRAY
	BmpSize = sizeof(BITMAPINFO) + (256 * sizeof(RGBQUAD));
	bitmap_info_[GRAY_8BIT] = (BITMAPINFO *)calloc(1, BmpSize);
	bitmap_info_[GRAY_8BIT]->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_[GRAY_8BIT]->bmiHeader.biWidth = thumb_image_width_;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biHeight = -(thumb_image_height_);
	bitmap_info_[GRAY_8BIT]->bmiHeader.biPlanes = 1;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biBitCount = 8;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biCompression = BI_RGB;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (256 * sizeof(RGBQUAD)) + (thumb_image_width_ * (thumb_image_height_));
	for (int counter = 0; counter < 256; counter++) {
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbBlue = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbGreen = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbRed = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbReserved = BYTE(0);
	}

	Invalidate(FALSE);
}

void ThumbListCtrlPro::Destroy() {

	allow_dragging_ = FALSE;

	if (bitmap_info_[COLOR_24BIT] != NULL) {
		delete bitmap_info_[COLOR_24BIT];
		bitmap_info_[COLOR_24BIT] = NULL;
	}
	if (bitmap_info_[GRAY_8BIT] != NULL) {
		delete bitmap_info_[GRAY_8BIT];
		bitmap_info_[GRAY_8BIT] = NULL;
	}

	EraseAllItems();

	rect_list_.RemoveAll();

	drag_image_.DeleteImageList();
}

void ThumbListCtrlPro::Move(CRect rect) {

	MoveWindow(rect);

	//client_rect_.left = client_rect_.top = 0;
	//client_rect_.right = (rect.Width() - (GetSystemMetrics(SM_CYVSCROLL) + Formation::spacing()));
	client_rect_.bottom = rect.Height();

	SetScrollBarPosition();

	Invalidate(FALSE);
}

void ThumbListCtrlPro::AddItem(cv::Mat& img, THUMBPRO_BORDER_COLOR status) {

	ItemInfo item_info;
	item_info.status1 = status;
	
	item_info.buffer.create(cv::Size(thumb_image_width_, thumb_image_height_), image_type_ == COLOR_24BIT ? CV_8UC3 : CV_8UC1);
	cv::resize(img, item_info.buffer, item_info.buffer.size());

	if (thumb_index_++ == max_items_) thumb_index_ = 1;
	item_info.text1.Format(_T("%d"), thumb_index_);
	item_info.text3 = Formation::GetTime();

	item_info_list_.push_back(item_info);

	if (int(item_info_list_.size()) > max_items_) {
		item_info_list_[0].buffer.release();
		item_info_list_.pop_front();
	}

	selected_item_index_ = int(item_info_list_.size()) - 1;

	GetParent()->PostMessage(WM_THUMBLISTCTRLPRO_ITEMINSERTED, (WPARAM)this, (LPARAM)selected_item_index_);

	if (!thumb_list_filled_) {
		PostMessage(WM_THUMBLISTCTRLPRO_SETSCROLLBARPOS);
		if (int(item_info_list_.size()) == max_items_) {
			thumb_list_filled_ = TRUE;
		}
	}

	if (IsWindowVisible()) Invalidate(FALSE);
}

void ThumbListCtrlPro::AddItem(cv::Mat& img, CString text1, CString text2, CString text3, THUMBPRO_BORDER_COLOR status) {

	ItemInfo item_info;
	item_info.status1 = status;

	item_info.buffer.create(cv::Size(thumb_image_width_, thumb_image_height_), image_type_ == COLOR_24BIT ? CV_8UC3 : CV_8UC1);
	cv::resize(img, item_info.buffer, item_info.buffer.size());

	if (thumb_index_++ == max_items_) thumb_index_ = 1;
	item_info.text1 = text1;
	item_info.text2 = text2;
	item_info.text3 = text3;

	item_info_list_.push_back(item_info);

	if (int(item_info_list_.size()) > max_items_) {
		item_info_list_[0].buffer.release();
		item_info_list_.pop_front();
	}

	selected_item_index_ = int(item_info_list_.size()) - 1;

	GetParent()->PostMessage(WM_THUMBLISTCTRLPRO_ITEMINSERTED, (WPARAM)this, (LPARAM)selected_item_index_);
	
	if (!thumb_list_filled_) {
		PostMessage(WM_THUMBLISTCTRLPRO_SETSCROLLBARPOS);
		if (int(item_info_list_.size()) == max_items_) {
			thumb_list_filled_ = TRUE;
		}
	}

	if (IsWindowVisible()) Invalidate(FALSE);
}

void ThumbListCtrlPro::AddItem(cv::Mat& img, COLORREF color_code, CString thumb_title, COLORREF text_color) {

	ItemInfo item_info;
	item_info.status1 = THUMBPRO_BORDER_COLOR::NO_BORDER;
	item_info.text_color = text_color;
	item_info.color_code = color_code;

	item_info.buffer.create(cv::Size(thumb_image_width_, thumb_image_height_), image_type_ == COLOR_24BIT ? CV_8UC3 : CV_8UC1);
	cv::resize(img, item_info.buffer, item_info.buffer.size());

	if (thumb_index_++ == max_items_) thumb_index_ = 1;
	item_info.title_text = thumb_title;

	item_info_list_.push_back(item_info);

	if (int(item_info_list_.size()) > max_items_) {
		item_info_list_[0].buffer.release();
		item_info_list_.pop_front();
	}

	selected_item_index_ = int(item_info_list_.size()) - 1;

	GetParent()->PostMessage(WM_THUMBLISTCTRLPRO_ITEMINSERTED, (WPARAM)this, (LPARAM)selected_item_index_);

	if (!thumb_list_filled_) {
		PostMessage(WM_THUMBLISTCTRLPRO_SETSCROLLBARPOS);
		if (int(item_info_list_.size()) == max_items_) {
			thumb_list_filled_ = TRUE;
		}
	}

	if (IsWindowVisible()) Invalidate(FALSE);
}

void ThumbListCtrlPro::ShowAllItems(BOOL show_all) {

	if (show_all) {
		CRect rect;
		rect.top = rect.left = 0;
		rect.right = thumb_image_width_;
		for (int index = 0; index < max_items_; index++) {
			rect.bottom = rect.top + thumb_image_height_;
			rect_list_[index] = rect;
			rect.top = rect.bottom + spacing_;
		}
	} else {
		for (int index = 0; index < int(item_info_list_.size()); index++) {
			if (item_info_list_[index].status1 != ThumbListCtrlPro::RED_BORDER) {
				rect_list_[index].SetRectEmpty();
			}
		}
	}

	SetScrollBarPosition();

	Invalidate(FALSE);
}

BOOL ThumbListCtrlPro::SetStatus1(int item_index, THUMBPRO_BORDER_COLOR border_color) {

	if (item_index < int(item_info_list_.size())) {

		selected_item_index_ = item_index;

		item_info_list_[item_index].status1 = border_color;

		Invalidate(FALSE);
	}

	return TRUE;
}

BOOL ThumbListCtrlPro::SetStatus2(int item_index, THUMBPRO_BORDER_COLOR border_color) {

	if (item_index < int(item_info_list_.size())) {

		selected_item_index_ = item_index;

		item_info_list_[item_index].status2 = border_color;

		Invalidate(FALSE);
	}

	return TRUE;
}

void ThumbListCtrlPro::SelectItem(int item_index) {

	selected_item_index_ = item_index;

	if (selected_item_index_ == -1) {

		PostMessage(WM_VSCROLL, SB_BOTTOM);

	} else if (selected_item_index_ != -1 && selected_item_index_ < int(item_info_list_.size())) {

		if (rect_list_[selected_item_index_].top < client_rect_.top) {
			int pos = rect_list_[selected_item_index_].top - client_rect_.top;
			SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
		}
		if (rect_list_[selected_item_index_].bottom > client_rect_.bottom) {
			int pos = rect_list_[selected_item_index_].bottom - client_rect_.bottom;
			SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
		}
	}
	Invalidate(FALSE);
}

void ThumbListCtrlPro::SetMaxItems(int max_items) {

	max_items_ = max_items; 
	if (int(item_info_list_.size()) != max_items_) {
		thumb_list_filled_ = FALSE;
	} else {
		thumb_list_filled_ = TRUE;
	}
}

int ThumbListCtrlPro::GetVisibleItemCount() {

	int item_count = 0;

	for (int index = 0; index < int(item_info_list_.size()); index++) {
		if (!rect_list_[index].IsRectEmpty()) {
			++item_count;
		}
	}

	return item_count;
}

int ThumbListCtrlPro::GetItemCount() {

	return int(item_info_list_.size());
}

void ThumbListCtrlPro::EraseAllItems() {

	thumb_list_filled_ = FALSE;
	selected_item_index_ = -1;
	thumb_index_ = 0;

	for(int index = 0; index < int(item_info_list_.size()); index++) {
		item_info_list_[index].buffer.release();
		item_info_list_[index].text1.Empty();
		item_info_list_[index].text2.Empty();
		item_info_list_[index].text3.Empty();
	}
	item_info_list_.clear();
	//rect_list_.RemoveAll();

	SetScrollBarPosition();

	if (m_hWnd != NULL) {
		Invalidate(FALSE);
	}
}

BOOL ThumbListCtrlPro::IsItemInsertionAllowed() {

	if (int(item_info_list_.size()) == max_items_) {
		return FALSE;
	}
	return TRUE;
}

void ThumbListCtrlPro::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!allow_selection_) {
		return;
	}

	selected_item_index_ = -1;
	for (int index = 0; index < max_items_; index++) {
		if (rect_list_[index].PtInRect(point)) {
			if (rect_list_[index].top < client_rect_.top) {
				int pos = rect_list_[index].top - client_rect_.top;
				SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
			}
			if (rect_list_[index].bottom > client_rect_.bottom) {
				int pos = rect_list_[index].bottom - client_rect_.bottom;
				SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
			}
			selected_item_index_ = index;
			Invalidate(FALSE);
			point_down_ = point;
			break;
		}
	}

	scroll_item_ = FALSE;
	drag_item_ = FALSE;

	SetCapture();

	CStatic::OnLButtonDown(nFlags, point);
}
void ThumbListCtrlPro::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON && (point_down_.x != -1 && point_down_.y != -1)) {
		
		if (!scroll_item_ && allow_dragging_) {

			if (!drag_item_) {

				if (selected_item_index_ != -1 && selected_item_index_ < int(item_info_list_.size())) {
					
					drag_image_.Create(thumb_image_width_, thumb_image_height_, ILC_MASK | ILC_COLOR32, 0, 0);
					Gdiplus::Bitmap img(bitmap_info_[image_type_], item_info_list_[selected_item_index_].buffer.data);
					HBITMAP hbitmap;
					img.GetHBITMAP(NULL, &hbitmap);
					CBitmap bitmap;
					bitmap.Attach(hbitmap);
					drag_image_.Add(&bitmap, RGB(0, 0, 0));
					bitmap.Detach();
					DeleteObject(hbitmap);
					
					drag_image_.BeginDrag(0, CPoint(-Formation::spacing() * 3, -Formation::spacing()));

					ClientToScreen(&point);
					drag_image_.DragEnter(GetDesktopWindow(), point);

					drag_item_ = TRUE;
				}

			} else {

				ClientToScreen(&point);
				drag_image_.DragMove(point);
			}
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void ThumbListCtrlPro::OnLButtonUp(UINT nFlags, CPoint point)
{	
	ReleaseCapture();

	if (drag_item_) {

		drag_image_.DragLeave(NULL);
		drag_image_.EndDrag();
		drag_image_.DeleteImageList();

		ClientToScreen(&point);
		CWnd* wnd_list_control = WindowFromPoint(point);
		if (wnd_list_control->IsKindOf(RUNTIME_CLASS(CListCtrl)) && wnd_list_control != this && selected_item_index_ >= 0) {
			CWnd* wnd = wnd_list_control->GetParent();
			SourceListCtrlProInfo sourcelistctrlinfo;
			sourcelistctrlinfo.thumblistctrl = this;
			sourcelistctrlinfo.drag_index = selected_item_index_;
			wnd->SendMessage(WM_THUMBLISTCTRLPRO_ITEMDRAGGED, (WPARAM)wnd_list_control, (LPARAM)&sourcelistctrlinfo);
		}

	} else {

		if (!scroll_item_ && allow_selection_ && selected_item_index_ >= 0) {
	
			GetParent()->SendMessageW(WM_THUMBLISTCTRLPRO_SELCHANGE, (WPARAM)this, (LPARAM)(selected_item_index_));
		}
	}

	point_down_.SetPoint(-1, -1);
	scroll_item_ = FALSE;
	drag_item_ = FALSE;

	CStatic::OnLButtonUp(nFlags, point);
}

void ThumbListCtrlPro::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iScrollBarPos = GetScrollPos( SB_VERT );

	switch( nSBCode )
	{
		case SB_LINEUP:
			iScrollBarPos = std::max(iScrollBarPos - Formation::spacing(), 0);
		break;
		case SB_LINEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + Formation::spacing(), GetScrollLimit(SB_VERT) );
		break;
		case SB_PAGEUP:
			iScrollBarPos = std::max(iScrollBarPos - (client_rect_.Height() - spacing_), 0);
		break;
		case SB_PAGEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + (client_rect_.Height() - spacing_), GetScrollLimit(SB_VERT));
		break;
		case SB_TOP:
			iScrollBarPos = 0; // std::min(iScrollBarPos + (client_rect_.Height() - spacing_), GetScrollLimit(SB_VERT));
		break;
		case SB_BOTTOM:
			iScrollBarPos = int(item_info_list_.size()) * (thumb_image_height_ + spacing_); // std::min(iScrollBarPos + (client_rect_.Height() - spacing_), GetScrollLimit(SB_VERT));
		break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			SCROLLINFO si;
			ZeroMemory( &si, sizeof(SCROLLINFO) );
			si.cbSize	= sizeof(SCROLLINFO);
			si.fMask	= SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS)) {
				iScrollBarPos = si.nTrackPos;
			} else {
				iScrollBarPos = (UINT)nPos;
			}
			break;
		}
	}		

	SetScrollPos(SB_VERT, iScrollBarPos);
	Invalidate(FALSE);
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT ThumbListCtrlPro::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCHITTEST || message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK)
		return ::DefWindowProc(m_hWnd, message, wParam, lParam);

	return CStatic::WindowProc(message, wParam, lParam);
}