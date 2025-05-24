#pragma once

#ifndef INCLUDE_BUTTON
#define INCLUDE_BUTTON __declspec(dllimport)
#endif //INCLUDE_BUTTON

#define WM_BN_CLICKED		WM_USER + 451

// Button

class INCLUDE_BUTTON Button : public CButton {

	DECLARE_DYNAMIC(Button)

public:
	struct ICON_INFO {
		HICON hIcon;			// Handle to icon
		DWORD dwWidth;		// Width of icon
		DWORD dwHeight;		// Height of icon
	};

	enum BUTTON_TYPE { BUTTON_REGULAR, BUTTON_CHECK_BOX, BUTTON_CHECK_BOX_PUSH_LIKE, BUTTON_RADIO, BUTTON_RADIO_PUSH_LIKE };
	enum ICON_ALIGNMENT {
		ST_ALIGN_HORIZ = 0,			// Icon/bitmap on the left, text on the right
		ST_ALIGN_HORIZ_RIGHT,		// Icon/bitmap on the right, text on the left
		ST_ALIGN_VERT,				// Icon/bitmap on the top, text on the bottom (both are center aligned)
		ST_ALIGN_CENTER,			// Icon/bitmap on the top in the center, text on the bottom
		ST_ALIGN_OVERLAP			// Icon/bitmap on the same space as text
	};

	Button();
	virtual ~Button();

	void SetIcon(HICON icon, HICON icon_on_button_click, ICON_ALIGNMENT alignment);
	void SetText(CString text, CString text_on_button_click = _T(""));
	CString GetText();
	CString GetText(BOOL pressed_off);
	void Create(BUTTON_TYPE button_type, COLORREF back_color, COLORREF back_color_on_button_click, BOOL highlight_change_value = FALSE);
	void Destroy();
	void SetCheck(BOOL check, BOOL highlight = FALSE);
	BOOL GetCheck() { return button_pressed_; }
	void EnableWnd(BOOL bEnable = TRUE);
	void SetBackColor(COLORREF back_color);
	void SetBorderColor(COLORREF border_color);
	void SetTextColor(COLORREF text_color);
	void SetTextFlashColor(COLORREF flash_color);
	void SetDisableColor(COLORREF disable_color);

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);

protected:
	virtual void DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();

private:
	CRect client_rect_;
	CString text_;
	CString text_on_button_click_;
	BOOL button_pressed_;
	BOOL prev_button_pressed_;
	BOOL button_disabled_;
	BOOL button_down_;
	BOOL button_clicked_;
	BOOL button_status_on_click_;
	BOOL text_flash_flag_;
	BOOL highlight_changed_value_; //Applicable only to button with checkbox style
	BUTTON_TYPE button_type_;
	POINT image_org_pt_;
	ICON_ALIGNMENT icon_alignment_;
	ICON_INFO icon_info_[2];

	COLORREF back_color_, back_color_on_click_, disable_color_, border_color_;
	COLORREF text_color_;
	COLORREF text_flash_color_;

	DECLARE_MESSAGE_MAP();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

