#pragma once

#define WM_EDIT_CHARCHANGE		WM_USER + 428
#define WM_EDIT_ENTERPRESSED	WM_USER + 429

// Edit

class Edit : public CEdit
{
	DECLARE_DYNAMIC(Edit)

public:
	enum KEYBOARD_TYPE { KB_ALPHANUMERIC, KB_NUMERIC, KB_FLOATING_NUMERIC, KB_NEGATIVE_NUMERIC, KB_FLOATING_NEGATIVE_NUMERIC };
	
	Edit();
	virtual ~Edit();

	void SetBackcolor(COLORREF clr) { 
		if (clr != color_back_) {
			color_back_ = clr;
			brush_backcolor_.DeleteObject();
			brush_backcolor_.CreateSolidBrush(color_back_);
		}
	}
	void SetTextcolor(COLORREF clr) {
		if (clr != color_text_) {
			color_text_ = clr;
		}
	}
	void SetKeyBoardType(Edit::KEYBOARD_TYPE keyboard_type) {
		if (keyboard_type_ != keyboard_type) {
			keyboard_type_ = keyboard_type;
			DWORD style = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_ALPHANUMERIC) {
				style &= ~ES_NUMBER;
			} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_NUMERIC) {
				style |= ES_NUMBER;
				holder_ = L".-";
			} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
				style &= ~ES_NUMBER;
				holder_ = L"-";
			} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
				style &= ~ES_NUMBER;
				holder_ = L".";
			} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC) {
				style &= ~ES_NUMBER;
				holder_ = L"";
			}
			::SetWindowLong(GetSafeHwnd(), GWL_STYLE, style);
		}
	}
	void SetKeyBoardCaptionBarText(CString keyboard_captionbar_text) {
		keyboard_captionbar_text_ = keyboard_captionbar_text;
	}
	void ShowKeyboard(BOOL show_keyboard) {
		show_keyboard_ = show_keyboard;
	}
	void ShowMultilingualKeyboard(BOOL show) {
		multilingual_ = show;
	}
	void SetHolder(CString holder) { holder_ = holder; }
	void OpenKeyBoard();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

private:
	CBrush brush_backcolor_;
	COLORREF color_back_;
	COLORREF color_text_;
	CString holder_;

	CString keyboard_captionbar_text_;
	BOOL multilingual_;
	BOOL show_keyboard_;
	
	Edit::KEYBOARD_TYPE keyboard_type_; //0 - Alphanumeric, 1 - OnlyNumeric, 2 - FloatingNumeric, 3 - NegativeNumeric, 4 - FloatingNegativeNumeric;
		 
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnPasteMessage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnShowTipMessage(WPARAM wparam, LPARAM lparam);
};