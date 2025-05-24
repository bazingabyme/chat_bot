// teach_listControl.cpp : implementation file
//

#include "stdafx.h"

#include "commctrl.h"
#pragma comment(lib, "comctl32.lib")

#define INCLUDE_LISTCTRL_THUMB __declspec(dllexport)
#include "thumblistctrl.h"

// ThumbListCtrl

IMPLEMENT_DYNAMIC(ThumbListCtrl, CListCtrl)

ThumbListCtrl::ThumbListCtrl()
{
	max_items_ = 0;
	allow_selection_ = TRUE;
	allow_dragging_ = FALSE;
	allow_deletion_ = FALSE;
	text_on_image_ = FALSE;
	show_date_time_ = FALSE;

	thumb_image_list_.m_hImageList = NULL;
	drag_image_ptr_ = NULL;
	cs_.OwningThread = NULL;
	//handle_ = NULL;

	bitmap_info_[COLOR_24BIT] = NULL;
	bitmap_info_[GRAY_8BIT] = NULL;
}

ThumbListCtrl::~ThumbListCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ThumbListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &ThumbListCtrl::OnLvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_THUMBLISTCTRL_ADDITEM, AddThumbImage)
	ON_MESSAGE(WM_THUMBLISTCTRL_INSERTITEM, InsertItemMessage)
END_MESSAGE_MAP()

// ThumbListCtrl message handlers

void ThumbListCtrl::Destroy() {

	allow_dragging_ = FALSE;
	allow_deletion_ = FALSE;

	//ResetEvent(handle_);
	//CloseHandle(handle_);

	if (bitmap_info_[COLOR_24BIT] != NULL) {
		delete bitmap_info_[COLOR_24BIT];
		bitmap_info_[COLOR_24BIT] = NULL;
	}
	if (bitmap_info_[GRAY_8BIT] != NULL) {
		delete bitmap_info_[GRAY_8BIT];
		bitmap_info_[GRAY_8BIT] = NULL;
	}
	if (drag_image_ptr_ != NULL) {
		delete drag_image_ptr_;
		drag_image_ptr_ = NULL;
	}

	if (thumb_image_list_.GetSafeHandle() != NULL) {
		for (int i = 0; i < thumb_image_list_.GetImageCount(); i++) {
			thumb_image_list_.Remove(i);
		}
		thumb_image_list_.DeleteImageList();
	}
	
	DeleteObject(bitmap_);

	thumb_status_.clear();
	thumb_text_.clear();
	thumb_time_text_.clear();

	if (cs_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_);
		cs_.OwningThread = NULL;
	}
}

void ThumbListCtrl::SetMaxItems(int max_items) {

	//if (cs_.OwningThread != NULL) {
		max_items_ = max_items;
	//}
}

