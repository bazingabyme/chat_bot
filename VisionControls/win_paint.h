#pragma once

#include "win_shape.h"
#include "keyboard_dlg.h"

#ifndef INCLUDE_WINPAINT
#define INCLUDE_WINPAINT  __declspec(dllimport)
#endif //INCLUDE_WINPAINT

//#define WM_WINPAINT_REFRESH						WM_USER + 416
//#define WM_WINPAINT_REFRESHIMAGE				WM_USER + 417
//#define WM_WINPAINT_UPDATE						WM_USER + 418
//#define WM_WINPAINT_UPDATEIMAGE					WM_USER + 419
#define WM_WINPAINT_LBUTTONDOWN					WM_USER + 420
#define WM_WINPAINT_MOUSEMOVE					WM_USER + 421
#define WM_WINPAINT_LBUTTONUP					WM_USER + 422
//#define WM_WINPAINT_UNLOCK_UPDATE				WM_USER + 423
//#define WM_WINPAINT_VIEW_INDEX_CHANGED			WM_USER + 424
#define WM_WINPAINT_NAVIGATION_LEFT_CLICKED		WM_USER + 431
#define WM_WINPAINT_NAVIGATION_RIGHT_CLICKED	WM_USER + 432
#define WM_WINPAINT_NAVIGATION_UP_CLICKED		WM_USER + 433
#define WM_WINPAINT_NAVIGATION_DOWN_CLICKED		WM_USER + 434
#define WM_WINPAINT_IMAGESTORE_PASS_CLICKED		WM_USER + 454
#define WM_WINPAINT_IMAGESTORE_FAIL_CLICKED		WM_USER + 455
#define WM_WINPAINT_IMAGESTORE_GRAB_CLICKED		WM_USER + 456
#define WM_WINPAINT_IMAGESTORE_SHARE_CLICKED	WM_USER + 457
#define WM_WINPAINT_IMAGESTORE_CLEAR_CLICKED	WM_USER + 458

// WinPaintMenu

class WinPaintMenu : public CMenu
{
public:
	WinPaintMenu();
	virtual ~WinPaintMenu();
	void Destroy();
	void SetFont(CFont* font);
	void SetTextAlignment(int alignment);
	void SetMenuItemHeight(int menu_item_height);
	void SetMenuItemWidth(int menu_item_width);
	CArray<HICON>& GetMenuItemIconList() { return icon_list_; }
	void AddMenuItemIcon(HICON icon);
	void ChangeToOwnerDraw(WinPaintMenu* menuctrl);

private:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	CArray<HICON> icon_list_;
	CFont* font_;
	int alignment_;
	int menu_item_width_;
	int menu_item_height_;
};

// WinPaint

class INCLUDE_WINPAINT WinPaint : public CStatic
{
	DECLARE_DYNAMIC(WinPaint)

public:
	CRect aoi_for_multipleview_;
	BOOL toptobottom_view_;

	struct ShapeInfo {
		int family;
		CString tag;
		CString data;
		WinShape::SHAPE_TYPE shape_type;
		CRect prev_rect;
		CRect rect;
		float angle;
		int index;
		int shape_operation;
		CPoint point;
		BOOL status;
	};

