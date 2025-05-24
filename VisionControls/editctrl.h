#pragma once

#ifndef INCLUDE_EDITCTRL
#define INCLUDE_EDITCTRL __declspec(dllimport)
#endif //INCLUDE_EDITCTRL

#include "edit.h"

// EditCtrl

class INCLUDE_EDITCTRL EditCtrl : public CStatic
{
	DECLARE_DYNAMIC(EditCtrl)

public:
	EditCtrl();
	virtual ~EditCtrl();

	void Create(int text_area = 50, BOOL password_style = FALSE, BOOL show_keyboard = TRUE, BOOL multiline = FALSE, int text_limit = -1);
	void Destroy();
	void SetText(CString text);
	void SetTextAlignment(DWORD align, BOOL rightside);
	void SetKeyBoardType(Edit::KEYBOARD_TYPE keyboard_type);
	void ShowMultilingualKeyboard(BOOL show);
	void SetHolder(CString holder);
	void SetTextLimit(int limit);
	void SetReadOnly(BOOL readonly);
	BOOL IsReadOnly();
	void SetFocus();
	void SetEditText(CString text);
	void SetEditValue(LONG value);
	CString GetEditText();
	LONG GetEditValue();
	void SetLabelBackColor(COLORREF label_backcolor) { label_backcolor_ = label_backcolor; }
	void SetLabelBorderColor(COLORREF label_bordercolor) { label_bordercolor_ = label_bordercolor; }
	void SetEditBackColor(COLORREF edit_backcolor);
	void SetEditTextColor(COLORREF edit_textcolor);

protected:
	virtual void PreSubclassWindow();

private:
	Edit ctrEdit;

	CRect client_rect_;
	CRect text_rect_;
	CRect edit_rect_;
	CRect edit_rect_topedge_;
	CRect edit_rect_bottomedge_;

	BOOL multiline_;
	CString text_;
	DWORD text_align_[2];

	COLORREF label_backcolor_, label_bordercolor_;
	COLORREF edit_backcolor_, edit_textcolor_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnEditEnterPressedMessage(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
};