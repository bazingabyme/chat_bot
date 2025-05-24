#include "stdafx.h"

#define INCLUDE_WINPAINT __declspec(dllexport)
#include "win_paint.h"

// WinPaintMenu

WinPaintMenu::WinPaintMenu()
{
	font_ = NULL;
	alignment_ = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	menu_item_height_ = 40;
	menu_item_width_ = 40;
}

WinPaintMenu::~WinPaintMenu()
{
	  
}

void WinPaintMenu::Destroy() {

	for (int i = 0; i < icon_list_.GetSize(); i++) {
		DestroyIcon(icon_list_[i]);
	}
	icon_list_.RemoveAll();

	DestroyMenu();

	font_ = NULL;
}

void WinPaintMenu::SetFont(CFont* font) {

	font_ = font;
}

void WinPaintMenu::SetTextAlignment(int alignment) {

	alignment_ = alignment;
}

void WinPaintMenu::SetMenuItemHeight(int menu_item_height) {

	menu_item_height_ = menu_item_height;
}

void WinPaintMenu::SetMenuItemWidth(int menu_item_width) {

	menu_item_width_ = menu_item_width;
}

void WinPaintMenu::AddMenuItemIcon(HICON icon) {

	icon_list_.Add(icon);
}

void WinPaintMenu::ChangeToOwnerDraw(WinPaintMenu *menuctrl)
{
	//get the number of the menu items of the parent menu
	int iMenuCount = menuctrl->GetMenuItemCount();
	UINT nID;	//use to hold the identifier of the menu items
	for (int i = 0; i < iMenuCount; i++) {
		if (menuctrl->GetSubMenu(i)) { //if the parent menu has sub menu
			menuctrl->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, 0, (LPCTSTR)NULL);
			ChangeToOwnerDraw((WinPaintMenu*)menuctrl->GetSubMenu(i));
		} else {
			nID = menuctrl->GetMenuItemID(i);
			menuctrl->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, (UINT)nID, (LPCTSTR)NULL);
		}
	}
}

void WinPaintMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//assign the height of the menu item
	lpMeasureItemStruct->itemHeight = menu_item_height_;
	//assign the width of the menu item
	lpMeasureItemStruct->itemWidth = menu_item_width_; //str.GetLength() * 7;
}

void WinPaintMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (pDC != NULL) {

		CString str;
		GetMenuString(lpDrawItemStruct->itemID, str, MF_BYCOMMAND);

		if (font_ != NULL) {
			pDC->SelectObject(font_);
		}
		CRect rect(lpDrawItemStruct->rcItem);
		rect.DeflateRect(0, 0, 0, 1);

		//draw an orange background
		if (lpDrawItemStruct->itemState & ODS_DISABLED) {
			pDC->FillSolidRect(rect, DISABLE_COLOR);
		} else {
			pDC->FillSolidRect(rect, BACKCOLOR1);
		}
		pDC->SetTextColor(WHITE_COLOR);

		//if the menu item is selected
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) && (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {

			if (lpDrawItemStruct->itemState & ODS_DISABLED) {
				pDC->FillSolidRect(rect, DISABLE_COLOR);
			}
			else {
				pDC->FillSolidRect(rect, BACKCOLOR1_SEL);
			}
		}

		int icon_index = ::GetMenuPosFromID(GetSafeHmenu(), lpDrawItemStruct->itemID);
		if (icon_index < icon_list_.GetSize() && icon_list_.GetSize() > 0) {

			HICON icon = icon_list_[icon_index];

			ICONINFO iconinfo;
			::GetIconInfo(icon, &iconinfo);
			DWORD dw_width = (DWORD)(iconinfo.xHotspot * 2);
			DWORD dw_height = (DWORD)(iconinfo.yHotspot * 2);
			::DeleteObject(iconinfo.hbmMask);
			::DeleteObject(iconinfo.hbmColor);

			CRect icon_rect(lpDrawItemStruct->rcItem);
			icon_rect.top = icon_rect.CenterPoint().y - (dw_height / 2);
			icon_rect.bottom = icon_rect.top + dw_height;
			icon_rect.left += (icon_rect.top - lpDrawItemStruct->rcItem.top);
			icon_rect.right = icon_rect.left + dw_width;

			//DrawIconEx(pDC->GetSafeHdc(), icon_rect.left, icon_rect.top, icon, dw_width, dw_height, 0, NULL, DI_NORMAL);
			pDC->DrawState(icon_rect.TopLeft(), CSize(icon_rect.Width(), icon_rect.Height()), icon, (lpDrawItemStruct->itemState & ODS_DISABLED ? DSS_DISABLED : DSS_NORMAL), (CBrush*)NULL);

			rect.left = icon_rect.right + (icon_rect.top - lpDrawItemStruct->rcItem.top) - Formation::spacing();
		}

		rect.left += Formation::spacing();

		//str = L" " + str + L" ";
		pDC->DrawText(Formation::PrepareString(*pDC, str, rect), rect, alignment_);
	}
}

// WinPaint

IMPLEMENT_DYNAMIC(WinPaint, CStatic)

WinPaint::WinPaint(void)
{
	old_main_bitmap_ = NULL;
	old_captionbar_bitmap_ = NULL;
	old_captionbar_render_bitmap_ = NULL;
	old_toolbar_bitmap_ = NULL;
	old_image_bitmap_ = NULL;
	old_shape_bitmap_ = NULL;
	old_multiple_image_bitmap_ = NULL;

	aoi_bitmap_info_ = NULL;
	bitmap_info_ = NULL;
	
	winview_.RemoveAll();
	aoi_for_multipleview_.SetRectEmpty();
	toptobottom_view_ = FALSE;

	pan_point_.SetPoint(0, 0);
	full_image_ = FALSE;

	// ------ + INITIALIZE VARIABLES
	selected_point_count_ = 0;
	memset(&selection_points_[0], 0, sizeof(CPoint) * 4);

	image_operation_ = IMAGE_OPERATION::NONE;
	previous_rect_.SetRectEmpty();
	current_rect_.SetRectEmpty();
	shape_ = NULL;

	width_arc_  = 0;
	height_arc_ = 0;
	line_width_ = 1;
	line_style_ = 0;
	line_color_ = RGB(255,255,255);
	fill_color_ = RGB(0,0,0);
	cs_for_winpaint_.OwningThread = NULL;
	cs_for_image_.OwningThread = NULL;
	cs_for_shapes_.OwningThread = NULL;
	cs_for_captionbar_.OwningThread = NULL;
	cs_for_render_captionbar_.OwningThread = NULL;
	cs_for_toolbar_.OwningThread = NULL;
	cs_for_toolbar_imagestore_.OwningThread = NULL;
	cs_for_freeze_on_error_alarm_.OwningThread = NULL;
	// ------ - INITIALIZE VARIABLES
}

void WinPaint::Destroy()
{
	winview_.RemoveAll();
	aoi_for_multipleview_.SetRectEmpty();

	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		delete shape;
	}
	window_shapes_.RemoveAll();
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		delete shape;
	}
	shapes_.RemoveAll();
	visible_family_list_for_multiple_view_.RemoveAll();

	free_hand_points_image_.clear();
	free_hand_points_window_.clear();

	drawing_pen_.DeleteObject();

	if (old_main_bitmap_ != NULL) {
		main_dc_.SelectObject(old_main_bitmap_);
		main_bitmap_.DeleteObject();
		old_main_bitmap_ = NULL;
	}
	if (old_captionbar_bitmap_ != NULL) {
		captionbar_dc_.SelectObject(old_captionbar_bitmap_);
		captionbar_bitmap_.DeleteObject();
		old_captionbar_bitmap_ = NULL;
	}
	if (old_captionbar_render_bitmap_ != NULL) {
		captionbar_render_dc_.SelectObject(old_captionbar_render_bitmap_);
		captionbar_render_bitmap_.DeleteObject();
		old_captionbar_render_bitmap_ = NULL;
	}
	if (old_toolbar_bitmap_ != NULL) {
		toolbar_dc_.SelectObject(old_toolbar_bitmap_);
		toolbar_bitmap_.DeleteObject();
		old_toolbar_bitmap_ = NULL;
	}
	if (old_image_bitmap_ != NULL) {
		image_dc_.SelectObject(old_image_bitmap_);
		image_bitmap_.DeleteObject();
		old_image_bitmap_ = NULL;
	}
	if (old_shape_bitmap_ != NULL) {
		shape_dc_.SelectObject(old_shape_bitmap_);
		shape_bitmap_.DeleteObject();
		old_shape_bitmap_ = NULL;
	}
	if (old_multiple_image_bitmap_ != NULL) {
		multiple_image_dc_.SelectObject(old_multiple_image_bitmap_);
		multiple_image_bitmap_.DeleteObject();
		old_multiple_image_bitmap_ = NULL;
	}
	captionbar_dc_.DeleteDC();
	captionbar_render_dc_.DeleteDC();
	toolbar_dc_.DeleteDC();
	image_dc_.DeleteDC();
	shape_dc_.DeleteDC();
	multiple_image_dc_.DeleteDC();
	main_dc_.DeleteDC();

	aoi_wnd_image_.release();

	if (aoi_bitmap_info_ != NULL) {
		free(aoi_bitmap_info_);
		aoi_bitmap_info_ = NULL;
	}
	if (bitmap_info_ != NULL) {
		free(bitmap_info_);
		bitmap_info_ = NULL;
	}
	
	if (cs_for_winpaint_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_winpaint_);
	}
	if (cs_for_image_.OwningThread != NULL) {
        DeleteCriticalSection(&cs_for_image_);
    }
	if (cs_for_shapes_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_shapes_);
	}
	if (cs_for_captionbar_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_captionbar_);
	}
	if (cs_for_render_captionbar_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_render_captionbar_);
	}
	if (cs_for_toolbar_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_toolbar_);
	}
	if (cs_for_toolbar_imagestore_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_toolbar_imagestore_);
	}
	if (cs_for_freeze_on_error_alarm_.OwningThread != NULL) {
		DeleteCriticalSection(&cs_for_freeze_on_error_alarm_);
	}

	winpaintmenu_navigation_.Destroy();
}

WinPaint::~WinPaint(void)
{
	Destroy();
}

BEGIN_MESSAGE_MAP(WinPaint, CStatic)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDMENU_ROTATE, WinPaint::OnRotate)
	ON_COMMAND(IDMENU_MOVE, WinPaint::OnMove)
	ON_COMMAND(IDMENU_RESIZE, WinPaint::OnResize)
END_MESSAGE_MAP()

// WinPaint message handlers

bool WinPaint::CreateDisplay(long image_width, long image_height, BOOL color_image, int show_toolbar, int show_captionbar)
{
	//arrow_cursor_ = LoadCursor(NULL, IDC_ARROW);
	//draw_cursor_ = LoadCursor(NULL, IDC_CROSS);
	//pencil_cursor_ = LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_PENCIL));
	//move_cursor_ = LoadCursor(NULL, IDC_SIZEALL);
	////select_cursor_ = LoadCursor(NULL, IDC_SIZEALL);
	////move_cursor_ = LoadCursor(NULL, IDC_SIZEALL);
	//pan_cursor_ = LoadCursor(NULL, IDC_HAND);
	//resize_leftbottom_cursor_ = LoadCursor(NULL, IDC_SIZENESW);
	//resize_lefttop_cursor_ = LoadCursor(NULL, IDC_SIZENWSE);
	//rotate_cursor_ = LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_ROTATE));

	InitializeCriticalSection(&cs_for_winpaint_);
	InitializeCriticalSection(&cs_for_image_);
	InitializeCriticalSection(&cs_for_shapes_);
	InitializeCriticalSection(&cs_for_captionbar_);
	InitializeCriticalSection(&cs_for_render_captionbar_);
	InitializeCriticalSection(&cs_for_toolbar_);
	InitializeCriticalSection(&cs_for_toolbar_imagestore_);
	InitializeCriticalSection(&cs_for_freeze_on_error_alarm_);

	drawing_pen_.CreatePen(PS_SOLID, 1, WHITE_COLOR);

	width_arc_  = 0;
	height_arc_ = 0;
	line_width_ = 1;
	line_style_ = 0;
	line_color_ = WHITE_COLOR;
	fill_color_ = BLACK_COLOR;
	tag_ = L"0";
	data_ = L"";
	overlap_area_ = 100;
	bounding_rect_.SetRect(0, 0, image_width, image_height);
	family_ = 0;
	save_image_path_ = L"";

	shape_selection_ = WinShape::SHAPE_SELECTION::SINGLE;
	shape_selection_size_ = Formation::spacing() + Formation::spacingHalf();
	image_type_ = color_image;
	show_toolbar_ = show_toolbar;
	show_captionbar_ = show_captionbar;
	image_index_ = 0;
	winview_.RemoveAll();
	aoi_for_multipleview_.SetRectEmpty();
	view_count_ = 0;
	view_index_ = 0;
	allow_to_change_image_view_ = FALSE;
	multiple_image_view_ = FALSE;
	lock_image_ = FALSE;
	center_line_ = FALSE;
	show_hide_shapes_ = TRUE;
	show_tiny_image_view_ = FALSE;
	toolbar_color_ = LABEL_COLOR; // RGB(0, 142, 119);

	winpaintmenu_navigation_.Destroy();
	winpaintmenu_navigation_.Attach(GetSubMenu(LoadMenu(g_resource_handle, MAKEINTRESOURCE(IDMENU_NAVIGATION_OPTIONS)), 0));
	winpaintmenu_navigation_.SetFont(&Formation::font(Formation::FONT_SIZE::BIG_FONT));
	winpaintmenu_navigation_.SetMenuItemHeight(Formation::control_height() + Formation::spacing());
	winpaintmenu_navigation_.SetMenuItemWidth(Formation::control_height() - Formation::spacing());
	winpaintmenu_navigation_.AddMenuItemIcon((HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_ROTATE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR));
	winpaintmenu_navigation_.AddMenuItemIcon((HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_MOVE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR));
	winpaintmenu_navigation_.AddMenuItemIcon((HICON)LoadImage(g_resource_handle, MAKEINTRESOURCE(IDI_RESIZE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR));
	winpaintmenu_navigation_.ChangeToOwnerDraw(&winpaintmenu_navigation_);
	navigation_menu_index_ = 1;
	toolbar_region_enable_[TOOLBAR_REGION::REGULAR_BUTTONS] = TRUE;
	toolbar_region_enable_[TOOLBAR_REGION::PIXEL_INFORMATION] = TRUE;
	toolbar_region_enable_[TOOLBAR_REGION::NAVIGATION_BUTTONS] = FALSE;
	toolbar_region_enable_[TOOLBAR_REGION::IMAGESTORE_BUTTONS] = FALSE;
	toolbar_region_visible_[TOOLBAR_REGION::REGULAR_BUTTONS] = TRUE;
	toolbar_region_visible_[TOOLBAR_REGION::PIXEL_INFORMATION] = TRUE;
	toolbar_region_visible_[TOOLBAR_REGION::NAVIGATION_BUTTONS] = TRUE;
	toolbar_region_visible_[TOOLBAR_REGION::IMAGESTORE_BUTTONS] = TRUE;

	multiple_shapes_selection_ = FALSE;

	shape_ = NULL;
	previous_rect_.SetRectEmpty();
	current_rect_.SetRectEmpty();
	down_point_.SetPoint(-1, -1);
	mouse_move_ = FALSE;

	selected_point_count_ = 0;
	selected_point_index_ = 0;
	memset(&selection_points_[0], 0, sizeof(CPoint)* 10);
	
	lock_draw_operation_ = FALSE;
	shape_operation_ = WinShape::SHAPE_OPERATION::NONE;
	image_operation_ = IMAGE_OPERATION::NONE;
	
	GetClientRect(client_rect_);
	wnd_width_ = client_rect_.Width();
	wnd_height_ = client_rect_.Height();
	if (show_toolbar_ == 1) {
		wnd_height_ -= Formation::control_height();
	}
	img_width_ = image_width;
	img_height_ = image_height;
	wnd_width_max_ = wnd_width_;
	wnd_height_max_ = wnd_height_;
	img_shape_width_ = image_width;
	img_shape_height_ = image_height;

	show_captionbar_region_ = 0;

	for (int i = 0; i < sizeof(toolbar_buttons_rect_) / sizeof(toolbar_buttons_rect_[0]); i++) {
		toolbar_buttons_disable_[i] = FALSE;
		toolbar_buttons_visible_[i] = TRUE;
	}
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON] = FALSE;
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON] = FALSE;
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::LOCK_IMAGE] = FALSE;
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::SAVE_IMAGE] = FALSE;
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_PASS] = FALSE;
	toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_FAIL] = FALSE;
	show_toolbar_region_ = FALSE;
	toolbar_region_index_ = 0;

	imagestore_pass_on_ = FALSE;
	imagestore_fail_on_ = FALSE;
	imagestore_grab_on_ = FALSE;

	CDC* dc = GetDC();
	main_dc_.DeleteDC();
	main_dc_.CreateCompatibleDC(dc);
	main_dc_.SetStretchBltMode(COLORONCOLOR);
	main_dc_.SelectObject(Formation::font(Formation::BIG_FONT));
	image_dc_.DeleteDC();
	image_dc_.CreateCompatibleDC(dc);
	image_dc_.SetStretchBltMode(COLORONCOLOR);
	shape_dc_.DeleteDC();
	shape_dc_.CreateCompatibleDC(dc);
	shape_dc_.SetStretchBltMode(COLORONCOLOR);
	multiple_image_dc_.DeleteDC();
	multiple_image_dc_.CreateCompatibleDC(dc);
	multiple_image_dc_.SetStretchBltMode(COLORONCOLOR);
	captionbar_dc_.DeleteDC();
	captionbar_dc_.CreateCompatibleDC(dc);
	captionbar_dc_.SelectObject(Formation::font(Formation::SMALL_FONT));
	captionbar_dc_.SetBkMode(TRANSPARENT);
	captionbar_dc_.SetTextColor(BLACK_COLOR);
	captionbar_render_dc_.DeleteDC();
	captionbar_render_dc_.CreateCompatibleDC(dc);
	captionbar_render_dc_.SelectObject(Formation::font(Formation::SMALL_FONT));
	captionbar_render_dc_.SetBkMode(TRANSPARENT);
	captionbar_render_dc_.SetTextColor(BLACK_COLOR);
	toolbar_dc_.DeleteDC();
	toolbar_dc_.CreateCompatibleDC(dc);
	toolbar_dc_.SelectObject(Formation::font(Formation::SMALL_FONT));
	toolbar_dc_.SetBkMode(TRANSPARENT);
	toolbar_dc_.SetTextColor(BLACK_COLOR);
	ReleaseDC(dc);

	int bitmap_size = color_image ? sizeof(BITMAPINFO) : (sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD)));
	bitmap_info_ = (BITMAPINFO *)calloc(1, bitmap_size);
	aoi_bitmap_info_ = (BITMAPINFO*)calloc(1, bitmap_size);
	aoi_bitmap_info_->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	aoi_bitmap_info_->bmiHeader.biPlanes = 1;
	aoi_bitmap_info_->bmiHeader.biCompression = BI_RGB;

	CalculateZoomIndex(image_width, image_height);
	ImageSizeChanged(image_width, image_height);
	WindowSizeChanged();
	CreateCaptionbar();
	CreateToolbar();

	SetDefaultView(TRUE);

	/*if (show_toolbar == 2) {
		SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(GetSafeHwnd(), 0, (255 * 50 / 100), LWA_ALPHA);

		HINSTANCE old_resource_handle = AfxGetResourceHandle();
		AfxSetResourceHandle(g_resource_handle);
		CRect wnd_rect;
		GetWindowRect(&wnd_rect);
		wnd_rect.bottom = wnd_rect.top + Formation::control_height();
		winpaint_dlg_.Create(IDD_WINPAINT_DLG, this);
		winpaint_dlg_.MoveWindow(wnd_rect);
		winpaint_dlg_.ShowWindow(SW_SHOW);
		AfxSetResourceHandle(old_resource_handle);
	}*/

	return true;
}

void WinPaint::CalculateZoomIndex(long image_width, long image_height) {

	image_operation_ = IMAGE_OPERATION::NONE;
	full_image_ = FALSE;
	pan_point_.SetPoint(0, 0);
	panning_on_ = FALSE;
	zoom_index_ = ZOOM_OPERATION::FIT;
	window_image_dimentions_ratio_ = 0;

	if (image_width != 0 && image_height != 0) {
		if (wnd_width_ > image_width && wnd_height_ > image_height) {
			double zoom_index_w = (double)wnd_width_ / image_width;
			double zoom_index_h = (double)wnd_height_ / image_height;
			zoom_index_w = ((zoom_index_w - floor(zoom_index_w)) < 0.5 ? floor(zoom_index_w) : ceil(zoom_index_w));
			if (zoom_index_w <= 0) zoom_index_w = 1;
			zoom_index_h = ((zoom_index_h - floor(zoom_index_h)) < 0.5 ? floor(zoom_index_h) : ceil(zoom_index_h));
			if (zoom_index_h <= 0) zoom_index_h = 1;
			zoom_index_ = zoom_index_w < zoom_index_h ? (int)zoom_index_w : (int)zoom_index_h;
			if (zoom_index_ > ZOOM_OPERATION::Z500) {
				zoom_index_ = ZOOM_OPERATION::Z500;
			}
			full_image_ = zoom_index_ > ZOOM_OPERATION::FIT ? TRUE : FALSE;
			window_image_dimentions_ratio_ = 1;
		} else if (wnd_width_ == image_width && wnd_height_ == image_height) {
			window_image_dimentions_ratio_ = 2;
			full_image_ = zoom_index_ = TRUE;
		} else if (wnd_width_ < image_width && wnd_height_ >= image_height) {
			window_image_dimentions_ratio_ = 3;
			full_image_ = zoom_index_ = FALSE;
		} else if (wnd_width_ >= image_width && wnd_height_ < image_height) {
			window_image_dimentions_ratio_ = 4;
			full_image_ = zoom_index_ = FALSE;
		} else {
			//if ratio of window height and window is doesn't match with image height and width
			//int calculated_height = (wnd_width_ * img_height_ / img_width_);
			//printf("WinW %d, WinH %d, ImgW %d, ImgH %d, CalH %d\n", wnd_width_, wnd_height_, img_width_, img_height_, calculated_height);
			//if (wnd_height_ > calculated_height + 5 || wnd_height_ < calculated_height - 5) { //Ignore +/-5 pixels
			//	window_image_dimentions_ratio_ = 5;
			//	full_image_ = zoom_index_ = FALSE;
			//}
		}
	}
	img_display_width_ = wnd_width_;
	img_display_height_ = wnd_height_;
	if (zoom_index_ > ZOOM_OPERATION::FIT) {
		img_display_width_ /= zoom_index_;
		img_display_height_ /= zoom_index_;
	}
}

BOOL WinPaint::ZoomToOriginal() {

	show_tiny_image_view_ = FALSE;

	int zoom_index = zoom_index_;
	if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON] && zoom_index > ZOOM_OPERATION::FIT) {
		zoom_index_ = ZOOM_OPERATION::FIT;
	} else if (zoom_index > ZOOM_OPERATION::Z100) {
		zoom_index_ = ZOOM_OPERATION::Z100;
	}
	if (zoom_index != zoom_index_) {

		full_image_ = (zoom_index_ == ZOOM_OPERATION::FIT) ? FALSE : TRUE;
		pan_point_.x = 0;
		pan_point_.y = 0;
		img_display_width_ = wnd_width_;
		img_display_height_ = wnd_height_;

		if (panning_on_) {
			panning_on_ = !panning_on_;
			previous_image_operation_ = (image_operation_ == IMAGE_PAN) ? previous_image_operation_ : image_operation_;
			if (panning_on_) {
				SetOperation(IMAGE_PAN);
			} else {
				SetOperation(previous_image_operation_);
			}
		}
		if (shape_ != NULL) {
			shape_->set_selected(FALSE);
		}

		Update();
		UpdateButtonInfoInToolbar();

		return TRUE;
	}
	return FALSE;
}

void WinPaint::ZoomShape() {

	if (shape_ != NULL) {
		ZoomShape(shape_);
	}
}

void WinPaint::ZoomShape(WinShape* shape) {

	if (shape != NULL) {
		CRect shape_rect = shape->rectangle(TRUE);
		long width_ratio = client_rect_.Width() / shape_rect.Width();
		long height_ratio = client_rect_.Height() / shape_rect.Height();
		long zoom_factor = (width_ratio < height_ratio ? width_ratio : height_ratio);
		if (zoom_factor > 1) {
			zoom_factor = zoom_factor - 1;
		}
		if (zoom_factor > WinPaint::ZOOM_OPERATION::Z500) {
			zoom_factor = WinPaint::ZOOM_OPERATION::Z500;
		}
		if (zoom_factor > WinPaint::ZOOM_OPERATION::FIT) {
			if (!show_toolbar_region_) {
				show_toolbar_region_ = !show_toolbar_region_;
				ToolbarRegionButtonClicked();
			}
			CPoint code_cg = shape_rect.CenterPoint();
			code_cg.x -= client_rect_.Width() / (2 * zoom_factor);
			code_cg.y -= client_rect_.Height() / (2 * zoom_factor);
			Zoom(zoom_factor, code_cg);
		}
	}
}

void WinPaint::Zoom(int zoom_index, CPoint pan_point) {

	zoom_index_ = zoom_index;

	if (zoom_index == ZOOM_OPERATION::FIT) {
		return;
	}

	//zoom_index_ = zoom_index;
	pan_point_ = pan_point;
	
	if (zoom_index_ == ZOOM_OPERATION::Z100) {
		if (window_image_dimentions_ratio_ == 3) {
			pan_point_.y = 0;
		} else if (window_image_dimentions_ratio_ == 4 || window_image_dimentions_ratio_ == 5) {
			pan_point_.x = 0;
		}
	/*} else {
		img_display_width_ = wnd_width_ / zoom_index_;
		img_display_height_ = wnd_height_ / zoom_index_;*/
	}
	img_display_width_ = wnd_width_ / zoom_index_;
	img_display_height_ = wnd_height_ / zoom_index_;

	pan_point_.x = (pan_point_.x > (img_width_ - img_display_width_)) ? (img_width_ - img_display_width_) : pan_point_.x;
	pan_point_.x = pan_point_.x < 0 ? 0 : pan_point_.x;
	pan_point_.y = (pan_point_.y >(img_height_ - img_display_height_)) ? (img_height_ - img_display_height_) : pan_point_.y;
	pan_point_.y = pan_point_.y < 0 ? 0 : pan_point_.y;

	full_image_ = zoom_index_ > ZOOM_OPERATION::FIT ? TRUE : FALSE;

	Update();
	UpdateButtonInfoInToolbar();
}

void WinPaint::ImageSizeChanged(long image_width, long image_height) {

	if (old_image_bitmap_ != NULL) {
		image_dc_.SelectObject(old_image_bitmap_);
	}
	image_bitmap_.DeleteObject();
	if (old_shape_bitmap_ != NULL) {
		shape_dc_.SelectObject(old_shape_bitmap_);
	}
	shape_bitmap_.DeleteObject();
	CDC* dc = GetDC();
	image_bitmap_.CreateCompatibleBitmap(dc, image_width, image_height);
	old_image_bitmap_ = image_dc_.SelectObject(&image_bitmap_);
	if (window_image_dimentions_ratio_) {
		if (window_image_dimentions_ratio_ == 3) {
			img_shape_width_ = img_width_;
			img_shape_height_ = wnd_height_;
		} else if (window_image_dimentions_ratio_ == 4 || window_image_dimentions_ratio_ == 5) {
			img_shape_width_ = wnd_width_;
			img_shape_height_ = img_height_;
		} else {
			img_shape_width_ = wnd_width_;
			img_shape_height_ = wnd_height_;
		}
	} else {
		img_shape_width_ = image_width;
		img_shape_height_ = image_height;
	}
	shape_bitmap_.CreateCompatibleBitmap(dc, img_shape_width_, img_shape_height_);
	old_shape_bitmap_ = shape_dc_.SelectObject(&shape_bitmap_);
	shape_dc_.BitBlt(0, 0, img_shape_width_, img_shape_height_, NULL, 0, 0, BLACKNESS);
	ReleaseDC(dc);

	bitmap_info_->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_->bmiHeader.biWidth = image_width;
	bitmap_info_->bmiHeader.biHeight = -image_height;
	bitmap_info_->bmiHeader.biPlanes = 1;
	bitmap_info_->bmiHeader.biCompression = BI_RGB;

	if (image_type_) { //24 BIT COLOR IMAGE
		bitmap_info_->bmiHeader.biBitCount = 24;
		bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(image_width * image_height * 3);
	} else { //GRAY IMAGE
		bitmap_info_->bmiHeader.biBitCount = 8;
		bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD)) + (image_width * image_height);
		for (int counter = 0; counter < 256; counter++) {
			bitmap_info_->bmiColors[counter].rgbBlue = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbGreen = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbRed = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbReserved = BYTE(0);
		}
	}
}

void WinPaint::CreateCaptionbar() {

	if (show_captionbar_) {

		EnterCriticalSection(&cs_for_render_captionbar_);

		captionbar_rect_.SetRect(client_rect_.left, client_rect_.top, client_rect_.right, client_rect_.top + Formation::control_height());

		CDC* dc = GetDC();
		if (old_captionbar_bitmap_ != NULL) {
			captionbar_dc_.SelectObject(old_captionbar_bitmap_);
		}
		captionbar_bitmap_.DeleteObject();
		captionbar_bitmap_.CreateCompatibleBitmap(dc, captionbar_rect_.Width(), captionbar_rect_.Height());
		old_captionbar_bitmap_ = captionbar_dc_.SelectObject(&captionbar_bitmap_);
		captionbar_dc_.FillSolidRect(captionbar_rect_, LABEL_COLOR3);
		if (old_captionbar_render_bitmap_ != NULL) {
			captionbar_render_dc_.SelectObject(old_captionbar_render_bitmap_);
		}
		captionbar_render_bitmap_.DeleteObject();
		captionbar_render_bitmap_.CreateCompatibleBitmap(dc, captionbar_rect_.Width(), captionbar_rect_.Height());
		old_captionbar_render_bitmap_ = captionbar_render_dc_.SelectObject(&captionbar_render_bitmap_);
		captionbar_render_dc_.FillSolidRect(captionbar_rect_, LABEL_COLOR3);
		ReleaseDC(dc);
		
		captionbar_button_selection_index_ = -1;
		
		int left = captionbar_rect_.right;
		captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].SetRectEmpty();
		if (show_captionbar_ == 2) {
			left -= (Formation::heading_height() - Formation::spacing());
			captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].SetRect(left, 0, captionbar_rect_.right, Formation::control_height());
			left -= Formation::spacing();
		}
		if (show_captionbar_ == 1) {
			show_captionbar_region_ = 2;
		}

		right_text_button_status_ = FALSE;
		left_text_ = right_text_[0] = right_text_[1] = _T("");
		left_text_color_ = right_text_color_ = WHITE_COLOR;
		color_code_ = LABEL_COLOR;

		captionbar_buttons_rect_[4] = captionbar_buttons_rect_[3] = captionbar_buttons_rect_[2] = captionbar_buttons_rect_[1] = captionbar_rect_;

		//Color Code
		captionbar_buttons_rect_[4].left = Formation::spacing3(); 
		captionbar_buttons_rect_[4].top = Formation::spacing2();
		captionbar_buttons_rect_[4].right = Formation::spacing4() + Formation::spacing2();
		captionbar_buttons_rect_[4].bottom -= Formation::spacing2();
		//Left Text
		captionbar_buttons_rect_[3].left = captionbar_buttons_rect_[4].right + Formation::spacing2();
		captionbar_buttons_rect_[3].right = captionbar_buttons_rect_[3].left + client_rect_.Width() * 33 / 100;
		//Right Text Button
		captionbar_buttons_rect_[1].right = captionbar_buttons_rect_[0].left - Formation::spacing();
		captionbar_buttons_rect_[1].left = captionbar_buttons_rect_[1].right - Formation::heading_height();
		//Right Text
		captionbar_buttons_rect_[2].left = captionbar_buttons_rect_[3].right + Formation::spacing();
		captionbar_buttons_rect_[2].right = captionbar_buttons_rect_[1].left;

		captionbar_buttons_visible_[0] = TRUE;
		captionbar_buttons_visible_[1] = TRUE;
		captionbar_buttons_visible_[2] = TRUE;
		captionbar_buttons_visible_[3] = TRUE;
		captionbar_buttons_visible_[4] = TRUE;

		LeaveCriticalSection(&cs_for_render_captionbar_);

		UpdateButtonInfoInCaptionbar(TRUE);
	}
}