void ThumbListCtrl::Create(int count, int image_width, int image_height, int columns, int rows, CRect scroll_rect, THUMB_IMAGE_TYPE type, BOOL horz_spacing, BOOL vert_spacing) {
	
	max_items_ = count;
	rows_ = rows;
	cols_ = columns;
	scroll_rect_ = scroll_rect;
	image_type_ = type;

	drag_image_ptr_ = NULL;
	dragging_ = FALSE;
	image_width_ = 0;
	image_height_ = 0;
	dragging_index_ = 0;

	InitializeCriticalSection(&cs_);
	
	//TO HIDE BOTH SCROLL BARS
	GetClientRect(client_rect_);
											   
	//TO RETRIEVE THUMB IMAGE WIDTH AND HEIGHT
	//Here we set the position of the window to the clientrect + the size of the scrollbars
	int cxvs = GetSystemMetrics(SM_CXVSCROLL); //Get the system metrics - VERT
	//int cyvs = GetSystemMetrics(SM_CYVSCROLL); //Get the system metrics - HORZ
	//SetWindowPos(NULL, client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.bottom, SWP_NOMOVE | SWP_NOZORDER);
	if (max_items_ > 1 && cols_ == 1) {
		//HRGN client_rgn = CreateRectRgn(client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.bottom);
		//SetWindowRgn(client_rgn, TRUE);
		//DeleteObject(client_rgn);
		//client_rect_.bottom += (cyvs);
		image_width_ = ((client_rect_.Width() - (cxvs / 4) - cxvs) / columns);
	} else if (max_items_ > 1 && (rows > 1 || columns > 1)) {
		//HRGN client_rgn = CreateRectRgn(client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.bottom);
		////SetWindowRgn(client_rgn, TRUE);
		//DeleteObject(client_rgn);
		//int cxvs = GetSystemMetrics(SM_CXVSCROLL); //Get the system metrics - VERT
		//int cyvs = GetSystemMetrics(SM_CYVSCROLL); //Get the system metrics - HORZ
		//client_rect_.bottom += (cyvs);
		image_width_ = ((client_rect_.Width() - (cxvs / 2) - (scroll_rect.IsRectEmpty() ? 0 : cxvs)) / columns) ;
	} else if (max_items_ == 1) {
		image_width_ = client_rect_.Width();
	}
	image_height_ = (image_width_ * image_height / image_width);
	CRect calc_rect;
	CDC* dc = GetDC();
	dc->SelectObject(GetFont());
	dc->DrawText(Language::GetString(IDSTRINGT_NOTE_C), &calc_rect, DT_CALCRECT);
	ReleaseDC(dc);
	vertical_gap_ = calc_rect.Height() + Formation::spacing();
	item_index_ = 0;
	thumb_index_ = 0;
	//handle_ = CreateEvent(NULL, FALSE, FALSE, NULL);

	//CREATE IMAGELIST AND ASSING IT TO LIST CONTROL
	thumb_image_list_.Create(image_width_, image_height_, ILC_COLOR32, 0, 0);
	SetImageList(&thumb_image_list_, LVSIL_NORMAL);

	//TO MANAGE SPACING BETWEEN THUMBS
	int horz, vert;
	GetItemSpacing(false, &horz, &vert);
	if (!horz_spacing && !vert_spacing) {
		SetIconSpacing(horz - (horz - image_width_) + 1, vert - (vert - image_height_) + 1); //3 X 2 Matrix of thumbnails
	} else if (!horz_spacing && vert_spacing) {
		SetIconSpacing(horz - (horz - image_width_) + (scroll_rect.IsRectEmpty() ? 0 : 1), 0);
	} else if (horz_spacing && !vert_spacing) {
		SetIconSpacing(0, vert - (vert - image_height_) + 1);
	}
	
	delete_rect_.SetRectEmpty();
	if (allow_deletion_) {
		int diameter = (image_height_ / 3);
		if (diameter > Formation::control_height()) {
			diameter = Formation::control_height();
		}
		delete_rect_.SetRect(image_width_ - diameter, 0, image_width_, diameter);
	}

	//GENERATE BITMAP INFO HEADER FOR 24BIT COLOR
	long BmpSize = sizeof(BITMAPINFO);
	bitmap_info_[COLOR_24BIT] = (BITMAPINFO *)calloc(1, BmpSize);
	bitmap_info_[COLOR_24BIT]->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_[COLOR_24BIT]->bmiHeader.biWidth = image_width;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biHeight = -image_height;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biPlanes = 1;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biBitCount = 24;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biCompression = BI_RGB;
	bitmap_info_[COLOR_24BIT]->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(image_width * image_height * 3);

	//GENERATE BITMAP INFO HEADER FOR FOR 8BIT GRAY
	BmpSize = sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD));
	bitmap_info_[GRAY_8BIT] = (BITMAPINFO *)calloc(1, BmpSize);
	bitmap_info_[GRAY_8BIT]->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_[GRAY_8BIT]->bmiHeader.biWidth = image_width;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biHeight = -image_height;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biPlanes = 1;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biBitCount = 8;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biCompression = BI_RGB;
	bitmap_info_[GRAY_8BIT]->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD)) + (image_width * image_height);
	for (int counter = 0; counter < 256; counter++) {
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbBlue = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbGreen = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbRed = BYTE(counter);
		bitmap_info_[GRAY_8BIT]->bmiColors[counter].rgbReserved = BYTE(0);
	}

	SetTextBkColor(GetBkColor());

	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
}

void ThumbListCtrl::AllowSelection(BOOL allow) {

	allow_selection_ = allow;
	//EnableWindow(allow_selection_);
}