	enum IMAGE_TYPE { GRAY_8BIT, COLOR_24BIT };
	enum IMAGE_OPERATION { NONE, CHANGE_IMAGE, IMAGE_PAN, PICK_COLOR, SELECT_SHAPE, ERASER, DRAW_FREE_HAND, DRAW_CIRCLE, DRAW_ROUNDRECT, DRAW_ELLIPSE, DRAW_RECT, DRAW_LINE, DRAW_TEXT };
	enum ZOOM_OPERATION { FIT, Z100, Z200, Z300, Z400, Z500 };
	enum CAPTIONBAR_BUTTONS { CAPTIONBAR_SHOW_REGION_BUTTON, CAPTIONLBAR_RIGHT_TEXT_BUTTON, CAPTIONLBAR_RIGHT_TEXT, CAPTIONLBAR_LEFT_TEXT, CAPTIONLBAR_COLOR_CODE }; //From Right To Left
	enum TOOLBAR_BUTTONS { TOOLBAR_SHOW_REGION_BUTTON, TOOLBAR_REGION_BUTTON, PAN_BUTTON, ZOOM_SHAPE_BUTTON, ZOOM_IN_BUTTON, ZOOM_OUT_BUTTON, FIT_TO_IMAGE_BUTTON, CENTER_LINE, SOURCE_VIEW_BUTTON, SHOW_SHAPES_BUTTON, MULTIPLE_VIEW_BUTTON, LOCK_IMAGE, SAVE_IMAGE, NAVIGATION_MENU_BUTTON, NAVIGATION_CLOCKWISE_BUTTON, NAVIGATION_ANTICLOCKWISE_BUTTON, NAVIGATION_LEFT_BUTTON, NAVIGATION_RIGHT_BUTTON, NAVIGATION_UP_BUTTON, NAVIGATION_DOWN_BUTTON, IMAGESTORE_PASS, IMAGESTORE_FAIL, IMAGESTORE_GRAB, IMAGESTORE_SHARE, IMAGESTORE_CLEAR }; //From Right To Left
	enum TOOLBAR_REGION { REGULAR_BUTTONS, PIXEL_INFORMATION, NAVIGATION_BUTTONS, IMAGESTORE_BUTTONS };

