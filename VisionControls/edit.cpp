// edit.cpp : implementation file
//

#include "stdafx.h"
#include "vision_controls.h"

// Edit

IMPLEMENT_DYNAMIC(Edit, CEdit)

Edit::Edit()
{
	color_text_ = BLACK_COLOR;
	color_back_ = WHITE_COLOR;
	brush_backcolor_.CreateSolidBrush(color_back_);

	keyboard_type_ = Edit::KEYBOARD_TYPE::KB_ALPHANUMERIC;
	keyboard_captionbar_text_ = L"";
	multilingual_ = FALSE;
	show_keyboard_ = TRUE;

	holder_ = _T("'\\");
}

Edit::~Edit()
{
	brush_backcolor_.DeleteObject();
}

BEGIN_MESSAGE_MAP(Edit, CEdit)
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_PASTE, OnPasteMessage)
	ON_MESSAGE(EM_SHOWBALLOONTIP, OnShowTipMessage)
END_MESSAGE_MAP()

// edit message handlers

BOOL Edit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_RBUTTONDOWN)
		return TRUE;

	return CEdit::PreTranslateMessage(pMsg);
}

void Edit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TCHAR c = nChar;

	if (holder_.Find(c) == -1 || c == '\b') {
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetParent()->SendMessageW(WM_EDIT_CHARCHANGE, (WPARAM)this, (LPARAM)c);
	}
}

HBRUSH Edit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor(color_back_);
	pDC->SetTextColor(color_text_);

	return (HBRUSH)brush_backcolor_;
}

void Edit::OpenKeyBoard() {

	BOOL password_style = FALSE;
	if ((GetStyle() & ES_PASSWORD) == ES_PASSWORD) {
		password_style = TRUE;
	}

	CString edit_text;
	GetWindowText(edit_text);
	KeyboardDlg keyboard_dlg;
	
	KeyboardDlg::KEYBOARD_TYPE keyboard_type;
	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
		keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC;
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_NUMERIC) {
		keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_NUMERIC;
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
		keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NUMERIC;
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
		keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC;
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC) {
		keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC;
	}
	keyboard_dlg.SetKeyboardType(keyboard_type);
	int text_limit = GetLimitText();
	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
		if (text_limit == -1) {
			text_limit = FULL_KEY_LENGTH;
			SetLimitText(text_limit);
		}
	} else {
		if (text_limit == -1) {
			text_limit = NUM_KEY_LENGTH;
			SetLimitText(text_limit);
		}
	}
	keyboard_dlg.SetLimitText(text_limit);
	keyboard_dlg.SetHolder(holder_);
	if (keyboard_dlg.OpenKeyboard(keyboard_captionbar_text_, edit_text, password_style, FALSE, multilingual_) == IDOK) {

		edit_text = keyboard_dlg.GetEditText();

		if (edit_text.GetLength() > 0 && holder_.Find(edit_text.GetAt(edit_text.GetLength() - 1)) != -1) {
			Formation::MsgBox(Language::GetString(IDSTRINGM_FOLLOWING_CHARACTERS, holder_));
			return;
		}
		/*for (int i = 0; i < edit_text.GetLength(); i++) {
			if (holder_.Find(edit_text.GetAt(i)) != -1) {
				Formation::MsgBox(Language::GetString(IDSTRINGM_FOLLOWING_CHARACTERS, holder_));
				return;
			}
		}*/

		SetWindowText(edit_text);
		SetSel(edit_text.GetLength(), edit_text.GetLength() + 1);

		GetParent()->SendMessageW(WM_EDIT_ENTERPRESSED, (WPARAM)this);
	}
}

void Edit::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);

	if ((GetStyle() & ES_READONLY) == ES_READONLY) {
		return;
	}

	if (show_keyboard_) {
		OpenKeyBoard();
		SetFocus();
	}
	//CEdit::OnLButtonDown(nFlags, point);
}

LRESULT Edit::OnPasteMessage(WPARAM wparam, LPARAM lparam) {

	//printf("\n OnPasteMessage :: Can't paste");
	return 0;
}

LRESULT Edit::OnShowTipMessage(WPARAM wparam, LPARAM lparam) {

	//printf("\n OnShowTipMessage :: Can't show tip");
	return 0;
}