#pragma once

#include <deque>

#ifndef INCLUDE_LISTCTRL_THUMB
#define INCLUDE_LISTCTRL_THUMB __declspec(dllimport)
#endif //INCLUDE_LISTCTRL_THUMB

#define WM_THUMBLISTCTRL_ADDITEM				WM_USER + 414
#define WM_THUMBLISTCTRL_ITEMDRAGGED			WM_USER + 415
#define WM_THUMBLISTCTRL_ITEMADDED_OR_REMVOED	WM_USER + 441
#define WM_THUMBLISTCTRL_INSERTITEM				WM_USER + 442

// ThumbListCtrl

class INCLUDE_LISTCTRL_THUMB ThumbListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(ThumbListCtrl)

public:
	struct SourceListCtrlInfo {
		SourceListCtrlInfo() {
			thumblistctrl = NULL;
			drag_index = -1;
		}
		ThumbListCtrl* thumblistctrl;
		int drag_index;
	};
	enum THUMB_IMAGE_TYPE { GRAY_8BIT, COLOR_24BIT };
	enum THUMB_BORDER_COLOR { NO_BORDER, GREEN_BORDER, RED_BORDER };

	ThumbListCtrl();
	virtual ~ThumbListCtrl();

	void Create(int count, int image_width, int image_height, int columns, int rows, CRect scroll_rect, THUMB_IMAGE_TYPE type, BOOL horz_spacing, BOOL vert_spacing);
	void SetTextOnImage(BOOL text_on_image) { text_on_image_ = text_on_image; }
	void SetMaxItems(int max_items);
	BOOL IsSelectionAllowed() { return allow_selection_; }
	void AllowSelection(BOOL allow_selection);
	void AllowDragging(BOOL allow_dragging) { allow_dragging_ = allow_dragging; }
	void AllowDeletion(BOOL allow_deletion) { allow_deletion_ = allow_deletion; }
	void ShowItemDateTime(BOOL show) { show_date_time_ = show; }
	void Destroy();
	BOOL AddThumbImage(unsigned char* image_buffer, int thumb_index, CString text = L"");
	BOOL AddThumbImage(HBITMAP hbitmap, int bitmap_width, int bitmap_height, CString text);
	BOOL SetCurrentStatus(int index, unsigned char* buffer, THUMB_BORDER_COLOR border_color);
	CString GetThumbText(int index);
	BOOL IsDraggingAllowed() { return allow_dragging_; }
	BOOL IsItemInsertionAllowed();
	int EraseItem(int index);
	void EraseAllItems();
	int GetThumbImageWidth() { return image_width_; }
	int GetThumbImageHeight() { return image_height_; }

	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT	AddThumbImage(WPARAM wparam, LPARAM lparam);

private:
	LRESULT	InsertItemMessage(WPARAM wparam, LPARAM lparam);

	CRITICAL_SECTION cs_;
	//HANDLE handle_;

	BITMAPINFO* bitmap_info_[2];
	THUMB_IMAGE_TYPE image_type_;
	CRect client_rect_;
	int rows_;
	int cols_;
	int max_items_;

	CImageList thumb_image_list_;
	CImageList* drag_image_ptr_; //For creating and managing the drag-image
	int item_index_;
	int thumb_index_;
	int image_width_;
	int image_height_;
	int vertical_gap_;
	int dragging_index_;
	BOOL dragging_;
	BOOL allow_dragging_;
	BOOL allow_deletion_;
	BOOL allow_selection_;
	BOOL text_on_image_;
	BOOL show_date_time_;
	CRect delete_rect_;
	CRect scroll_rect_;

	HBITMAP bitmap_;
	Gdiplus::Bitmap* thumbnail_;

	std::deque<THUMB_BORDER_COLOR> thumb_status_;
	std::deque<CString> thumb_text_;
	std::deque<CString> thumb_time_text_;

	DECLARE_MESSAGE_MAP()
};