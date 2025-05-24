#pragma once

#include <timeapi.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#ifndef INCLUDE_KEYBOARD
#define INCLUDE_KEYBOARD __declspec(dllimport)
#endif //INCLUDE_KEYBOARD

#define WM_KEYBOARD_EDIT_CHARCHANGE		WM_USER + 448

#define FULL_KEY_LENGTH 40
#define NUM_KEY_LENGTH	8

// KeyBoardEdit

class KeyBoardEdit : public CEdit
{
	DECLARE_DYNAMIC(KeyBoardEdit)

public:
	KeyBoardEdit();
	~KeyBoardEdit();
	void SetBackcolor(COLORREF clr);
	void SetTextcolor(COLORREF clr);

protected:
private:
	CBrush brush_backcolor_;
	COLORREF color_back_;
	COLORREF color_text_;
	CString holder_;

public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

// KeyboardDlg dialog

class INCLUDE_KEYBOARD KeyboardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(KeyboardDlg)

public:
	enum KEYBOARD_TYPE { KB_ALPHANUMERIC, KB_NUMERIC, KB_FLOATING_NUMERIC, KB_NEGATIVE_NUMERIC, KB_FLOATING_NEGATIVE_NUMERIC };
	enum KEYBOARD_SPECIAL_KEYS { KB_NONE, KB_CAPS, KB_BACK, KB_SYMBOL, KB_SPACE, KB_OK };
	enum { IDD = IDD_KEYBOARD_DLG };

	KeyboardDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KeyboardDlg();

	int OpenKeyboard(CString title_text, CString edit_text, BOOL password_style, BOOL disable_edit_box, BOOL multilingual);
	void SetLimitText(int text_limit) { text_limit_ = text_limit; }
	void SetKeyboardType(KeyboardDlg::KEYBOARD_TYPE edit_type);
	void SetHolder(CString holder) { holder_ = holder; }
	void SetParent(CWnd* parent_wnd) { parent_wnd_ = parent_wnd; }
	CString GetEditText() { return edit_text_; }
	void SetOkOnKeypress(BOOL ok_on_keypress) { ok_on_keypress_ = ok_on_keypress; }
	void TimerFired(UINT wTimerID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

private:
	void PrepareKeys(BOOL fresh_popup);
	int FillChars();
	void FillPossibleChars();
	void CharClicked(CString character);
	void BackKeyClicked();
	void OkClicked();
	KEYBOARD_SPECIAL_KEYS GetSpecialKeyIndex(int button_index);
	void CloseKeyboard(int okcancel);
	void DrawRoundedRectangle(Gdiplus::Graphics& g, CRect r, int d, Gdiplus::Brush* brush);
	void DrawRoundedRectangle(Gdiplus::Graphics& g, CRect r, int d, Gdiplus::Pen* pen);

	BOOL close_button_clicked_;

	CWnd* parent_wnd_;
	KeyBoardEdit ctrEdit;
	CRect show_password_rect_;
	BOOL show_password_button_clicked_;
	BOOL password_style_;
	BOOL show_password_;
	BOOL disable_edit_box_;
	int text_limit_;
	CString holder_;
	CString edit_text_;
	CString title_text_;
	Gdiplus::Point back_space_points_[5];
	Gdiplus::Point caps_lock_points_[7];
	Gdiplus::Point ok_button_points_[3];

	CPoint keyboard_position_;
	CArray<CRect> alphabets_rect_; //if index 0 - 38
	int buttons_in_row_[5];
	
	std::vector<std::pair<CString, CString>> possible_chars_array_;
	MMRESULT possible_chars_timer_event_;
	CStringArray possible_chars_;
	CArray<CRect> possible_chars_rect_;
	int possible_char_button_index_;

	CStringArray chars_array[3];
	int chars_array_index_;

	CRect client_rect_;
	CRect title_rect_;
	CRect close_rect_;
	CRect edit_rect_;
	CRect upper_rect_;
	CRect bottom_rect_;
	int button_index_;
	BOOL ok_on_keypress_;

	MMRESULT back_key_timer_event_;
	int current_caret_pos_;

	//Used to open with same keys as closed in previously
	int last_chars_array_index_;

	CPoint down_point_;
	BOOL keydown_;
	BOOL keyup_;
	KeyboardDlg::KEYBOARD_TYPE keyboard_type_; //0 - Alphanumeric, 1 - OnlyNumeric, 2 - FloatingNumeric, 3 - NegativeNumeric, 4 - FloatingNegativeNumeric;

	//Language
	CRect language_view_rect_;
	BOOL multilingual_;
	BOOL language_view_;
	CArray<CRect> lang_rect_list_;
	CStringArray language_list_in_eng_;
	CStringArray language_list_;
	CDWordArray lang_id_list_;
	CDWordArray sublang_id_list_;
	CStringArray space_text_list_;
	DWORD lang_id_;
	DWORD sublang_id_;
	int prev_lang_index_;
	int selected_lang_index_;
	
	//History
	CRect history_view_rect_;
	BOOL show_history_view_;
	BOOL history_view_;
	CArray<CRect> history_rect_list_;
	CStringArray history_text_list_;
	int selected_history_index_;

	//Hindi language
	void SetCharsForButtons10to20Hindi(CString alphabet);
	CString hindi_chars_;
	CString hindi_chars_vowels_;
	CString hindi_chars_consonants_;

	BOOL activate_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};