void WinPaint::ShowCaptionbarButton(CAPTIONBAR_BUTTONS button, BOOL visible) {

	if (button >= 0 && button < sizeof(captionbar_buttons_rect_) / sizeof(captionbar_buttons_rect_[0])) {

		captionbar_buttons_visible_[button] = visible;

		UpdateButtonInfoInCaptionbar(TRUE);
	}
}

void WinPaint::UpdateButtonInfoInCaptionbar(BOOL redraw) {

	if (show_captionbar_ && captionbar_render_dc_.GetSafeHdc() != NULL) {

		EnterCriticalSection(&cs_for_render_captionbar_);

		Gdiplus::Graphics graphics(captionbar_render_dc_);
		//graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		//CGdiPlusBitmapResource bitmap_gdi;
		int icon_size = Formation::icon_size(Formation::MEDIUM_ICON);

		if (show_captionbar_ == 2 && show_captionbar_region_ != 2) {
			captionbar_render_dc_.FillSolidRect(captionbar_rect_, LABEL_COLOR3);
		} else {
			captionbar_render_dc_.FillSolidRect(captionbar_rect_, LABEL_COLOR);
		}

		//printf("----+-----\n");
		if (captionbar_buttons_visible_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE]) {
			Gdiplus::Point poly_points[3];
			poly_points[0].X = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].CenterPoint().x - Formation::spacing() - Formation::spacing();
			poly_points[1].X = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].CenterPoint().x;
			poly_points[2].X = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].CenterPoint().x + Formation::spacing() + Formation::spacing();
			poly_points[0].Y = poly_points[2].Y = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].CenterPoint().y + Formation::spacing() + Formation::spacing() / 2;
			poly_points[1].Y = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].CenterPoint().y - Formation::spacing() - Formation::spacing() / 2;
			Gdiplus::SolidBrush gdi_brush(Gdiplus::Color(GetRValue(color_code_), GetGValue(color_code_), GetBValue(color_code_)));
			graphics.FillPolygon(&gdi_brush, poly_points, 3);
		//	printf("COLORCODERECT %d,%d,%d,%d\n", captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].left, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].top, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].right, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_COLOR_CODE].bottom);
		}
		if (captionbar_buttons_visible_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT]) {
			captionbar_render_dc_.SetTextColor(left_text_color_);
			CRect rect = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT];
			captionbar_render_dc_.SelectObject(Formation::font(Formation::MEDIUM_FONT));
			captionbar_render_dc_.DrawText(left_text_, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		//	printf("LTEXTRECT %d,%d,%d,%d\n", captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT].left, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT].top, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT].right, captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_LEFT_TEXT].bottom);
		//	printf("%d,%s\n", left_text_color_, left_text_);
		}
		if (captionbar_buttons_visible_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT] && show_captionbar_region_ != 0) {
			if (right_text_button_status_) {
				captionbar_render_dc_.SelectObject(Formation::font(Formation::SMALL_FONT));
			} else {
				captionbar_render_dc_.SelectObject(Formation::font(Formation::MEDIUM_FONT));
			}
			captionbar_render_dc_.SetTextColor(right_text_color_);
			captionbar_render_dc_.DrawText(right_text_[right_text_button_status_], captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT], DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		}
		if (captionbar_buttons_visible_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON] && show_captionbar_region_ != 0) {
			if (captionbar_button_selection_index_ == CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON) {
				captionbar_render_dc_.FillSolidRect(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON], BACKCOLOR1_SEL);
			} else {
				captionbar_render_dc_.FillSolidRect(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON], show_captionbar_region_ == 2 ? LABEL_COLOR : LABEL_COLOR3);
			}
			CRect rect_show_region = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON];
			rect_show_region.left = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON].CenterPoint().x - Formation::spacingHalf();
			rect_show_region.right = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON].CenterPoint().x + Formation::spacingHalf() + 1;
			rect_show_region.top = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON].CenterPoint().y - Formation::spacingHalf();
			rect_show_region.bottom = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON].CenterPoint().y + Formation::spacingHalf() + 1;
			if (right_text_button_status_) {
				captionbar_render_dc_.FillSolidRect(rect_show_region, GRAY_COLOR);
			} else {
				captionbar_render_dc_.FillSolidRect(rect_show_region, BLACK_COLOR3);
			}
		}
		if (captionbar_buttons_visible_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON]) {

			CRect rect_temp = captionbar_buttons_rect_[CAPTIONBAR_SHOW_REGION_BUTTON];
			//rect_temp.InflateRect(Formation::spacing(), 0, 0, 0); //Include separator space
			if (captionbar_button_selection_index_ == CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON) {
				captionbar_render_dc_.FillSolidRect(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON], BACKCOLOR1_SEL);
			} else {
				captionbar_render_dc_.FillSolidRect(rect_temp, show_captionbar_region_ == 2 ? LABEL_COLOR : LABEL_COLOR3);
			}
			CRect rect_show_region(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON]);
			rect_show_region.left = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().x - Formation::spacing() / 2;
			rect_show_region.right = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().x + Formation::spacing() / 2 + 1;
			rect_show_region.top = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().y - Formation::spacing() / 2;
			rect_show_region.bottom = captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().y + Formation::spacing() / 2 + 1;
			if (show_captionbar_region_ == 0) {
				captionbar_render_dc_.FillSolidRect(rect_show_region, BLACK_COLOR3);
			} else if (show_captionbar_region_ == 1) {
				captionbar_render_dc_.FillSolidRect(rect_show_region, GRAY_COLOR);
			} else {
				captionbar_render_dc_.FillSolidRect(rect_show_region, WHITE_COLOR3);
			}

			if (show_captionbar_region_ != 0) {
				if (show_captionbar_region_ == 2) {
					captionbar_render_dc_.SelectObject(Formation::labellinecolor_pen());
				} else {
					captionbar_render_dc_.SelectObject(Formation::disablecolor_pen());
				}
				captionbar_render_dc_.MoveTo(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].left - Formation::spacingHalf(), captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().y - (icon_size / 3));
				captionbar_render_dc_.LineTo(captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].left - Formation::spacingHalf(), captionbar_buttons_rect_[CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON].CenterPoint().y + (icon_size / 3));
			}
		}
		
		EnterCriticalSection(&cs_for_captionbar_);
		captionbar_dc_.BitBlt(0, 0, captionbar_rect_.Width(), captionbar_rect_.Height(), &captionbar_render_dc_, 0, 0, SRCCOPY);
		LeaveCriticalSection(&cs_for_captionbar_);

		LeaveCriticalSection(&cs_for_render_captionbar_);
		
		if(redraw) InvalidateRect(captionbar_rect_);
	}
}

void WinPaint::SetLeftText(CString text, COLORREF text_color, BOOL redraw) {

	left_text_ = text;
	left_text_color_ = text_color;
	
	UpdateButtonInfoInCaptionbar(redraw);
}

void WinPaint::SetRightText(CString text, COLORREF text_color, BOOL check_on, BOOL redraw) {

	right_text_[check_on] = text;
	right_text_color_ = text_color;

	UpdateButtonInfoInCaptionbar(redraw);
}

void WinPaint::SetColorCode(COLORREF color, BOOL redraw) {

	color_code_ = color;

	UpdateButtonInfoInCaptionbar(redraw);
}

COLORREF WinPaint::GetColorCode() {

	return color_code_;
}

void WinPaint::CreateToolbar() {

	if (show_toolbar_) {

		EnterCriticalSection(&cs_for_toolbar_);

		toolbar_rect_.SetRect(client_rect_.left, client_rect_.bottom - Formation::control_height(), client_rect_.right, client_rect_.bottom);

		CDC* dc = GetDC();
		if (old_toolbar_bitmap_ != NULL) {
			toolbar_dc_.SelectObject(old_toolbar_bitmap_);
		}
		toolbar_bitmap_.DeleteObject();
		toolbar_bitmap_.CreateCompatibleBitmap(dc, toolbar_rect_.Width(), toolbar_rect_.Height());
		old_toolbar_bitmap_ = toolbar_dc_.SelectObject(&toolbar_bitmap_);
		//toolbar_dc_.BitBlt(0, 0, toolbar_rect_.Width(), toolbar_rect_.Height(), NULL, 0, 0, BLACKNESS);
		toolbar_dc_.FillSolidRect(CRect(0, 0, toolbar_rect_.Width(), toolbar_rect_.Height()), LABEL_COLOR3);
		//toolbar_dc_.SelectObject(Formation::labelcolor_pen());
		//toolbar_dc_.SelectObject(Formation::scrollcolor_brush()); //Toolbar color
		//toolbar_dc_.Rectangle(toolbar_rect_.left, 0, toolbar_rect_.right, toolbar_rect_.Height() + 1);
		ReleaseDC(dc);

		toolbar_button_selection_index_ = -1;

		/*if (window_image_dimentions_ratio_ == 1) {
			toolbar_buttons_visible_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON] = FALSE;
		} else {
			toolbar_buttons_visible_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON] = TRUE;
		}*/
		
		for (int i = NAVIGATION_MENU_BUTTON; i <= NAVIGATION_DOWN_BUTTON; i++) {
			toolbar_buttons_visible_[i] = FALSE;
		}
		navigation_menu_index_ = 1;
		toolbar_buttons_visible_[NAVIGATION_LEFT_BUTTON] = toolbar_buttons_visible_[NAVIGATION_UP_BUTTON] = TRUE;
		toolbar_buttons_visible_[NAVIGATION_RIGHT_BUTTON] = toolbar_buttons_visible_[NAVIGATION_DOWN_BUTTON] = TRUE;
		toolbar_buttons_visible_[NAVIGATION_CLOCKWISE_BUTTON] = toolbar_buttons_visible_[NAVIGATION_ANTICLOCKWISE_BUTTON] = TRUE;

		LeaveCriticalSection(&cs_for_toolbar_);

		RefreshToolbar();
	}
}

void WinPaint::RefreshToolbar() {

	if (show_toolbar_) {

		EnterCriticalSection(&cs_for_toolbar_);

		/*toolbar_dc_.SelectObject(Formation::labelcolor_pen());
		toolbar_dc_.SelectObject(Formation::labelcolor_brush());
		toolbar_dc_.Rectangle(toolbar_rect_.left, 0, toolbar_rect_.right, toolbar_rect_.Height() + 1);*/

		int left = toolbar_rect_.right;
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::TOOLBAR_SHOW_REGION_BUTTON].SetRectEmpty();
		if (show_toolbar_ == 2) {
			left -= (Formation::heading_height() - Formation::spacing());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::TOOLBAR_SHOW_REGION_BUTTON].SetRect(left, 0, toolbar_rect_.right, Formation::control_height());
			left -= Formation::spacing();
		}
		if (show_toolbar_ == 1) {
			show_toolbar_region_ = TRUE;
		}
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON].SetRectEmpty();
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON]) {
			left -= Formation::control_height();
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		int temp_left = left;

		//ZOOM CONTROLS
		BOOL zoom_controls_visible = FALSE;
		for (int button_index = TOOLBAR_BUTTONS::PAN_BUTTON; button_index <= TOOLBAR_BUTTONS::ZOOM_OUT_BUTTON; button_index++) {
			if (toolbar_buttons_visible_[button_index]) {
				left -= (Formation::control_height() + Formation::spacing());
				toolbar_buttons_rect_[button_index].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				zoom_controls_visible = TRUE;
			}
		}
		if (window_image_dimentions_ratio_ == 1 || window_image_dimentions_ratio_ == 2) {
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON].SetRectEmpty();
			toolbar_buttons_visible_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON] = FALSE;
		} else {
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON]) {
				left -= (Formation::control_height() + Formation::spacing());
				toolbar_buttons_rect_[TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				zoom_controls_visible = TRUE;
			}
		}
		zoom_text_rect_.SetRectEmpty();
		if (zoom_controls_visible) {
			left -= (int)(Formation::control_height() * 1.5);
			zoom_text_rect_.SetRect(left, 0, left + (int)(Formation::control_height() * 1.5), Formation::control_height());
		}
		//CENTER LINE
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::CENTER_LINE].SetRectEmpty();
		if (window_image_dimentions_ratio_ == 0 || window_image_dimentions_ratio_ == 2 || window_image_dimentions_ratio_ == 5) {
			if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
				if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::CENTER_LINE]) {
					left -= (Formation::control_height() + Formation::spacing());
					toolbar_buttons_rect_[TOOLBAR_BUTTONS::CENTER_LINE].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				}
			}
		}
		//SOURCE/BINARY VIEW
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::SOURCE_VIEW_BUTTON].SetRectEmpty();
		if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::SOURCE_VIEW_BUTTON]) {
				left -= (Formation::control_height() + Formation::spacing());
				toolbar_buttons_rect_[TOOLBAR_BUTTONS::SOURCE_VIEW_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
			}
		}
		//SHOW SHAPES
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON].SetRectEmpty();
		if (window_image_dimentions_ratio_ == 0 || window_image_dimentions_ratio_ == 2 || window_image_dimentions_ratio_ == 5) {
			if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
				if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON]) {
					left -= (Formation::control_height() + Formation::spacing());
					toolbar_buttons_rect_[TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				}
			}
		}
		//MULTIPLE VIEW
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON].SetRectEmpty();
		if (window_image_dimentions_ratio_ == 0 || window_image_dimentions_ratio_ == 2 || window_image_dimentions_ratio_ == 5) {
			if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
				if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON]) {
					left -= (Formation::control_height() + Formation::spacing());
					toolbar_buttons_rect_[TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				}
			}
		}
		//LOCK IMAGE ALARM
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::LOCK_IMAGE].SetRectEmpty();
		if (window_image_dimentions_ratio_ == 0 || window_image_dimentions_ratio_ == 2 || window_image_dimentions_ratio_ == 5) {
			if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
				if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::LOCK_IMAGE]) {
					left -= (Formation::control_height() + Formation::spacing());
					toolbar_buttons_rect_[TOOLBAR_BUTTONS::LOCK_IMAGE].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				}
			}
		}
		//SAVE IMAGE
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::SAVE_IMAGE].SetRectEmpty();
		if (window_image_dimentions_ratio_ == 0 || window_image_dimentions_ratio_ == 2 || window_image_dimentions_ratio_ == 5) {
			if ((left - (Formation::control_height() + Formation::spacing())) > 0) {
				if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::SAVE_IMAGE]) {
					left -= (Formation::control_height() + Formation::spacing());
					toolbar_buttons_rect_[TOOLBAR_BUTTONS::SAVE_IMAGE].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
				}
			}
		}

		left = toolbar_rect_.left + Formation::spacing();
		x_text_rect_.SetRectEmpty();
		y_text_rect_.SetRectEmpty();
		rgb_color_rect_.SetRectEmpty();
		rgb_text_rect_.SetRectEmpty();
		hsl_color_rect_.SetRectEmpty();
		hsl_text_rect_.SetRectEmpty();
		//X/Y POSITION
		if (left + Formation::control_height() * 4 < toolbar_rect_.right) {
			x_text_rect_.SetRect(left, 0, left + Formation::control_height() * 2, Formation::control_height());
			y_text_rect_.SetRect(x_text_rect_.right, 0, x_text_rect_.right + Formation::control_height() * 2, Formation::control_height());
			left += (Formation::control_height() * 4 + Formation::spacing());
		}
		//RGB COLOR
		if (left + Formation::control_height() * 5 < toolbar_rect_.right) {
			rgb_color_rect_.SetRect(left, Formation::spacing2(), left + Formation::spacing4(), Formation::control_height() - Formation::spacing2());
			rgb_text_rect_.SetRect(rgb_color_rect_.right + Formation::spacing(), 0, rgb_color_rect_.right + (Formation::control_height() * 4), Formation::control_height());
			left += (Formation::control_height() * 4 + Formation::spacing4() + Formation::spacing());
		}
		//HSL COLOR
		if (left + Formation::control_height() * 5 + Formation::spacing4() < toolbar_rect_.right) {
			hsl_color_rect_.SetRect(left, Formation::spacing2(), left + Formation::spacing4(), Formation::control_height() - Formation::spacing2());
			hsl_text_rect_.SetRect(hsl_color_rect_.right + Formation::spacing(), 0, hsl_color_rect_.right + (Formation::control_height() * 4), Formation::control_height());
			left += (Formation::control_height() * 5 + Formation::spacing4() + Formation::spacing());
		}
		if (show_toolbar_ == 2) {
			toolbar_pixinfo_right_ = toolbar_rect_.right - Formation::control_height() - Formation::heading_height() - Formation::spacing() + 1;
		} else {
			toolbar_pixinfo_right_ = toolbar_rect_.right - Formation::control_height() - Formation::spacing() + 1;
		}
		if (!toolbar_buttons_visible_[TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON]) {
		 	toolbar_pixinfo_right_ += (Formation::control_height() + Formation::spacing() + 1);
		}

		//NAVIGATION BUTTONS
		left = temp_left;
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_MENU_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_UP_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_RIGHT_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_LEFT_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_CLOCKWISE_BUTTON].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_ANTICLOCKWISE_BUTTON].SetRectEmpty();
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_UP_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_UP_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_RIGHT_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_RIGHT_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_LEFT_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_LEFT_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_CLOCKWISE_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_CLOCKWISE_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::NAVIGATION_ANTICLOCKWISE_BUTTON] && (left - Formation::control_height()) > toolbar_rect_.left) {
			left -= (Formation::control_height());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::NAVIGATION_ANTICLOCKWISE_BUTTON].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}

		//IMAGE STORE
		left = temp_left;
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE].SetRectEmpty();
		imagestore_text_rect_.SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_FAIL].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_PASS].SetRectEmpty();
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB].SetRectEmpty();
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR] && (left - (Formation::control_height() + Formation::spacing())) > toolbar_rect_.left) {
			left -= (Formation::control_height() + Formation::spacing());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE] && (left - (Formation::control_height() + Formation::spacing())) > toolbar_rect_.left) {
			left -= (Formation::control_height() + Formation::spacing());
			toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		}
		left -= ((Formation::control_height() * 4) + Formation::spacing());
		imagestore_text_rect_.SetRect(left, 0, left + (Formation::control_height() * 4), Formation::control_height());
		left -= (Formation::control_height() + Formation::spacing());
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_FAIL].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
		left -= (Formation::control_height() + Formation::spacing());
		toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_PASS].SetRect(left, 0, left + Formation::control_height(), Formation::control_height());
                
		LeaveCriticalSection(&cs_for_toolbar_);

		UpdatePixelInfoInToolbar();
		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::WindowSizeChanged() {

	CDC* dc = GetDC();
	if (old_main_bitmap_ != NULL) {
		main_dc_.SelectObject(old_main_bitmap_);
	}
	main_bitmap_.DeleteObject();
	main_bitmap_.CreateCompatibleBitmap(dc, client_rect_.Width(), client_rect_.Height());
	old_main_bitmap_ = main_dc_.SelectObject(&main_bitmap_);
	ReleaseDC(dc);
}

void WinPaint::ModifyDisplay(long image_width, long image_height, int show_toolbar, int show_captionbar) {

	EnterCriticalSection(&cs_for_winpaint_);

	show_toolbar_ = show_toolbar;
	show_captionbar_ = show_captionbar;

	int current_wnd_width = wnd_width_;
	int current_wnd_height = wnd_height_;
	long current_image_width = img_width_;
	long current_image_height = img_height_;
	BOOL image_size_changed = FALSE;
	BOOL window_size_changed = FALSE;
	
	GetClientRect(client_rect_);
	wnd_width_ = client_rect_.Width();
	wnd_height_ = client_rect_.Height();
	if (show_toolbar_ == 1) {
		wnd_height_ -= Formation::control_height();
	}

	if (image_width != 0 && image_height != 0) {
		if (image_width != current_image_width || image_height != current_image_height) { //Image size changed
			image_size_changed = TRUE;
		}
	}
	if (wnd_width_ != current_wnd_width || wnd_height_ != current_wnd_height) { //Image size changed
		window_size_changed = TRUE;
	}
	
	CalculateZoomIndex(image_width, image_height);

	if (image_size_changed) { //Image size changed

		img_width_ = image_width;
		img_height_ = image_height;

		bounding_rect_.SetRect(0, 0, image_width, image_height);

		ImageSizeChanged(image_width, image_height);

		if (view_count_ > 0) {
			UpdateImage();
		} else {
			SetDefaultView(TRUE);
		}

	} else if (window_size_changed) { //Window size changed

//printf("\n|||||||||||||||||||||||||||||| WndW = %d, WndH = %d ||||||||||||||||||||||||||||||||\n", wnd_width_, wnd_height_);
		WindowSizeChanged();

		if (view_count_ > 0) {
			UpdateImage();
		} else {
			SetDefaultView(FALSE);
		}
		
		EnterCriticalSection(&cs_for_shapes_);
		for (int i = 0; i < shapes_.GetSize(); i++) {
			WinShape* shape = shapes_.GetAt(i);
			if ((shape->family() > 0 && shape->family() < 100) || (shape->family() > WINPAINT_BINARY_VIEW_SHAPES_FAMILY && shape->family() < WINPAINT_BINARY_VIEW_SHAPES_FAMILY + 100)) {
			} else {
				if (shape->shape_type() == WinShape::SHAPE_TYPE::POLYGON) {
					WinPolygon* polygon = (WinPolygon*)shape;
					CArray<POINT> wnd_points;
					for (int index = 0; index < polygon->points(TRUE).GetSize(); index++) {
						wnd_points.Add(GetPointWRTWindow(polygon->points(TRUE)[index]));
					}
					polygon->set_points(wnd_points, FALSE);
					wnd_points.RemoveAll();
				} else {
					shape->set_window_rectangle(GetRectWRTWindow(shape->rectangle(TRUE)));
				}
			}
		}
		LeaveCriticalSection(&cs_for_shapes_);

		SwitchViews(multiple_image_view_);
	}

	CreateCaptionbar();
	CreateToolbar();

	LeaveCriticalSection(&cs_for_winpaint_);
}

void WinPaint::SetDefaultView(BOOL redraw) {

	int width = wnd_width_max_ / 7;
	CRect rect(0, 0, 0, wnd_height_max_);
	for (int i = 0; i < 7; i++) {
		rect.left = rect.right;
		if (i == 6) {
			rect.right = wnd_width_max_;
		} else {
			rect.right += width;
		}
		COLORREF rainbow_color = image_type_ ? ((i % 2 == 0) ? BACKCOLOR1 : BACKCOLOR2) : ((i % 2 == 0) ? RGB(90, 90, 90) : RGB(225, 225, 225));
		main_dc_.FillSolidRect(rect, rainbow_color);
	}
	if (view_count_ > 1) {
		multiple_image_dc_.FillSolidRect(rect, BLACK_COLOR);
	}

	if (img_width_ == 0 || img_height_ == 0 || view_count_ == 0 || winview_[0].buffer[0] == NULL) {
	
		int width = img_display_width_ / 7;
		CRect rect(0, 0, 0, img_display_height_);
		for (int i = 0; i < 7; i++) {
			rect.left = rect.right;
			if (i == 6) {
				rect.right = img_display_width_;
			} else {
				rect.right += width;
			}
			COLORREF rainbow_color = image_type_ ? ((i % 2 == 0) ? BACKCOLOR1 : BACKCOLOR2) : ((i % 2 == 0) ? RGB(90, 90, 90) : RGB(225, 225, 225));
			image_dc_.FillSolidRect(rect, rainbow_color);
		}

		if (redraw) Invalidate(FALSE);

	} else {
		
		for (int view_index = 0; view_index < view_count_; view_index++) {
			for (int image_index = 0; image_index < 2; image_index++) {
				if (winview_[view_index].buffer[image_index] != NULL) {

					cv::Rect rect(0, 0, 0, winview_[view_index].buffer[image_index]->rows);
					rect.width = winview_[view_index].buffer[image_index]->cols / 7;
					for (int i = 0; i < 7; i++) {
						if (i == 6) {
							rect.width = img_width_ - rect.x;
						}
						cv::Mat roi_buffer(*winview_[view_index].buffer[image_index], rect);
						COLORREF rainbow_color = image_type_ ? ((i % 2 == 0) ? BACKCOLOR1 : BACKCOLOR2) : ((i % 2 == 0) ? RGB(90, 90, 90) : RGB(225, 225, 225));
						roi_buffer = cv::Scalar(GetBValue(rainbow_color), GetGValue(rainbow_color), GetRValue(rainbow_color));
						roi_buffer.release();
						rect.x = rect.x + rect.width;
					}
				}
			}
			UpdateImage(winview_[view_index].buffer[0], winview_[view_index].buffer[1], view_index, (view_count_ > 1));
		}
	}
}

void WinPaint::LockImage(BOOL lock) {

	EnterCriticalSection(&cs_for_freeze_on_error_alarm_);
	if (lock_image_ != lock) {
		lock_image_ = lock;

		toolbar_buttons_disable_[TOOLBAR_BUTTONS::LOCK_IMAGE] = !lock;

		UpdateButtonInfoInToolbar();
	}
	LeaveCriticalSection(&cs_for_freeze_on_error_alarm_);
}

void WinPaint::SaveImage() {

	//Check directory path
	if (save_image_path_.IsEmpty()) {
		Formation::MsgBox(Language::GetString(IDSTRINGT_INVALID_PATH));
		return;
	}

	//Check directory size
	int saved_images_dir_size = 0;
	WIN32_FIND_DATA findfiledata = { 0 };
	CString file_path = save_image_path_ + L"\\*.bmp";
	HANDLE findfilehandle = FindFirstFile(file_path, &findfiledata);
	if (findfilehandle != INVALID_HANDLE_VALUE) {
		do {
			saved_images_dir_size += int(((findfiledata.nFileSizeHigh * (MAXDWORD)+double(findfiledata.nFileSizeLow)) / 1024) / 1024);
		} while (FindNextFile(findfilehandle, &findfiledata) != 0);
		FindClose(findfilehandle);
	}
	if (saved_images_dir_size > 1024) { //If greater then 1GB
		if (Formation::MsgBox(Language::GetString(IDSTRINGM_SAVEDIMAGES_DIRECTORY_IS_FULL_DO_YOU_WANT_TO_CLEAR_IT), MB_YESNO | MB_ICONQUESTION) == IDYES) {
			int len = save_image_path_.GetLength();
			TCHAR pszFrom[1024]; //4 to handle wide char
			wcscpy_s(pszFrom, len + 2, save_image_path_);
			pszFrom[len] = 0;
			pszFrom[len + 1] = 0;
			SHFILEOPSTRUCT fileop;
			fileop.hwnd = NULL;    // no status display
			fileop.wFunc = FO_DELETE;  // delete operation
			fileop.pFrom = pszFrom;  // source file name as double null terminated string
			fileop.pTo = NULL;    // no destination needed
			fileop.fFlags = FOF_NO_UI;  // do not prompt the user
			fileop.fAnyOperationsAborted = FALSE;
			fileop.lpszProgressTitle = NULL;
			fileop.hNameMappings = NULL;
			int ret = SHFileOperation(&fileop); //SHFileOperation returns zero if successful; otherwise nonzero 
			CreateDirectory(save_image_path_, NULL);
		} else {
			return;
		}
	}

	//Save image
	while (1) {
		KeyboardDlg keyboard;
		keyboard.SetHolder(_T("\\:/*?'<>|"));
		if (keyboard.OpenKeyboard(save_image_path_ + L" | " + Language::GetString(IDSTRINGT_ENTER_FILENAME), L"", FALSE, FALSE, FALSE) == IDOK) {
			CString image_name = keyboard.GetEditText();
			image_name.Trim();
			if (image_name.IsEmpty()) {
				Formation::MsgBox(Language::GetString(IDSTRINGM_FILENAME_EMPTY));
				continue;
			}
			
			CString file_path = save_image_path_ + _T("\\") + image_name + _T(".bmp");

			WIN32_FIND_DATA findfiledata = { 0 };
			HANDLE findfilehandle = FindFirstFile(file_path, &findfiledata);
			if (findfilehandle != INVALID_HANDLE_VALUE) {
				CString str;
				str.Format(L"<%s> %s", image_name, Language::GetString(IDSTRINGM_NAME_ALREADY_EXIST));
				Formation::MsgBox(str);
				FindClose(findfilehandle);
				continue;
			}

			Formation::WaitOpen(Language::GetString(IDSTRINGT_PLEASE_WAIT), Language::GetString(IDSTRINGT_SAVING));
			if (winview_[view_index_].buffer[image_index_]) {
				cv::imwrite(CT2A(file_path).m_psz, *winview_[view_index_].buffer[image_index_]);
			}
			Sleep(500);
			Formation::WaitClose();
		}
		break;
	}
}