	WinPaint(void);
	virtual ~WinPaint(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	bool CreateDisplay(long image_width, long image_height, BOOL color_image = TRUE, int show_toolbar = 1, int show_captionbar = 0); //0-Hide, 1-Show Fixed, 2-Show Toggle
	void ModifyDisplay(long image_width = 0, long image_height = 0, int show_toolbar = 1, int show_captionbar = 0);
	void ClearDisplay(BOOL redraw);
	void ClearImageForMultipleView(BOOL redraw);

	void Destroy();
	void SetParamForMultipleView(BOOL toptobottom_view);
	void SetAOIForMultipleView(CRect aoi);
	void RegisterViews(cv::Mat* src_buffer = NULL, cv::Mat* bin_buffer = NULL);
	void RegisterViews(CArray<cv::Mat*> buffers, CRect aoi);
	void RegisterViews(CArray<cv::Mat*> &src_buffers, CArray<cv::Mat*> &bin_buffers, CRect aoi_rect);
	void SwitchViews(BOOL multiple_image_view);
	BOOL ZoomToOriginal();
	void ZoomShape();
	void ZoomShape(WinShape* shape);
	void Zoom(int zoom_index, CPoint pan_point = CPoint(0,0));
	void RefreshShapes(BOOL redraw);
	void SetDefaultView(BOOL redraw);

	int GetImageWidth() { return img_width_; }
	int GetImageHeight() { return img_height_; }

	void SetShapeProperties(int line_width, int line_style, float width_arc, float height_arc, COLORREF line_color, COLORREF fill_color, CString tag);
	BOOL ChangeImage(int index);
	void SetShapeSelectionSize(int size) { shape_selection_size_ = size; }
	void AllowToChangeImageView(BOOL allow) { allow_to_change_image_view_ = allow; }
	void ShapeSelection(WinShape::SHAPE_SELECTION shape_selection);
	int GetZoomIndex() { return zoom_index_; }
	CPoint GetPanPoint() { return pan_point_; }
	void SetXYPosition(CPoint position) { xy_position_ = position; }
	CPoint GetXYPosition() { return xy_position_; }
	void SetCaptionbarColor(COLORREF captionbar_color) { captionbar_color_ = captionbar_color; }
	void SetToolbarColor(COLORREF toolbar_color) { toolbar_color_ = toolbar_color; }
	void DisableZoomOperation(BOOL disable);
	void ShowZoomOperation(BOOL visible);
	void ShowCaptionbarButton(CAPTIONBAR_BUTTONS button, BOOL visible);
	void DisableToolbarButton(TOOLBAR_BUTTONS button, BOOL disable);
	void ShowToolbarButton(TOOLBAR_BUTTONS button, BOOL visible, DWORD update = 1); //0-None, 1-Refresh Toolbar, 2-Update Toolbar
	void EnableToolbarRegion(TOOLBAR_REGION toolbar_region, BOOL enable);
	void SetToolbarRegion(TOOLBAR_REGION toolbar_region);
	void SwitchToolbarRegion();
	cv::Mat* GetCurrentDisplayedBuffer() {
		if (int(winview_.GetSize()) > 0) {
			return winview_[0].buffer[0];
		}
		return NULL;
	}
	void SetSaveImagePath(CString path) { save_image_path_ = path; }
	void SetLeftText(CString text, COLORREF text_color, BOOL redraw);
	void SetRightText(CString text, COLORREF text_color, BOOL check_on, BOOL redraw);
	void SetColorCode(COLORREF color, BOOL redraw);
	COLORREF GetColorCode();

	void ChangeTag(int old_tag, int new_tag);
	void ChangeTag(CString old_tag, CString new_tag);
	void set_family(int family) { family_ = family; }
	int family() { return family_; }
	void set_tag(int tag, int overlap_area = 0, CRect bounding_rect = CRect()) { 
		tag_.Format(L"%d", tag); 
		overlap_area_ = overlap_area;
		if (bounding_rect.IsRectEmpty()) {
			bounding_rect_.SetRect(0, 0, img_width_, img_height_);
		} else {
			bounding_rect_ = bounding_rect;
		}
	}
	void set_tag(CString tag, int overlap_area = 0, CRect bounding_rect = CRect()) {
		tag_ = tag; 
		overlap_area_ = overlap_area;
		if (bounding_rect.IsRectEmpty()) {
			bounding_rect_.SetRect(0, 0, img_width_, img_height_);
		} else {
			bounding_rect_ = bounding_rect;
		}
	}
	CString tag() { return tag_; }
	void set_data(int data) {
		data_.Format(L"%d", data);
	}
	void set_data(CString data) {
		data_ = data;
	}
	CString data() { return data_; }

	void SetOperation(IMAGE_OPERATION image_operation, BOOL change_prev_operation = FALSE);
	const IMAGE_OPERATION GetOperation() { return image_operation_; }
	const IMAGE_OPERATION GetPreviousOperation() { return previous_image_operation_; }
	const BOOL IsDrawOperationLocked() { return lock_draw_operation_; }
	inline BOOL IsBinaryImageView() { return image_index_; }
	inline BOOL IsMultipleImageView() { return multiple_image_view_; }
	inline BOOL IsFullImage() { return full_image_; }
	inline BOOL IsWndImgRatioSame() { return window_image_dimentions_ratio_ == 2; }
	void ShowHideShapes(BOOL show);
	inline BOOL IsShapesOn() { return show_hide_shapes_; }
	void LockImage(BOOL lock);
	BOOL IsImageLocked() { return lock_image_; }
	void SaveImage();
	void PanImage(BOOL pan);
	BOOL IsPanningOn() { return panning_on_; }
	void set_multipleview(BOOL multipleview);
	void set_view_index(int view_index);
	int view_index() { return view_index_; }

	BOOL RefreshImage(cv::Mat* buffer);
	BOOL Refresh(cv::Mat* buffer);
	BOOL UpdateImage(cv::Mat* src_image = NULL, cv::Mat* bin_image = NULL, int view_index = 0, BOOL update_multiple_view = FALSE);
	BOOL Update(cv::Mat* src_image = NULL, cv::Mat* bin_image = NULL, int view_index = 0, BOOL update_multiple_view = FALSE);

	int GetAllRect(CString tag, std::vector<CRect>& rect_list);
	int GetSelectedShapesIndex(std::vector<int>& index_list);
	inline std::vector<cv::Point>& GetFreeHandPoints() { return free_hand_points_image_; }

	int GetShapesCount();
	int GetShapesCount(int family);
	int GetShapesCount(int family, int tag);
	int GetShapesCount(int family, CString tag);

	void LockDrawOperation(BOOL lock);
	void LockUnlockAllShapes(int family, BOOL lock);
	void LockAllShapes();
	void LockAllShapes(int family);
	void LockShapes(int family, int tag);
	void LockShapes(int family, CString tag);
	void LockShapes(int tag, WinShape::SHAPE_OPERATION operation, BOOL lock);
	void LockShapes(CString tag, WinShape::SHAPE_OPERATION operation, BOOL lock);
	void LockShapes(int family, int tag, WinShape::SHAPE_OPERATION operation, BOOL lock);
	void LockShapes(int family, CString tag, WinShape::SHAPE_OPERATION operation, BOOL lock);

	void DeleteAllShapes(BOOL redraw);
	void DeleteAllShapes(int family, BOOL redraw);
	void DeleteShapes(int family, int tag, BOOL redraw);
	void DeleteShapes(int family, CString tag, BOOL redraw);
	void DeleteShape(int family, int tag, int index, BOOL redraw);
	void DeleteShape(int family, CString tag, int index, BOOL redraw);
	
	void ShowShapes(BOOL show);
	void ShowShapes(int family, BOOL show);
	void ShowShapes(int family, int tag, BOOL show);
	void ShowShapes(int family, CString tag, BOOL show);
	void ShowShape(int family, CString tag, int index, BOOL show);
	void ShowShape(int family, int tag, int index, BOOL show);

	void SelectAllShapes(BOOL select, BOOL redraw);
	void SelectAllShapes(int family, int tag, BOOL select, BOOL redraw);
	void SelectAllShapes(int family, CString tag, BOOL select, BOOL redraw);
	const WinPaint::ShapeInfo* SelectShape(int family, int tag, int index, BOOL select, BOOL redraw);
	const WinPaint::ShapeInfo* SelectShape(int family, CString tag, int index, BOOL select, BOOL redraw);
	WinPaint::ShapeInfo GetShapeInfo() { return shape_info_; }
	WinPaint::ShapeInfo GetShapeInfo(int family, int tag, int index);
	WinPaint::ShapeInfo GetShapeInfo(int family, CString tag, int index);
	WinShape* GetShape(int family, int tag, int index);
	WinShape* GetShape(int family, CString tag, int index);
	void SetAsNewBorn(int family, int tag, int index);
	void SetAsNewBorn(int family, CString tag, int index);

	int GetLastWindowShapeIndex(int family, CString tag, WinShape::SHAPE_TYPE shape_type);
	void AddString(int family, CRect rect, CString text, int format, int backmode, int height, int weight, bool italic, bool underline, CString face_name, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw);
	void AddString(int family, CRect rect, CString text, int format, int backmode, HFONT font, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw);
	void SetString(int family, int index, CString text, BOOL redraw);
	void VisibleString(int family, BOOL visible, BOOL redraw);
	void VisibleString(int family, int index, BOOL visible, BOOL redraw);
	void RemoveAllStrings(BOOL redraw);
	void RemoveString(int family, BOOL redraw);
	void RemoveString(CRect rect, BOOL redraw);

	void DrawCircle(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index = -1, BOOL multiple_of_4 = TRUE, int shape_selection_size = 0);
	void DeleteCircle(CRect rect, BOOL redraw);
	void DrawRect(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index = -1, BOOL multiple_of_4 = TRUE, int shape_selection_size = 0);
	void DeleteRect(CRect rect, BOOL redraw);
	void DrawRoundRect(CRect rect, int linewidth, int linestyle, float widtharc, float heightarc, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index = -1, BOOL multiple_of_4 = TRUE, int shape_selection_size = 0);
	void DeleteRoundRect(CRect rect, BOOL redraw);
	void DrawEllipse(CRect rect, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw, int view_index = -1, BOOL multiple_of_4 = TRUE, int shape_selection_size = 0);
	void DeleteEllipse(CRect rect, BOOL redraw);
	void DrawLine(CPoint left_top_point, CPoint right_bottom_point, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, float angle, BOOL redraw);
	void DeleteLine(CPoint left_top_point, CPoint right_bottom_point, BOOL redraw);
	void DrawPolygon(CArray<POINT>& points, CString text, HFONT font, int format, int linewidth, int linestyle, COLORREF linecolor, COLORREF fillcolor, COLORREF textcolor, float angle, BOOL redraw);
	void DrawString(CRect rect, CString text, int format, int backmode, int height, int weight, bool italic, bool underline, CString face_name, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw, int view_index = -1);
	void DrawString(CRect rect, CString text, int format, int backmode, HFONT font, COLORREF text_color, COLORREF back_color, float angle, BOOL redraw, int view_index = -1);
	void DeleteString(CRect rect, BOOL redraw);
	void DrawPoints(CArray<CPoint>& points, COLORREF color, BOOL redraw, int view_index = -1);
	void DrawPoints(std::vector<CPoint>& points, COLORREF color, BOOL redraw, int view_index = -1);

	BOOL IsShapeVisible(int family);
	BOOL IsShapeVisible(int family, int tag, int index);
	BOOL IsShapeVisible(int family, CString tag, int index);
	CRect GetRect(int family, int tag, int index, BOOL respect_to_img);
	CRect GetRect(int family, CString tag, int index, BOOL respect_to_img);
	void SetRect(int family, int tag, int index, CRect rect, BOOL redraw, int view_index = -1);
	void SetRect(int family, CString tag, int index, CRect rect, BOOL redraw, int view_index = -1);

	void ChangeBoundingRect(int family, CString tag, int index, CRect rect);
	BOOL ChangeAngle(int family, int tag, int index, float angle, BOOL redraw, int view_index = -1);
	BOOL ChangeAngle(int family, CString tag, int index, float angle, BOOL redraw, int view_index = -1);
	BOOL ChangeRect(int family, int tag, int index, CRect rect, BOOL redraw, int view_index = -1);
	BOOL ChangeRect(int family, CString tag, int index, CRect rect, BOOL redraw, int view_index = -1);
	BOOL ChangeRectResizeStyle(int family, CString tag, int index, BOOL resize_from_left_right_only, BOOL redraw);
	void ChangeColor(int family, CString tag, COLORREF color, BOOL redraw);
	void ChangeColor(int family, int tag, COLORREF color, BOOL redraw);
	void ChangeColor(int family, int tag, int index, COLORREF color, BOOL redraw);
	void ChangeColor(int family, CString tag, int index, COLORREF color, BOOL redraw);
	void ChangeFillColor(int family, int tag, int index, COLORREF color, BOOL redraw);
	void ChangeFillColor(int family, CString tag, int index, COLORREF color, BOOL redraw);
	void ChangeLineWidth(int family, int tag, int index, int width, BOOL redraw);
	void ChangeLineWidth(int family, CString tag, int index, int width, BOOL redraw);
	void ChangeLineStyle(int family, int tag, int index, int style, BOOL redraw);
	void ChangeLineStyle(int family, CString tag, int index, int style, BOOL redraw);
	void ChangePoints(int family, int tag, int index, CArray<POINT>& points, BOOL redraw);
	void ChangePoints(int family, CString tag, int index, CArray<POINT>& points, BOOL redraw);
	void ChangeTextColor(int family, int tag, int index, COLORREF color, BOOL redraw);
	void ChangeTextColor(int family, CString tag, int index, COLORREF color, BOOL redraw);
	void ChangeText(int family, int tag, int index, CString text, BOOL redraw);
	void ChangeText(int family, CString tag, int index, CString text, BOOL redraw);

	void EditPolygon(int family, CString tag, int index, CArray<POINT>& points, CString text, COLORREF linecolor, COLORREF fillcolor, COLORREF textcolor, BOOL redraw);
	void EditRect(CRect Rect, CRect PrevRect, BOOL redraw);
	void EditRectArc(CRect Rect, float WidthArc, float HeightArc, BOOL redraw);
	void EditLine(CPoint PointTL, CPoint PointRB, CPoint PrevPointTL, CPoint PrevPointRB, BOOL redraw);
	void EditWidth(CRect Rect, int Width, BOOL redraw);
	void EditStyle(CRect Rect, int Style, BOOL redraw);
	void EditColor(CRect Rect, COLORREF Color, BOOL redraw);
	void EditFillColor(CRect Rect, COLORREF Color, BOOL redraw);
	void EditTextProperties(CRect Rect, CString Text, int TextFormat, int TextBackMode, COLORREF TextColor, COLORREF TextBackColor, BOOL redraw);
	void EditFontProperties(CRect Rect, int Height, int Weight, bool Italic, bool Underline, CString FontFace, BOOL redraw);

	CRect GetShapeRectWnd(int index);
	CRect GetShapeRectImg();
	CRect GetShapeRectImg(int index);
	int GetShapeAllPointsWnd(CPoint* Point);
	int GetShapeAllPointsImg(CPoint* Point);

	void SetImageStoreLimit(int limit);
	void SetImageStoreValue(int value);

	CRITICAL_SECTION& cs_for_winpaint() { return cs_for_winpaint_; }

protected:
	DECLARE_MESSAGE_MAP()

private:
	struct WinView {
		WinView() {
			buffer[0] = NULL;
			buffer[1] = NULL;
			winrect.SetRectEmpty();
		}
		CRect winrect;
		cv::Mat* buffer[2];
	};

	void ShowPixelInfo(CPoint point);
	void ResetParameters();
	void CalculateZoomIndex(long image_width, long image_height);
	void ImageSizeChanged(long image_width, long image_height);
	void CreateCaptionbar();
	void UpdateButtonInfoInCaptionbar(BOOL redraw);
	void CreateToolbar();
	void RefreshToolbar();
	void UpdatePixelInfoInToolbar();
	void UpdateButtonInfoInToolbar();
	void WindowSizeChanged();
	HWND GetHWND() { return GetSafeHwnd(); }
	void DrawShape();
	BOOL RectInsideBoundingRect(WinShape* shape, CRect& rect);
	BOOL CheckOverlapping(WinShape* shape, WinShape::SHAPE_TYPE shape_type, CRect rect);
	void ShowTinyImageView();
	void BuildImage(cv::Mat& buffer, int view_index = 0, BOOL update_multiple_view = FALSE);
	void CrossLine();
	BOOL SelectRect(CRect Rect, WinShape* shape, BOOL redraw);
	void SelectLine(CPoint PointX1Y1, CPoint PointX2Y2, BOOL redraw);
	void SelectNone(BOOL redraw);
	void GetRectInWindow(CRect& rect); //Used in zoom view
	void GetRectInImage(CRect& rect); //Used in zoom view
	void GetPointInWindow(CPoint& point); //Used in zoom view
	void GetPointInImage(CPoint& point); //Used in zoom view
	CRect GetRectWRTWindow(const CRect& rect);
	CRect GetRectWRTWinViewWindow(const CRect& winview_rect, const CRect& rect, BOOL only_lefttop = FALSE);
	CRect GetRectWRTImage(const CRect& rect);
	CPoint GetPointWRTWindow(const CPoint& point);
	CPoint GetPointWRTImage(const CPoint& point);
	int GetIntWRTWindow(const int& integer);
	int GetLastIndex(int family, CString tag);
	float FindAngle(CPoint first_point, CPoint second_point);
	void ToolbarRegionButtonClicked();
	void OnRotate();
	void OnMove();
	void OnResize();
	BOOL NavigateShape(BOOL manually, int type, int direction); //type 1-Rotate, 2-Move, 3-Resize //direction 1-Clock, 2-Anticlock 1-left, 2-up, 3-right, 4-down 1-Lefttop, 2-Righttop, 3-Rightbottom, 4-Leftbottom
	void DrawImageStoreProgressBar(BOOL redraw);

	ULONG_PTR           gdiplusToken;
	CRITICAL_SECTION	cs_for_winpaint_;
	CRITICAL_SECTION	cs_for_image_;
	CRITICAL_SECTION	cs_for_shapes_;
	CRITICAL_SECTION	cs_for_captionbar_;
	CRITICAL_SECTION	cs_for_render_captionbar_;
	CRITICAL_SECTION	cs_for_toolbar_;
	CRITICAL_SECTION	cs_for_toolbar_imagestore_;
	CRITICAL_SECTION	cs_for_freeze_on_error_alarm_;
	WinPixelInfo		pixel_info_;
	CArray<WinView>		winview_;
	int					wnd_width_max_; //Used when windows size changed..set it to maximum size or primary window width
	int					wnd_height_max_; //Used when windows size changed..set it to maximum size or primary window height
	int					view_count_;
	int					view_index_;
	int					image_index_;
	int					image_type_;
	int					shape_selection_size_;

	int					window_image_dimentions_ratio_;	//0 - image w/h both > window w/h 
														//1 - image w/h both < window w/h 
														//2 - image w/h both = window w/h 
														//3 - image w > window w and image h < window h
														//4 - image w < window w and image h > window h

	IMAGE_OPERATION				previous_image_operation_;
	IMAGE_OPERATION				image_operation_;
	WinShape::SHAPE_OPERATION	shape_operation_;
	WinShape::SHAPE_SELECTION	shape_selection_;
	BOOL						full_image_;
	BOOL						show_tiny_image_view_;
	BOOL						multiple_shapes_selection_;
	BOOL						lock_draw_operation_;
	BOOL						mouse_move_;

	int					wnd_width_;
	int					wnd_height_;
	int					img_width_;
	int					img_height_;
	int					img_display_width_;
	int					img_display_height_;
	int					img_shape_width_;
	int					img_shape_height_;

	CRect				client_rect_;
	CRect				current_rect_;
	CRect				previous_rect_;
	CPoint				pan_point_;
	CPoint				down_point_;

	CDC					main_dc_;
	CDC					captionbar_dc_, captionbar_render_dc_;
	CDC					toolbar_dc_;
	CDC					image_dc_;
	CDC					multiple_image_dc_;
	CDC					shape_dc_;
	CBitmap				main_bitmap_;
	CBitmap*			old_main_bitmap_;
	CBitmap				captionbar_bitmap_;
	CBitmap*			old_captionbar_bitmap_;
	CBitmap				captionbar_render_bitmap_;
	CBitmap*			old_captionbar_render_bitmap_;
	CBitmap				toolbar_bitmap_;
	CBitmap*			old_toolbar_bitmap_;
	CBitmap				image_bitmap_;
	CBitmap*			old_image_bitmap_;
	CBitmap				multiple_image_bitmap_;
	CBitmap*			old_multiple_image_bitmap_;
	CBitmap				shape_bitmap_;
	CBitmap*			old_shape_bitmap_;

	BITMAPINFO*			bitmap_info_;
	BITMAPINFO*			aoi_bitmap_info_;
	cv::Mat				aoi_wnd_image_;

	CArray<WinShape*>	shapes_;
	WinShape*			shape_;
	ShapeInfo			shape_info_;

	CArray<WinShape*>	window_shapes_;

	float				width_arc_;
	float				height_arc_;
	int					line_width_;
	int					line_style_;
	COLORREF			line_color_;
	COLORREF			fill_color_;
	int					shape_index_;
	CString				tag_;
	CString				data_;
	int					overlap_area_;
	CRect				bounding_rect_;
	int					family_;
	CString				tag_for_shape_selection_;
	CString				save_image_path_;
	CDWordArray			visible_family_list_for_multiple_view_;

	CPen				drawing_pen_;

	CPoint				rect_corner_for_resize_;
	float				angle_on_mouse_down_;
	float				angle_on_mouse_move_;
	CPoint				center_point_;
	CPoint				selection_points_[10];
	int					selected_point_count_;
	int					selected_point_index_;

	std::vector<CPoint>		free_hand_points_window_;
	std::vector<cv::Point>	free_hand_points_image_;

	COLORREF			captionbar_color_;
	int					show_captionbar_; //0-Hide, 1-Show Fixed, 2-Show Toggle
	CRect				captionbar_rect_;
	CRect				captionbar_buttons_rect_[5]; //4-Color Code,3-Left Text,2-Right Text,1-Right Text Button,0-Show Region
	BOOL				captionbar_buttons_visible_[5];
	BOOL				clicked_on_captionbar_area_;
	int					captionbar_button_selection_index_;
	DWORD				show_captionbar_region_; //0-Transparent,1-With Right Text, 2-All
	CString				left_text_;
	CString				right_text_[2];
	COLORREF			left_text_color_;
	COLORREF			right_text_color_;
	COLORREF			color_code_;
	BOOL				right_text_button_clicked_;
	BOOL				right_text_button_status_;

	COLORREF			toolbar_color_;
	int					show_toolbar_; //0-Hide, 1-Show Fixed, 2-Show Toggle
	CRect				toolbar_rect_;
	int					toolbar_pixinfo_right_; //For repaint of toolbar only
	CRect				toolbar_buttons_rect_[25];		//0. pan_button_rect 1. zoomshape_button_rect 2. zoomin_button_rect 3. zoomout_button_rect 4. fit to image 5. source/binary view 6. multipleview 7. lock image alarm 8. cross line 9. region 10. navigation menu 11. navigation clockwise 12. navigation anticlockwise 13. navigation left 14. navigation right 15. navigation up 16. navigation down 17. save image 18. show shapes 20. image store all 21. image store fail 22. image store share 23. image store clear
	BOOL				toolbar_buttons_disable_[25];	//0. pan_button_rect 1. zoomshape_button_rect 2. zoomin_button_rect 3. zoomout_button_rect 4. fit to image 5. source/binary view 6. multipleview 7. lock image alarm 8. cross line 9. region 10. navigation menu 11. navigation clockwise 12. navigation anticlockwise 13. navigation left 14. navigation right 15. navigation up 16. navigation down 17. save image 18. show shapes 
	BOOL				toolbar_buttons_visible_[25];	//0. pan_button_rect 1. zoomshape_button_rect 2. zoomin_button_rect 3. zoomout_button_rect 4. fit to image 5. source/binary view 6. multipleview 7. lock image alarm 8. cross line 9. region 10. navigation menu 11. navigation clockwise 12. navigation anticlockwise 13. navigation left 14. navigation right 15. navigation up 16. navigation down 17. save image 18. show shapes 
	BOOL				clicked_on_toolbar_area_;
	int					toolbar_button_selection_index_;
	CPoint				xy_position_;
	CRect				x_text_rect_;
	CRect				y_text_rect_;
	CRect				rgb_color_rect_;
	CRect				rgb_text_rect_;
	CRect				hsl_color_rect_;
	CRect				hsl_text_rect_;
	BOOL				lock_image_;
	BOOL				allow_to_change_image_view_;
	BOOL				multiple_image_view_;
	BOOL				center_line_;
	BOOL				show_hide_shapes_;
	CRect				zoom_text_rect_;
	int					zoom_index_;
	BOOL				panning_on_;
	CRect				imagestore_text_rect_;
	int					imagestore_value_;
	int					imagestore_limit_;
	BOOL				imagestore_pass_on_;
	BOOL				imagestore_fail_on_;
	BOOL				imagestore_grab_on_;

	WinPaintMenu		winpaintmenu_navigation_;
	int					navigation_menu_index_; //0-Rotate, 1-Move, 2-ResizeLeftTop, 3-ResizeRightTop, 4-ResizeRightBottom, 5-ResizeLeftBottom
	BOOL				toolbar_region_enable_[4];
	BOOL				toolbar_region_visible_[4];
	int					toolbar_region_index_;
	BOOL				show_toolbar_region_;
};