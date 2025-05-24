#pragma once

#define WINPAINT_BINARY_VIEW_SHAPES_FAMILY		5000
#define WINPAINT_FIXED_SHAPES_FAMILY			6000
#define WINPAINT_USER_FAMILY					9000

class WinShape
{
public:
	enum SHAPE_TYPE { CIRCLE, ROUNDRECT, ELLIPSE, RECTANGLE, LINE, TEXT, POLYGON, PIXEL };
	enum SHAPE_OPERATION { NONE, DRAW, SELECT, MOVE, ROTATE, RESIZE };
	enum SHAPE_SELECTION { SINGLE, MULTIPLE, ALL };

	WinShape(void) {
		selected_ = FALSE;
		tag_ = L"0";
		data_ = L"";
		overlap_area_ = 0;
		bounding_rect_.SetRectEmpty();
		family_ = 0;
		angle_ = 0;
		index_ = 0;
		new_born_ = FALSE;
		visible_ = TRUE;
		multiple_of_4_ = TRUE;
		lock_.Add(0); lock_.Add(0); //Dummy Insertions for none and draw
		lock_.Add(0); lock_.Add(0); lock_.Add(0); lock_.Add(0); //Select, move, rotate, resize
	}
	virtual ~WinShape(void) {
		lock_.RemoveAll();
	}

	virtual void draw(HDC hdc_drawing, BOOL full_image) = 0;

	void set_rectangle(CRect& window_rect, CRect& image_rect) {

		if (multiple_of_4_ && !image_rect.IsRectEmpty()) {
			image_rect.right -= (image_rect.Width() % 4);
			image_rect.bottom -= (image_rect.Height() % 4);
		}
		left_top_points_[0].SetPoint(window_rect.left, window_rect.top); right_bottom_points_[0].SetPoint(window_rect.right, window_rect.bottom);
		left_top_points_[1].SetPoint(image_rect.left, image_rect.top); right_bottom_points_[1].SetPoint(image_rect.right, image_rect.bottom);
	}	
	void set_window_rectangle(CRect& window_rect) {
		left_top_points_[0].SetPoint(window_rect.left, window_rect.top); right_bottom_points_[0].SetPoint(window_rect.right, window_rect.bottom);
	}
	CRect rectangle(BOOL full_image) { return CRect(left_top_points_[full_image].x, left_top_points_[full_image].y, right_bottom_points_[full_image].x, right_bottom_points_[full_image].y); }
	void set_left_top_points(CPoint& window_point, CPoint& image_point) {
		left_top_points_[0] = window_point;
		left_top_points_[1] = image_point;
	}
	void set_right_bottom_points(CPoint& window_point, CPoint& image_point) {
		right_bottom_points_[0] = window_point;
		right_bottom_points_[1] = image_point;
	}
	const CPoint& left_top_points(BOOL full_image) { return left_top_points_[full_image]; }
	const CPoint& right_bottom_points(BOOL full_image) { return right_bottom_points_[full_image]; }
	CPoint center(BOOL full_image) { return CPoint(left_top_points_[full_image].x + (right_bottom_points_[full_image].x - left_top_points_[full_image].x) / 2, left_top_points_[full_image].y + (right_bottom_points_[full_image].y - left_top_points_[full_image].y) / 2); }

	virtual void set_shape_type(SHAPE_TYPE shape_type) { 
		shape_type_ = shape_type; 
		if (shape_type_ == CIRCLE) {
			lock_.SetAt(ROTATE, TRUE);
		}
	}
	virtual SHAPE_TYPE shape_type() { return shape_type_; }
	virtual void set_line_width(const int& line_width) { line_width_ = line_width; }
	virtual const int& line_width() { return line_width_; }
	virtual void set_line_style(const int& line_style) { line_style_ = line_style; }
	virtual const int& line_style() { return line_style_; }
	virtual void set_line_color(COLORREF line_color) { line_color_ = line_color; }
	virtual const COLORREF& line_color() { return line_color_; }
	virtual void set_fill_color(COLORREF fill_color) { fill_color_ = fill_color; }
	virtual const COLORREF& fill_color() { return fill_color_; }
	virtual void set_new_born(BOOL new_born) { new_born_ = new_born; }
	virtual BOOL new_born() { return new_born_; }
	virtual void set_selected(BOOL selected) { selected_ = selected; }
	virtual BOOL selected() { return selected_; }
	virtual void set_visible(BOOL visible) { visible_ = visible; }
	virtual const BOOL& visible() { return visible_; }
	virtual void set_multiple_of_4(BOOL multiple_of_4) { multiple_of_4_ = multiple_of_4; }
	virtual const BOOL& multiple_of_4() { return multiple_of_4_; }
	virtual void set_tag(CString tag, int overlap_area, CRect bounding_rect) {
		tag_ = tag;
		overlap_area_ = overlap_area;
		bounding_rect_ = bounding_rect;
	}	
	virtual const CString& tag() { return tag_; }
	virtual void set_data(CString data) { data_ = data; }
	virtual const CString& data() { return data_; }
	virtual const int& overlap_area() { return overlap_area_; }
	virtual void set_bounding_rect(CRect bounding_rect) { bounding_rect_ = bounding_rect; }
	virtual const CRect& bounding_rect() { return bounding_rect_; }
	virtual void set_family(int family) { family_ = family; }
	virtual const int& family() { return family_; }
	virtual void set_angle(float angle) { angle_ = angle; }
	virtual const float& angle() { return angle_; }
	virtual void set_index(int index) { index_ = index; }
	virtual const int& index() { return index_; }
	virtual void set_lock(DWORD operation, BOOL lock) {
		if (operation == NONE) {
			lock_.SetAt(SELECT, lock); lock_.SetAt(MOVE, lock); lock_.SetAt(RESIZE, lock);
			if (shape_type_ != CIRCLE) {
				lock_.SetAt(ROTATE, lock);
			}
		} else {
			if (shape_type_ == CIRCLE && operation == ROTATE) {
			} else {
				if (operation == MOVE && lock == FALSE) {
					lock_.SetAt(SELECT, FALSE);
				}
				lock_.SetAt(operation, lock);
			}
		}
	}
	virtual BOOL lock(DWORD operation) { return lock_.GetAt(operation); }
	virtual void set_shape_selection_size(int size) { shape_selection_size_ = size; }
	virtual const int& shape_selection_size() { return shape_selection_size_; }

private:
	CPoint left_top_points_[2];
	CPoint right_bottom_points_[2];