void WinPaint::PanImage(BOOL pan) {

	if (panning_on_ != pan) {
		panning_on_ = pan;

		if (panning_on_) {
			SetOperation(IMAGE_OPERATION::IMAGE_PAN);
		} else {
			SetOperation(IMAGE_OPERATION::NONE);
		}
		SelectAllShapes(FALSE, FALSE);

		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::CrossLine() {

	CString old_tag = tag_;
	int old_family = family_;

	family_ = WINPAINT_FIXED_SHAPES_FAMILY;
	tag_ = L"1000";

	DeleteShapes(family_, tag_, FALSE);

	if (center_line_) {
		int offset = Formation::control_height() + Formation::spacing4();
		CPoint left_top_point = CPoint(img_width_ / 2, 0);
		CPoint right_bottom_point = CPoint(img_width_ / 2, img_height_ - 1);
		DrawLine(left_top_point, right_bottom_point, 1, PS_SOLID, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		CPoint left_top_point1 = left_top_point;
		CPoint right_bottom_point1 = right_bottom_point;
		while(left_top_point1.x > 0) {
			left_top_point1.x -= offset;
			right_bottom_point1.x -= offset;
			DrawLine(left_top_point1, right_bottom_point1, 1, PS_DOT, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		}
		left_top_point1 = left_top_point;
		right_bottom_point1 = right_bottom_point;
		while (left_top_point1.x < img_width_) {
			left_top_point1.x += offset;
			right_bottom_point1.x += offset;
			DrawLine(left_top_point1, right_bottom_point1, 1, PS_DOT, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		}

		left_top_point.SetPoint(0, img_height_ / 2);
		right_bottom_point.SetPoint(img_width_, img_height_ / 2);
		DrawLine(left_top_point, right_bottom_point, 1, PS_SOLID, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		left_top_point1 = left_top_point;
		right_bottom_point1 = right_bottom_point;
		while (left_top_point1.y > 0) {
			left_top_point1.y -= offset;
			right_bottom_point1.y -= offset;
			DrawLine(left_top_point1, right_bottom_point1, 1, PS_DOT, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		}
		left_top_point1 = left_top_point;
		right_bottom_point1 = right_bottom_point;
		while (left_top_point1.y < img_height_) {
			left_top_point1.y += offset;
			right_bottom_point1.y += offset;
			DrawLine(left_top_point1, right_bottom_point1, 1, PS_DOT, SCROLL_COLOR, BLACK_COLOR, 0, FALSE);
		}
	}

	RefreshShapes(TRUE);

	tag_ = old_tag;
	family_ = old_family;
}

void WinPaint::ShowHideShapes(BOOL show) {

	show_hide_shapes_ = show;
	
	/*ShowShapes(show_hide_shapes_);

	RefreshShapes(TRUE);*/

	UpdateButtonInfoInToolbar();

	Invalidate(FALSE);
}

void WinPaint::set_multipleview(BOOL multipleview) {

	multiple_image_view_ = multipleview;
}

void WinPaint::set_view_index(int view_index) {
	
	view_index_ = view_index;
}

void WinPaint::SwitchViews(BOOL multiple_image_view) {

	if (multiple_image_view_ != multiple_image_view) {
		multiple_image_view_ = multiple_image_view;
		//DisableZoomOperation(multiple_image_view_);
	}

	if (!ZoomToOriginal()) {
		RefreshShapes(TRUE); //To get effect of multiple_image_view_ if ZoomToOriginal() not able to execute Update()
	}

	ShowZoomOperation(!multiple_image_view);
	ShowShapes(0, !multiple_image_view);
	RefreshShapes(TRUE);
}

void WinPaint::SetParamForMultipleView(BOOL toptobottom_view) {

	toptobottom_view_ = toptobottom_view;
}

void WinPaint::SetAOIForMultipleView(CRect aoi) {

	aoi_for_multipleview_ = aoi;

	CDC* dc = GetDC();
	if (old_multiple_image_bitmap_ != NULL) {
		multiple_image_dc_.SelectObject(old_multiple_image_bitmap_);
	}
	multiple_image_bitmap_.DeleteObject();
	multiple_image_bitmap_.CreateCompatibleBitmap(dc, wnd_width_max_, wnd_height_max_);
	old_multiple_image_bitmap_ = multiple_image_dc_.SelectObject(&multiple_image_bitmap_);
	multiple_image_dc_.BitBlt(0, 0, wnd_width_max_, wnd_height_max_, NULL, 0, 0, BLACKNESS);
	ReleaseDC(dc);

	std::vector<RECT> best_floor_wise_rect;
	Formation::GetStrokeImagePosition(wnd_width_max_, wnd_height_max_, 1, aoi_for_multipleview_.Width(), aoi_for_multipleview_.Height(), view_count_, best_floor_wise_rect, toptobottom_view_);
	for (int view_index = 0; view_index < view_count_; view_index++) {
		best_floor_wise_rect[view_index].right -= ((best_floor_wise_rect[view_index].right - best_floor_wise_rect[view_index].left) % 4);
		winview_[view_index].winrect.SetRect(best_floor_wise_rect[view_index].left, best_floor_wise_rect[view_index].top, best_floor_wise_rect[view_index].right, best_floor_wise_rect[view_index].bottom);
	}
	best_floor_wise_rect.clear();

	aoi_bitmap_info_->bmiHeader.biWidth = winview_[0].winrect.Width();
	aoi_bitmap_info_->bmiHeader.biHeight = -winview_[0].winrect.Height();
	if (image_type_) { //24 BIT COLOR IMAGE
		aoi_bitmap_info_->bmiHeader.biBitCount = 24;
		aoi_bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(aoi_bitmap_info_->bmiHeader.biWidth * aoi_bitmap_info_->bmiHeader.biHeight * 3);
	} else { //GRAY IMAGE
		aoi_bitmap_info_->bmiHeader.biBitCount = 8;
		aoi_bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD)) + (aoi_bitmap_info_->bmiHeader.biWidth * aoi_bitmap_info_->bmiHeader.biHeight);
		for (int counter = 0; counter < 256; counter++) {
			aoi_bitmap_info_->bmiColors[counter].rgbBlue = BYTE(counter);
			aoi_bitmap_info_->bmiColors[counter].rgbGreen = BYTE(counter);
			aoi_bitmap_info_->bmiColors[counter].rgbRed = BYTE(counter);
			aoi_bitmap_info_->bmiColors[counter].rgbReserved = BYTE(0);
		}
	}

	aoi_wnd_image_.release();
	if (image_type_) {
		aoi_wnd_image_.create(winview_[0].winrect.Height(), winview_[0].winrect.Width(), CV_8UC3);
	} else {
		aoi_wnd_image_.create(winview_[0].winrect.Height(), winview_[0].winrect.Width(), CV_8UC1);
	}

	//EnterCriticalSection(&cs_for_shapes_);
	//FIRST CONVERT ALL DRAWINGS OF ALL FAMILIES TO RESPECTED WINDOW OF MULTIVIEW
	for (int index = 1; index <= view_count_; index++) {
		for (int i = 0; i < shapes_.GetSize(); i++) {
			WinShape* shape = shapes_.GetAt(i);
			if (shape->family() == index || shape->family() == WINPAINT_BINARY_VIEW_SHAPES_FAMILY + index) {
				shape->set_window_rectangle(GetRectWRTWinViewWindow(winview_[index - 1].winrect, shape->rectangle(TRUE)));
			}
		}
	}
	//FIRST CONVERT ALL WINDOW DRAWINGS OF ALL FAMILIES TO RESPECTED WINDOW OF MULTIVIEW
	/*for (int view_index = 1; view_index <= view_count_; view_index++) {
		for (int i = 0; i < window_shapes_.GetSize(); i++) {
			WinShape* shape = window_shapes_.GetAt(i);
			if (shape->family() == view_index) {
				shape->set_window_rectangle(GetRectWRTWinViewWindow(winview_[view_index - 1].winrect, shape->rectangle(TRUE), TRUE));
			}
		}
	}*/
	//LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::RegisterViews(cv::Mat* src_buffer, cv::Mat* bin_buffer) {

	EnterCriticalSection(&cs_for_winpaint_);

	winview_.RemoveAll();
	view_count_ = 1;
	view_index_ = 0;

	if (multiple_image_view_) {
		SwitchViews(FALSE);
	}

	WinView winview;
	winview_.Add(winview);
	winview_[0].winrect.SetRect(0, 0, wnd_width_max_, wnd_height_max_);
	winview_[0].buffer[0] = src_buffer;
	winview_[0].buffer[1] = bin_buffer;

	//ShowToolbarButton(WinPaint::MULTIPLE_VIEW_BUTTON, (view_count_ > 1));

	LeaveCriticalSection(&cs_for_winpaint_);
}

void WinPaint::RegisterViews(CArray<cv::Mat*> buffers, CRect aoi_rect) {

	EnterCriticalSection(&cs_for_winpaint_);

	winview_.RemoveAll();
	view_count_ = int(buffers.GetSize());
	view_index_ = 0;

	if (multiple_image_view_) {
		SwitchViews(FALSE);
	}

	WinView winview;
	for (int index = 0; index < view_count_; index++) {
		winview_.Add(winview);
	}
	SetAOIForMultipleView(aoi_rect);

	for (int index = 0; index < view_count_; index++) {
		winview_[index].buffer[0] = buffers[index];
	}

	//ShowToolbarButton(WinPaint::MULTIPLE_VIEW_BUTTON, (view_count_ > 1));

	LeaveCriticalSection(&cs_for_winpaint_);
}

void WinPaint::RegisterViews(CArray<cv::Mat*> &src_buffers, CArray<cv::Mat*> &bin_buffers, CRect aoi_rect) {

	EnterCriticalSection(&cs_for_winpaint_);

	winview_.RemoveAll();
	view_count_ = int(src_buffers.GetSize());
	view_index_ = 0;

	if (multiple_image_view_) {
		SwitchViews(FALSE);
	}

	WinView winview;
	for (int index = 0; index < view_count_; index++) {
		winview_.Add(winview);
	}
	SetAOIForMultipleView(aoi_rect);

	for (int index = 0; index < view_count_; index++) {
		winview_[index].buffer[0] = src_buffers[index];
		winview_[index].buffer[1] = bin_buffers[index];
	}

	//ShowToolbarButton(WinPaint::MULTIPLE_VIEW_BUTTON, (view_count_ > 1));

	LeaveCriticalSection(&cs_for_winpaint_);
}

BOOL WinPaint::ChangeImage(int index) {

	if ((index < 0 || index > 1) && image_index_ != index) {
		return FALSE;
	}

	if (winview_[view_index_].buffer[index] == NULL) {
		return FALSE;
	}

	EnterCriticalSection(&cs_for_winpaint_);

	image_index_ = index;

	for(int view_index = 0; view_index < (multiple_image_view_ ? view_count_ : 1); view_index++) {
		Update(winview_[view_index].buffer[0], winview_[view_index].buffer[1], view_index, multiple_image_view_);
	}
	UpdateButtonInfoInToolbar();

	LeaveCriticalSection(&cs_for_winpaint_);

	return TRUE;
}

void WinPaint::SetOperation(IMAGE_OPERATION image_operation, BOOL change_prev_operation)
{
	image_operation_ = image_operation;
	if (change_prev_operation) previous_image_operation_ = image_operation;
	if (image_operation_ == IMAGE_OPERATION::NONE) SelectNone(true);
	if (!panning_on_ && image_operation_ == IMAGE_OPERATION::IMAGE_PAN) {
		panning_on_ = TRUE;
		UpdateButtonInfoInToolbar();
	} else if (panning_on_ && image_operation_ != IMAGE_OPERATION::IMAGE_PAN) {
		panning_on_ = FALSE;
		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::SetShapeProperties(int line_width, int line_style, float width_arc, float height_arc, COLORREF line_color, COLORREF fill_color, CString tag)
{
	line_width_ = line_width;
	line_style_ = line_style;
	width_arc_ = width_arc;
	height_arc_ = height_arc;
	line_color_ = line_color;
	fill_color_ = fill_color;
	tag_ = tag;
	tag_for_shape_selection_ = tag;
}

void WinPaint::ShapeSelection(WinShape::SHAPE_SELECTION shape_selection) {

	shape_selection_ = shape_selection;

	tag_for_shape_selection_ = L"";
	if (shape_info_.index != -1) {
		tag_for_shape_selection_ = shape_info_.tag;
	}
}

BOOL WinPaint::SelectRect(CRect Rect, WinShape* shape, BOOL redraw)	{
	
	selected_point_count_ = 7;
	CPoint corner_points[7];

	BOOL resize_from_left_right_only = FALSE;
	if (shape->shape_type() == WinShape::RECTANGLE) {
		WinRect* rectangle = dynamic_cast<WinRect*>(shape);
		resize_from_left_right_only = rectangle->resize_style();
	}

	//if (angle_on_mouse_move_ != 0) {
	if(shape->angle() != 0) {

		double fangle = (double)shape->angle() * (CV_PI / 180);
		if (resize_from_left_right_only) {
			corner_points[5].x = (LONG)(center_point_.x + ((Rect.left - center_point_.x) * cos(fangle) - (Rect.CenterPoint().x - center_point_.y) * sin(fangle)));
			corner_points[5].y = (LONG)(center_point_.y + ((Rect.left - center_point_.x) * sin(fangle) + (Rect.CenterPoint().x - center_point_.y) * cos(fangle)));
			corner_points[6].x = (LONG)(center_point_.x + ((Rect.right - center_point_.x) * cos(fangle) - (Rect.CenterPoint().x - center_point_.y) * sin(fangle)));
			corner_points[6].y = (LONG)(center_point_.y + ((Rect.right - center_point_.x) * sin(fangle) + (Rect.CenterPoint().x - center_point_.y) * cos(fangle)));
		} else {
			corner_points[1].x = (LONG)(center_point_.x + ((Rect.left - center_point_.x) * cos(fangle) - (Rect.top - center_point_.y) * sin(fangle)));
			corner_points[1].y = (LONG)(center_point_.y + ((Rect.left - center_point_.x) * sin(fangle) + (Rect.top - center_point_.y) * cos(fangle)));
			corner_points[2].x = (LONG)(center_point_.x + ((Rect.right - center_point_.x) * cos(fangle) - (Rect.top - center_point_.y) * sin(fangle)));
			corner_points[2].y = (LONG)(center_point_.y + ((Rect.right - center_point_.x) * sin(fangle) + (Rect.top - center_point_.y) * cos(fangle)));
			corner_points[3].x = (LONG)(center_point_.x + ((Rect.right - center_point_.x) * cos(fangle) - (Rect.bottom - center_point_.y) * sin(fangle)));
			corner_points[3].y = (LONG)(center_point_.y + ((Rect.right - center_point_.x) * sin(fangle) + (Rect.bottom - center_point_.y) * cos(fangle)));
			corner_points[4].x = (LONG)(center_point_.x + ((Rect.left - center_point_.x) * cos(fangle) - (Rect.bottom - center_point_.y) * sin(fangle)));
			corner_points[4].y = (LONG)(center_point_.y + ((Rect.left - center_point_.x) * sin(fangle) + (Rect.bottom - center_point_.y) * cos(fangle)));
		}		

	} else {

		selection_points_[0].SetPoint(Rect.CenterPoint().x, Rect.CenterPoint().x);
		if (resize_from_left_right_only) {
			corner_points[5].SetPoint(Rect.left, Rect.CenterPoint().y);
			corner_points[6].SetPoint(Rect.right, Rect.CenterPoint().y);
		} else {
			corner_points[1].SetPoint(Rect.left, Rect.top);
			corner_points[2].SetPoint(Rect.right, Rect.top);
			corner_points[3].SetPoint(Rect.right, Rect.bottom);
			corner_points[4].SetPoint(Rect.left, Rect.bottom);
		}
	}
	
	for (int i = 1; i < selected_point_count_; i++) {
		selection_points_[i] = corner_points[i];
	}
	selection_points_[0].x = (selection_points_[1].x + selection_points_[2].x) / 2;
	selection_points_[0].y = (selection_points_[1].y + selection_points_[2].y) / 2;
	
	return TRUE;
}

void WinPaint::SelectLine(CPoint PointX1Y1, CPoint PointX2Y2, BOOL redraw)
{
	PointX1Y1.Offset(pan_point_);
	PointX2Y2.Offset(pan_point_);

	selected_point_count_ = 2;
	selection_points_[0] = PointX1Y1;
	selection_points_[1] = PointX2Y2;
}

void WinPaint::SelectNone(BOOL redraw)
{
	memset(&rect_corner_for_resize_, 0, sizeof(CPoint));
	angle_on_mouse_move_ = 0.0;
	selected_point_count_ = 0;
	memset(&selection_points_[0], 0, sizeof(CPoint) * 10);
}

void WinPaint::DrawShape() {
	
	if (!mouse_move_) return;

	if (GetOperation() == IMAGE_OPERATION::DRAW_FREE_HAND) {

		if (free_hand_points_window_.size() > 0) {
			Gdiplus::Point* points = new Gdiplus::Point[free_hand_points_window_.size()];
			for (int i = 0; i < free_hand_points_window_.size(); i++) {
				points[i].X = free_hand_points_window_[i].x;
				points[i].Y = free_hand_points_window_[i].y;
			}
			Gdiplus::Graphics g(main_dc_);
			Gdiplus::HatchBrush brush(Gdiplus::HatchStyle25Percent, Gdiplus::Color::Lime, Gdiplus::Color::GreenMask);
			g.FillPolygon(&brush, points, (int)free_hand_points_window_.size());
		}

	} else {
		
		if (shape_ != NULL) {
			if ((shape_operation_ == WinShape::SHAPE_OPERATION::MOVE && shape_->lock(shape_operation_)) ||
				(shape_operation_ == WinShape::SHAPE_OPERATION::RESIZE && shape_->lock(shape_operation_)) ||
				(shape_operation_ == WinShape::SHAPE_OPERATION::ROTATE && shape_->lock(shape_operation_)) ||
				(shape_operation_ == WinShape::SHAPE_OPERATION::SELECT && shape_->lock(shape_operation_))) {
				return;
			}
		}
		if (current_rect_.IsRectEmpty()) {
			return;
		}
		if (GetOperation() == IMAGE_OPERATION::DRAW_CIRCLE || GetOperation() == IMAGE_OPERATION::DRAW_ROUNDRECT || GetOperation() == IMAGE_OPERATION::DRAW_ELLIPSE || 
			GetOperation() == IMAGE_OPERATION::DRAW_RECT || GetOperation() == IMAGE_OPERATION::DRAW_LINE) {

			CPen* old_pen = main_dc_.SelectObject(&drawing_pen_);
			main_dc_.SetROP2(R2_NOT);
			main_dc_.SelectObject(GetStockObject(NULL_BRUSH));
			XFORM xform;
			int nGraphicsMode = main_dc_.SetGraphicsMode(GM_ADVANCED);
			if (angle_on_mouse_move_ != 0) {
				double fangle = (double)angle_on_mouse_move_ / 180. * 3.1415926;
				xform.eM11 = (float)cos(fangle);
				xform.eM12 = (float)sin(fangle);
				xform.eM21 = (float)-sin(fangle);
				xform.eM22 = (float)cos(fangle);
				xform.eDx = (float)(center_point_.x - cos(fangle)*center_point_.x + sin(fangle)*center_point_.y);
				xform.eDy = (float)(center_point_.y - cos(fangle)*center_point_.y - sin(fangle)*center_point_.x);

				main_dc_.SetWorldTransform(&xform);
			}
			if (GetOperation() == IMAGE_OPERATION::DRAW_RECT) {
				main_dc_.Rectangle(current_rect_);
			} else if (GetOperation() == IMAGE_OPERATION::DRAW_ROUNDRECT) {
				int width = (current_rect_.Width() > current_rect_.Height()) ? current_rect_.Height() : current_rect_.Width();
				main_dc_.RoundRect(current_rect_.left, current_rect_.top, current_rect_.right, current_rect_.bottom, width, width);
			} else if (GetOperation() == IMAGE_OPERATION::DRAW_CIRCLE || GetOperation() == IMAGE_OPERATION::DRAW_ELLIPSE) {
				main_dc_.Ellipse(current_rect_);
			} else if (GetOperation() == IMAGE_OPERATION::DRAW_LINE) {
				main_dc_.MoveTo(current_rect_.TopLeft());
				main_dc_.LineTo(current_rect_.BottomRight());
			}
			if (angle_on_mouse_move_ != 0) {
				xform.eM11 = (float)1.0;
				xform.eM12 = (float)0;
				xform.eM21 = (float)0;
				xform.eM22 = (float)1.0;
				xform.eDx = (float)0;
				xform.eDy = (float)0;

				main_dc_.SetWorldTransform(&xform);
			}
			main_dc_.SetGraphicsMode(nGraphicsMode);
			main_dc_.SelectObject(old_pen);
		}
	}
}

void WinPaint::UpdatePixelInfoInToolbar() {

	if (show_toolbar_ && toolbar_dc_.GetSafeHdc() != NULL) {

		if (!show_toolbar_region_ || toolbar_region_index_ != 1) {
			InvalidateRect(toolbar_rect_);
			return;
		}

		EnterCriticalSection(&cs_for_toolbar_);

		CRect fill_rect(0, 0, toolbar_pixinfo_right_, toolbar_rect_.Height() + 1);
		toolbar_dc_.FillSolidRect(fill_rect, LABEL_COLOR);
		
		CString text;
		text.Format(L" X:%d", xy_position_.x);
		toolbar_dc_.DrawText(text, x_text_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		text.Format(L" Y:%d", xy_position_.y);
		toolbar_dc_.DrawText(text, y_text_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		BYTE B = 0, G = 0, R = 0;
		if (view_index_ < int(winview_.GetSize())) {
			if (winview_[view_index_].buffer[0]->data != NULL) {
				if (image_type_ == COLOR_24BIT) {
					unsigned char* selected_pixel = winview_[view_index_].buffer[0]->data + (xy_position_.y * img_width_ * 3) + (xy_position_.x * 3);
					if (selected_pixel != NULL) {
						B = *(selected_pixel);
						G = *(selected_pixel + 1);
						R = *(selected_pixel + 2);
					}
				} else {
					unsigned char* selected_pixel = winview_[view_index_].buffer[0]->data + (xy_position_.y * img_width_) + (xy_position_.x);
					if (selected_pixel != NULL) {
						B = G = R = *(selected_pixel);
					}
				}
			}
		}
		text.Format(L"RGB:%d,%d,%d", R, G, B);
		toolbar_dc_.DrawText(text, text.GetLength(), rgb_text_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		toolbar_dc_.FillSolidRect(rgb_color_rect_, RGB(R, G, B));

		cv::Mat rgb(1, 1, CV_8UC3);
		cv::Mat hsl(1, 1, CV_8UC3);
		rgb.at<cv::Vec3b>(0, 0) = cv::Vec3b(B, G, R);
		cv::cvtColor(rgb, hsl, cv::COLOR_BGR2HSV);
		cv::Vec3b hsl_color = hsl.at<cv::Vec3b>(0, 0);
		rgb.release();
		hsl.release();
		text.Format(L"CPW:%d,%d,%d", hsl_color[0], hsl_color[1], hsl_color[2]);
		toolbar_dc_.DrawText(text, hsl_text_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		COLORREF hsl_clr = hsl_color[0] << 16 | hsl_color[1] << 8 | hsl_color[2];
		toolbar_dc_.FillSolidRect(hsl_color_rect_, hsl_clr);
		
		toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
		toolbar_dc_.MoveTo(x_text_rect_.right - Formation::spacing(), x_text_rect_.top + Formation::spacing());
		toolbar_dc_.LineTo(x_text_rect_.right - Formation::spacing(), x_text_rect_.bottom - Formation::spacing());
		toolbar_dc_.MoveTo(y_text_rect_.right - Formation::spacing(), y_text_rect_.top + Formation::spacing());
		toolbar_dc_.LineTo(y_text_rect_.right - Formation::spacing(), y_text_rect_.bottom - Formation::spacing());
		toolbar_dc_.MoveTo(rgb_text_rect_.right - Formation::spacing(), rgb_text_rect_.top + Formation::spacing());
		toolbar_dc_.LineTo(rgb_text_rect_.right - Formation::spacing(), rgb_text_rect_.bottom - Formation::spacing());
		toolbar_dc_.MoveTo(hsl_text_rect_.right - Formation::spacing(), hsl_text_rect_.top + Formation::spacing());
		toolbar_dc_.LineTo(hsl_text_rect_.right - Formation::spacing(), hsl_text_rect_.bottom - Formation::spacing());
		
		LeaveCriticalSection(&cs_for_toolbar_);

		InvalidateRect(toolbar_rect_);
	}
}

void DrawTransparentBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource) {

	// We are going to paint the two DDB's in sequence to the destination.
	// 1st the monochrome bitmap will be blitted using an AND operation to
	// cut a hole in the destination. The color image will then be ORed
	// with the destination, filling it into the hole, but leaving the
	// surrounding area untouched.
	CDC dcCompatible;

	CBitmap *pBitmapOld;

	CBitmap bm;
	dcCompatible.CreateCompatibleDC(pDC);

	bm.CreateCompatibleBitmap(pDC, wWidth, wHeight);

	pBitmapOld = dcCompatible.SelectObject(&bm);

	dcCompatible.FillSolidRect(CRect(0, 0, wWidth, wHeight), RGB(0, 0, 0));

	pDC->BitBlt(xStart, yStart, wWidth, wHeight, &dcCompatible, 0, 0, SRCAND);

	dcCompatible.SelectObject(pBitmapOld);

	pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC, 0, 0, SRCPAINT);
}

void WinPaint::UpdateButtonInfoInToolbar() {

	if (show_toolbar_ && toolbar_dc_.GetSafeHdc() != NULL) {

		EnterCriticalSection(&cs_for_toolbar_);

		int icon_size = Formation::icon_size(Formation::MEDIUM_ICON);

		/*if (!IsWindowEnabled()) {
			toolbar_dc_.FillSolidRect(CRect(0, 0, toolbar_rect_.Width(), toolbar_rect_.Height()), DISABLE_COLOR);
			InvalidateRect(toolbar_rect_);
			LeaveCriticalSection(&cs_for_toolbar_);
			return;
		}*/

		//Toolbar Show Region Button
		if (show_toolbar_ == 2) {
			int button_index = TOOLBAR_BUTTONS::TOOLBAR_SHOW_REGION_BUTTON;
			CRect rect_show_region(toolbar_buttons_rect_[button_index]);
			rect_show_region.left = toolbar_buttons_rect_[button_index].CenterPoint().x - Formation::spacing() / 2;
			rect_show_region.right = toolbar_buttons_rect_[button_index].CenterPoint().x + Formation::spacing() / 2 + 1;
			rect_show_region.top = toolbar_buttons_rect_[button_index].CenterPoint().y - Formation::spacing() / 2;
			rect_show_region.bottom = toolbar_buttons_rect_[button_index].CenterPoint().y + Formation::spacing() / 2 + 1;
			if (!show_toolbar_region_) {
				toolbar_dc_.FillSolidRect(CRect(0, 0, toolbar_rect_.Width(), toolbar_rect_.Height()), LABEL_COLOR3);
				if (toolbar_button_selection_index_ == button_index) {
					toolbar_dc_.FillSolidRect(toolbar_buttons_rect_[button_index], BACKCOLOR1_SEL);
				}
				toolbar_dc_.FillSolidRect(rect_show_region, BLACK_COLOR3);
				/*toolbar_dc_.SelectObject(Formation::blackcolor_pen2());
				toolbar_dc_.MoveTo(toolbar_buttons_rect_[button_index].CenterPoint().x + Formation::spacing(), toolbar_buttons_rect_[button_index].CenterPoint().y - Formation::spacing());
				toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].CenterPoint().x, toolbar_buttons_rect_[button_index].CenterPoint().y);
				toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].CenterPoint().x + Formation::spacing(), toolbar_buttons_rect_[button_index].CenterPoint().y + Formation::spacing());*/
				LeaveCriticalSection(&cs_for_toolbar_);
				InvalidateRect(toolbar_rect_);
				return;
			}

			CRect rect_temp = toolbar_buttons_rect_[button_index];
			rect_temp.InflateRect(Formation::spacing(), 0, 0, 0); //Include separator space
			toolbar_dc_.FillSolidRect(rect_temp, LABEL_COLOR);
			if (toolbar_button_selection_index_ == button_index) {
				toolbar_dc_.FillSolidRect(toolbar_buttons_rect_[button_index], BACKCOLOR1_SEL);
			}
			toolbar_dc_.FillSolidRect(rect_show_region, WHITE_COLOR3);
			/*toolbar_dc_.SelectObject(Formation::blackcolor_pen2());
			toolbar_dc_.MoveTo(toolbar_buttons_rect_[button_index].CenterPoint().x - Formation::spacing(), toolbar_buttons_rect_[button_index].CenterPoint().y - Formation::spacing());
			toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].CenterPoint().x, toolbar_buttons_rect_[button_index].CenterPoint().y);
			toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].CenterPoint().x - Formation::spacing(), toolbar_buttons_rect_[button_index].CenterPoint().y + Formation::spacing());*/
			toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
			toolbar_dc_.MoveTo(toolbar_buttons_rect_[TOOLBAR_SHOW_REGION_BUTTON].left - Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_SHOW_REGION_BUTTON].CenterPoint().y + (icon_size / 2));
			toolbar_dc_.LineTo(toolbar_buttons_rect_[TOOLBAR_SHOW_REGION_BUTTON].left - Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_SHOW_REGION_BUTTON].CenterPoint().y - (icon_size / 2));
		}

		//Toolbar Region Button
		int button_index = TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON;
		if (toolbar_buttons_visible_[button_index]) {
			CRect rect_temp = toolbar_buttons_rect_[button_index];
			rect_temp.InflateRect(Formation::spacing(), 0, 0, 0); //Include separator space
			toolbar_dc_.FillSolidRect(rect_temp, LABEL_COLOR);
			if (button_index == toolbar_button_selection_index_) {
				toolbar_dc_.FillSolidRect(toolbar_buttons_rect_[button_index], BACKCOLOR1_SEL);
			}
			toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
			toolbar_dc_.MoveTo(toolbar_buttons_rect_[button_index].left - Formation::spacingHalf(), toolbar_buttons_rect_[button_index].CenterPoint().y + (icon_size / 2));
			toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].left - Formation::spacingHalf(), toolbar_buttons_rect_[button_index].CenterPoint().y - (icon_size / 2));

			CString button_seq = L"1";
			button_seq += (toolbar_region_enable_[TOOLBAR_REGION::PIXEL_INFORMATION] ? L"1" : L"0");
			button_seq += (toolbar_region_enable_[TOOLBAR_REGION::NAVIGATION_BUTTONS] ? L"1" : L"0");
			button_seq += (toolbar_region_enable_[TOOLBAR_REGION::IMAGESTORE_BUTTONS] ? L"1" : L"0");
			CRect rect[4];
			rect[0].top = rect[1].top = toolbar_buttons_rect_[button_index].CenterPoint().y - (Formation::spacing() + Formation::spacingHalf());
			rect[0].bottom = rect[1].bottom = rect[0].top + Formation::spacing();
			rect[0].left = toolbar_buttons_rect_[button_index].CenterPoint().x - (Formation::spacing() + Formation::spacingHalf());
			rect[0].right = rect[0].left + Formation::spacing();
			rect[1].left = rect[0].right + Formation::spacing();
			rect[1].right = rect[1].left + Formation::spacing();
			rect[2].top = rect[3].top = rect[0].bottom + Formation::spacing();
			rect[2].bottom = rect[3].bottom = rect[2].top + Formation::spacing();
			rect[3].left = rect[0].left;
			rect[3].right = rect[3].left + Formation::spacing();
			rect[2].left = rect[3].right + Formation::spacing();
			rect[2].right = rect[2].left + Formation::spacing();

			for (int i = 0; i < 4; i++) {
				if (button_seq[i] == '0') {
					toolbar_dc_.FillSolidRect(rect[i], LABEL_LINE_COLOR);
				} else {
					toolbar_dc_.FillSolidRect(rect[i], (i == toolbar_region_index_) ? BLACK_COLOR : BLACK_COLOR3);
				}
			}
			/*CString button_seq = L"110";
			if (toolbar_region_enable_[TOOLBAR_REGION::NAVIGATION_BUTTONS]) {
				button_seq = L"111";
			}
			CRect rect[3];
			rect[0].top = rect[1].top = rect[2].top = toolbar_buttons_rect_[button_index].CenterPoint().y - (Formation::spacing() / 2);
			rect[0].bottom = rect[0].top + Formation::spacing();
			rect[1].bottom = rect[1].top + Formation::spacing();
			rect[2].bottom = rect[2].top + Formation::spacing();
			if (button_seq == L"111") {
				rect[1].left = toolbar_buttons_rect_[button_index].CenterPoint().x - (Formation::spacing() / 2);
				rect[0].left = rect[1].left - (Formation::spacing() + (Formation::spacing() / 2));
				rect[2].left = rect[1].left + (Formation::spacing() + (Formation::spacing() / 2));
				rect[0].right = rect[0].left + Formation::spacing();
				rect[1].right = rect[1].left + Formation::spacing();
				rect[2].right = rect[2].left + Formation::spacing();
			} else if (button_seq == L"110") {
				rect[0].left = toolbar_buttons_rect_[button_index].CenterPoint().x - Formation::spacing() - Formation::spacing() / 2;
				rect[0].right = rect[0].left + Formation::spacing();
				rect[1].left = rect[0].right + Formation::spacing() / 2;
				rect[1].right = rect[1].left + Formation::spacing();
				rect[2].SetRectEmpty();
			}
			for (int i = 0; i < 3; i++) {
				if (i == toolbar_region_index_) {
					toolbar_dc_.FillSolidRect(rect[i], BLACK_COLOR);
				} else {
					toolbar_dc_.FillSolidRect(rect[i], BLACK_COLOR3);
				}
			} */
		}
		Gdiplus::Graphics graphics(toolbar_dc_);
		graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		CGdiPlusBitmapResource bitmap_gdi;

		toolbar_dc_.SelectObject(GetStockObject(NULL_BRUSH));
		if (toolbar_region_index_ == 0 || toolbar_region_index_ == 2 || toolbar_region_index_ == 3) {
			toolbar_dc_.FillSolidRect(CRect(0, 0, toolbar_pixinfo_right_, toolbar_rect_.Height()), LABEL_COLOR);
			for (int i = 2; i < sizeof(toolbar_buttons_rect_) / sizeof(toolbar_buttons_rect_[0]); i++) {
				if (toolbar_buttons_visible_[i]) {
					if (i == toolbar_button_selection_index_) {
						toolbar_dc_.SelectObject(Formation::blackcolor_pen());
					} else {
						toolbar_dc_.SelectObject(GetStockObject(NULL_PEN));
					}
					toolbar_dc_.Rectangle(toolbar_buttons_rect_[i].left + 1, toolbar_buttons_rect_[i].top + 1, toolbar_buttons_rect_[i].right - 1, toolbar_buttons_rect_[i].bottom - 1);
				}
			}
			toolbar_dc_.SelectObject(GetStockObject(NULL_PEN));
		}

		if(toolbar_region_index_ == 2) {

			int button_index = TOOLBAR_BUTTONS::NAVIGATION_MENU_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;

				int resource_id = IDI_WINPAINT_ROTATE;
				if (navigation_menu_index_ == 1) { //Move
					resource_id = IDI_WINPAINT_MOVE;
				} else if (navigation_menu_index_ == 2) { //Resize
					resource_id = IDI_WINPAINT_RESIZE;
				}
				if (bitmap_gdi.Load(MAKEINTRESOURCE(resource_id), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}

				//Draw Arrow
				CRect rect(toolbar_buttons_rect_[button_index].right - Formation::spacing3(), toolbar_buttons_rect_[button_index].CenterPoint().y - Formation::spacing(),
					toolbar_buttons_rect_[button_index].right - Formation::spacing(), toolbar_buttons_rect_[button_index].CenterPoint().y + Formation::spacing());
				Gdiplus::Point point[3];
				point[0].X = rect.left;
				point[1].X = rect.CenterPoint().x;
				point[2].X = rect.right;
				point[0].Y = point[2].Y = rect.top;
				point[1].Y = rect.bottom;
				graphics.FillPolygon(Formation::blackcolor_brush_gdi(), point, 3);
			}
			if (toolbar_buttons_visible_[button_index]) {
				toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
				toolbar_dc_.MoveTo(toolbar_buttons_rect_[button_index].right + Formation::spacingHalf(), toolbar_buttons_rect_[button_index].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(toolbar_buttons_rect_[button_index].right + Formation::spacingHalf(), toolbar_buttons_rect_[button_index].CenterPoint().y - (icon_size / 2));
			}
			if (navigation_menu_index_ == 0) { //Rotate
				
			} else if (navigation_menu_index_ == 1) { //Move
				button_index = TOOLBAR_BUTTONS::NAVIGATION_CLOCKWISE_BUTTON;
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_CLOCKWISE), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
				button_index = TOOLBAR_BUTTONS::NAVIGATION_ANTICLOCKWISE_BUTTON;
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_ANTICLOCKWISE), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
				button_index = TOOLBAR_BUTTONS::NAVIGATION_LEFT_BUTTON;
				if (toolbar_buttons_visible_[button_index]) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_LEFT), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
				button_index = TOOLBAR_BUTTONS::NAVIGATION_RIGHT_BUTTON;
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_RIGHT), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
				button_index = TOOLBAR_BUTTONS::NAVIGATION_UP_BUTTON;
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_UP), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
				button_index = TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON;
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
					int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
					int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
					if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_DOWN), L"USERICN", g_resource_handle)) {
						graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
						bitmap_gdi.Empty();
					}
				}
			} else if (navigation_menu_index_ == 2) { //Resize
				
			}

		} else if (toolbar_region_index_ == 0) {

			BOOL show_zoom_controls = TRUE;
			if (!zoom_text_rect_.IsRectEmpty()) {
				CString text = Formation::PrepareString(toolbar_dc_, _T("FIT"), zoom_text_rect_);
				if (zoom_index_ > ZOOM_OPERATION::FIT) {
					text.Format(L"%d%%", zoom_index_ * 100);
				}
				toolbar_dc_.DrawText(text, zoom_text_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				show_zoom_controls = FALSE;
			}
			int button_index = TOOLBAR_BUTTONS::PAN_BUTTON;
			if (toolbar_buttons_visible_[button_index]) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(panning_on_ ? MAKEINTRESOURCE(IDI_WINPAINT_PINOFF) : MAKEINTRESOURCE(IDI_WINPAINT_PINON), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::ZOOM_SHAPE_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_ZOOMSHAPE), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			} 
			button_index = TOOLBAR_BUTTONS::ZOOM_IN_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_ZOOMIN), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::ZOOM_OUT_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_ZOOMOUT), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_FIT), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::CENTER_LINE;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(center_line_ ? MAKEINTRESOURCE(IDI_WINPAINT_CENTERLINEON) : MAKEINTRESOURCE(IDI_WINPAINT_CENTERLINEOFF), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::SOURCE_VIEW_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load((image_index_ == 1) ? MAKEINTRESOURCE(IDI_WINPAINT_SOURCEVIEW) : MAKEINTRESOURCE(IDI_WINPAINT_BINARYVIEW), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(multiple_image_view_ ? MAKEINTRESOURCE(IDI_WINPAINT_SINGLEVIEW) : MAKEINTRESOURCE(IDI_WINPAINT_MULTIPLEVIEW), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(show_hide_shapes_ ? MAKEINTRESOURCE(IDI_WINPAINT_HIDESHAPES) : MAKEINTRESOURCE(IDI_WINPAINT_SHOWSHAPES), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::LOCK_IMAGE;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(lock_image_ ? MAKEINTRESOURCE(IDI_WINPAINT_LOCKIMAGE) : MAKEINTRESOURCE(IDI_WINPAINT_UNLOCKIMAGE), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::SAVE_IMAGE;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				Gdiplus::Rect rect_gdi(toolbar_buttons_rect_[button_index].left + Formation::spacing(), toolbar_buttons_rect_[button_index].top + Formation::spacing(), toolbar_buttons_rect_[button_index].Width() - (Formation::spacing() * 2), toolbar_buttons_rect_[button_index].Height() - (Formation::spacing() * 2));
				int x = rect_gdi.X + (rect_gdi.Width - icon_size + 1) / 2;
				int y = rect_gdi.Y + (rect_gdi.Height - icon_size + 1) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_SAVEIMAGE), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
		
			toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
			//if (show_zoom_controls) {
				toolbar_dc_.MoveTo(zoom_text_rect_.left - Formation::spacing(), zoom_text_rect_.CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(zoom_text_rect_.left - Formation::spacing(), zoom_text_rect_.CenterPoint().y - (icon_size / 2));
				//}
			for (int i = TOOLBAR_BUTTONS::CENTER_LINE; i <= SAVE_IMAGE; i++) {
				if (toolbar_buttons_visible_[i]) {
					toolbar_dc_.MoveTo(toolbar_buttons_rect_[i].left - Formation::spacingHalf(), toolbar_buttons_rect_[i].CenterPoint().y + (icon_size / 2));
					toolbar_dc_.LineTo(toolbar_buttons_rect_[i].left - Formation::spacingHalf(), toolbar_buttons_rect_[i].CenterPoint().y - (icon_size / 2));
				}
			}

		} else if (toolbar_region_index_ == 3) {

			DrawImageStoreProgressBar(FALSE);

			button_index = TOOLBAR_BUTTONS::IMAGESTORE_PASS;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				toolbar_dc_.SelectObject(Formation::acceptcolor_pen());
				CRect rect = toolbar_buttons_rect_[button_index];
				rect.left = toolbar_buttons_rect_[button_index].CenterPoint().x - (icon_size / 2);
				rect.right = toolbar_buttons_rect_[button_index].CenterPoint().x + (icon_size / 2);
				rect.top = toolbar_buttons_rect_[button_index].CenterPoint().y - (icon_size / 2);
				rect.bottom = toolbar_buttons_rect_[button_index].CenterPoint().y + (icon_size / 2);
				toolbar_dc_.Rectangle(rect);
				if (imagestore_pass_on_) {
					rect.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
					toolbar_dc_.FillSolidRect(rect, ACCEPT_COLOR);
				}
			} 
			button_index = TOOLBAR_BUTTONS::IMAGESTORE_FAIL;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				toolbar_dc_.SelectObject(Formation::rejectcolor_pen());
				CRect rect = toolbar_buttons_rect_[button_index];
				rect.left = toolbar_buttons_rect_[button_index].CenterPoint().x - (icon_size / 2);
				rect.right = toolbar_buttons_rect_[button_index].CenterPoint().x + (icon_size / 2);
				rect.top = toolbar_buttons_rect_[button_index].CenterPoint().y - (icon_size / 2);
				rect.bottom = toolbar_buttons_rect_[button_index].CenterPoint().y + (icon_size / 2);
				toolbar_dc_.Rectangle(rect);
				if (imagestore_fail_on_) {
					rect.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
					toolbar_dc_.FillSolidRect(rect, REJECT_COLOR);
				}
			}
			button_index = TOOLBAR_BUTTONS::IMAGESTORE_GRAB;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				toolbar_dc_.SelectObject(Formation::blackcolor_pen());
				CRect rect = toolbar_buttons_rect_[button_index];
				rect.left = toolbar_buttons_rect_[button_index].CenterPoint().x - (icon_size / 2);
				rect.right = toolbar_buttons_rect_[button_index].CenterPoint().x + (icon_size / 2);
				rect.top = toolbar_buttons_rect_[button_index].CenterPoint().y - (icon_size / 2);
				rect.bottom = toolbar_buttons_rect_[button_index].CenterPoint().y + (icon_size / 2);
				toolbar_dc_.Rectangle(rect);
				if (imagestore_grab_on_) {
					rect.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
					toolbar_dc_.FillSolidRect(rect, BLACK_COLOR);
				}
			}
			button_index = TOOLBAR_BUTTONS::IMAGESTORE_SHARE;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_SHARE), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
			button_index = TOOLBAR_BUTTONS::IMAGESTORE_CLEAR;
			if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_rect_[button_index].IsRectEmpty()) {
				int x = toolbar_buttons_rect_[button_index].left + (toolbar_buttons_rect_[button_index].Width() - icon_size) / 2;
				int y = toolbar_buttons_rect_[button_index].top + (toolbar_buttons_rect_[button_index].Height() - icon_size) / 2;
				if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_WINPAINT_CLEAR), L"USERICN", g_resource_handle)) {
					graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
					bitmap_gdi.Empty();
				}
			}
		
			BOOL imagestore_caption = FALSE;
			CRect imagestore_caption_rect = imagestore_text_rect_;
			imagestore_caption_rect.left = toolbar_rect_.left + Formation::spacing2();
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_PASS]) {
				imagestore_caption_rect.right = toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_PASS].left - Formation::spacing3();
			} else if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB]) {
				imagestore_caption_rect.right = toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB].left - Formation::spacing3();
			}
			if (imagestore_caption_rect.Width() > Formation::control_height() * 4) {
				toolbar_dc_.SelectObject(Formation::font(Formation::MEDIUM_FONT));
				toolbar_dc_.DrawText(Language::GetString(IDSTRINGT_IMAGEBANK), imagestore_caption_rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
				toolbar_dc_.SelectObject(Formation::font(Formation::SMALL_FONT));
				imagestore_caption = TRUE;
			}

			toolbar_dc_.SelectObject(Formation::labellinecolor_pen());
			if (imagestore_caption) {
				toolbar_dc_.MoveTo(imagestore_caption_rect.right + Formation::spacing2() + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(imagestore_caption_rect.right + Formation::spacing2() + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y - (icon_size / 2));
			}
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_PASS]) {
				toolbar_dc_.MoveTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_PASS].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_PASS].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y - (icon_size / 2));
			}
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_FAIL] || toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB]) {
				toolbar_dc_.MoveTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_GRAB].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y - (icon_size / 2));
			}
			if (!imagestore_text_rect_.IsRectEmpty()) {
				toolbar_dc_.MoveTo(imagestore_text_rect_.right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(imagestore_text_rect_.right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y - (icon_size / 2));
			}
			if (toolbar_buttons_visible_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE]) {
				toolbar_dc_.MoveTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y + (icon_size / 2));
				toolbar_dc_.LineTo(toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_SHARE].right + Formation::spacingHalf(), toolbar_buttons_rect_[TOOLBAR_BUTTONS::IMAGESTORE_CLEAR].CenterPoint().y - (icon_size / 2));
			}
		}
		LeaveCriticalSection(&cs_for_toolbar_);
		InvalidateRect(toolbar_rect_, FALSE);
	}
}