int ThumbListCtrl::EraseItem(int index) {

	CPoint pos(::GetMessagePos());
	ScreenToClient(&pos);

	CRect rect(delete_rect_);
	rect.MoveToX(delete_rect_.left + ((index % cols_) * image_width_));
	rect.MoveToY(delete_rect_.top + ((index / cols_) * image_height_));

	if (rect.PtInRect(pos)) {
		if (Formation::MsgBox(Language::GetString(IDSTRINGM_DELETE_ITEM), MB_YESNO | MB_ICONQUESTION) == IDYES) {
			thumb_image_list_.Remove(index);
			DeleteAllItems();
			--item_index_;
			for (int i = 0; i < item_index_; i++) {
				InsertItem(i, L"", i);
			}

			GetParent()->SendMessage(WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED, (WPARAM)this, (LPARAM)item_index_);

			return 1;
		}
	}
	return 0;
}

void ThumbListCtrl::EraseAllItems() {

	thumb_index_ = 0;
	item_index_ = 0;
	if (thumb_image_list_.GetSafeHandle() != NULL) {
		int item_count = thumb_image_list_.GetImageCount();
		for (int i = 0; i < item_count; i++) {
			thumb_image_list_.Remove(0);
		}
	}
	thumb_status_.clear();
	thumb_text_.clear();
	thumb_time_text_.clear();
	
	DeleteAllItems();

	GetParent()->SendMessage(WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED, (WPARAM)this, (LPARAM)item_index_);
}

