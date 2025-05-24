#pragma once

#include <deque>

#ifndef INCLUDE_THUMBLISTCTRLPRO
#define INCLUDE_THUMBLISTCTRLPRO __declspec(dllimport)
#endif //INCLUDE_THUMBLISTCTRLPRO

#define WM_THUMBLISTCTRLPRO_SELCHANGE				WM_USER + 443
#define WM_THUMBLISTCTRLPRO_ITEMDRAGGED				WM_USER + 444
#define WM_THUMBLISTCTRLPRO_ITEMINSERTED			WM_USER + 445
#define WM_THUMBLISTCTRLPRO_SETSCROLLBARPOS			WM_USER + 446

// ThumbListCtrlPro

class INCLUDE_THUMBLISTCTRLPRO ThumbListCtrlPro : public CStatic
{
	DECLARE_DYNAMIC(ThumbListCtrlPro)

public:
	struct SourceListCtrlProInfo {
		SourceListCtrlProInfo() {
			thumblistctrl = NULL;
			drag_index = -1;
		}
		ThumbListCtrlPro* thumblistctrl;
		int drag_index;
	};

	enum THUMBPRO_BORDER_COLOR { NO_BORDER, GREEN_BORDER, RED_BORDER };
	enum THUMBPRO_IMAGE_TYPE { GRAY_8BIT, COLOR_24BIT };

	ThumbListCtrlPro();
	virtual ~ThumbListCtrlPro();
	void Create(int max_items, int image_width, int image_height, int spacing, THUMBPRO_IMAGE_TYPE image_type);
	void Destroy();
	void Move(CRect rect);
	void AddItem(cv::Mat& img, THUMBPRO_BORDER_COLOR status = NO_BORDER);
	void AddItem(cv::Mat& img, CString text1, CString text2, CString text3, THUMBPRO_BORDER_COLOR status = NO_BORDER);
	void AddItem(cv::Mat& img, COLORREF color_code, CString thumb_title, COLORREF text_color);
	void ShowAllItems(BOOL show_all);
	BOOL SetStatus1(int item_index, THUMBPRO_BORDER_COLOR border_color);
	BOOL SetStatus2(int item_index, THUMBPRO_BORDER_COLOR border_color);
	void SelectItem(int item_index);
	void SetMaxItems(int max_items);
	int GetVisibleItemCount();
	int GetItemCount();
	int GetCurSel() { return selected_item_index_; }
	void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
	void SetTextColor(COLORREF text_color) { text_color_ = text_color; }
	void AllowSelection(BOOL allow_selection) { allow_selection_ = allow_selection; }
	void AllowDragging(BOOL allow_dragging) { allow_dragging_ = allow_dragging; }
	void EraseAllItems();
	BOOL IsDraggingAllowed() { return allow_dragging_; }
	BOOL IsItemInsertionAllowed();
	void SetThumbTitleHeight(int thumb_title_height) { thumb_title_height_ = thumb_title_height; }
	void SetThumbTitleTextAlign(DWORD align) { thumb_title_align_ = align; }

private:
	struct ItemInfo {
		ItemInfo() {
			title_text.Empty();
			text1.Empty();
			text2.Empty();
			text3.Empty();
			status1 = THUMBPRO_BORDER_COLOR::NO_BORDER;
			status2 = THUMBPRO_BORDER_COLOR::NO_BORDER;
			text_color = LABEL_COLOR;
			color_code = BLACK_COLOR;
		}
		~ItemInfo() {
			buffer.release();
		}
		CString title_text;
		CString text1;
		CString text2;
		CString text3;
		cv::Mat buffer;
		COLORREF text_color;
		COLORREF color_code;
		THUMBPRO_BORDER_COLOR status1;
		THUMBPRO_BORDER_COLOR status2;
	};

	std::deque<ItemInfo> item_info_list_;
	CArray<CRect> rect_list_;
	CRect client_rect_;
	int max_items_;
	int source_image_width_;
	int source_image_height_;
	int thumb_image_width_;
	int thumb_image_height_;
	int thumb_title_height_;
	int spacing_;
	COLORREF back_color_;
	COLORREF text_color_;
	DWORD thumb_title_align_;

	BITMAPINFO* bitmap_info_[2];
	THUMBPRO_IMAGE_TYPE image_type_;
	int thumb_index_;
	int selected_item_index_;
	int dragging_index_;
	BOOL dragging_;
	BOOL allow_dragging_;
	BOOL allow_selection_;
	CPoint point_down_;
	BOOL scroll_item_;
	BOOL drag_item_;
	CImageList drag_image_;
	BOOL thumb_list_filled_;

	LRESULT SetScrollBarPosition(WPARAM wparam = 0, LPARAM lparam = 0);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};