void WinPaint::OnPaint()
{
	CPaintDC dc(this);

	if (main_dc_.GetSafeHdc() != NULL) {

		EnterCriticalSection(&cs_for_shapes_);

		EnterCriticalSection(&cs_for_image_);

		main_dc_.BitBlt(0, 0, wnd_width_, wnd_height_, NULL, 0, 0, BLACKNESS);

		if (multiple_image_view_) {
			main_dc_.BitBlt(0, 0, wnd_width_, wnd_height_, &multiple_image_dc_, 0, 0, SRCCOPY);
		} else {
			main_dc_.StretchBlt(0, 0, wnd_width_, wnd_height_, &image_dc_, pan_point_.x, pan_point_.y, img_display_width_, img_display_height_, SRCCOPY);
		}

		LeaveCriticalSection(&cs_for_image_);

		if(show_hide_shapes_) {
			if (multiple_image_view_) {
				main_dc_.TransparentBlt(0, 0, wnd_width_, wnd_height_, &shape_dc_, 0, 0, wnd_width_, wnd_height_, RGB(0, 0, 0));
			} else {
				main_dc_.TransparentBlt(0, 0, wnd_width_, wnd_height_, &shape_dc_, pan_point_.x, pan_point_.y, img_display_width_, img_display_height_, RGB(0, 0, 0));
			}
			DrawShape();
			//Draw Window Shapes
			for (int i = 0; i < window_shapes_.GetSize(); i++) {
				WinShape* shape = window_shapes_.GetAt(i);
				if (multiple_image_view_) {
					if (shape->family() > 0 && shape->family() < 100) {
						shape->draw(main_dc_.GetSafeHdc(), FALSE);
					}
				} else {
					if (shape->family() == 0) {
						shape->draw(main_dc_.GetSafeHdc(), FALSE);
					}
				}
			}
		}

		if (show_tiny_image_view_) {
			ShowTinyImageView();
		}

		EnterCriticalSection(&cs_for_captionbar_);
		main_dc_.TransparentBlt(0, 0, captionbar_rect_.Width(), captionbar_rect_.Height(), &captionbar_dc_, 0, 0, captionbar_rect_.Width(), captionbar_rect_.Height(), LABEL_COLOR3);
		LeaveCriticalSection(&cs_for_captionbar_);
		
		EnterCriticalSection(&cs_for_toolbar_);
		main_dc_.TransparentBlt(0, toolbar_rect_.top, toolbar_rect_.Width(), toolbar_rect_.Height(), &toolbar_dc_, 0, 0, toolbar_rect_.Width(), toolbar_rect_.Height(), LABEL_COLOR3);
		LeaveCriticalSection(&cs_for_toolbar_);

		dc.BitBlt(0, 0, client_rect_.Width(), client_rect_.Height(), &main_dc_, 0, 0, SRCCOPY);

		//DrawTransparentBitmap(&dc, 0, toolbar_rect_.top, toolbar_rect_.Width(), toolbar_rect_.Height(), &toolbar_dc_, 0, 0);

		LeaveCriticalSection(&cs_for_shapes_);
	}
}

void WinPaint::ClearDisplay(BOOL redraw) {

	EnterCriticalSection(&cs_for_winpaint_);
	SetDefaultView(redraw);
	DeleteAllShapes(redraw);
	LeaveCriticalSection(&cs_for_winpaint_);
}

void WinPaint::ClearImageForMultipleView(BOOL redraw) {

	EnterCriticalSection(&cs_for_image_);
	multiple_image_dc_.BitBlt(0, 0, wnd_width_max_, wnd_height_max_, NULL, 0, 0, BLACKNESS);
	//Disturb the post analysis when inspection is running
	/*for (int view_index = 0; view_index < view_count_; view_index++) {
		for (int image_index = 0; image_index < 2; image_index++) {
			if (winview_[view_index].buffer[image_index] != NULL) {
				*winview_[view_index].buffer[image_index] = cv::Scalar::all(0);
			}
		}
	}*/
	if (redraw) Invalidate(FALSE);
	LeaveCriticalSection(&cs_for_image_);
}

void WinPaint::ShowTinyImageView() {

	CRect full_image_rect;
	full_image_rect.right = wnd_width_ - Formation::spacing2();
	full_image_rect.bottom = wnd_height_ - Formation::spacing2();
	if(show_toolbar_ == 2) { //Toggle Toolbar
		full_image_rect.bottom -= Formation::control_height();
	}
	full_image_rect.left = full_image_rect.right - (wnd_width_ * 25 / 100);
	full_image_rect.top = full_image_rect.bottom - (wnd_height_ * 25 / 100);

	CRect zoom_image_rect;
	zoom_image_rect.left = full_image_rect.left + (pan_point_.x * full_image_rect.Width() / img_width_);
	zoom_image_rect.top = full_image_rect.top + (pan_point_.y * full_image_rect.Height() / img_height_);
	zoom_image_rect.right = zoom_image_rect.left + (img_display_width_ * full_image_rect.Width() / img_width_);
	zoom_image_rect.bottom = zoom_image_rect.top + (img_display_height_ * full_image_rect.Height() / img_height_);

	main_dc_.StretchBlt(full_image_rect.left, full_image_rect.top, full_image_rect.Width(), full_image_rect.Height(), &image_dc_, 0, 0, img_width_, img_height_, SRCCOPY);
	main_dc_.SelectObject(&Formation::rejectcolor_pen2());
	main_dc_.SelectObject(GetStockObject(NULL_BRUSH));
	main_dc_.Rectangle(zoom_image_rect);
	main_dc_.Draw3dRect(full_image_rect, WHITE_COLOR, DISABLE_COLOR);
}

void WinPaint::BuildImage(cv::Mat& buffer, int view_index, BOOL update_multiple_view) {

	if (view_index == 0) {

		int dest_width = img_width_;
		int dest_height = img_height_;
		if (zoom_index_ == ZOOM_OPERATION::FIT) {
			if (window_image_dimentions_ratio_ == 0) {
				dest_width = wnd_width_;
				dest_height = wnd_height_;
			} else if (window_image_dimentions_ratio_ == 3) {
				//image_dc_.FillSolidRect(0, 0, img_width_, img_height_, BLACK_COLOR);
				image_dc_.BitBlt(0, 0, img_width_, img_height_, NULL, 0, 0, BLACKNESS);
				dest_width = wnd_width_;
				dest_height = img_height_ * wnd_width_ / img_width_;
			} else if (window_image_dimentions_ratio_ == 4) {
				//image_dc_.FillSolidRect(0, 0, img_width_, img_height_, BLACK_COLOR);
				image_dc_.BitBlt(0, 0, img_width_, img_height_, NULL, 0, 0, BLACKNESS);
				dest_width = img_width_ * wnd_height_ / img_height_;
				dest_height = wnd_height_;
			} else if (window_image_dimentions_ratio_ == 5) {
				//image_dc_.FillSolidRect(0, 0, img_width_, img_height_, BLACK_COLOR);
				image_dc_.BitBlt(0, 0, img_width_, img_height_, NULL, 0, 0, BLACKNESS);
				dest_width = img_width_ * wnd_height_ / img_height_;
				dest_height = wnd_height_;
				if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
					dest_height = img_height_ * wnd_width_ / img_width_;
					dest_width = wnd_width_;
				}
			}
		}
		/*Gdiplus::Bitmap img(bitmap_info_, buffer.data);
		Gdiplus::Graphics gr_photo(image_dc_);
		gr_photo.DrawImage(&img, Gdiplus::Rect(0, 0, dest_width, dest_height));*/
		StretchDIBits(image_dc_.GetSafeHdc(), 0, 0, dest_width, dest_height, 0, 0, img_width_, img_height_, buffer.data, bitmap_info_, DIB_RGB_COLORS, SRCCOPY);
	}

	if (update_multiple_view && !aoi_for_multipleview_.IsRectEmpty()) {
		
		cv::Mat aoi_image(buffer, cv::Rect(aoi_for_multipleview_.left, aoi_for_multipleview_.top, aoi_for_multipleview_.Width(), aoi_for_multipleview_.Height()));
		cv::resize(aoi_image, aoi_wnd_image_, cv::Size(winview_[view_index].winrect.Width(), winview_[view_index].winrect.Height()));
		aoi_image.release();

		StretchDIBits(multiple_image_dc_.GetSafeHdc(), winview_[view_index].winrect.left, winview_[view_index].winrect.top, winview_[view_index].winrect.Width(), winview_[view_index].winrect.Height(), 
			0, 0, winview_[view_index].winrect.Width(), winview_[view_index].winrect.Height(), aoi_wnd_image_.data, aoi_bitmap_info_, DIB_RGB_COLORS, SRCCOPY);
	}
}

BOOL WinPaint::RefreshImage(cv::Mat* image) {

	if (image == NULL) {
		return -1;
	}

	EnterCriticalSection(&cs_for_image_);
	BuildImage(*image);
	Invalidate(FALSE);
	LeaveCriticalSection(&cs_for_image_);

	return TRUE;
}

BOOL WinPaint::Refresh(cv::Mat* image) {

	RefreshShapes(FALSE);

	return RefreshImage(image);
}

BOOL WinPaint::UpdateImage(cv::Mat* src_image, cv::Mat* bin_image, int view_index, BOOL update_multiple_view) {

//int64 st_time = cv::getTickCount();

	EnterCriticalSection(&cs_for_image_);

	if (src_image != NULL) {
		winview_[view_index].buffer[0] = src_image;
	}
	if (bin_image != NULL) {
		winview_[view_index].buffer[1] = bin_image;
	}

	if (winview_[view_index].buffer[image_index_] != NULL) {
		BuildImage(*winview_[view_index].buffer[image_index_], view_index, update_multiple_view);
		Invalidate(FALSE);
	}

	LeaveCriticalSection(&cs_for_image_);

//wprintf(L"\t\t\UpdateImage %.2f\n", (cv::getTickCount() - st_time) / cv::getTickFrequency() * 1000);

	return TRUE;
}

BOOL WinPaint::Update(cv::Mat* src_image, cv::Mat* bin_image, int view_index, BOOL update_multiple_view) {

//int64 st_time = cv::getTickCount();

	RefreshShapes(FALSE);

//wprintf(L"\t\t\UpdateShape %.2f\n", (cv::getTickCount() - st_time) / cv::getTickFrequency() * 1000);

	return UpdateImage(src_image, bin_image, view_index, update_multiple_view);
}

void WinPaint::RefreshShapes(BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	shape_dc_.BitBlt(0, 0, img_shape_width_, img_shape_height_, NULL, 0, 0, BLACKNESS);
	//shape_dc_.FillSolidRect(0, 0, img_width_, img_height_, BLACK_COLOR);
	
	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if(shape != NULL) {
			if (multiple_image_view_) {
				if (shape->family() > 0 && shape->family() < 100) {
					shape->draw(shape_dc_.GetSafeHdc(), full_image_);
				}
				if (image_index_ == 1) {
					if (shape->family() > WINPAINT_BINARY_VIEW_SHAPES_FAMILY && shape->family() < WINPAINT_FIXED_SHAPES_FAMILY) {
						shape->draw(shape_dc_.GetSafeHdc(), full_image_);
					}
				}
			} else {
				if (shape->family() == 0 || (shape->family() >= 100 && shape->family() < WINPAINT_BINARY_VIEW_SHAPES_FAMILY)) {
					shape->draw(shape_dc_.GetSafeHdc(), full_image_);
				}
				if (image_index_ == 1) {
					if (shape->family() == WINPAINT_BINARY_VIEW_SHAPES_FAMILY || (shape->family() >= WINPAINT_BINARY_VIEW_SHAPES_FAMILY + 100 && shape->family() < WINPAINT_FIXED_SHAPES_FAMILY)) {
						shape->draw(shape_dc_.GetSafeHdc(), full_image_);
					}
				} 
				if (shape->family() >= WINPAINT_FIXED_SHAPES_FAMILY && shape->family() < WINPAINT_USER_FAMILY) {
					shape->draw(shape_dc_.GetSafeHdc(), full_image_);
				}
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) Invalidate(FALSE);
}

void WinPaint::DeleteAllShapes(BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	shape_dc_.BitBlt(0, 0, img_shape_width_, img_shape_height_, NULL, 0, 0, BLACKNESS);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		delete shape;
	}
	shapes_.RemoveAll();

	if (center_line_) {
		center_line_ = FALSE;
		UpdateButtonInfoInToolbar();
	}

	if (show_hide_shapes_) {
		show_hide_shapes_ = TRUE;
		UpdateButtonInfoInToolbar();
	}

	free_hand_points_image_.clear();
	free_hand_points_window_.clear();

	shape_ = NULL;
	SelectNone(false);

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteAllShapes(int family, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	INT_PTR size = shapes_.GetSize();
	for (int i = 0; i < size; i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family) {
			if (shape_ == shape) {
				shape_ = NULL;
				SelectNone(false);
			}
			shapes_.RemoveAt(i);
			delete shape;
			--i;
			--size;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteShapes(int family, int tag, BOOL redraw)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	DeleteShapes(family, str_tag, redraw);
}

void WinPaint::DeleteShapes(int family, CString tag, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			if (shape_ == shape) {
				shape_ = NULL;
				SelectNone(false);
			}
			shapes_.RemoveAt(i);
			delete shape;
			--i;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteShape(int family, int tag, int index, BOOL redraw)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	DeleteShape(family, str_tag, index, redraw);
}

void WinPaint::DeleteShape(int family, CString tag, int index, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	INT_PTR size = shapes_.GetSize();
	for (int i = 0; i < size; i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			if (shape_ == shape) {
				shape_ = NULL;
				SelectNone(false);
			}
			shapes_.RemoveAt(i);
			delete shape;
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

int WinPaint::GetAllRect(CString tag, std::vector<CRect>& rect_list) {

	EnterCriticalSection(&cs_for_shapes_);

	rect_list.clear();
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->tag() == tag) {
			/*CRect rect(shape->rectangle(TRUE));
			cv::RotatedRect rr(cv::Point2f(rect.CenterPoint().x, rect.CenterPoint().y), cv::Size2f(rect.Width(), rect.Height()), shape->angle());
			rect.left = rr.boundingRect().x;
			rect.top = rr.boundingRect().y;
			rect.right = rr.boundingRect().x + rr.boundingRect().width;
			rect.bottom = rr.boundingRect().y + rr.boundingRect().height;
			rect_list.push_back(rect);*/
			rect_list.push_back(shape->rectangle(TRUE));
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return (int)rect_list.size();
}

int WinPaint::GetSelectedShapesIndex(std::vector<int>& index_list) {

	EnterCriticalSection(&cs_for_shapes_);

	index_list.clear();
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->selected() == TRUE) {
			int index = shape->index();
			index_list.push_back(index);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return (int)index_list.size();
}

void WinPaint::ChangeTag(int old_tag, int new_tag) {

	CString str_old_tag, str_new_tag;
	str_old_tag.Format(L"%d", old_tag);
	str_new_tag.Format(L"%d", new_tag);

	ChangeTag(str_old_tag, str_new_tag);
}

void WinPaint::ChangeTag(CString old_tag, CString new_tag) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->tag() == old_tag) {
			shape->set_tag(new_tag, shape->overlap_area(), shape->bounding_rect());
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

int WinPaint::GetShapesCount() {

	EnterCriticalSection(&cs_for_shapes_);

	int count = 0;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() < WINPAINT_FIXED_SHAPES_FAMILY) {
			count++;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return count;
}

int WinPaint::GetShapesCount(int family) {

	EnterCriticalSection(&cs_for_shapes_);

	int count = 0;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->family() < WINPAINT_FIXED_SHAPES_FAMILY) {
			count++;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return count;
}

int WinPaint::GetShapesCount(int family, int tag) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return GetShapesCount(family, str_tag);
}

int WinPaint::GetShapesCount(int family, CString tag) {

	EnterCriticalSection(&cs_for_shapes_);

	int count = 0;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->family() < WINPAINT_FIXED_SHAPES_FAMILY) {
			count++;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return count;
}

void WinPaint::LockDrawOperation(BOOL lock) {

	lock_draw_operation_ = lock;
}

void WinPaint::LockUnlockAllShapes(int family, BOOL lock) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family) {
			shape->set_lock(WinShape::SHAPE_OPERATION::SELECT, lock);
			shape->set_lock(WinShape::SHAPE_OPERATION::MOVE, lock);
			shape->set_lock(WinShape::SHAPE_OPERATION::RESIZE, lock);
			shape->set_lock(WinShape::SHAPE_OPERATION::ROTATE, lock);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::LockAllShapes() {

	EnterCriticalSection(&cs_for_shapes_);

	//LockDrawOperation(TRUE);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		shape->set_lock(WinShape::SHAPE_OPERATION::SELECT, TRUE);
		shape->set_lock(WinShape::SHAPE_OPERATION::MOVE, TRUE);
		shape->set_lock(WinShape::SHAPE_OPERATION::RESIZE, TRUE);
		shape->set_lock(WinShape::SHAPE_OPERATION::ROTATE, TRUE);
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::LockAllShapes(int family) {

	LockUnlockAllShapes(family, TRUE);
}

void WinPaint::LockShapes(int family, int tag) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	LockShapes(family, str_tag);
}

void WinPaint::LockShapes(int family, CString tag) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			shape->set_lock(WinShape::SHAPE_OPERATION::SELECT, TRUE);
			shape->set_lock(WinShape::SHAPE_OPERATION::MOVE, TRUE);
			shape->set_lock(WinShape::SHAPE_OPERATION::RESIZE, TRUE);
			shape->set_lock(WinShape::SHAPE_OPERATION::ROTATE, TRUE);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::LockShapes(int family, int tag, WinShape::SHAPE_OPERATION operation, BOOL lock) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	LockShapes(family, str_tag, operation, lock);
}

void WinPaint::LockShapes(int family, CString tag, WinShape::SHAPE_OPERATION operation, BOOL lock) {

	EnterCriticalSection(&cs_for_shapes_);

	if (operation == WinShape::NONE || operation == WinShape::DRAW) {
		LockDrawOperation(lock);
	}

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			shape->set_lock(operation, lock);
		}
	}

	if (operation) {
		SelectNone(TRUE);
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::LockShapes(int tag, WinShape::SHAPE_OPERATION operation, BOOL lock) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	LockShapes(str_tag, operation, lock);
}

void WinPaint::LockShapes(CString tag, WinShape::SHAPE_OPERATION operation, BOOL lock) {

	EnterCriticalSection(&cs_for_shapes_);

	if (operation == WinShape::NONE || operation == WinShape::DRAW) {
		LockDrawOperation(lock);
	}

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family_ && shape->tag() == tag) {
			shape->set_lock(operation, lock);
		}
	}

	if (operation) {
		SelectNone(TRUE);
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

int WinPaint::GetLastIndex(int family, CString tag) {

	int index = 0;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			++index;
		}
	}
	return index;
}