	SHAPE_TYPE shape_type_;
	int line_width_;
	int line_style_;
	COLORREF line_color_;
	COLORREF fill_color_;
	CDWordArray lock_;
	CString tag_;
	CString data_;
	int overlap_area_;
	CRect bounding_rect_;
	int family_;
	BOOL selected_;
	BOOL new_born_;
	BOOL visible_;
	BOOL multiple_of_4_;
	float angle_;
	int index_;

	int shape_selection_size_;
};

class WinRect : public WinShape
{
	BOOL resize_from_left_right_only_;
public:
	WinRect(void) { 
		resize_from_left_right_only_ = FALSE; 
	}
	~WinRect(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);
	void set_resize_style(BOOL resize_from_left_right_only) { resize_from_left_right_only_ = resize_from_left_right_only; }
	BOOL resize_style() { return resize_from_left_right_only_; }
};

class WinRoundRect : public WinShape
{
	float width_arc_;
	float height_arc_;

public:
	WinRoundRect(void) {
		width_arc_ = height_arc_ = 0;
	}
	~WinRoundRect(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);

	void set_arc(const float& width_arc, const float& height_arc) { width_arc_ = width_arc; height_arc_ = height_arc; }
};

class WinEllipse : public WinShape
{
public:
	WinEllipse(void) {}
	~WinEllipse(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);
};

class WinLine : public WinShape
{
public:
	WinLine(void) {}
	~WinLine(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);
};

class WinPolygon : public WinShape
{
	CArray<POINT> points_[2];
	CString text_;
	HFONT font_;
	int format_;
	COLORREF text_color_;

public:
	WinPolygon(void) { points_[0].RemoveAll(); points_[1].RemoveAll(); }
	~WinPolygon(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);
	CArray<POINT>& points(BOOL full_image) {
		return points_[full_image];
	}
	void set_text(CString text) { text_ = text; }
	void set_font(HFONT font) { font_ = font; }
	void set_format(int format) { format_ = format; }
	void set_text_color(COLORREF text_color) { text_color_ = text_color; }
	void set_points(CArray<POINT>& points, BOOL full_image) {
		points_[full_image].RemoveAll();
		for (int index = 0; index < points.GetSize(); index++) {
			points_[full_image].Add(points[index]);
		}
	}
};

class WinPoint : public WinShape
{
	CArray<CPoint> points_[2];
	COLORREF color_;

public:
	WinPoint(void) { points_[0].RemoveAll(); points_[1].RemoveAll(); }
	~WinPoint(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);
	CArray<CPoint>& points(BOOL full_image) {
		return points_[full_image];
	}
	void set_color(COLORREF color) { color_ = color; }
	void set_points(CArray<CPoint>& points, BOOL full_image) {
		points_[full_image].RemoveAll();
		for (int index = 0; index < points.GetSize(); index++) {
			points_[full_image].Add(points[index]);
		}
	}
	void set_points(std::vector<CPoint>& points, BOOL full_image) {
		points_[full_image].RemoveAll();
		for (int index = 0; index < points.size(); index++) {
			points_[full_image].Add(points[index]);
		}
	}
};

class WinText : public WinShape
{
	CString text_;
	int format_;
	int backmode_;
	HFONT font_;
	int font_height_[2];
	bool font_italic_;
	bool font_underline_;
	int font_weight_;
	CString font_facename_;

public:
	WinText(void) { font_ = NULL; }
	~WinText(void) {}

	void draw(HDC hdc_drawing, BOOL full_image);

	void SetTextProperties(const CString& text, const int& format, const int& backmode, const COLORREF& textcolor, const COLORREF& backcolor) { 
		text_ = text;
		format_ = format;
		backmode_ = backmode;
		set_line_color(textcolor);
		set_fill_color(backcolor);
	}

	void SetFontProperties(const int& height_wrt_window, const int& height_wrt_image, const int& weight, const bool& italic, const bool& underline, const CString& fontface) {
		font_height_[0] = height_wrt_window;
		font_height_[1] = height_wrt_image;
		font_weight_ = weight;
		font_italic_ = italic;
		font_underline_ = underline;
		font_facename_ = fontface;
	}
	void set_text(CString text) { text_ = text; }
	CString text() { return text_; }
	void SetFontProperties(const HFONT& font) { font_ = font; }
};

class WinPixelInfo
{
	CPoint point_;
	COLORREF line_color_;
	COLORREF fill_color_;
	COLORREF title_color_;
	COLORREF text_color_;
	COLORREF pixel_color_;

public:
	WinPixelInfo(void) {
		line_color_ = LABEL_COLOR;
		fill_color_ = LABEL_COLOR;
		title_color_ = WHITE_COLOR;
		text_color_ = WHITE_COLOR;
		pixel_color_ = RGB(0, 0, 0);
	}
	~WinPixelInfo(void) {}

	void SetPoint(const CPoint& point, const COLORREF& color) { point_ = point; pixel_color_ = color; }
	void ShowPixelInfo(HDC hdcDrawing, BYTE R, BYTE G, BYTE B);
};