BOOL ThumbListCtrl::AddThumbImage(HBITMAP hbitmap, int bitmap_width, int bitmap_height, CString text) {

	Gdiplus::Bitmap image(hbitmap, NULL);
	Gdiplus::Bitmap *bm_photo = new Gdiplus::Bitmap(bitmap_width, bitmap_height, PixelFormat24bppRGB);
	bm_photo->SetResolution(image.GetHorizontalResolution(), image.GetVerticalResolution());
	Gdiplus::Graphics *gr_photo = Gdiplus::Graphics::FromImage(bm_photo);
	gr_photo->Clear(Gdiplus::Color(255, 255, 255, 255));
	gr_photo->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	gr_photo->DrawImage(&image, Gdiplus::Rect(0, 0, bitmap_width, bitmap_height));

	if (text_on_image_) {
		HDC hdc = gr_photo->GetHDC();
		CDC cdc;
		cdc.Attach(hdc);
		cdc.SetBkMode(TRANSPARENT);
		cdc.SetTextColor(LABEL_COLOR);
		cdc.SelectObject(Formation::font(Formation::SMALL_FONT));
		cdc.DrawText(text, CRect(0, 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
		cdc.Detach();
		gr_photo->ReleaseHDC(hdc);
	}

	HBITMAP hbmReturn = NULL;
	bm_photo->GetHBITMAP(Gdiplus::Color(255, 255, 255, 255), &hbmReturn);
	CBitmap cbitmap;
	cbitmap.Attach(hbmReturn);
	thumb_image_list_.Add(&cbitmap, RGB(0, 0, 0));
	thumb_text_.push_back(text);

	InsertItem(item_index_, text_on_image_ ? L"" : text, item_index_);

	delete gr_photo;
	delete bm_photo;
	cbitmap.Detach();
	DeleteObject(hbmReturn);

	item_index_++;

	GetParent()->SendMessage(WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED, (WPARAM)this, (LPARAM)item_index_);

	return TRUE;
}

BOOL ThumbListCtrl::AddThumbImage(unsigned char* image_buffer, int thumb_index, CString text) {

	if (item_index_ == max_items_) {
		return FALSE;
	}
	Gdiplus::Bitmap img(bitmap_info_[image_type_], image_buffer);
	thumbnail_ = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(image_width_, image_height_, NULL, NULL));
	Gdiplus::Graphics graphics(thumbnail_);
	if (allow_deletion_) { //Draw delete button
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		CRect ellipse_rect = delete_rect_;
		//ellipse_rect.DeflateRect(0, Formation::spacing(), Formation::spacing2(), 0);
		int offset = ellipse_rect.Height() / 3;
		graphics.FillEllipse(Formation::spancolor1_brush_gdi(), ellipse_rect.left, ellipse_rect.top, ellipse_rect.Width(), ellipse_rect.Height());
		graphics.DrawLine(Formation::whitecolor_pen_gdi(), Gdiplus::Point(ellipse_rect.left + offset, ellipse_rect.top + offset), Gdiplus::Point(ellipse_rect.right - offset, ellipse_rect.bottom - offset));
		graphics.DrawLine(Formation::whitecolor_pen_gdi(), Gdiplus::Point(ellipse_rect.right - offset, ellipse_rect.top + offset), Gdiplus::Point(ellipse_rect.left + offset, ellipse_rect.bottom - offset));
	}

	if (text_on_image_) {
		HDC hdc = graphics.GetHDC();
		if (hdc != NULL) {
			CDC cdc;
			cdc.Attach(hdc);
			cdc.SetBkMode(TRANSPARENT);
			cdc.SetTextColor(LABEL_COLOR);
			cdc.SelectObject(Formation::font(Formation::SMALL_FONT));
			cdc.DrawText(text, CRect(0, 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
			cdc.Detach();
			graphics.ReleaseHDC(hdc);
		}
	}
	thumbnail_->GetHBITMAP(NULL, &bitmap_);
	CBitmap image;
	image.Attach(bitmap_);
	thumb_image_list_.Add(&image, RGB(0, 0, 0));
	thumb_text_.push_back(text);

	InsertItem(item_index_, text_on_image_ ? L"" : text, item_index_);

	item_index_++;

	delete thumbnail_;	

	GetParent()->SendMessage(WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED, (WPARAM)this, (LPARAM)item_index_);

	return TRUE;
}

LRESULT	ThumbListCtrl::InsertItemMessage(WPARAM wparam, LPARAM lparam) {

	int item_index = (int)wparam;
	POINT pt;
	pt.x = LOWORD(lparam);
	pt.y = HIWORD(lparam);
	//POINT pt = { 0, 0 };
	//pt.y = vertical_gap_ + item_index_ * (image_height_ + vertical_gap_);

	CString text;
	text.Format(_T("%d"), item_index);
	InsertItem(item_index, text_on_image_ ? L"" : text, item_index);
wprintf(L"About to insert2\n");
	SetItemPosition(item_index, pt);
	EnsureVisible(item_index - 1, FALSE);
wprintf(L"About to insert3\n");

	//SetEvent(handle_);

	return 0;
}

LRESULT	ThumbListCtrl::AddThumbImage(WPARAM wparam, LPARAM lparam) {

	EnterCriticalSection(&cs_);

	unsigned char* image_buffer = (unsigned char*)wparam;
	THUMB_BORDER_COLOR border_color = (THUMB_BORDER_COLOR)lparam;
	/*char path[255];
	sprintf_s(path, "CAM%d\\%d.bmp", component_index_ + 1, image_index_);
	cv::String cv_path(path);
	cv::Mat cv_file_buf = cv::imread(cv_path);
	image_buffer = cv_file_buf.data;*/

	//int64 st_time = cv::getTickCount();
	//CString console_text;

	Gdiplus::Bitmap img(bitmap_info_[image_type_], image_buffer);
	thumbnail_ = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(image_width_, image_height_, NULL, NULL));
	Gdiplus::Graphics graphics(thumbnail_);

	/*console_text.Format(L"\t\t\t#### AddThumbImage1 %.2f\n", (cv::getTickCount() - st_time) / cv::getTickFrequency() * 1000);
	Formation::SetConsoleText(console_text, 0);
	st_time = cv::getTickCount();*/

	HDC hdc = graphics.GetHDC();
	CDC cdc;
	cdc.Attach(hdc);
	if (border_color != THUMB_BORDER_COLOR::NO_BORDER) {
		if (border_color == THUMB_BORDER_COLOR::GREEN_BORDER) {
			cdc.FillSolidRect(CRect(Formation::spacing(), Formation::spacing(), Formation::spacing3(), Formation::spacing3()), ACCEPT_COLOR);
		} else if (border_color == THUMB_BORDER_COLOR::RED_BORDER) {
			cdc.FillSolidRect(CRect(Formation::spacing(), Formation::spacing(), Formation::spacing3(), Formation::spacing3()), REJECT_COLOR);
		}
	}
	if (thumb_index_++ == max_items_) {
		thumb_index_ = 1;
	}

	CString text;
	text.Format(_T("%d"), thumb_index_);
	CString time_text = Formation::GetTime();
	if (text_on_image_) {
		cdc.SetBkMode(TRANSPARENT);
		cdc.SetTextColor(LABEL_COLOR);
		cdc.SetROP2(COLORONCOLOR);
		cdc.SelectObject(Formation::font(Formation::SMALL_FONT));
		cdc.DrawText(text, CRect(Formation::spacing(), 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
		if (show_date_time_) {
			cdc.DrawText(time_text, CRect(0, 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
		}
	}
	cdc.Detach();
	graphics.ReleaseHDC(hdc);

	thumbnail_->GetHBITMAP(NULL, &bitmap_);
	CBitmap image;
	image.Attach(bitmap_);
	thumb_image_list_.Add(&image, RGB(0, 0, 0));
	thumb_status_.push_back(border_color);
	thumb_text_.push_back(text);
	thumb_time_text_.push_back(time_text);

	//console_text.Format(L"\t\t\t#### AddThumbImage2 %.2f\n", (cv::getTickCount() - st_time) / cv::getTickFrequency() * 1000);
	//Formation::SetConsoleText(console_text, 0);

	if (item_index_ != max_items_) {

		POINT pt = { 0, 0 };
		pt.y = vertical_gap_ + item_index_ * (image_height_ + vertical_gap_);

		//int64 st_time = cv::getTickCount();
		InsertItem(item_index_, text_on_image_ ? L"" : text, item_index_);
		/*console_text.Format(L"\t\t\t#### AddThumbImageAddItem %.2f\n", (cv::getTickCount() - st_time) / cv::getTickFrequency() * 1000);
		Formation::SetConsoleText(console_text, 0);*/

		SetItemPosition(item_index_, pt);

		++item_index_;

	} else {

		thumb_image_list_.Remove(0);
		thumb_status_.pop_front();
		thumb_text_.pop_front();
		thumb_time_text_.pop_front();

		/*LVITEM lvitem;
		lvitem.mask = LVIF_IMAGE;
		POINT pt = { 0, 0 };
		int index = thumb_index_;
		for (int i = 0; i < item_index_; i++) {
			if (index++ == max_items_) {
				index = 1;
			}

			lvitem.iItem = i;
			GetItem(&lvitem);
			lvitem.iImage = i;
			TCHAR sz[10];
			wsprintf(sz, _T("%d"), index);
			//wcsncpy_s(lvitem.pszText, sz, sizeof(sz));
			SetItem(&lvitem);

			pt.y = vertical_gap_ + i * (image_height_ + vertical_gap_);
			SetItemPosition(i, pt);
		}*/

		/*CString str_text;
		int index = thumb_index_;
		for (int i = 0; i < max_items_; i++) {
			if (index++ == max_items_) {
				index = 1;
			}
			str_text.Format(_T("%d"), index);
			SetItemText(i, 0, str_text);
		}*/

		RedrawItems(0, max_items_);
	}

	EnsureVisible(item_index_ - 1, FALSE);

	delete thumbnail_;

	//CWnd* wnd = GetParent();
	//wnd->InvalidateRect(scroll_rect_, FALSE);
	LeaveCriticalSection(&cs_);

	GetParent()->SendMessage(WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED, (WPARAM)this, (LPARAM)item_index_);

	return 0;
}

BOOL ThumbListCtrl::SetCurrentStatus(int index, unsigned char* buffer, THUMB_BORDER_COLOR border_color) {

	if (index < thumb_image_list_.GetImageCount()) {

		Gdiplus::Bitmap img(bitmap_info_[image_type_], buffer);
		thumbnail_ = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(image_width_, image_height_, NULL, NULL));
		Gdiplus::Graphics graphics(thumbnail_);

		HDC hdc = graphics.GetHDC();
		CDC cdc;
		cdc.Attach(hdc);
		if (border_color == THUMB_BORDER_COLOR::GREEN_BORDER) {
			cdc.FillSolidRect(CRect(image_width_ - Formation::spacing3(), Formation::spacing(), image_width_ - Formation::spacing(), Formation::spacing3()), ACCEPT_COLOR);
		} else if (border_color == THUMB_BORDER_COLOR::RED_BORDER) {
			cdc.FillSolidRect(CRect(image_width_ - Formation::spacing3(), Formation::spacing(), image_width_ - Formation::spacing(), Formation::spacing3()), REJECT_COLOR);
		}
		if (thumb_status_[index] == THUMB_BORDER_COLOR::GREEN_BORDER) {
			cdc.FillSolidRect(CRect(Formation::spacing(), Formation::spacing(), Formation::spacing3(), Formation::spacing3()), ACCEPT_COLOR);
		} else if (thumb_status_[index] == THUMB_BORDER_COLOR::RED_BORDER) {
			cdc.FillSolidRect(CRect(Formation::spacing(), Formation::spacing(), Formation::spacing3(), Formation::spacing3()), REJECT_COLOR);
		}
		if (text_on_image_) {
			cdc.SetBkMode(TRANSPARENT);
			cdc.SetTextColor(LABEL_COLOR);
			cdc.SetROP2(COLORONCOLOR);
			cdc.SelectObject(Formation::font(Formation::SMALL_FONT));
			cdc.DrawText(thumb_text_[index], CRect(Formation::spacing(), 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
			cdc.DrawText(thumb_time_text_[index], CRect(0, 0, image_width_ - Formation::spacing(), image_height_ - 1), DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
		}
		cdc.Detach();
		graphics.ReleaseHDC(hdc);

		thumbnail_->GetHBITMAP(NULL, &bitmap_);
		CBitmap image;
		image.Attach(bitmap_);

		thumb_image_list_.Replace(index, &image, NULL);
		
		Invalidate();

		delete thumbnail_;
	}

	return TRUE;
}

CString ThumbListCtrl::GetThumbText(int index) {

	if (index < (int)thumb_text_.size()) {
		return thumb_text_[index];
	}
	return L"";
}

BOOL ThumbListCtrl::IsItemInsertionAllowed() {

	if (item_index_ == max_items_) {
		return FALSE;
	}
	return TRUE;
}

LRESULT ThumbListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT) {
		CWnd* wnd = GetParent();
		//wnd->InvalidateRect(scroll_rect_, FALSE);
	}

	return CListCtrl::WindowProc(message, wParam, lParam);
}

void ThumbListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (allow_dragging_) {

		dragging_index_ = pNMLV->iItem;
		drag_image_ptr_ = NULL;
		dragging_ = true;

		SetCapture();
	}

	*pResult = 0;
}

void ThumbListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (dragging_) {

		if (drag_image_ptr_ == NULL) {
			drag_image_ptr_ = CreateDragImage(dragging_index_, &point);
			drag_image_ptr_->BeginDrag(0, CPoint(0, 0));
			drag_image_ptr_->DragEnter(GetDesktopWindow(), point);
		}
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		drag_image_ptr_->DragMove(pt); //move the drag image to those coordinates
	}
}

void ThumbListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (dragging_) {

		drag_image_ptr_->DragLeave(GetDesktopWindow());
		drag_image_ptr_->EndDrag();
		delete drag_image_ptr_; //must delete it because it was created at the beginning of the drag
		drag_image_ptr_ = NULL;

		ClientToScreen(&point);
		CWnd* wnd = WindowFromPoint(point);
		if (wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) && wnd != this) {
			ThumbListCtrl* wnd_list_control = dynamic_cast<ThumbListCtrl*>(wnd);
			if (wnd_list_control != NULL) {
				if (wnd_list_control->IsItemInsertionAllowed()) {
					CWnd* wnd = wnd_list_control->GetParent();
					SourceListCtrlInfo sourcelistctrlinfo;
					sourcelistctrlinfo.thumblistctrl = this;
					sourcelistctrlinfo.drag_index = dragging_index_;
					wnd->SendMessage(WM_THUMBLISTCTRL_ITEMDRAGGED, (WPARAM)wnd_list_control, (LPARAM)&sourcelistctrlinfo);
				}
			}
		}
	}
	dragging_ = FALSE;
}