WinPaint::ShapeInfo WinPaint::GetShapeInfo(int family, int tag, int index) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return GetShapeInfo(family, str_tag, index);
}

WinPaint::ShapeInfo WinPaint::GetShapeInfo(int family, CString tag, int index)
{
	EnterCriticalSection(&cs_for_shapes_);

	ShapeInfo shape_info;
	shape_info.shape_operation = WinShape::SHAPE_OPERATION::NONE;
	shape_info.shape_type = WinShape::SHAPE_TYPE::RECTANGLE;
	shape_info.angle = 0;
	shape_info.index = -1;
	shape_info.rect = CRect(0, 0, 0, 0);
	shape_info.tag = L"General";
	shape_info.family = 0;

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {

			CRect rect = shape->rectangle(full_image_);
			if (zoom_index_ > ZOOM_OPERATION::FIT) {
				GetRectInWindow(rect);
			}

			//SelectRect(rect, shape, TRUE);

			shape_info.tag = shape->tag();
			shape_info.shape_type = shape->shape_type();
			shape_info.angle = shape->angle();
			shape_info.index = shape->index();
			shape_info.rect = shape->rectangle(TRUE);
			shape_info.shape_operation = shape_operation_;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return shape_info;
}

WinShape* WinPaint::GetShape(int family, int tag, int index)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	return GetShape(family, str_tag, index);
}

WinShape* WinPaint::GetShape(int family, CString tag, int index)
{
	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			LeaveCriticalSection(&cs_for_shapes_);
			return shape;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return NULL;
}

void WinPaint::SetAsNewBorn(int family, int tag, int index)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	SetAsNewBorn(family, str_tag, index);
}

void WinPaint::SetAsNewBorn(int family, CString tag, int index)
{
	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			shape->set_new_born(TRUE);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::DisableZoomOperation(BOOL disable) {

	if (multiple_image_view_ && !disable) {
		return;
	}

	toolbar_buttons_disable_[WinPaint::PAN_BUTTON] = disable;
	toolbar_buttons_disable_[WinPaint::ZOOM_SHAPE_BUTTON] = disable;
	toolbar_buttons_disable_[WinPaint::ZOOM_IN_BUTTON] = disable;
	toolbar_buttons_disable_[WinPaint::ZOOM_OUT_BUTTON] = disable;
	toolbar_buttons_disable_[WinPaint::FIT_TO_IMAGE_BUTTON] = disable;
	toolbar_buttons_disable_[WinPaint::CENTER_LINE] = disable;

	//UpdateButtonInfoInToolbar();
}

void WinPaint::ShowZoomOperation(BOOL visible) {

	toolbar_buttons_visible_[WinPaint::PAN_BUTTON] = visible;
	toolbar_buttons_visible_[WinPaint::ZOOM_SHAPE_BUTTON] = visible;
	toolbar_buttons_visible_[WinPaint::ZOOM_IN_BUTTON] = visible;
	toolbar_buttons_visible_[WinPaint::ZOOM_OUT_BUTTON] = visible;
	toolbar_buttons_visible_[WinPaint::FIT_TO_IMAGE_BUTTON] = visible;
	toolbar_buttons_visible_[WinPaint::CENTER_LINE] = visible;

	RefreshToolbar();
}

void WinPaint::DisableToolbarButton(TOOLBAR_BUTTONS button, BOOL disable) {

	if (button >= 0 && button < sizeof(toolbar_buttons_rect_) / sizeof(toolbar_buttons_rect_[0])) {

		toolbar_buttons_disable_[button] = disable;

		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::ShowToolbarButton(TOOLBAR_BUTTONS button, BOOL visible, DWORD update) {

	if (button >= 0 && button < sizeof(toolbar_buttons_rect_) / sizeof(toolbar_buttons_rect_[0])) {

		toolbar_buttons_visible_[button] = visible;
	
		if (button == SHOW_SHAPES_BUTTON && !visible) {
			if (!IsShapesOn()) {
				ShowHideShapes(TRUE);
			}
		}

		if (update == 1) {
			RefreshToolbar();
		} else if (update == 2) {
			UpdateButtonInfoInToolbar();
		}
	}
}

void WinPaint::ToolbarRegionButtonClicked() {

	while (1) {
		if (toolbar_region_index_ == 4) {
			toolbar_region_index_ = 0;
		}
		if (toolbar_region_enable_[toolbar_region_index_]) {
			if (toolbar_region_index_ == 1) {
				UpdatePixelInfoInToolbar();
			}
			break;
		}
		++toolbar_region_index_;
	}
}

void WinPaint::EnableToolbarRegion(TOOLBAR_REGION toolbar_region, BOOL enable) {

	if (!enable) {
		ToolbarRegionButtonClicked();
	}
	toolbar_region_enable_[toolbar_region] = enable;
	UpdateButtonInfoInToolbar();
}

void WinPaint::SetToolbarRegion(TOOLBAR_REGION toolbar_region) {

	if (toolbar_region_enable_[toolbar_region]) {
		toolbar_region_index_ = toolbar_region;
		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::SwitchToolbarRegion() {
	
	if (show_toolbar_) {
		show_toolbar_region_ = TRUE;
		ToolbarRegionButtonClicked();
		UpdateButtonInfoInToolbar();
		//InvalidateRect(toolbar_rect_, FALSE);
	}
}

void WinPaint::ShowShapes(BOOL show) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		shape->set_visible(show);
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::ShowShapes(int family, BOOL show) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family) {
			shape->set_visible(show);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::ShowShapes(int family, int tag, BOOL show) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ShowShapes(family, str_tag, show);
}

void WinPaint::ShowShapes(int family, CString tag, BOOL show) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			shape->set_visible(show);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::ShowShape(int family, int tag, int index, BOOL show) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ShowShape(family, str_tag, index, show);
}

void WinPaint::ShowShape(int family, CString tag, int index, BOOL show) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			shape->set_visible(show);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

void WinPaint::SelectAllShapes(BOOL select, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		shape->set_selected(select);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::SelectAllShapes(int family, int tag, BOOL select, BOOL redraw)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	SelectAllShapes(family, str_tag, select, redraw);
}

void WinPaint::SelectAllShapes(int family, CString tag, BOOL select, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {

			shape->set_selected(select);

			if (select) {
				CRect rect = shape->rectangle(full_image_);
				if (zoom_index_ > ZOOM_OPERATION::FIT) {
					rect.left *= zoom_index_;
					rect.top *= zoom_index_;
					rect.right *= zoom_index_;
					rect.bottom *= zoom_index_;
					rect.OffsetRect(-pan_point_);
				}
				SelectRect(rect, shape, TRUE);
			}

			if (shape_ == NULL || (shape_ != NULL && (shape_->family() != family || shape_->tag() != tag))) {
				shape_ = shape;
				shape_info_.family = shape_->family();
				shape_info_.tag = shape_->tag();
				shape_info_.shape_type = shape_->shape_type();
				shape_info_.angle = shape_->angle();
				shape_info_.index = shape_->index();
				shape_info_.rect = shape_->rectangle(TRUE);
				shape_info_.shape_operation = shape_operation_;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

const WinPaint::ShapeInfo* WinPaint::SelectShape(int family, int tag, int index, BOOL select, BOOL redraw)
{
	CString str_tag;
	str_tag.Format(L"%d", tag);

	return SelectShape(family, str_tag, index, select, redraw);
}

const WinPaint::ShapeInfo* WinPaint::SelectShape(int family, CString tag, int index, BOOL select, BOOL redraw)
{
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape != NULL && shape->family() == family && shape->tag() == tag && shape->index() == index) {
			
			SelectAllShapes(FALSE, FALSE);

			EnterCriticalSection(&cs_for_shapes_);

			shape_ = shape;
			shape->set_selected(select);

			if (select) {
				CRect rect = shape_->rectangle(full_image_);
				if (zoom_index_ > ZOOM_OPERATION::FIT) {
					rect.left *= zoom_index_;
					rect.top *= zoom_index_;
					rect.right *= zoom_index_;
					rect.bottom *= zoom_index_;
					rect.OffsetRect(-pan_point_);
				}
				SelectRect(rect, shape_, TRUE);
			}
			shape_info_.family = shape_->family();
			shape_info_.tag = shape_->tag();
			shape_info_.shape_type = shape_->shape_type();
			shape_info_.angle = shape_->angle();
			shape_info_.index = shape_->index();
			shape_info_.rect = shape_->rectangle(TRUE);
			shape_info_.shape_operation = shape_operation_;
			
			LeaveCriticalSection(&cs_for_shapes_);

			if (redraw) RefreshShapes(redraw);

			return &shape_info_;
		}
	}
	return NULL;
}

int WinPaint::GetLastWindowShapeIndex(int family, CString tag, WinShape::SHAPE_TYPE shape_type) {

	//EnterCriticalSection(&cs_for_shapes_);

	int index = 0;
	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			++index;
		}
	}

	//LeaveCriticalSection(&cs_for_shapes_);

	return index;
}

void WinPaint::AddString(int family, CRect rect, CString text, int format, int backmode, int height, int weight, bool italic, bool underline, CString face_name, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw) {

	WinText* wintext = new WinText();
	if (wintext != NULL) {

		if (family > 0 && family < view_count_) {
			
			CRect rectTemp(rect);
			rect.left = winview_[family - 1].winrect.left + rect.left;
			rect.top = winview_[family - 1].winrect.top + rect.top;
			rect.right = rect.left + rectTemp.Width();
			rect.bottom = rect.top + rectTemp.Height();
			/*double dblTemp = 0.0;
			CRect rectTemp(rect);
			double width_ratio_wnd = 0.0;
			double height_ratio_wnd = 0.0;
			width_ratio_wnd = (double)winview_[family - 1].winrect.Width() / aoi_for_multipleview_.Width();
			height_ratio_wnd = (double)winview_[family - 1].winrect.Height() / aoi_for_multipleview_.Height();
			
			dblTemp = rectTemp.left * width_ratio_wnd;
			rectTemp.left = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			dblTemp = rectTemp.top * height_ratio_wnd;
			rectTemp.top = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			dblTemp = rectTemp.right * width_ratio_wnd;
			rectTemp.right = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			dblTemp = rectTemp.bottom * height_ratio_wnd;
			rectTemp.bottom = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			rectTemp.left += winview_[family - 1].winrect.left;
			rectTemp.top += winview_[family - 1].winrect.top;
			rectTemp.right += winview_[family - 1].winrect.left;
			rectTemp.bottom += winview_[family - 1].winrect.top;
			rect = rectTemp;*/
		}

		wintext->set_multiple_of_4(FALSE);
		wintext->set_shape_type(WinShape::TEXT);
		wintext->set_rectangle(rect, rect);
		wintext->SetTextProperties(text, format, backmode, text_color, back_color);
		wintext->SetFontProperties(height, height, weight, italic, underline, face_name);
		wintext->set_index(GetLastWindowShapeIndex(family, 0, WinShape::SHAPE_TYPE::TEXT));
		wintext->set_tag(0, overlap_area_, bounding_rect_);
		wintext->set_family(family);
		wintext->set_angle(angle);
		wintext->set_lock(WinShape::SHAPE_OPERATION::NONE, FALSE);

		window_shapes_.Add(wintext);
	}

	if (redraw) Invalidate(FALSE);
}

void WinPaint::AddString(int family, CRect rect, CString text, int format, int backmode, HFONT font, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw) {

	WinText* wintext = new WinText();
	if (wintext != NULL) {

		if (family > 0 && family <= view_count_) {
			
			CRect rectTemp(rect);
			rect.left = winview_[family - 1].winrect.left + rect.left;
			rect.top = winview_[family - 1].winrect.top + rect.top;
			rect.right = rect.left + rectTemp.Width();
			rect.bottom = rect.top + rectTemp.Height();

			/*CRect rectTemp(rect);
			double width_ratio_wnd = (double)winview_[family - 1].winrect.Width() / aoi_for_multipleview_.Width();
			double height_ratio_wnd = (double)winview_[family - 1].winrect.Height() / aoi_for_multipleview_.Height();
			
			double dblTemp = rectTemp.left * width_ratio_wnd;
			rectTemp.left = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			dblTemp = rectTemp.top * height_ratio_wnd;
			rectTemp.top = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
			rectTemp.left += winview_[family - 1].winrect.left;
			rectTemp.top += winview_[family - 1].winrect.top;
			rectTemp.right = rectTemp.left + rect.Width();
			rectTemp.bottom = rectTemp.top + rect.Height();
			rect = rectTemp;*/
		}
				
		wintext->set_multiple_of_4(FALSE);
		wintext->set_shape_type(WinShape::TEXT);
		wintext->set_rectangle(rect, rect);
		wintext->SetTextProperties(text, format, backmode, text_color, back_color);
		wintext->SetFontProperties(font);
		wintext->set_index(GetLastWindowShapeIndex(family, 0, WinShape::SHAPE_TYPE::TEXT));
		wintext->set_tag(0, overlap_area_, bounding_rect_);
		wintext->set_family(family);
		wintext->set_angle(angle);
		wintext->set_lock(WinShape::SHAPE_OPERATION::NONE, FALSE);

		window_shapes_.Add(wintext);
	}
	if (redraw) Invalidate(FALSE);
}

void WinPaint::SetString(int family, int index, CString text, BOOL redraw) {

	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		if (shape->family() == family && shape->index() == index) {
			if (shape->shape_type() == WinShape::TEXT) {
				WinText* wintext = dynamic_cast<WinText*>(shape);
				wintext->set_text(text);
				break;
			}
		}
	}

	if (redraw) Invalidate(FALSE);
}

void WinPaint::VisibleString(int family, BOOL visible, BOOL redraw) {

	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		if (window_shapes_[i]->family() == family) {
			window_shapes_[i]->set_visible(visible);
			if (redraw) Invalidate(FALSE);
		}
	}
}

void WinPaint::VisibleString(int family, int index, BOOL visible, BOOL redraw) {

	if (index < (int)window_shapes_.GetSize()) {
		if(window_shapes_[index]->family() == family) {
			window_shapes_[index]->set_visible(visible);
			if (redraw) Invalidate(FALSE);
		}
	}
}

void WinPaint::RemoveAllStrings(BOOL redraw) {

	CRect result_rect;
	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		delete shape;
	}
	window_shapes_.RemoveAll();

	if (redraw) Invalidate(FALSE);
}

void WinPaint::RemoveString(int family, BOOL redraw) {

	CRect result_rect;
	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		if (shape->family() == family) {
			window_shapes_.RemoveAt(i);
			delete shape;
		}
	}
	if (redraw) Invalidate(FALSE);
}

void WinPaint::RemoveString(CRect rect, BOOL redraw) {

	CRect result_rect;
	for (int i = 0; i < window_shapes_.GetSize(); i++) {
		WinShape* shape = window_shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::TEXT) {
			WinText* text = dynamic_cast<WinText*>(shape);
			CRect temp_rect = text->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
				//if (Rect.EqualRect(text->rectangle(full_image_))) {
				window_shapes_.RemoveAt(i);
				delete text;
				break;
			}
		}
	}
	if (redraw) Invalidate(FALSE);
}

void WinPaint::DrawCircle(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index, BOOL multiple_of_4, int shape_selection_size)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinEllipse* ellipse = new WinEllipse();
	if (ellipse != NULL) {

		/*CRect rect_in_window_view = rect;
		CRect rect_in_image_view = GetRectWRTImage(rect);
		if (full_image_ || (respect_to_img && !full_image_)) {*/
			CRect rect_in_window_view = GetRectWRTWindow(rect);
			if (view_index != -1 && view_index < view_count_) {
				rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
			}
			CRect rect_in_image_view = rect;
		//}

		ellipse->set_shape_type(WinShape::CIRCLE);
		ellipse->set_multiple_of_4(multiple_of_4);
		ellipse->set_rectangle(rect_in_window_view, rect_in_image_view);
		ellipse->set_line_width(linewidth);
		ellipse->set_line_style(linestyle);
		ellipse->set_line_color(linecolor);
		ellipse->set_fill_color(fillcolor);
		ellipse->set_index(GetLastIndex(family_, tag_));
		ellipse->set_tag(tag_, overlap_area_, bounding_rect_);
		ellipse->set_data(data_);
		ellipse->set_family(family_);
		ellipse->set_angle(angle);
		ellipse->set_shape_selection_size(shape_selection_size == 0 ? shape_selection_size_ : shape_selection_size);

		shapes_.Add(ellipse);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteCircle(CRect rect, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::CIRCLE) {
			WinEllipse* ellipse = dynamic_cast<WinEllipse*>(shape);
			CRect temp_rect = ellipse->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
				//if (Rect.EqualRect(ellipse->rectangle(full_image_))) {
				SelectNone(false);
				shapes_.RemoveAt(i);
				delete ellipse;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawRect(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index, BOOL multiple_of_4, int shape_selection_size)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinRect* rectangle = new WinRect();
	if ( rectangle != NULL ) {

		/*CRect rect_in_window_view = rect;
		CRect rect_in_image_view = GetRectWRTImage(rect);
		if (full_image_ || (respect_to_img && !full_image_)) {*/
		CRect rect_in_window_view = GetRectWRTWindow(rect);
		if (view_index != -1 && view_index < view_count_) {
			rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
		}
		CRect rect_in_image_view = rect;
		//}

		rectangle->set_shape_type(WinShape::RECTANGLE);
		rectangle->set_multiple_of_4(multiple_of_4);
		rectangle->set_rectangle(rect_in_window_view, rect_in_image_view);
		rectangle->set_line_width(linewidth);
		rectangle->set_line_style(linestyle);
		rectangle->set_line_color(linecolor);
		rectangle->set_fill_color(fillcolor);
		rectangle->set_index(GetLastIndex(family_, tag_));
		rectangle->set_tag(tag_, overlap_area_, bounding_rect_);
		rectangle->set_data(data_);
		rectangle->set_family(family_);
		rectangle->set_angle(angle);
		rectangle->set_shape_selection_size(shape_selection_size == 0 ? shape_selection_size_ : shape_selection_size);

		shapes_.Add(rectangle);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteRect(CRect Rect, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::RECTANGLE) {
			WinRect* rectangle = dynamic_cast<WinRect*>(shape);
			CRect temp_rect = rectangle->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, Rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
			//if (Rect.EqualRect(temp_rect)) {
				SelectNone(false);
				shapes_.RemoveAt(i);
				delete rectangle;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawRoundRect(CRect rect, int linewidth, int linestyle, float widtharc, float heightarc, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index, BOOL multiple_of_4, int shape_selection_size)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinRoundRect* round_rectangle = new WinRoundRect();
	if ( round_rectangle != NULL ) {

		CRect rect_in_window_view = GetRectWRTWindow(rect);
		if (view_index != -1 && view_index < view_count_) {
			rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
		}
		CRect rect_in_image_view = rect;

		round_rectangle->set_shape_type(WinShape::ROUNDRECT);
		round_rectangle->set_multiple_of_4(multiple_of_4);
		round_rectangle->set_rectangle(rect_in_window_view, rect_in_image_view);
		round_rectangle->set_arc(widtharc, heightarc);
		round_rectangle->set_line_width(linewidth);
		round_rectangle->set_line_style(linestyle);
		round_rectangle->set_line_color(linecolor);
		round_rectangle->set_fill_color(fillcolor);
		round_rectangle->set_index(GetLastIndex(family_, tag_));
		round_rectangle->set_tag(tag_, overlap_area_, bounding_rect_);
		round_rectangle->set_data(data_);
		round_rectangle->set_family(family_);
		round_rectangle->set_angle(angle);
		round_rectangle->set_shape_selection_size(shape_selection_size == 0 ? shape_selection_size_ : shape_selection_size);

		shapes_.Add(round_rectangle);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteRoundRect(CRect rect, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::ROUNDRECT) {
			WinRoundRect* round_rectangle = dynamic_cast<WinRoundRect*>(shape);
			CRect temp_rect = round_rectangle->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
			//if (Rect.EqualRect(round_rectangle->rectangle(full_image_))) {
				SelectNone(false);
				shapes_.RemoveAt(i);
				delete round_rectangle;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawEllipse(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index, BOOL multiple_of_4, int shape_selection_size)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinEllipse* ellipse = new WinEllipse();
	if ( ellipse != NULL ) {
		
		CRect rect_in_window_view = GetRectWRTWindow(rect);
		if (view_index != -1 && view_index < view_count_) {
			rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
		}
		CRect rect_in_image_view = rect;

		ellipse->set_shape_type(WinShape::ELLIPSE);
		ellipse->set_multiple_of_4(multiple_of_4);
		ellipse->set_rectangle(rect_in_window_view, rect_in_image_view);
		ellipse->set_line_width(linewidth);
		ellipse->set_line_style(linestyle);
		ellipse->set_line_color(linecolor);
		ellipse->set_fill_color(fillcolor);
		ellipse->set_index(GetLastIndex(family_, tag_));
		ellipse->set_tag(tag_, overlap_area_, bounding_rect_);
		ellipse->set_data(data_);
		ellipse->set_family(family_);
		ellipse->set_angle(angle);
		ellipse->set_shape_selection_size(shape_selection_size == 0 ? shape_selection_size_ : shape_selection_size);

		shapes_.Add(ellipse);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteEllipse(CRect rect, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::ELLIPSE) {
			WinEllipse* ellipse = dynamic_cast<WinEllipse*>(shape);
			CRect temp_rect = ellipse->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
			//if (Rect.EqualRect(ellipse->rectangle(full_image_))) {
				SelectNone(false);
				shapes_.RemoveAt(i);
				delete ellipse;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawLine(CPoint left_top_point, CPoint right_bottom_point, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinLine* line = new WinLine();
	if ( line != NULL ) {
		
		CPoint left_top_point_in_window_view = GetPointWRTWindow(left_top_point);
		CPoint right_bottom_point_in_window_view = GetPointWRTWindow(right_bottom_point);
		CPoint left_top_point_in_image_view = left_top_point;
		CPoint right_bottom_point_in_image_view = right_bottom_point;
		
		line->set_shape_type(WinShape::LINE);
		line->set_left_top_points(left_top_point_in_window_view, left_top_point_in_image_view);
		line->set_right_bottom_points(right_bottom_point_in_window_view, right_bottom_point_in_image_view);
		line->set_line_width(linewidth);
		line->set_line_style(linestyle);
		line->set_line_color(linecolor);
		line->set_index(GetLastIndex(family_, tag_));
		line->set_tag(tag_, overlap_area_, bounding_rect_);
		line->set_data(data_);
		line->set_family(family_);
		line->set_angle(angle);
		line->set_shape_selection_size(shape_selection_size_);

		shapes_.Add(line);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteLine(CPoint left_top_point, CPoint right_bottom_point, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::LINE) {
			WinLine* line = dynamic_cast<WinLine*>(shape);
			CRect Rect(left_top_point, right_bottom_point);
			CRect temp_rect = line->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, Rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
			//if (line->GetPointLT(full_image_) == PointLT && line->GetPointRB(full_image_) == PointRB) {
				SelectNone(false);
				shapes_.RemoveAt(i);
				delete line;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawPolygon(CArray<POINT>& points, CString text, HFONT font, int format, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, COLORREF textcolor, float angle, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	WinPolygon* polygon = new WinPolygon();
	if (polygon != NULL) {

		CArray<POINT> wnd_points;
		for (int index = 0; index < points.GetSize(); index++) {
			wnd_points.Add(GetPointWRTWindow(points[index]));
		}
		polygon->set_shape_type(WinShape::POLYGON);
		polygon->set_points(points, TRUE);
		polygon->set_points(wnd_points, FALSE);
		wnd_points.RemoveAll();
		polygon->set_text(text);
		polygon->set_font(font);
		polygon->set_format(format);
		polygon->set_line_width(linewidth);
		polygon->set_line_style(linestyle);
		polygon->set_line_color(linecolor);
		polygon->set_fill_color(fillcolor);
		polygon->set_text_color(textcolor);
		polygon->set_index(GetLastIndex(family_, tag_));
		polygon->set_tag(tag_, overlap_area_, bounding_rect_);
		polygon->set_data(data_);
		polygon->set_family(family_);
		polygon->set_angle(angle);
		polygon->set_shape_selection_size(shape_selection_size_);

		shapes_.Add(polygon);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawString(CRect rect, CString text, int format, int backmode, int height, int weight, bool italic, bool underline, CString face_name, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw, int view_index)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinText* wintext = new WinText();
	if (wintext != NULL) {
		
		CRect rect_in_window_view = GetRectWRTWindow(rect);
		if (view_index != -1 && view_index < view_count_) {
			rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
		}
		CRect rect_in_image_view = rect;
		int height_in_window_view = GetIntWRTWindow(height);
		int height_in_image_view = height;

		wintext->set_shape_type(WinShape::TEXT);
		wintext->set_rectangle(rect_in_window_view, rect_in_image_view);

		wintext->SetTextProperties(text, format, backmode, text_color, back_color);
		wintext->SetFontProperties(height_in_window_view, height_in_image_view, weight, italic, underline, face_name);
		wintext->set_index(GetLastIndex(family_, tag_));
		wintext->set_tag(tag_, overlap_area_, bounding_rect_);
		wintext->set_data(data_);
		wintext->set_family(family_);
		wintext->set_angle(angle);
		wintext->set_shape_selection_size(shape_selection_size_);

		shapes_.Add(wintext);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawString(CRect rect, CString text, int format, int backmode, HFONT font, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw, int view_index)
{
	EnterCriticalSection(&cs_for_shapes_);

	WinText* wintext = new WinText();
	if (wintext != NULL) {

		CRect rect_in_window_view = GetRectWRTWindow(rect);
		if (view_index != -1 && view_index < view_count_) {
			rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
		}
		CRect rect_in_image_view = rect;

		wintext->set_shape_type(WinShape::TEXT);
		wintext->set_rectangle(rect_in_window_view, rect_in_image_view);

		wintext->SetTextProperties(text, format, backmode, text_color, back_color);
		wintext->SetFontProperties(font);
		wintext->set_index(GetLastIndex(family_, tag_));
		wintext->set_tag(tag_, overlap_area_, bounding_rect_);
		wintext->set_data(data_);
		wintext->set_family(family_);
		wintext->set_angle(angle);
		wintext->set_shape_selection_size(shape_selection_size_);

		shapes_.Add(wintext);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DeleteString(CRect Rect, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect result_rect;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::TEXT) {
			WinText* text = dynamic_cast<WinText*>(shape);
			CRect temp_rect = text->rectangle(TRUE);
			result_rect.SubtractRect(temp_rect, Rect);
			if (result_rect.Width() < 2 || result_rect.Height() < 2) {
			//if (Rect.EqualRect(text->rectangle(full_image_))) {
				shapes_.RemoveAt(i);
				delete text;
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawPoints(CArray<CPoint>& points, COLORREF color, BOOL redraw, int view_index) {

	EnterCriticalSection(&cs_for_shapes_);

	WinPoint* point = new WinPoint();
	if (point != NULL) {

		CArray<CPoint> wnd_points;
		for (int index = 0; index < points.GetSize(); index++) {
			wnd_points.Add(GetPointWRTWindow(points[index]));
		}
		point->set_shape_type(WinShape::PIXEL);
		point->set_points(points, TRUE);
		point->set_points(wnd_points, FALSE);
		wnd_points.RemoveAll();
		point->set_color(color);
		point->set_index(GetLastIndex(family_, tag_));
		point->set_tag(tag_, overlap_area_, bounding_rect_);
		point->set_data(data_);
		point->set_family(family_);

		shapes_.Add(point);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::DrawPoints(std::vector<CPoint>& points, COLORREF color, BOOL redraw, int view_index) {

	EnterCriticalSection(&cs_for_shapes_);

	WinPoint* point = new WinPoint();
	if (point != NULL) {

		CArray<CPoint> wnd_points;
		for (int index = 0; index < points.size(); index++) {
			wnd_points.Add(GetPointWRTWindow(points[index]));
		}
		point->set_shape_type(WinShape::PIXEL);
		point->set_points(points, TRUE);
		point->set_points(wnd_points, FALSE);
		wnd_points.RemoveAll();
		point->set_color(color);
		point->set_index(GetLastIndex(family_, tag_));
		point->set_tag(tag_, overlap_area_, bounding_rect_);
		point->set_data(data_);
		point->set_family(family_);

		shapes_.Add(point);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditPolygon(int family, CString tag, int index, CArray<POINT>& points, CString text, COLORREF linecolor, COLORREF fillcolor, COLORREF textcolor, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::SHAPE_TYPE::POLYGON) {
			if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
				WinPolygon* winpolygon = dynamic_cast<WinPolygon*>(shape);
				if (winpolygon != NULL) {
					CArray<POINT> wnd_points;
					for (int index = 0; index < points.GetSize(); index++) {
						wnd_points.Add(GetPointWRTWindow(points[index]));
					}
					winpolygon->set_points(points, TRUE);
					winpolygon->set_points(wnd_points, FALSE);
					winpolygon->set_text(text);
					winpolygon->set_line_color(linecolor);
					winpolygon->set_fill_color(fillcolor);
					winpolygon->set_text_color(textcolor);
					wnd_points.RemoveAll();
				}
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditRect(CRect Rect, CRect PrevRect, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::RECTANGLE) {
			WinRect* rectangle = dynamic_cast<WinRect*>(shape);
			if (PrevRect.EqualRect(rectangle->rectangle(full_image_))) {
				//rectangle->set_rectangle(Rect, GetRespectRect(Rect, full_image_));
				break;
			}
		} else if (shape->shape_type() == WinShape::ROUNDRECT) {
			WinRoundRect* round_rectangle = dynamic_cast<WinRoundRect*>(shape);
			if (PrevRect.EqualRect(round_rectangle->rectangle(full_image_))) {
				//round_rectangle->set_rectangle(Rect, GetRespectRect(Rect, full_image_));
				break;
			}
		} else if (shape->shape_type() == WinShape::ELLIPSE) {
			WinEllipse* ellipse = dynamic_cast<WinEllipse*>(shape);
			if (PrevRect.EqualRect(ellipse->rectangle(full_image_))) {
				//ellipse->set_rectangle(Rect, GetRespectRect(Rect, full_image_));
				break;
			}
		} else if (shape->shape_type() == WinShape::TEXT) {
			WinText* text = dynamic_cast<WinText*>(shape);
			if (PrevRect.EqualRect(text->rectangle(full_image_))) {
				//text->set_rectangle(Rect, GetRespectRect(Rect, full_image_));
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditLine(CPoint PointTL, CPoint PointRB, CPoint PrevPointTL, CPoint PrevPointRB, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect Rect(PointTL.x, PointTL.y, PointRB.x, PointRB.y);
	CRect PrevRect(PrevPointTL.x, PrevPointTL.y, PrevPointRB.x, PrevPointRB.y);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::LINE) {
			WinLine* line = dynamic_cast<WinLine*>(shape);
			if (PrevRect.EqualRect(line->rectangle(full_image_))) {
				//line->set_rectangle(Rect, GetRespectRect(Rect, full_image_));
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditRectArc(CRect Rect, float WidthArc, float HeightArc, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::RECTANGLE) {
			WinRoundRect* round_rectangle = dynamic_cast<WinRoundRect*>(shape);
			if (Rect.EqualRect(round_rectangle->rectangle(full_image_))) {
				round_rectangle->set_arc(WidthArc, HeightArc);
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditWidth(CRect Rect, int Width, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::RECTANGLE) {
			WinRect* rectangle = dynamic_cast<WinRect*>(shape);
			if (Rect.EqualRect(rectangle->rectangle(full_image_))) {
				rectangle->set_line_width(Width);
				break;
			}
		} else if (shape->shape_type() == WinShape::ROUNDRECT) {
			WinRoundRect* round_rectangle = dynamic_cast<WinRoundRect*>(shape);
			if (Rect.EqualRect(round_rectangle->rectangle(full_image_))) {
				round_rectangle->set_line_width(Width);
				break;
			}
		} else if (shape->shape_type() == WinShape::ELLIPSE) {
			WinEllipse* ellipse = dynamic_cast<WinEllipse*>(shape);
			if (Rect.EqualRect(ellipse->rectangle(full_image_))) {
				ellipse->set_line_width(Width);
				break;
			}
		} else if (shape->shape_type() == WinShape::LINE) {
			WinLine* line = dynamic_cast<WinLine*>(shape);
			if (Rect.EqualRect(line->rectangle(full_image_))) {
				line->set_line_width(Width);
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditStyle(CRect Rect, int Style, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::RECTANGLE) {
			WinRect* rectangle = dynamic_cast<WinRect*>(shape);
			if (Rect.EqualRect(rectangle->rectangle(full_image_))) {
				rectangle->set_line_style(Style);
				break;
			}
		} else if (shape->shape_type() == WinShape::ROUNDRECT) {
			WinRoundRect* round_rectangle = dynamic_cast<WinRoundRect*>(shape);
			if (Rect.EqualRect(round_rectangle->rectangle(full_image_))) {
				round_rectangle->set_line_style(Style);
				break;
			}
		} else if (shape->shape_type() == WinShape::ELLIPSE) {
			WinEllipse* ellipse = dynamic_cast<WinEllipse*>(shape);
			if (Rect.EqualRect(ellipse->rectangle(full_image_))) {
				ellipse->set_line_style(Style);
				break;
			}
		} else if (shape->shape_type() == WinShape::LINE) {
			WinLine* line = dynamic_cast<WinLine*>(shape);
			if (Rect.EqualRect(line->rectangle(full_image_))) {
				line->set_line_style(Style);
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

BOOL WinPaint::IsShapeVisible(int family) {

	EnterCriticalSection(&cs_for_shapes_);

	BOOL shape_visible = FALSE;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family) {
			shape_visible = shape->visible();
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return shape_visible;
}

BOOL WinPaint::IsShapeVisible(int family, int tag, int index) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return IsShapeVisible(family, str_tag, index);
}

BOOL WinPaint::IsShapeVisible(int family, CString tag, int index) {

	EnterCriticalSection(&cs_for_shapes_);

	BOOL shape_visible = FALSE;
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			shape_visible = shape->visible();
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return shape_visible;
}

CRect WinPaint::GetRect(int family, int tag, int index, BOOL respect_to_img) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return GetRect(family, str_tag, index, respect_to_img);
}

CRect WinPaint::GetRect(int family, CString tag, int index, BOOL respect_to_img) {

	EnterCriticalSection(&cs_for_shapes_);

	CRect rect(0, 0, 0, 0);
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			rect = shape->rectangle(respect_to_img);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return rect;
}

void WinPaint::SetRect(int family, int tag, int index, CRect rect, BOOL redraw, int view_index) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	SetRect(family, str_tag, index, rect, redraw, view_index);
}

void WinPaint::SetRect(int family, CString tag, int index, CRect rect, BOOL redraw, int view_index) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			CRect rect_in_window_view = GetRectWRTWindow(rect);
			if (view_index != -1 && view_index < view_count_) {
				rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
			}
			CRect rect_in_image_view = rect;
			shape->set_rectangle(rect_in_window_view, rect_in_image_view);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeBoundingRect(int family, CString tag, int index, CRect rect) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			shape->set_bounding_rect(rect);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
}

BOOL WinPaint::ChangeAngle(int family, int tag, int index, float angle, BOOL redraw, int view_index) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return ChangeAngle(family, str_tag, index, angle, redraw, view_index);
}

BOOL WinPaint::ChangeAngle(int family, CString tag, int index, float angle, BOOL redraw, int view_index) {

	if (lock_image_) return FALSE;

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {

			if (!CheckOverlapping(shape, shape->shape_type(), shape->rectangle(TRUE))) {
				LeaveCriticalSection(&cs_for_shapes_);
				return FALSE;
			}
			if (!RectInsideBoundingRect(shape, shape->rectangle(TRUE))) {
				LeaveCriticalSection(&cs_for_shapes_);
				return FALSE;
			}

			shape->set_angle(angle);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);

	return TRUE;
}
BOOL WinPaint::ChangeRect(int family, int tag, int index, CRect rect, BOOL redraw, int view_index) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return ChangeRect(family, str_tag, index, rect, redraw, view_index);
}

BOOL WinPaint::ChangeRect(int family, CString tag, int index, CRect rect, BOOL redraw, int view_index) {

	if (lock_image_) return FALSE;

	EnterCriticalSection(&cs_for_shapes_);

	for (int shape_index = 0; shape_index < shapes_.GetSize(); shape_index++) {
		WinShape* shape = shapes_.GetAt(shape_index);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {

			if (!CheckOverlapping(shape, shape->shape_type(), rect)) {
				LeaveCriticalSection(&cs_for_shapes_);
				return FALSE;
			}
			if (!RectInsideBoundingRect(shape, rect)) {
				LeaveCriticalSection(&cs_for_shapes_);
				return FALSE;
			}

			CRect rect_in_window_view = GetRectWRTWindow(rect);
			if (view_index != -1 && view_index < view_count_) {
				rect_in_window_view = GetRectWRTWinViewWindow(winview_[view_index].winrect, rect);
			}
			CRect rect_in_image_view = rect;
			shape->set_rectangle(rect_in_window_view, rect_in_image_view);

			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);

	return TRUE;
}

BOOL WinPaint::ChangeRectResizeStyle(int family, CString tag, int index, BOOL resize_from_left_right_only, BOOL redraw) {

	if (lock_image_) return FALSE;

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			if(shape->shape_type() == WinShape::RECTANGLE) {
				WinRect* rectangle = dynamic_cast<WinRect*>(shape);
				rectangle->set_resize_style(resize_from_left_right_only);
			}
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);

	return TRUE;
}

void WinPaint::ChangeColor(int family, int tag, COLORREF color, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangeColor(family, str_tag, color, redraw);
}

void WinPaint::ChangeColor(int family, CString tag, COLORREF color, BOOL redraw) {

	if (lock_image_) return;

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag) {
			shape->set_line_color(color);
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeColor(int family, int tag, int index, COLORREF color, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	return ChangeColor(family, str_tag, index, color, redraw);
}

void WinPaint::ChangeColor(int family, CString tag, int index, COLORREF color, BOOL redraw) {

	if (lock_image_) return;

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index && shape->shape_type() != WinShape::TEXT) {
			shape->set_line_color(color);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeFillColor(int family, int tag, int index, COLORREF color, BOOL redraw) {
	
	CString str_tag;
	str_tag.Format(L"%d", tag);

	return ChangeColor(family, str_tag, index, color, redraw);
}

void WinPaint::ChangeFillColor(int family, CString tag, int index, COLORREF color, BOOL redraw) {

	if (lock_image_) return;

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index && shape->shape_type() != WinShape::TEXT) {
			shape->set_fill_color(color);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeLineWidth(int family, int tag, int index, int width, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangeLineWidth(family, str_tag, index, width, redraw);
}

void WinPaint::ChangeLineWidth(int family, CString tag, int index, int width, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index && shape->shape_type() != WinShape::TEXT) {
			shape->set_line_width(width);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeLineStyle(int family, int tag, int index, int style, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangeLineStyle(family, str_tag, index, style, redraw);
}

void WinPaint::ChangeLineStyle(int family, CString tag, int index, int style, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index && shape->shape_type() != WinShape::TEXT) {
			shape->set_line_style(style);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangePoints(int family, int tag, int index, CArray<POINT>& points, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangePoints(family, str_tag, index, points, redraw);
}

void WinPaint::ChangePoints(int family, CString tag, int index, CArray<POINT>& points, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			if (shape->shape_type() == WinShape::SHAPE_TYPE::POLYGON) {
				WinPolygon* winpolygon = dynamic_cast<WinPolygon*>(shape);
				if (winpolygon != NULL) {
					CArray<POINT> wnd_points;
					for (int index = 0; index < points.GetSize(); index++) {
						wnd_points.Add(GetPointWRTWindow(points[index]));
					}
					winpolygon->set_points(points, TRUE);
					winpolygon->set_points(wnd_points, FALSE);
					wnd_points.RemoveAll();
				}
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);
	
	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeTextColor(int family, int tag, int index, COLORREF color, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangeTextColor(family, str_tag, index, color, redraw);
}

void WinPaint::ChangeTextColor(int family, CString tag, int index, COLORREF color, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			if (shape->shape_type() == WinShape::SHAPE_TYPE::TEXT) {
				WinText* wintext = dynamic_cast<WinText*>(shape);
				if (wintext != NULL) {
					wintext->set_line_color(color);
				}
			} else if (shape->shape_type() == WinShape::SHAPE_TYPE::POLYGON) {
				WinPolygon* winpolygon = dynamic_cast<WinPolygon*>(shape);
				if (winpolygon != NULL) {
					winpolygon->set_text_color(color);
				}
			}
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::ChangeText(int family, int tag, int index, CString text, BOOL redraw) {

	CString str_tag;
	str_tag.Format(L"%d", tag);

	ChangeText(family, str_tag, index, text, redraw);
}

void WinPaint::ChangeText(int family, CString tag, int index, CString text, BOOL redraw) {

	EnterCriticalSection(&cs_for_shapes_);

	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() == index) {
			if (shape->shape_type() == WinShape::SHAPE_TYPE::TEXT) {
				WinText* wintext = dynamic_cast<WinText*>(shape);
				if (wintext != NULL) {
					wintext->set_text(text);
				}
			} else if (shape->shape_type() == WinShape::SHAPE_TYPE::POLYGON) {
				WinPolygon* winpolygon = dynamic_cast<WinPolygon*>(shape);
				if (winpolygon != NULL) {
					winpolygon->set_text(text);
				}
			}
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditColor(CRect Rect, COLORREF Color, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (Rect.EqualRect(shape->rectangle(full_image_))) {
			shape->set_line_color(Color);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditFillColor(CRect Rect, COLORREF Color, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (Rect.EqualRect(shape->rectangle(full_image_))) {
			shape->set_fill_color(Color);
			break;
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditTextProperties(CRect Rect, CString Text, int TextFormat, int TextBackMode, COLORREF TextColor, COLORREF TextBackColor, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::TEXT) {
			WinText* text = dynamic_cast<WinText*>(shape);
			if (Rect.EqualRect(text->rectangle(full_image_))) {
				text->SetTextProperties(Text, TextFormat, TextBackMode, TextColor, TextBackColor);
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

void WinPaint::EditFontProperties(CRect Rect, int Height, int Weight, bool Italic, bool Underline, CString FontFace, BOOL redraw)
{
	EnterCriticalSection(&cs_for_shapes_);

	for ( int i = 0 ; i < shapes_.GetSize() ; i++ ) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->shape_type() == WinShape::TEXT) {
			WinText* text = dynamic_cast<WinText*>(shape);
			if (Rect.EqualRect(text->rectangle(full_image_))) {
				//text->SetFontProperties(Height, GetRespectInt(Height, full_image_), Weight, Italic, Underline, FontFace, full_image_);
				break;
			}
		}
	}

	LeaveCriticalSection(&cs_for_shapes_);

	if (redraw) RefreshShapes(redraw);
}

float WinPaint::FindAngle(CPoint first_point, CPoint second_point) {

	float A = (float)(first_point.x - second_point.x);
	float O = (float)(first_point.y - second_point.y);
	return (float)(atan2(O, A) * (180 / CV_PI));
}

void WinPaint::OnRotate() {

	for (int i = NAVIGATION_CLOCKWISE_BUTTON; i <= NAVIGATION_DOWN_BUTTON; i++) {
		toolbar_buttons_visible_[i] = FALSE;
	}

	navigation_menu_index_ = 0;
	toolbar_buttons_visible_[NAVIGATION_CLOCKWISE_BUTTON] = toolbar_buttons_visible_[NAVIGATION_ANTICLOCKWISE_BUTTON] = TRUE;

	toolbar_button_selection_index_ = -1;
	UpdateButtonInfoInToolbar();
}

void WinPaint::OnMove() {

	for (int i = NAVIGATION_CLOCKWISE_BUTTON; i <= NAVIGATION_DOWN_BUTTON; i++) {
		toolbar_buttons_visible_[i] = FALSE;
	}

	navigation_menu_index_ = 1;
	toolbar_buttons_visible_[NAVIGATION_LEFT_BUTTON] = toolbar_buttons_visible_[NAVIGATION_UP_BUTTON] = TRUE;
	toolbar_buttons_visible_[NAVIGATION_RIGHT_BUTTON] = toolbar_buttons_visible_[NAVIGATION_DOWN_BUTTON] = TRUE;
	toolbar_buttons_visible_[NAVIGATION_CLOCKWISE_BUTTON] = toolbar_buttons_visible_[NAVIGATION_ANTICLOCKWISE_BUTTON] = TRUE;

	toolbar_button_selection_index_ = -1;
	UpdateButtonInfoInToolbar();
}

void WinPaint::OnResize() {

	for (int i = NAVIGATION_CLOCKWISE_BUTTON; i <= NAVIGATION_DOWN_BUTTON; i++) {
		toolbar_buttons_visible_[i] = FALSE;
	}

	navigation_menu_index_ = 2;

	toolbar_button_selection_index_ = -1;
	UpdateButtonInfoInToolbar();
}

BOOL WinPaint::NavigateShape(BOOL manually, int type, int direction) {

	if (!manually) {
		angle_on_mouse_move_ = shape_->angle();
		current_rect_ = shape_->rectangle(full_image_);
		if (zoom_index_ > ZOOM_OPERATION::FIT) {
			GetRectInWindow(current_rect_);
		}
		if (type == 1) { //Rotate
			if (direction == 1) { //Clockwise
				if (++angle_on_mouse_move_ >= 270) {
					angle_on_mouse_move_ = -90;
				}
			} else if (direction == 2) { //Anticlockwise
				if (--angle_on_mouse_move_ < -90) {
					angle_on_mouse_move_ = 269;
				}
			}
		} else if (type == 2) { //Move
			int offset = (zoom_index_ == ZOOM_OPERATION::FIT) ? 1 : zoom_index_ * 1;
			if (direction == 1) { //Left
				current_rect_.left -= offset;
				current_rect_.right -= offset;
				/*cv::Point2f new_points_cv[4];
				cv::Point2f center_point_cv((float)current_rect_.CenterPoint().x - offset, (float)current_rect_.CenterPoint().y);
				cv::Size2f size_cv((float)current_rect_.Width(), (float)current_rect_.Height());
				cv::RotatedRect rotated_rect_cv(center_point_cv, size_cv, angle_on_mouse_move_);
				rotated_rect_cv.points(new_points_cv);
				current_rect_.left = new_points_cv[1].x;
				current_rect_.right = new_points_cv[2].x;*/
			} else if (direction == 2) { //Up
				current_rect_.top -= offset;
				current_rect_.bottom -= offset;
			} else if (direction == 3) { //Right
				current_rect_.left += offset;
				current_rect_.right += offset;
			} else if (direction == 4) { //Down
				current_rect_.top += offset;
				current_rect_.bottom += offset;
			}
		} else if (type == 3) { //Resize
			int distance_to_move_x = 4;// (zoom_index_ == ZOOM_OPERATION::FIT) ? 4 : zoom_index_ * 4;
			int distance_to_move_y = 4;// (zoom_index_ == ZOOM_OPERATION::FIT) ? 4 : zoom_index_ * 4;
			if (direction == 1) { //Lefttop -
				CPoint down_point = CPoint(selection_points_[2].x, selection_points_[2].y);
				float angle_on_mouse_down = FindAngle(selection_points_[1], down_point);
				float angle = FindAngle(CPoint(down_point.x + distance_to_move_x, down_point.y + distance_to_move_y), down_point);
				float result = (angle - angle_on_mouse_down);
				float fangle = (float)(result * (CV_PI / 180));
				float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
				current_rect_.left = (LONG)(current_rect_.left + ((distance)* cos(fangle)));
				current_rect_.top = (LONG)(current_rect_.top + ((distance)* sin(fangle)));
			} else if (direction == 2) { //Lefttop +
				int distance_to_move_x = -4;// (zoom_index_ == ZOOM_OPERATION::FIT) ? 4 : zoom_index_ * 4;
				int distance_to_move_y = -4;// (zoom_index_ == ZOOM_OPERATION::FIT) ? 4 : zoom_index_ * 4;
				CPoint down_point = CPoint(current_rect_.left, current_rect_.top);
				float angle_on_mouse_down = FindAngle(selection_points_[2], down_point);
				float angle = FindAngle(CPoint(down_point.x - distance_to_move_x, down_point.y - distance_to_move_y), down_point);
				float result = (angle - angle_on_mouse_down);
				float fangle = (float)(result * (CV_PI / 180));
				float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
				current_rect_.left = (LONG)(current_rect_.left + ((distance)* cos(fangle)));
				current_rect_.top = (LONG)(current_rect_.top + ((distance)* sin(fangle)));
			} else if (direction == 3) { //Righttop -
				//current_rect_.right -= offset;
				//current_rect_.top += offset;
			} else if (direction == 4) { //Righttop +
				//current_rect_.right += offset;
				//current_rect_.top -= offset;
			} else if (direction == 5) { //Rightbottom -
				//current_rect_.right -= offset;
				//current_rect_.bottom -= offset;
			} else if (direction == 6) { //Rightbottom +
				//current_rect_.right += offset;
				//current_rect_.bottom += offset;
			} else if (direction == 7) { //Leftbottom -
				//current_rect_.left += offset;
				//current_rect_.bottom -= offset;
			} else if (direction == 8) { //Leftbottom +
				//current_rect_.left -= offset;
				//current_rect_.bottom += offset;
			}
		}
	}
	//if (type != 1) {
	//	// Calibrating co-ordinates when shape is rotated
	//	if (angle_on_mouse_move_ > 0) {
	//		SelectRect(current_rect_, shape_, FALSE);
	//		int midx = 0, midy = 0;
	//		for (int i = 1; i < 5; i++) {
	//			midx += selection_points_[i].x;
	//			midy += selection_points_[i].y;
	//		}
	//		int diffx = (midx / 4) - current_rect_.CenterPoint().x;
	//		int diffy = (midy / 4) - current_rect_.CenterPoint().y;
	//		current_rect_.MoveToXY(current_rect_.left + diffx, current_rect_.top + diffy);
	//	}
	//}
	if (zoom_index_ > ZOOM_OPERATION::FIT) {
		GetRectInImage(current_rect_);
	} else {
		current_rect_ = GetRectWRTImage(current_rect_);
	}

	CRect image_rect = shape_->rectangle(TRUE);
	if (current_rect_.top < 0) {
		current_rect_.top = 0;
		current_rect_.bottom = image_rect.Height();
	}
	if (current_rect_.left < 0) {
		current_rect_.left = 0;
		current_rect_.right = image_rect.Width();
	}
	if (current_rect_.bottom > img_height_) {
		current_rect_.bottom = img_height_ - 1;
		current_rect_.top = current_rect_.bottom - image_rect.Height();
	}
	if (current_rect_.right > img_width_) {
		current_rect_.right = img_width_ - 1;
		current_rect_.left = current_rect_.right - image_rect.Width();
	}

	if (!CheckOverlapping(shape_, shape_->shape_type(), current_rect_)) {
		return FALSE;
	}
	if (!RectInsideBoundingRect(shape_, current_rect_)) {
		return FALSE;
	}

	if (type == 1) {
		shape_->set_angle(angle_on_mouse_move_);
	} else {
		CRect rect_in_window_view = GetRectWRTWindow(current_rect_);
		shape_->set_rectangle(rect_in_window_view, current_rect_);
	}
	RefreshShapes(TRUE);

	SelectRect(current_rect_, shape_, TRUE);

	if (type == 2) { //Move
		if (direction == 1) { //Left
			GetParent()->SendMessageW(WM_WINPAINT_NAVIGATION_LEFT_CLICKED, (WPARAM)this, (LPARAM)shape_);
		} else if (direction == 2) { //Up
			GetParent()->SendMessageW(WM_WINPAINT_NAVIGATION_UP_CLICKED, (WPARAM)this, (LPARAM)shape_);
		} else if (direction == 3) { //Right
			GetParent()->SendMessageW(WM_WINPAINT_NAVIGATION_RIGHT_CLICKED, (WPARAM)this, (LPARAM)shape_);
		} else if (direction == 4) { //Down
			GetParent()->SendMessageW(WM_WINPAINT_NAVIGATION_DOWN_CLICKED, (WPARAM)this, (LPARAM)shape_);
		}
	}

	return TRUE;
}

void WinPaint::DrawImageStoreProgressBar(BOOL redraw) {

	if (!imagestore_text_rect_.IsRectEmpty()) {

		if (redraw) EnterCriticalSection(&cs_for_toolbar_);

		EnterCriticalSection(&cs_for_toolbar_imagestore_);

		CRect image_store_rect = imagestore_text_rect_;
		image_store_rect.DeflateRect(Formation::spacing(), 0, Formation::spacing(), 0);
		toolbar_dc_.FillSolidRect(image_store_rect, LABEL_COLOR);
		int icon_size = Formation::icon_size(Formation::MEDIUM_ICON);
		CRect bar_rect = image_store_rect;
		bar_rect.left += (Formation::control_height() / 2);
		bar_rect.right -= (Formation::control_height() / 2);
		bar_rect.bottom = bar_rect.CenterPoint().y + (icon_size / 2);
		bar_rect.top = bar_rect.bottom - Formation::spacingHalf();
		toolbar_dc_.FillSolidRect(bar_rect, IsWindowEnabled() ? LABEL_LINE_COLOR : DISABLE_COLOR);
		CRect progress_rect = bar_rect;
		if (imagestore_limit_ > 0) {
			progress_rect.right = progress_rect.left + imagestore_value_ * bar_rect.Width() / imagestore_limit_;
		}
		if (progress_rect.right > client_rect_.right) {
			progress_rect.right = client_rect_.right;
		}
		toolbar_dc_.FillSolidRect(progress_rect, BLACK_COLOR);
		CString text;
		text.Format(L"%d/%d %s", imagestore_value_, imagestore_limit_, Language::GetString(IDSTRINGT_IMAGES_S));
		toolbar_dc_.DrawText(text, image_store_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		LeaveCriticalSection(&cs_for_toolbar_imagestore_);

		if (redraw) LeaveCriticalSection(&cs_for_toolbar_);

		if (redraw) InvalidateRect(image_store_rect, FALSE);
	}
}

void WinPaint::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	mouse_move_ = FALSE;

	if (show_captionbar_) {

		clicked_on_captionbar_area_ = FALSE;
		if (captionbar_rect_.PtInRect(point)) {

			clicked_on_captionbar_area_ = TRUE;

			for (int button_index = 0; button_index < (sizeof(captionbar_buttons_rect_) / sizeof(captionbar_buttons_rect_[0])); button_index++) {
				if (captionbar_buttons_rect_[button_index].PtInRect(point)) {
					BOOL button_clicked = FALSE;
					if (button_index == CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON) {
						button_clicked = TRUE;
					} else if (button_index == CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON) {
						button_clicked = TRUE;
					}
					if (button_clicked) {
						captionbar_button_selection_index_ = button_index;
						UpdateButtonInfoInCaptionbar(FALSE);
						InvalidateRect(captionbar_buttons_rect_[button_index], FALSE);
						break;
					}
				}
			}
			return;
		}
	}
	if (show_toolbar_) {

		clicked_on_toolbar_area_ = FALSE;
		toolbar_button_selection_index_ = -1;

		if (toolbar_rect_.PtInRect(point)) {

			clicked_on_toolbar_area_ = TRUE;
			point.y -= toolbar_rect_.top;

			for (int button_index = 0; button_index < (sizeof(toolbar_buttons_rect_) / sizeof(toolbar_buttons_rect_[0])); button_index++) {
				if (toolbar_buttons_visible_[button_index] && !toolbar_buttons_disable_[button_index]) {
					if (toolbar_buttons_rect_[button_index].PtInRect(point)) {
						BOOL button_clicked = FALSE;
						if (button_index == TOOLBAR_BUTTONS::TOOLBAR_SHOW_REGION_BUTTON) {
							button_clicked = TRUE;
						} else if (show_toolbar_region_) {
							if (button_index == TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON) {
								button_clicked = TRUE;
							} else if (button_index >= TOOLBAR_BUTTONS::PAN_BUTTON && button_index <= TOOLBAR_BUTTONS::SAVE_IMAGE) {
								if (toolbar_region_index_ == 0) {
									if (toolbar_region_enable_[TOOLBAR_REGION::REGULAR_BUTTONS]) {
										button_clicked = TRUE;
									}
								}
							} else if (button_index >= TOOLBAR_BUTTONS::NAVIGATION_MENU_BUTTON && button_index <= TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON) {
								if (toolbar_region_index_ == 2) {
									if (toolbar_region_enable_[TOOLBAR_REGION::NAVIGATION_BUTTONS]) {
										button_clicked = TRUE;
									}
								}
							} else if (button_index >= TOOLBAR_BUTTONS::IMAGESTORE_PASS && button_index <= TOOLBAR_BUTTONS::IMAGESTORE_CLEAR) {
								if (toolbar_region_index_ == 3) {
									if (toolbar_region_enable_[TOOLBAR_REGION::IMAGESTORE_BUTTONS]) {
										button_clicked = TRUE;
									}
								}
							}
						}
						if (button_clicked) {
							toolbar_button_selection_index_ = button_index;
							UpdateButtonInfoInToolbar();
							break;
						}
					}
				}
			}
			return;
		}
	}

	CPoint point_on_image = point;
	if (zoom_index_ > ZOOM_OPERATION::FIT) {
		GetPointInImage(point_on_image);
		xy_position_ = point_on_image;
	} else {
		xy_position_ = GetPointWRTImage(point_on_image);
	}
	if (xy_position_.x >= img_width_ || xy_position_.y >= img_height_) {
		ReleaseCapture();
		return;
	}
	UpdatePixelInfoInToolbar();

	ResetParameters();
	down_point_ = point;
	shape_operation_ = WinShape::SHAPE_OPERATION::NONE;

	shape_info_.shape_operation = WinShape::SHAPE_OPERATION::NONE;
	shape_info_.shape_type = WinShape::SHAPE_TYPE::RECTANGLE;
	shape_info_.angle = 0;
	shape_info_.index = -1;
	shape_info_.rect = CRect(0, 0, 0, 0);
	shape_info_.prev_rect = CRect(0, 0, 0, 0);
	shape_info_.tag = L"0";
	shape_info_.data = L"";
	shape_info_.family = 0;
	shape_info_.point = xy_position_;
		
	if (GetOperation() == IMAGE_OPERATION::NONE) {
		GetParent()->SendMessageW(WM_WINPAINT_LBUTTONDOWN, (WPARAM)this, (LPARAM)&shape_info_);
		ReleaseCapture();
		return;
	}

	CRect wndRect;
	GetWindowRect(wndRect);

	CRect clip_rect = wndRect;
	int new_image_width = wndRect.Width();
	int new_image_height = wndRect.Height();
	if (window_image_dimentions_ratio_ == 1 && zoom_index_ == FIT) {
		new_image_width = ((zoom_index_ == FIT) ? img_width_ : img_width_ * zoom_index_);
		new_image_height = ((zoom_index_ == FIT) ? img_height_ : img_height_ * zoom_index_);
	} else if (window_image_dimentions_ratio_ == 3 && zoom_index_ == FIT) {
		new_image_height = img_height_ * wnd_width_ / img_width_;
	} else if (window_image_dimentions_ratio_ == 4 && zoom_index_ == FIT) {
		new_image_width = img_width_ * wnd_height_ / img_height_;
	//} else if (window_image_dimentions_ratio_ == 5 && zoom_index_ == FIT) {
	//	new_image_width = img_width_ * wnd_height_ / img_height_;
	//	if (new_image_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
	//		new_image_height = img_height_ * wnd_width_ / img_width_;
	//		new_image_width = wnd_width_;
	//	}
	}
	clip_rect.right = clip_rect.left + ((wndRect.Width() < new_image_width) ? wndRect.Width() : new_image_width);
	clip_rect.bottom = clip_rect.top + ((wndRect.Height() < new_image_height) ? wndRect.Height() : new_image_height);
	ClipCursor(clip_rect);

	if (GetOperation() == IMAGE_OPERATION::IMAGE_PAN) {
		
		::SetCursor(LoadCursor(NULL, IDC_HAND));

		if (!multiple_image_view_) {
			show_tiny_image_view_ = TRUE;
		}

	} else if (GetOperation() == IMAGE_OPERATION::PICK_COLOR) {

		::SetCursor(LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_COLOR_PICKER)));

	} else if (GetOperation() == IMAGE_OPERATION::ERASER) {

		::SetCursor(LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_ERASER)));

	} else if (GetOperation() == IMAGE_OPERATION::DRAW_FREE_HAND) {

		::SetCursor(LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_PENCIL)));

		free_hand_points_window_.clear();
		free_hand_points_window_.push_back(point);
		Invalidate(FALSE);

	} else {

		selected_point_index_ = -1;

		//IF SHAPE IS SELECTED
		if (shape_ != NULL) {
			for (int i = 0; i < selected_point_count_; i++) {
				CRect rectTemp(selection_points_[i].x - (Formation::spacing4()), selection_points_[i].y - (Formation::spacing4()), selection_points_[i].x + (Formation::spacing4()), selection_points_[i].y + (Formation::spacing4()));
				if (i == 0) rectTemp.DeflateRect(Formation::spacing2(), Formation::spacing2(), Formation::spacing2(), Formation::spacing2()); //Rotation
				if (rectTemp.PtInRect(point)) {
					if (i == 0 && !shape_->lock(WinShape::ROTATE)) {
						selected_point_index_ = i;
						shape_operation_ = WinShape::ROTATE;
						break;
					} else if ((i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 6) && !shape_->lock(WinShape::RESIZE)) {
						selected_point_index_ = i;
						shape_operation_ = WinShape::RESIZE;
						break;
					}
				}
			}
			if (selected_point_index_ != -1) {

				current_rect_ = shape_->rectangle(full_image_);
				if (zoom_index_ > ZOOM_OPERATION::FIT) {
					GetRectInWindow(current_rect_);
				}
				previous_rect_ = current_rect_;
				angle_on_mouse_move_ = shape_->angle();
				center_point_ = current_rect_.CenterPoint();

				if (selected_point_index_ == 0) { //ROTATION
					::SetCursor(LoadCursor(g_resource_handle, MAKEINTRESOURCE(IDC_ROTATE)));
					angle_on_mouse_down_ = FindAngle(selection_points_[0], current_rect_.CenterPoint()) - shape_->angle();
				} else if (selected_point_index_ == 1) { //RESIZE FROM LEFT-TOP
					::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
					rect_corner_for_resize_.SetPoint(current_rect_.left, current_rect_.top);
					angle_on_mouse_down_ = FindAngle(selection_points_[2], down_point_);
				} else if (selected_point_index_ == 2 && shape_->lock(WinShape::RESIZE) == FALSE) { //RESIZE FROM RIGHT-TOP
					::SetCursor(LoadCursor(NULL, IDC_SIZENESW));
					rect_corner_for_resize_.SetPoint(current_rect_.right, current_rect_.top);
					angle_on_mouse_down_ = FindAngle(selection_points_[1], down_point_);
				} else if (selected_point_index_ == 3 && shape_->lock(WinShape::RESIZE) == FALSE) { //RESIZE FROM RIGHT-BOTTOM
					::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
					rect_corner_for_resize_.SetPoint(current_rect_.right, current_rect_.bottom);
					angle_on_mouse_down_ = FindAngle(selection_points_[4], down_point_);
				} else if (selected_point_index_ == 4 && shape_->lock(WinShape::RESIZE) == FALSE) { //RESIZE FROM LEFT-BOTTOM
					::SetCursor(LoadCursor(NULL, IDC_SIZENESW));
					rect_corner_for_resize_.SetPoint(current_rect_.left, current_rect_.bottom);
					angle_on_mouse_down_ = FindAngle(selection_points_[3], down_point_);
				} else if (selected_point_index_ == 5 && shape_->lock(WinShape::RESIZE) == FALSE) { //RESIZE FROM LEFT
					::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					rect_corner_for_resize_.SetPoint(current_rect_.left, current_rect_.CenterPoint().y);
					angle_on_mouse_down_ = FindAngle(selection_points_[5], down_point_);
				} else if (selected_point_index_ == 6 && shape_->lock(WinShape::RESIZE) == FALSE) { //RESIZE FROM RIGHT
					::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					rect_corner_for_resize_.SetPoint(current_rect_.right, current_rect_.CenterPoint().y);
					angle_on_mouse_down_ = FindAngle(selection_points_[6], down_point_);
				}
			}
		}

		//IF SHAPE IS NOT SELECTED THEN FIND SHAPE
		if (selected_point_index_ == -1) {

			//EnterCriticalSection(&cs_for_shapes_);

			for (int index = (int)shapes_.GetSize() - 1; index >= 0; index--) {
				WinShape* shape = shapes_.GetAt(index);
				if (shape->shape_type() != WinShape::LINE && shape->shape_type() != WinShape::POLYGON && shape->shape_type() != WinShape::TEXT) {
					if (shape->visible() && (shape->lock(WinShape::SELECT) == FALSE || shape->lock(WinShape::MOVE) == FALSE)) {
						CRect rect = shape->rectangle(full_image_);

						int total_points = 4;
						cv::Point2f new_points_cv[4];
						cv::Point2f center_point_cv((float)rect.CenterPoint().x, (float)rect.CenterPoint().y);
						cv::Size2f size_cv((float)rect.Width(), (float)rect.Height());
						cv::RotatedRect rotated_rect_cv(center_point_cv, size_cv, shape->angle());
						rotated_rect_cv.points(new_points_cv);
						int j = total_points - 1;
						bool exist = false;
						for (int i = 0; i < total_points; i++) {
							if ((new_points_cv[i].y < point_on_image.y && new_points_cv[j].y >= point_on_image.y || new_points_cv[j].y < point_on_image.y && new_points_cv[i].y >= point_on_image.y)
								&& (new_points_cv[i].x <= point_on_image.x || new_points_cv[j].x <= point_on_image.x)) {
								exist ^= (new_points_cv[i].x + (point_on_image.y - new_points_cv[i].y) / (new_points_cv[j].y - new_points_cv[i].y) * (new_points_cv[j].x - new_points_cv[i].x) < point_on_image.x);
							}
							j = i;
						}

						if (exist) {
						
							shape_ = shape;

							if (shape->lock(WinShape::SELECT) == FALSE) {
								shape_operation_ = WinShape::SELECT;
								//For DrawShape function
								if (shape->shape_type() == WinShape::SHAPE_TYPE::CIRCLE) { image_operation_ = WinPaint::IMAGE_OPERATION::DRAW_CIRCLE; }
								if (shape->shape_type() == WinShape::SHAPE_TYPE::ROUNDRECT) { image_operation_ = WinPaint::IMAGE_OPERATION::DRAW_ROUNDRECT; }
								if (shape->shape_type() == WinShape::SHAPE_TYPE::ELLIPSE) { image_operation_ = WinPaint::IMAGE_OPERATION::DRAW_ELLIPSE; }
								if (shape->shape_type() == WinShape::SHAPE_TYPE::RECTANGLE) { image_operation_ = WinPaint::IMAGE_OPERATION::DRAW_RECT; }
								if (shape->shape_type() == WinShape::SHAPE_TYPE::LINE) { image_operation_ = WinPaint::IMAGE_OPERATION::DRAW_LINE; }
								//::SetCursor(select_cursor_);
							}
							//::SetCursor(LoadCursor(NULL, IDC_SIZEALL));

							if (shape_selection_ == WinShape::SHAPE_SELECTION::SINGLE) {
								for (int index = 0; index < shapes_.GetSize(); index++) {
									WinShape* temp_shape = shapes_.GetAt(index);
									if (shape == temp_shape) {
										temp_shape->set_selected(TRUE);
									} else {
										temp_shape->set_selected(FALSE);
									}
								}
							} else if (shape_selection_ == WinShape::SHAPE_SELECTION::MULTIPLE) {
								if (tag_for_shape_selection_ == L"") {
									tag_for_shape_selection_ = shape->tag();
								}
								if (tag_for_shape_selection_ == shape->tag()) {
									shape->set_selected(!shape->selected());
								} else {
									shape_ = NULL;
								}
								//ALLOW TO SELECT SHAPE WITH ANOTHER TAG
								BOOL all_shapes_are_deselected = TRUE;
								for (int index = 0; index < shapes_.GetSize(); index++) {
									WinShape* temp_shape = shapes_.GetAt(index);
									if (temp_shape->selected()) {
										all_shapes_are_deselected = FALSE;
										break;
									}
								}
								if (all_shapes_are_deselected) {
									tag_for_shape_selection_ = L"";
								}
							} else if (shape_selection_ == WinShape::SHAPE_SELECTION::ALL) {
								if (tag_for_shape_selection_ != shape->tag()) {
									tag_for_shape_selection_ = shape->tag();
								}
								for (int index = 0; index < shapes_.GetSize(); index++) {
									WinShape* temp_shape = shapes_.GetAt(index);
									if (temp_shape->family() == shape->family() && tag_for_shape_selection_ == temp_shape->tag()) {
										temp_shape->set_selected(TRUE);
									} else {
										temp_shape->set_selected(FALSE);
									}
								}
							}
							RefreshShapes(TRUE);

							if (shape_ != NULL) {
								if (zoom_index_ > ZOOM_OPERATION::FIT) {
									GetRectInWindow(rect);
								}
								current_rect_ = previous_rect_ = rect;
								angle_on_mouse_move_ = shape_->angle();
								center_point_ = current_rect_.CenterPoint();

								SelectRect(current_rect_, shape_, TRUE);
							}
							break;
						}
					}
				}
			}
			//LeaveCriticalSection(&cs_for_shapes_);
		}

		if (shape_ != NULL) {
			shape_info_.prev_rect = shape_->rectangle(TRUE);
		}

		//DRAW NEW SHAPE
		if (shape_operation_ == WinShape::NONE) {
			if (shape_ != NULL) {
				if (shape_selection_ == WinShape::SHAPE_SELECTION::SINGLE) {
					shape_->set_selected(FALSE);
					RefreshShapes(TRUE);
				}
			}
			if (lock_draw_operation_ == FALSE) {
				::SetCursor(LoadCursor(NULL, IDC_CROSS));
				shape_operation_ = WinShape::DRAW;
			}
			shape_ = NULL;
			SelectNone(true);
		}
	}

	GetParent()->SendMessageW(WM_WINPAINT_LBUTTONDOWN, (WPARAM)this, (LPARAM)&shape_info_);
}

void WinPaint::OnMouseMove(UINT nFlags, CPoint point)
{
	if (show_captionbar_ && clicked_on_captionbar_area_) {
		return;
	}

	if (show_toolbar_ && clicked_on_toolbar_area_) {
		return;
	}
	
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON && (down_point_.x != -1 || down_point_.y != -1)) {

		CPoint point_on_image = point;
		if (zoom_index_ > ZOOM_OPERATION::FIT) {
			GetPointInImage(point_on_image);
			xy_position_ = point_on_image;
		} else {
			xy_position_ = GetPointWRTImage(point_on_image);
		}

		if (GetOperation() == IMAGE_OPERATION::NONE) {

			if (xy_position_.x >= img_width_ || xy_position_.y >= img_height_) {
				ReleaseCapture();
				return;
			}
			UpdatePixelInfoInToolbar();

		} else if (GetOperation() == IMAGE_OPERATION::IMAGE_PAN) {
			
			if (full_image_) {
				int distance_to_move_x = (down_point_.x - point.x);
				int distance_to_move_y = (down_point_.y - point.y);

				if (zoom_index_ > ZOOM_OPERATION::FIT) {
					distance_to_move_x /= zoom_index_;
					distance_to_move_y /= zoom_index_;
				}

				pan_point_.x += distance_to_move_x;
				pan_point_.y += distance_to_move_y;

				pan_point_.x = (pan_point_.x > (img_width_ - img_display_width_)) ? (img_width_ - img_display_width_) : pan_point_.x;
				pan_point_.x = pan_point_.x < 0 ? 0 : pan_point_.x;
				pan_point_.y = (pan_point_.y >(img_height_ - img_display_height_)) ? (img_height_ - img_display_height_) : pan_point_.y;
				pan_point_.y = pan_point_.y < 0 ? 0 : pan_point_.y;

				down_point_ = point;

				Invalidate(FALSE);
			}

		} else if (GetOperation() == IMAGE_OPERATION::ERASER) {

		} else if (GetOperation() == IMAGE_OPERATION::DRAW_FREE_HAND) {

			mouse_move_ = TRUE;

			free_hand_points_window_.push_back(point);
			Invalidate(FALSE);
		
		} else { //if (GetOperation() == OP_SELECT_SHAPE) {

			mouse_move_ = TRUE;

			if (shape_operation_ == WinShape::ROTATE) {

				float angle = FindAngle(point, current_rect_.CenterPoint());
				angle_on_mouse_move_ = (angle - angle_on_mouse_down_);

			} else if (shape_operation_ == WinShape::RESIZE) {

				int distance_to_move_x = (point.x - down_point_.x);
				int distance_to_move_y = (point.y - down_point_.y);
				
				if (selected_point_index_ == 1) {

					float angle = FindAngle(point, down_point_);
					float result = (angle - angle_on_mouse_down_);
					float fangle = (float)(result * (CV_PI / 180));
					float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
					current_rect_.left = (LONG)(rect_corner_for_resize_.x + ((distance)* cos(fangle)));
					current_rect_.top = (LONG)(rect_corner_for_resize_.y + ((distance)* sin(fangle)));

				} else if (selected_point_index_ == 2) {

					float angle = FindAngle(down_point_, point);
					float result = (angle - angle_on_mouse_down_);
					float fangle = (float)(result * (CV_PI / 180));
					float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
					current_rect_.right = (LONG)(rect_corner_for_resize_.x + ((distance)* cos(fangle)));
					current_rect_.top = (LONG)(rect_corner_for_resize_.y + ((distance)* sin(fangle)));

				} else if (selected_point_index_ == 3) {
					
					float angle = FindAngle(down_point_, point);
					float result = (angle - angle_on_mouse_down_);
					float fangle = (float)(result * (CV_PI / 180));
					float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
					current_rect_.right = (LONG)(rect_corner_for_resize_.x + ((distance)* cos(fangle)));
					current_rect_.bottom = (LONG)(rect_corner_for_resize_.y + ((distance)* sin(fangle)));

				} else if (selected_point_index_ == 4) {

					float angle = FindAngle(point, down_point_);
					float result = (angle - angle_on_mouse_down_);
					float fangle = (float)(result * (CV_PI / 180));
					float distance = (float)(sqrt(distance_to_move_x * distance_to_move_x + distance_to_move_y * distance_to_move_y));
					current_rect_.left = (LONG)(rect_corner_for_resize_.x + ((distance)* cos(fangle)));
					current_rect_.bottom = (LONG)(rect_corner_for_resize_.y + ((distance)* sin(fangle)));

				} else if (selected_point_index_ == 5) {

					current_rect_.left = (LONG)(rect_corner_for_resize_.x + distance_to_move_x);

				} else if (selected_point_index_ == 6) {

					current_rect_.right = (LONG)(rect_corner_for_resize_.x + distance_to_move_x);
				}

			} else if (shape_operation_ == WinShape::SELECT) {

				int distance_to_move_x = (point.x - down_point_.x);
				int distance_to_move_y = (point.y - down_point_.y);
				if (abs(distance_to_move_x) > 3 || abs(distance_to_move_y) > 3) {
					if (shape_ != NULL) {
						if (shape_->lock(WinShape::MOVE) == FALSE) {
							shape_operation_ = WinShape::MOVE;
							//::SetCursor(move_cursor_);
						}
					}
				}

			} else if (shape_operation_ == WinShape::MOVE) {

				int distance_to_move_x = (point.x - down_point_.x);
				int distance_to_move_y = (point.y - down_point_.y);

				current_rect_.left = previous_rect_.left + distance_to_move_x;
				current_rect_.top = previous_rect_.top + distance_to_move_y;
				current_rect_.right = current_rect_.left + previous_rect_.Width();
				current_rect_.bottom = current_rect_.top + previous_rect_.Height();

				center_point_ = current_rect_.CenterPoint();

			} else if (shape_operation_ == WinShape::DRAW) {

				current_rect_.SetRect(down_point_, point);
				current_rect_.NormalizeRect(); //TO SWAP LEFT/RIGHT OR TOP/BOTTOM
			}

			Invalidate(FALSE);
		}
	
		GetParent()->SendMessageW(WM_WINPAINT_MOUSEMOVE, (WPARAM)this, (LPARAM)(&point_on_image));
	}
}

void WinPaint::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	
	mouse_move_ = FALSE;

	shape_info_.status = TRUE;
	shape_info_.point = point;
	if (zoom_index_ > ZOOM_OPERATION::FIT) {
		GetPointInImage(shape_info_.point);
	} else {
		shape_info_.point = GetPointWRTImage(shape_info_.point);
	}

	ClipCursor(NULL);
	
	if (show_captionbar_ && clicked_on_captionbar_area_) {

		if (captionbar_button_selection_index_ == CAPTIONBAR_BUTTONS::CAPTIONLBAR_RIGHT_TEXT_BUTTON) { //To toggle right text

			right_text_button_status_ = !right_text_button_status_;
			
		} else if (captionbar_button_selection_index_ == CAPTIONBAR_BUTTONS::CAPTIONBAR_SHOW_REGION_BUTTON) { //Show Region button

			++show_captionbar_region_;
			if (show_captionbar_region_ == 3) {
				show_captionbar_region_ = 0;
			}
		}

		captionbar_button_selection_index_ = -1;
		UpdateButtonInfoInCaptionbar(TRUE);
		return;
	}

	if (show_toolbar_ && clicked_on_toolbar_area_) {

		if (toolbar_button_selection_index_ == -1) {
			ResetParameters();
			return;
		}

		if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::PAN_BUTTON) { //Pan

			if (!panning_on_ && (zoom_index_ == ZOOM_OPERATION::FIT || (zoom_index_ == ZOOM_OPERATION::Z100 && window_image_dimentions_ratio_ == 2))) {
				toolbar_button_selection_index_ = -1;
				UpdateButtonInfoInToolbar();
				return;
			}

			panning_on_ = !panning_on_;
			previous_image_operation_ = (image_operation_ == IMAGE_PAN) ? previous_image_operation_ : image_operation_;
			if (panning_on_) {
				SetOperation(IMAGE_PAN);
			} else {
				SetOperation(previous_image_operation_);
			}
			SelectAllShapes(FALSE, FALSE);

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::ZOOM_SHAPE_BUTTON) { //ZoomShape

			ZoomShape();

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::ZOOM_IN_BUTTON) { //ZoomIn

			if ((zoom_index_) < ZOOM_OPERATION::Z500) {
				++zoom_index_;

				if (zoom_index_ == ZOOM_OPERATION::Z100) {
						
					pan_point_.x += (img_width_ - wnd_width_) / 2;
					pan_point_.y += (img_height_ - wnd_height_) / 2;
					if (window_image_dimentions_ratio_ == 3) {
						pan_point_.y = 0;
					} else if (window_image_dimentions_ratio_ == 4 || window_image_dimentions_ratio_ == 5) {
						pan_point_.x = 0;
					}
																									
				} else {

					img_display_width_ = wnd_width_ / zoom_index_;
					img_display_height_ = wnd_height_ / zoom_index_;
					int prev_width = wnd_width_ / (zoom_index_ - 1);
					int prev_height = wnd_height_ / (zoom_index_ - 1);
				
					pan_point_.x = pan_point_.x + (abs(img_display_width_ - prev_width) / 2);
					pan_point_.y = pan_point_.y + (abs(img_display_height_ - prev_height) / 2);

					pan_point_.x = (pan_point_.x > (img_width_ - img_display_width_)) ? (img_width_ - img_display_width_) : pan_point_.x;
					pan_point_.x = pan_point_.x < 0 ? 0 : pan_point_.x;
					pan_point_.y = (pan_point_.y >(img_height_ - img_display_height_)) ? (img_height_ - img_display_height_) : pan_point_.y;
					pan_point_.y = pan_point_.y < 0 ? 0 : pan_point_.y;
				}

				full_image_ = zoom_index_ > ZOOM_OPERATION::FIT ? TRUE : FALSE;

				SelectAllShapes(FALSE, FALSE);

				Update();
				SelectNone(false);
			}

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::ZOOM_OUT_BUTTON) { //ZoomOut

			if (zoom_index_ > ZOOM_OPERATION::FIT) {
				--zoom_index_;
				if (window_image_dimentions_ratio_ == 1 || window_image_dimentions_ratio_ == 2) {
					if (zoom_index_ == ZOOM_OPERATION::FIT) {
						zoom_index_ = ZOOM_OPERATION::Z100;
					}
				}

				if (zoom_index_ == ZOOM_OPERATION::FIT) {
						
					pan_point_.x = 0;
					pan_point_.y = 0;

				} else {

					img_display_width_ = wnd_width_ / zoom_index_;
					img_display_height_ = wnd_height_ / zoom_index_;
					int prev_width = wnd_width_ / (zoom_index_ + 1);
					int prev_height = wnd_height_ / (zoom_index_ + 1);

					pan_point_.x = pan_point_.x - (abs(img_display_width_ - prev_width) / 2);
					pan_point_.y = pan_point_.y - (abs(img_display_height_ - prev_height) / 2);

					pan_point_.x = (pan_point_.x > (img_width_ - img_display_width_)) ? (img_width_ - img_display_width_) : pan_point_.x;
					pan_point_.x = pan_point_.x < 0 ? 0 : pan_point_.x;
					pan_point_.y = (pan_point_.y > (img_height_ - img_display_height_)) ? (img_height_ - img_display_height_) : pan_point_.y;
					pan_point_.y = pan_point_.y < 0 ? 0 : pan_point_.y;
				}

				full_image_ = zoom_index_ > ZOOM_OPERATION::FIT ? TRUE : FALSE;
					
				if (panning_on_ && (zoom_index_ == ZOOM_OPERATION::FIT || (zoom_index_ == ZOOM_OPERATION::Z100 && window_image_dimentions_ratio_ == 2))) {
					panning_on_ = FALSE;
					SetOperation(previous_image_operation_);
				}

				SelectAllShapes(FALSE, FALSE);

				Update();
				SelectNone(false);
			}

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::FIT_TO_IMAGE_BUTTON) { //Fit

			zoom_index_ = ZOOM_OPERATION::FIT;
			full_image_ = FALSE;
			pan_point_.x = 0;
			pan_point_.y = 0;
			img_display_width_ = wnd_width_;
			img_display_height_ = wnd_height_;
			
			if (panning_on_) {
				panning_on_ = FALSE;
				SetOperation(previous_image_operation_);
			}
			SelectAllShapes(FALSE, FALSE);

			Update();
			SelectNone(false);

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::CENTER_LINE) { //Cross line

			center_line_ = !center_line_;
			CrossLine();

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::SHOW_SHAPES_BUTTON) { //Show/Hide Shapes

			ShowHideShapes(!show_hide_shapes_);

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::SOURCE_VIEW_BUTTON) { //Source/binary view

			if (allow_to_change_image_view_) {
				if (!ChangeImage(!image_index_)) {
					return;
				}
			}

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::MULTIPLE_VIEW_BUTTON) { //Multiple/single view

			SwitchViews(!multiple_image_view_);

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::LOCK_IMAGE) { //Lock/Unlock image

			if (lock_image_) {
				LockImage(FALSE);
			}

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::SAVE_IMAGE) { //Save image

			SaveImage();

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::TOOLBAR_SHOW_REGION_BUTTON) { //Show Region button

			show_toolbar_region_ = !show_toolbar_region_;
			if (show_toolbar_region_) {
				ToolbarRegionButtonClicked();
			}

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::TOOLBAR_REGION_BUTTON) { //Region button

			++toolbar_region_index_;
			ToolbarRegionButtonClicked();

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_MENU_BUTTON) { //Navigation Menu

			CRect rect = client_rect_;
			GetWindowRect(&rect);
			rect.left += toolbar_buttons_rect_[toolbar_button_selection_index_].left;
			rect.top += (toolbar_rect_.top - Formation::control_height() * 3 - Formation::spacing4());
			winpaintmenu_navigation_.TrackPopupMenu(TPM_LEFTALIGN, rect.left, rect.top, this, rect);

		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_ANTICLOCKWISE_BUTTON) { //Navigation Rotate
			if (shape_ != NULL && shape_->lock(WinShape::ROTATE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 1, 2);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_CLOCKWISE_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::ROTATE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 1, 1);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_LEFT_BUTTON) { //Navigation Move
			if (shape_ != NULL && shape_->lock(WinShape::MOVE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 2, 1);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_UP_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::MOVE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 2, 2);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_RIGHT_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::MOVE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 2, 3);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_DOWN_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::MOVE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 2, 4);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::IMAGESTORE_PASS) {
			imagestore_pass_on_ = !imagestore_pass_on_;
			GetParent()->SendMessageW(WM_WINPAINT_IMAGESTORE_PASS_CLICKED, (WPARAM)imagestore_pass_on_, (LPARAM)0);
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::IMAGESTORE_FAIL) {
			imagestore_fail_on_ = !imagestore_fail_on_;
			GetParent()->SendMessageW(WM_WINPAINT_IMAGESTORE_FAIL_CLICKED, (WPARAM)imagestore_fail_on_, (LPARAM)0);
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::IMAGESTORE_GRAB) {
			imagestore_grab_on_ = !imagestore_grab_on_;
			GetParent()->SendMessageW(WM_WINPAINT_IMAGESTORE_GRAB_CLICKED, (WPARAM)imagestore_grab_on_, (LPARAM)0);
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::IMAGESTORE_SHARE) {
			if (imagestore_value_ > 0) {
				GetParent()->SendMessageW(WM_WINPAINT_IMAGESTORE_SHARE_CLICKED, (WPARAM)0, (LPARAM)0);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::IMAGESTORE_CLEAR) {
			if (imagestore_value_ > 0) {
				GetParent()->SendMessageW(WM_WINPAINT_IMAGESTORE_CLEAR_CLICKED, (WPARAM)0, (LPARAM)0);
			}
		}
                  /*else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_LEFTTOPMINUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 1);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_LEFTTOPPLUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 2);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_RIGHTTOPMINUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 3);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_RIGHTTOPPLUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 4);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_RIGHTBOTTOMMINUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 5);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_RIGHTBOTTOMPLUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 6);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_LEFTBOTTOMMINUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 7);
			}
		} else if (toolbar_button_selection_index_ == TOOLBAR_BUTTONS::NAVIGATION_LEFTBOTTOMPLUS_BUTTON) {
			if (shape_ != NULL && shape_->lock(WinShape::RESIZE) == FALSE) {
				shape_info_.status = NavigateShape(FALSE, 3, 8);
			}
		}*/

		toolbar_button_selection_index_ = -1;
		UpdateButtonInfoInToolbar();
		return;
	}

	if (down_point_.x == -1 || down_point_.y == -1) {
		return;
	}

	if (GetOperation() == IMAGE_OPERATION::IMAGE_PAN) {

		show_tiny_image_view_ = FALSE;

		ResetParameters();
		return;

	} else if (GetOperation() == IMAGE_OPERATION::PICK_COLOR) {
		
		//

	} else if (GetOperation() == IMAGE_OPERATION::ERASER) {

	} else if (GetOperation() == IMAGE_OPERATION::DRAW_FREE_HAND) {

		free_hand_points_window_.push_back(point);
		free_hand_points_window_.push_back(down_point_);

		free_hand_points_image_.clear();
		for (int i = 0; i < free_hand_points_window_.size(); i++) {
			CPoint point(free_hand_points_window_[i].x, free_hand_points_window_[i].y);
			if (zoom_index_ > ZOOM_OPERATION::FIT) {
				GetPointInImage(point);
			} else {
				point = GetPointWRTImage(point);
			}
			free_hand_points_image_.push_back(cv::Point(point.x, point.y));
		}
		free_hand_points_window_.clear();
		
		Invalidate(FALSE);

	} else if (GetOperation() != IMAGE_OPERATION::NONE) {

		if (current_rect_.Width() < (Formation::spacing() + ((Formation::spacing3() / 2) * zoom_index_)) || current_rect_.Height() < (Formation::spacing() + ((Formation::spacing3() / 2) * zoom_index_))) {

			if (shape_operation_ == WinShape::DRAW) {
				SelectNone(false);
			}
			if (shape_operation_ == WinShape::RESIZE || shape_operation_ == WinShape::DRAW) {
				shape_operation_ = WinShape::NONE;
				ResetParameters();
				return;
			}
		}

		if (GetOperation() == DRAW_CIRCLE) {
			if (current_rect_.Height() > current_rect_.Width()) {
				current_rect_.bottom = current_rect_.top + current_rect_.Width();
			} else if (current_rect_.Height() < current_rect_.Width()) {
				current_rect_.right = current_rect_.left + current_rect_.Height();
			}
		}

		if (shape_operation_ == WinShape::DRAW) {
			
			if (zoom_index_ > ZOOM_OPERATION::FIT) {
				GetRectInImage(current_rect_); //To find rect in respect of image
			} else {
				current_rect_ = GetRectWRTImage(current_rect_);
			}	

			WinShape::SHAPE_TYPE shape_type = WinShape::RECTANGLE;
			if (GetOperation() == DRAW_CIRCLE) {
				shape_type = WinShape::CIRCLE;
			} else if (GetOperation() == DRAW_ROUNDRECT) {
				shape_type = WinShape::ROUNDRECT;
			} else if (GetOperation() == DRAW_ELLIPSE) {
				shape_type = WinShape::ELLIPSE;
			}

			if (!CheckOverlapping(NULL, shape_type, current_rect_)) {
				shape_operation_ = WinShape::NONE;
				ResetParameters();
				return;
			}
			if (!RectInsideBoundingRect(NULL, current_rect_)) {
				shape_operation_ = WinShape::NONE;
				ResetParameters();
				return;
			}

			if (GetOperation() == DRAW_CIRCLE) {
				DrawCircle(current_rect_, line_width_, line_style_, line_color_, fill_color_, 0, TRUE);
			} else if (GetOperation() == DRAW_ROUNDRECT) {
				DrawRoundRect(current_rect_, line_width_, line_style_, 1.0, 1.0, line_color_, fill_color_, 0, TRUE);
			} else if (GetOperation() == DRAW_ELLIPSE) {
				DrawEllipse(current_rect_, line_width_, line_style_, line_color_, fill_color_, 0, TRUE);
			} else if (GetOperation() == DRAW_RECT) {
				DrawRect(current_rect_, line_width_, line_style_, line_color_, fill_color_, 0, TRUE);
			}

			shape_ = shapes_.GetAt(shapes_.GetSize() - 1);

		} else if (shape_operation_ == WinShape::ROTATE && shape_ != NULL) {

			shape_info_.status = NavigateShape(TRUE, 1, 0);

		} else if ((shape_operation_ == WinShape::MOVE || shape_operation_ == WinShape::RESIZE)  && shape_ != NULL) {

			if (abs(down_point_.x - point.x) < Formation::spacing() / 2 && abs(down_point_.y - point.y) < Formation::spacing() / 2) { //If clicked to select a shape or slightly moved/resized
				shape_operation_ = WinShape::NONE;
				ResetParameters();
				return;
			}

			shape_info_.status = NavigateShape(TRUE, 2, 0);
		}

		if (shape_ != NULL) {

			current_rect_ = shape_->rectangle(full_image_);
			if (zoom_index_ > ZOOM_OPERATION::FIT) {
				GetRectInWindow(current_rect_);
			}
			SelectRect(current_rect_, shape_, FALSE);

			shape_info_.family = shape_->family();
			shape_info_.tag = shape_->tag();
			shape_info_.data = shape_->data();
			shape_info_.shape_type = shape_->shape_type();
			shape_info_.angle = shape_->angle();
			shape_info_.index = shape_->index();
			shape_info_.rect = shape_->rectangle(TRUE);
			shape_info_.shape_operation = shape_operation_;
		}
	}

	ResetParameters();

	GetParent()->SendMessageW(WM_WINPAINT_LBUTTONUP, (WPARAM)&point, (LPARAM)(&shape_info_));
}

void WinPaint::ShowPixelInfo(CPoint point) {

	RefreshShapes(FALSE);

	if (!toolbar_buttons_rect_[TOOLBAR_REGION_BUTTON].IsRectEmpty()) {
		return;
	}

	int PixInfoWidth = 172; /*Width of pix info box*/
	int PixInfoHeight = 62;  /*Height of pix info box*/
	int CursorOffsetX = 25;  /*Pointer Offset for X*/
	int CursorOffsetY = 25;  /*Pointer Offset for Y*/

	CPoint position;
	position.x = xy_position_.x - PixInfoWidth - CursorOffsetX;
	position.y = xy_position_.y - PixInfoHeight - CursorOffsetY;

	if (position.x < 0) {
		position.x = point.x;
	}
	if (position.y < 0) {
		position.y = point.y;
	}

	pixel_info_.SetPoint(position, GetPixel(image_dc_.GetSafeHdc(), xy_position_.x, xy_position_.y));

	BYTE B = 0, G = 0, R = 0;
	if (view_index_ < int(winview_.GetSize())) {
		if (winview_[view_index_].buffer[0] != NULL) {
			if (image_type_ == COLOR_24BIT) {
				unsigned char* selected_pixel = winview_[view_index_].buffer[0]->data + (xy_position_.y * img_width_ * 3) + (xy_position_.x * 3);
				B = *(selected_pixel);
				G = *(selected_pixel + 1);
				R = *(selected_pixel + 2);
			}
			else {
				unsigned char* selected_pixel = winview_[view_index_].buffer[0]->data + (xy_position_.y * img_width_) + (xy_position_.x);
				B = G = R = *(selected_pixel);
			}
		}
	}
	pixel_info_.ShowPixelInfo(shape_dc_.GetSafeHdc(), R, G, B);

	Invalidate(FALSE);
}

void WinPaint::ResetParameters() {

	current_rect_.SetRectEmpty();
	previous_rect_.SetRectEmpty();
	angle_on_mouse_down_ = 0;
	angle_on_mouse_move_ = 0;
	center_point_.SetPoint(0, 0);
	down_point_.SetPoint(-1, -1);

	Invalidate(FALSE);
	
	::SetCursor(LoadCursor(NULL, IDC_ARROW));
}

BOOL WinPaint::RectInsideBoundingRect(WinShape* shape, CRect& rect) {

	CRect bounding_rect = bounding_rect_;
	float angle = 0;
	if (shape != NULL) {
		bounding_rect = shape->bounding_rect();
		angle = shape->angle();
	}

	CRect rect_in_image_view = rect;
	if (angle > 0) {
		cv::RotatedRect rr(cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y), cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()), angle);
		rect_in_image_view.left = rr.boundingRect().x;
		rect_in_image_view.top = rr.boundingRect().y;
		rect_in_image_view.right = rr.boundingRect().x + rr.boundingRect().width;
		rect_in_image_view.bottom = rr.boundingRect().y + rr.boundingRect().height;
	}
	/*if (rect_in_image_view.top < 0 || rect_in_image_view.left < 0 || rect_in_image_view.bottom >= img_height_ || rect_in_image_view.right >= img_width_) {
		return FALSE;
	}*/
	/*if (angle == 0) {
		if (rect_in_image_view.top < 0) {
			rect_in_image_view.top = 0;
			rect_in_image_view.bottom = rect.Height();
		}
		if (rect_in_image_view.left < 0) {
			rect_in_image_view.left = 0;
			rect_in_image_view.right = rect.Width();
		}
		if (rect_in_image_view.bottom > img_height_) {
			rect_in_image_view.bottom = img_height_;
			rect_in_image_view.top = rect_in_image_view.bottom - rect.Height();
		}
		if (rect_in_image_view.right > img_width_) {
			rect_in_image_view.right = img_width_;
			rect_in_image_view.left = rect_in_image_view.right - rect.Width();
		}
		rect = rect_in_image_view;
	}*/

	CRect temp_rect;
	temp_rect.IntersectRect(bounding_rect, rect_in_image_view);
	if (temp_rect != rect_in_image_view){
		return FALSE;
	}
	return TRUE;
}

cv::Rect DrawRectangleOnBuffer(cv::Mat &mask, const cv::Point2f &cg, const cv::Size2f &size, float angle) {

	bool draw_on = true;
	bool solid_fill = true;
	cv::Scalar color = cv::Scalar::all(255);
	int line_thickness = 2;

	cv::RotatedRect rr(cg, size, angle);
	cv::Point2f pts[4];
	rr.points(pts);

	std::vector<cv::Point> fill_pts;
	long left = 0xfffffff, top = 0xfffffff, right = 0, bottom = 0;
	for (int i = 0; i < 4; i++) {
		left = left < (long)pts[i].x ? left : (long)pts[i].x;
		top = top < (long)pts[i].y ? top : (long)pts[i].y;
		right = right >(long)pts[i].x ? right : (long)pts[i].x;
		bottom = bottom >(long)pts[i].y ? bottom : (long)pts[i].y;
		fill_pts.push_back(cv::Point((long)pts[i].x, (long)pts[i].y));
	}

	if (left >= 0 && top >= 0 && right < mask.cols && bottom < mask.rows) {
		cv::Rect fit_rect(left, top, (right - left), (bottom - top));
		fit_rect.width -= (fit_rect.width % 4);
		fit_rect.height -= (fit_rect.height % 4);

		std::vector<std::vector<cv::Point>> multi_fill_pts;
		multi_fill_pts.push_back(fill_pts);
		if (draw_on) {
			if (solid_fill) {
				cv::drawContours(mask, multi_fill_pts, 0, color, cv::FILLED);
			}
			else if (!solid_fill) {
				cv::drawContours(mask, multi_fill_pts, 0, color, line_thickness);
			}
		}
		return fit_rect;
	}
	else {
		return cv::Rect();
	}
}


cv::Rect DrawCircleOnBuffer(cv::Mat &mask, const cv::Point2f &cg, const cv::Size2f &size) {

	bool draw_on = true;
	bool solid_fill = true;
	cv::Scalar color = cv::Scalar::all(255);
	int line_thickness = 2;

	long radius = (long)size.width / 2 < (long)size.height / 2 ? (long)size.width / 2 : (long)size.height / 2;

	if (radius == 0) {
		return cv::Rect();
	}

	long left = long(cg.x - radius);
	long top = long(cg.y - radius);
	long right = long(cg.x + radius);
	long bottom = long(cg.y + radius);

	if (left >= 0 && top >= 0 && right < mask.cols && bottom < mask.rows) {
		cv::Rect fit_rect(left, top, (right - left), (bottom - top));
		fit_rect.width -= (fit_rect.width % 4);
		fit_rect.height -= (fit_rect.height % 4);
		if (draw_on) {
			if (solid_fill) {
				cv::circle(mask, cg, radius, color, cv::FILLED);
			}
			else if (!solid_fill) {
				cv::circle(mask, cg, radius, color, line_thickness);
			}
		}
		return fit_rect;
	}
	else {
		return cv::Rect();
	}
}

cv::Rect DrawEllipseOnBuffer(cv::Mat &mask, const cv::Point2f &cg, const cv::Size2f &size, const float angle) {

	bool draw_on = true;
	bool solid_fill = true;
	cv::Scalar color = cv::Scalar::all(255);
	int line_thickness = 2;

	float a = size.width / 2;
	float b = size.height / 2;
	cv::Point pts[4];

	float phi = angle * float(CV_PI / 180);
	float tan_angle = tan(phi);
	float t = atan((-b*tan_angle) / a);
	float x = cg.x + a*cos(t)*cos(phi) - b*sin(t)*sin(phi);
	float y = cg.y + b*sin(t)*cos(phi) + a*cos(t)*sin(phi);
	pts[0] = cv::Point(cvRound(x), cvRound(y));

	t = atan((b*(1 / tan(phi))) / a);
	x = cg.x + a*cos(t)*cos(phi) - b*sin(t)*sin(phi);
	y = cg.y + b*sin(t)*cos(phi) + a*cos(t)*sin(phi);
	pts[1] = cv::Point(cvRound(x), cvRound(y));

	phi += (float)CV_PI;
	tan_angle = tan(phi);
	t = atan((-b*tan_angle) / a);
	x = cg.x + a*cos(t)*cos(phi) - b*sin(t)*sin(phi);
	y = cg.y + b*sin(t)*cos(phi) + a*cos(t)*sin(phi);
	pts[2] = cv::Point(cvRound(x), cvRound(y));

	t = atan((b*(1 / tan(phi))) / a);
	x = cg.x + a*cos(t)*cos(phi) - b*sin(t)*sin(phi);
	y = cg.y + b*sin(t)*cos(phi) + a*cos(t)*sin(phi);
	pts[3] = cv::Point(cvRound(x), cvRound(y));

	long left = 0xfffffff, top = 0xfffffff, right = 0, bottom = 0;
	for (int i = 0; i < 4; i++) {
		left = left < pts[i].x ? left : pts[i].x;
		top = top < pts[i].y ? top : pts[i].y;
		right = right > pts[i].x ? right : pts[i].x;
		bottom = bottom > pts[i].y ? bottom : pts[i].y;
	}

	if (left >= 0 && top >= 0 && right < mask.cols && bottom < mask.rows) {
		cv::Rect fit_rect(left, top, (right - left), (bottom - top));
		fit_rect.width -= (fit_rect.width % 4);
		fit_rect.height -= (fit_rect.height % 4);
		cv::RotatedRect rr(cg, size, angle);
		if (draw_on) {
			if (solid_fill) {
				cv::ellipse(mask, rr, color, cv::FILLED);
			}
			else if (!solid_fill) {
				cv::ellipse(mask, rr, color, line_thickness);
			}

		}
		return fit_rect;
	}
	else {
		return cv::Rect();
	}
}

cv::Rect DrawRoundedRectOnBuffer(cv::Mat &mask, const cv::Point2f &cg, const cv::Size2f &size, const float angle) {

	bool draw_on = true;
	bool solid_fill = true;
	cv::Scalar color = cv::Scalar::all(255);
	int line_thickness = 2;

	cv::Size2f internal_size = size;
	float internal_angle = angle;
	if (size.height > size.width) {
		internal_size.width = size.height;
		internal_size.height = size.width;
		internal_angle = 90 + angle;
	}
	const float phi = internal_angle * float(CV_PI / 180);
	const float corner_radius = internal_size.height / 2;
	cv::RotatedRect rr(cg, internal_size, internal_angle);
	cv::Point2f pts[4];
	rr.points(pts);

	cv::Point2f cg_top((pts[0].x + pts[1].x) / 2, (pts[0].y + pts[1].y) / 2);
	cv::Point2f cg_bottom((pts[2].x + pts[3].x) / 2, (pts[2].y + pts[3].y) / 2);

	cg_top.x = cg_top.x + (corner_radius * cos(phi));
	cg_top.y = cg_top.y + (corner_radius * sin(phi));
	cg_bottom.x = cg_bottom.x + (corner_radius * cos(phi + (float)CV_PI));
	cg_bottom.y = cg_bottom.y + (corner_radius * sin(phi + (float)CV_PI));

	if (internal_angle > 360 || internal_angle < -360) {
		internal_angle = 0;
	}

	float start_angle = internal_angle + 90;
	float end_angle = start_angle + 180;

	std::vector<cv::Point> poly_pts;
	long left = 0xfffffff, top = 0xfffffff, right = 0, bottom = 0;
	for (float i = start_angle; i < end_angle; i += 1) {
		float phi_temp = (float)i * float(CV_PI / 180);
		cv::Point pt1;
		pt1.x = int(cg_top.x + (corner_radius * cos(phi_temp)));
		pt1.y = int(cg_top.y + (corner_radius * sin(phi_temp)));

		if (pt1.x >= 0 && pt1.y >= 0 && pt1.x < mask.cols && pt1.y < mask.rows) {
			left = left < pt1.x ? left : pt1.x;
			top = top < pt1.y ? top : pt1.y;
			right = right > pt1.x ? right : pt1.x;
			bottom = bottom > pt1.y ? bottom : pt1.y;

			poly_pts.push_back(pt1);
		}
	}

	for (float i = start_angle; i < end_angle; i += 1) {
		float phi_temp = (float)i * float(CV_PI / 180);
		cv::Point pt1;
		phi_temp += (float)CV_PI;
		pt1.x = int(cg_bottom.x + (corner_radius * cos(phi_temp)));
		pt1.y = int(cg_bottom.y + (corner_radius * sin(phi_temp)));

		if (pt1.x >= 0 && pt1.y >= 0 && pt1.x < mask.cols && pt1.y < mask.rows) {
			left = left < pt1.x ? left : pt1.x;
			top = top < pt1.y ? top : pt1.y;
			right = right > pt1.x ? right : pt1.x;
			bottom = bottom > pt1.y ? bottom : pt1.y;

			poly_pts.push_back(pt1);
		}
	}

	if (left >= 0 && top >= 0 && right < mask.cols && bottom < mask.rows) {
		cv::Rect fit_rect(left, top, (right - left), (bottom - top));
		fit_rect.width -= (fit_rect.width % 4);
		fit_rect.height -= (fit_rect.height % 4);
		std::vector<std::vector<cv::Point>> multi_poly_pts;
		multi_poly_pts.push_back(poly_pts);
		if (draw_on) {
			if (solid_fill) {
				cv::drawContours(mask, multi_poly_pts, 0, color, cv::FILLED);
			}
			else if (!solid_fill) {
				cv::drawContours(mask, multi_poly_pts, 0, color, line_thickness);
			}
		}
		return fit_rect;
	}
	else {
		return cv::Rect();
	}
}

BOOL WinPaint::CheckOverlapping(WinShape* shape, WinShape::SHAPE_TYPE shape_type, CRect rect) {

	int family = family_;
	CString tag = tag_;
	int overlap_area = overlap_area_;
	int index = -1; 
	float angle = 0;

	if (shape != NULL) {
		family = shape->family();
		tag = shape->tag();
		overlap_area = shape->overlap_area();
		index = shape->index();
		angle = shape->angle();
	}

	if (overlap_area == 100) {
		return TRUE; //not overlap
	}

	cv::Mat mask(winview_[0].buffer[0]->size(), CV_8UC1);
	mask = cv::Scalar(0);

	CRect rect_in_image_view = rect;
	if (rect_in_image_view.top < 0) {
		rect_in_image_view.top = 0;
		rect_in_image_view.bottom = rect.Height();
	}
	if (rect_in_image_view.left < 0) {
		rect_in_image_view.left = 0;
		rect_in_image_view.right = rect.Width();
	}
	if (rect_in_image_view.bottom >= img_height_) {
		rect_in_image_view.bottom = img_height_ - 1;
	}
	if (rect_in_image_view.right >= img_width_) {
		rect_in_image_view.right = img_width_ - 1;
	}
	cv::Rect cavity_rect_cv;
	if (shape_type == WinShape::SHAPE_TYPE::RECTANGLE) {
		cavity_rect_cv = DrawRectangleOnBuffer(mask, cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y),
			cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()), angle);
	} else if (shape_type == WinShape::SHAPE_TYPE::ROUNDRECT) {
		cavity_rect_cv = DrawRoundedRectOnBuffer(mask, cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y),
			cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()), angle);
	} else if (shape_type == WinShape::SHAPE_TYPE::ELLIPSE) {
		cavity_rect_cv = DrawEllipseOnBuffer(mask, cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y),
			cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()), angle);
	} else if (shape_type == WinShape::SHAPE_TYPE::CIRCLE) {
		cavity_rect_cv = DrawCircleOnBuffer(mask, cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y),
			cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()));
	}
	int selected_cavity_pixel_count = cv::countNonZero(mask);

	cv::Mat mask2(mask.size(), CV_8UC1);
	mask2 = cv::Scalar(0);
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->family() == family && shape->tag() == tag && shape->index() != index && !shape->new_born()) {
			CRect rect(shape->rectangle(TRUE));
			if (shape->shape_type() == WinShape::SHAPE_TYPE::RECTANGLE) {
				cavity_rect_cv = DrawRectangleOnBuffer(mask2, cv::Point2f((float)rect.CenterPoint().x, (float)rect.CenterPoint().y), cv::Size2f((float)rect.Width(), (float)rect.Height()), angle);
			} else if (shape->shape_type() == WinShape::SHAPE_TYPE::ROUNDRECT) {
				cavity_rect_cv = DrawRoundedRectOnBuffer(mask2, cv::Point2f((float)rect.CenterPoint().x, (float)rect.CenterPoint().y), cv::Size2f((float)rect.Width(), (float)rect.Height()), angle);
			} else if (shape->shape_type() == WinShape::SHAPE_TYPE::ELLIPSE) {
				cavity_rect_cv = DrawEllipseOnBuffer(mask2, cv::Point2f((float)rect.CenterPoint().x, (float)rect.CenterPoint().y), cv::Size2f((float)rect.Width(), (float)rect.Height()), angle);
			} else if (shape->shape_type() == WinShape::SHAPE_TYPE::CIRCLE) {
				cavity_rect_cv = DrawCircleOnBuffer(mask2, cv::Point2f((float)rect.CenterPoint().x, (float)rect.CenterPoint().y), cv::Size2f((float)rect.Width(), (float)rect.Height()));
			}
		}
	}
	int rest_of_cavity_pixel_count = cv::countNonZero(mask2);

	int sum_pixel_count = selected_cavity_pixel_count + rest_of_cavity_pixel_count;

	cv::bitwise_or(mask, mask2, mask);
	int total_pixel_count = cv::countNonZero(mask);

	mask.release();
	mask2.release();

	int pixel_count = abs(total_pixel_count - sum_pixel_count);
	if (pixel_count > ((selected_cavity_pixel_count * overlap_area) / 100)) {
		return FALSE; //overlap
	} else {
		return TRUE; //not overlap
	}

	/*cv::RotatedRect round_rect1(cv::Point2f((float)rect_in_image_view.CenterPoint().x, (float)rect_in_image_view.CenterPoint().y), cv::Size2f((float)rect_in_image_view.Width(), (float)rect_in_image_view.Height()), angle);
	for (int i = 0; i < shapes_.GetSize(); i++) {
		WinShape* shape = shapes_.GetAt(i);
		if (shape->tag() == tag && shape->index() != index && !shape->new_born()) {
			CRect rect(shape->rectangle(TRUE));
			cv::RotatedRect round_rect2(cv::Point2f((float)rect.CenterPoint().x, (float)rect.CenterPoint().y), cv::Size2f((float)rect.Width(), (float)rect.Height()), angle);
			cv::Mat output;
			if (cv::rotatedRectangleIntersection(round_rect1, round_rect2, output) != cv::RectanglesIntersectTypes::INTERSECT_NONE) {
				output.release();
				return FALSE;
			}
			output.release();
		}
	}
	return TRUE;*/
}

void WinPaint::GetRectInWindow(CRect& rect) {

	rect.OffsetRect(-pan_point_);

	int width = rect.Width();
	int height = rect.Height();
	rect.left *= zoom_index_;
	rect.top *= zoom_index_;
	rect.right *= zoom_index_;
	rect.bottom *= zoom_index_;
	/*rect.right = rect.left + width;
	rect.bottom = rect.top + height;*/
}

void WinPaint::GetRectInImage(CRect& rect) {

	int width = rect.Width();
	int height = rect.Height();
	rect.left /= zoom_index_;
	rect.top /= zoom_index_;
	rect.right /= zoom_index_;
	rect.bottom /= zoom_index_;
	/*rect.right = rect.left + width;
	rect.bottom = rect.top + height;*/

	rect.OffsetRect(pan_point_);
}

void WinPaint::GetPointInWindow(CPoint& point) {

	point.Offset(-pan_point_);
	point.x *= zoom_index_;
	point.y *= zoom_index_;
}

void WinPaint::GetPointInImage(CPoint& point) {

	point.x /= zoom_index_;
	point.y /= zoom_index_;
	point.Offset(pan_point_);
}

CRect WinPaint::GetRectWRTWindow(const CRect& rect)
{
	double dblTemp = 0.0;
	CRect rectTemp(rect);
	double width_ratio_wnd = 0;
	double height_ratio_wnd = 0;
	if (window_image_dimentions_ratio_ == 3) {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)(img_height_ * wnd_width_ / img_width_) / img_height_;
	} else if (window_image_dimentions_ratio_ == 4) {
		width_ratio_wnd = (double)(img_width_ * wnd_height_ / img_height_) / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	} else if (window_image_dimentions_ratio_ == 5) {
		int dest_width = img_width_ * wnd_height_ / img_height_;
		int dest_height = wnd_height_;
		if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
			dest_height = img_height_ * wnd_width_ / img_width_;
			dest_width = wnd_width_;
		}
		width_ratio_wnd = (double)dest_width / img_width_;
		height_ratio_wnd = (double)dest_height / img_height_;
	} else {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	}
	dblTemp = rect.left * width_ratio_wnd;
	rectTemp.left = LONG((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.top * height_ratio_wnd;
	rectTemp.top = LONG((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.right * width_ratio_wnd;
	rectTemp.right = LONG((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.bottom * height_ratio_wnd;
	rectTemp.bottom = LONG((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));

	return rectTemp;
}

CRect WinPaint::GetRectWRTWinViewWindow(const CRect& winview_rect, const CRect& rect, BOOL only_lefttop)
{
	double dblTemp = 0.0;
	CRect rectTemp(rect);
	double width_ratio_wnd = (double)winview_rect.Width() / aoi_for_multipleview_.Width();
	double height_ratio_wnd = (double)winview_rect.Height() / aoi_for_multipleview_.Height();
	rectTemp.left -= aoi_for_multipleview_.left;
	rectTemp.top -= aoi_for_multipleview_.top;
	if (!only_lefttop) {
		rectTemp.right -= aoi_for_multipleview_.left;
		rectTemp.bottom -= aoi_for_multipleview_.top;
	}
	dblTemp = rectTemp.left * width_ratio_wnd;
	rectTemp.left = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rectTemp.top * height_ratio_wnd;
	rectTemp.top = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	if (!only_lefttop) {
		dblTemp = rectTemp.right * width_ratio_wnd;
		rectTemp.right = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
		dblTemp = rectTemp.bottom * height_ratio_wnd;
		rectTemp.bottom = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	}
	rectTemp.left += winview_rect.left;
	rectTemp.top += winview_rect.top;
	if (!only_lefttop) {
		rectTemp.right += winview_rect.left;
		rectTemp.bottom += winview_rect.top;
	} else {
		rectTemp.right = rectTemp.left + rect.Width();
		rectTemp.bottom = rectTemp.top + rect.Height();
	}
	return rectTemp;
}

CRect WinPaint::GetRectWRTImage(const CRect& rect)
{
	double dblTemp = 0.0;
	CRect rectTemp(rect);
	double width_ratio_img = 0;
	double height_ratio_img = 0;
	if (window_image_dimentions_ratio_ == 3) {
		width_ratio_img = (double)img_width_ / wnd_width_;
		height_ratio_img = (double)img_height_ / (img_height_ * wnd_width_ / img_width_);
	} else if (window_image_dimentions_ratio_ == 4) {
		width_ratio_img = (double)img_width_ / (img_width_ * wnd_height_ / img_height_);
		height_ratio_img = (double)img_height_ / wnd_height_;
	} else if (window_image_dimentions_ratio_ == 5) {
		int dest_width = img_width_ * wnd_height_ / img_height_;
		int dest_height = wnd_height_;
		if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
			dest_height = img_height_ * wnd_width_ / img_width_;
			dest_width = wnd_width_;
		}
		width_ratio_img = (double)img_width_ / dest_width;
		height_ratio_img = (double)img_height_ / dest_height;
	} else {
		width_ratio_img = (double)img_width_ / wnd_width_;
		height_ratio_img = (double)img_height_ / wnd_height_;
	}
	dblTemp = rect.left * width_ratio_img;
	rectTemp.left = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.top * height_ratio_img;
	rectTemp.top = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.right * width_ratio_img;
	rectTemp.right = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = rect.bottom * height_ratio_img;
	rectTemp.bottom = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));

	return rectTemp;
}

CPoint WinPaint::GetPointWRTWindow(const CPoint& point)
{
	double dblTemp = 0.0;
	CPoint pointTemp(point);
	double width_ratio_wnd = 0;
	double height_ratio_wnd = 0;
	if (window_image_dimentions_ratio_ == 3) {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)(img_height_ * wnd_width_ / img_width_) / img_height_;
	} else if (window_image_dimentions_ratio_ == 4) {
		width_ratio_wnd = (double)(img_width_ * wnd_height_ / img_height_) / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	} else if (window_image_dimentions_ratio_ == 5) {
		int dest_width = img_width_ * wnd_height_ / img_height_;
		int dest_height = wnd_height_;
		if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
			dest_height = img_height_ * wnd_width_ / img_width_;
			dest_width = wnd_width_;
		}
		width_ratio_wnd = (double)dest_width / img_width_;
		height_ratio_wnd = (double)dest_height / img_height_;
	} else {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	}
	dblTemp = point.x * width_ratio_wnd;
	pointTemp.x = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = point.y * height_ratio_wnd;
	pointTemp.y = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	return pointTemp;
}

CPoint WinPaint::GetPointWRTImage(const CPoint& point)
{
	double dblTemp = 0.0;
	CPoint pointTemp(point);
	double width_ratio_img = 0;
	double height_ratio_img = 0;
	if (window_image_dimentions_ratio_ == 3) {
		width_ratio_img = (double)img_width_ / wnd_width_;
		height_ratio_img = (double)img_height_ / (img_height_ * wnd_width_ / img_width_);
	} else if (window_image_dimentions_ratio_ == 4) {
		width_ratio_img = (double)img_width_ / (img_width_ * wnd_height_ / img_height_);
		height_ratio_img = (double)img_height_ / wnd_height_;
	} else if (window_image_dimentions_ratio_ == 5) {
		int dest_width = img_width_ * wnd_height_ / img_height_;
		int dest_height = wnd_height_;
		if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
			dest_height = img_height_ * wnd_width_ / img_width_;
			dest_width = wnd_width_;
		}
		width_ratio_img = (double)img_width_ / dest_width;
		height_ratio_img = (double)img_height_ / dest_height;
	} else {
		width_ratio_img = (double)img_width_ / wnd_width_;
		height_ratio_img = (double)img_height_ / wnd_height_;
	}
	dblTemp = point.x * width_ratio_img;
	pointTemp.x = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	dblTemp = point.y * height_ratio_img;
	pointTemp.y = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	return pointTemp;
}

int WinPaint::GetIntWRTWindow(const int& integer)
{
	double dblTemp = 0.0;
	int intTemp = integer;
	double width_ratio_wnd = 0;
	double height_ratio_wnd = 0;
	if (window_image_dimentions_ratio_ == 3) {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)(img_height_ * wnd_width_ / img_width_) / img_height_;
	} else if (window_image_dimentions_ratio_ == 4) {
		width_ratio_wnd = (double)(img_width_ * wnd_height_ / img_height_) / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	} else if (window_image_dimentions_ratio_ == 5) {
		int dest_width = img_width_ * wnd_height_ / img_height_;
		int dest_height = wnd_height_;
		if (dest_width > wnd_width_) { //If calculated image width is greater than window width then adjust the image height accordingly
			dest_height = img_height_ * wnd_width_ / img_width_;
			dest_width = wnd_width_;
		}
		width_ratio_wnd = (double)dest_width / img_width_;
		height_ratio_wnd = (double)dest_height / img_height_;
	} else {
		width_ratio_wnd = (double)wnd_width_ / img_width_;
		height_ratio_wnd = (double)wnd_height_ / img_height_;
	}
	dblTemp = integer * width_ratio_wnd;
	intTemp = (LONG)((dblTemp - floor(dblTemp)) < 0.5 ? floor(dblTemp) : ceil(dblTemp));
	return intTemp;
}

CRect WinPaint::GetShapeRectWnd(int index)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect rect(0, 0, 0, 0);
	if (index < shapes_.GetSize()) {
		rect = shapes_.GetAt(index)->rectangle(FALSE);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return rect;
}

CRect WinPaint::GetShapeRectImg()
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect rect(0, 0, 0, 0);
	if (shapes_.GetSize() > 0) {
		rect = shapes_.GetAt(shapes_.GetSize() - 1)->rectangle(TRUE);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return rect;
}

CRect WinPaint::GetShapeRectImg(int index)
{
	EnterCriticalSection(&cs_for_shapes_);

	CRect rect(0, 0, 0, 0);
	if (index < shapes_.GetSize()) {
		rect = shapes_.GetAt(index)->rectangle(TRUE);
	}

	LeaveCriticalSection(&cs_for_shapes_);

	return rect;
}

int WinPaint::GetShapeAllPointsWnd(CPoint* point)
{
	for ( int i = 0 ; i < selected_point_count_ ; i++ ) {
		point[i] = selection_points_[i];
	}
	return selected_point_count_;
}

int WinPaint::GetShapeAllPointsImg(CPoint* point)
{
	for ( int i = 0 ; i < selected_point_count_ ; i++ ) {
		point[i] = GetPointWRTImage(selection_points_[i]);
	}
	return selected_point_count_;
}

void WinPaint::SetImageStoreLimit(int limit) {

	imagestore_limit_ = limit;
	if (toolbar_region_index_ == 3) {
		UpdateButtonInfoInToolbar();
	}
}

void WinPaint::SetImageStoreValue(int value) {

	imagestore_value_ = value;
	if (toolbar_region_index_ == 3) {
		DrawImageStoreProgressBar(TRUE);
		//UpdateButtonInfoInToolbar();
	}
}

BOOL WinPaint::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CStatic::PreTranslateMessage(pMsg);
}

void WinPaint::OnEnable(BOOL bEnable)
{
	if (bEnable == FALSE) {
		SetDefaultView(FALSE);
	}
	//CStatic::OnEnable(bEnable);
}