// keyboard_dlg.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_KEYBOARD __declspec(dllexport)
#include "keyboard_dlg.h"

IMPLEMENT_DYNAMIC(KeyBoardEdit, CEdit)

KeyBoardEdit::KeyBoardEdit()
{
	color_text_ = BLACK_COLOR;
	color_back_ = WHITE_COLOR;
	brush_backcolor_.CreateSolidBrush(color_back_);
}

KeyBoardEdit::~KeyBoardEdit()
{
	brush_backcolor_.DeleteObject();
}

BEGIN_MESSAGE_MAP(KeyBoardEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

// edit message handlers

void KeyBoardEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TCHAR c = nChar;

	if (holder_.Find(c) == -1 || c == '\b') {
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		GetParent()->SendMessageW(WM_KEYBOARD_EDIT_CHARCHANGE, (WPARAM)this, (LPARAM)c);
	}
}

void KeyBoardEdit::SetBackcolor(COLORREF clr) {
	if (clr != color_back_) {
		color_back_ = clr;
		brush_backcolor_.DeleteObject();
		brush_backcolor_.CreateSolidBrush(color_back_);
	}
}

void KeyBoardEdit::SetTextcolor(COLORREF clr) {
	if (clr != color_text_) {
		color_text_ = clr;
	}
}

HBRUSH KeyBoardEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(color_text_);
	pDC->SetBkColor(color_back_);

	return (HBRUSH)brush_backcolor_;
}

// KeyboardDlg dialog

IMPLEMENT_DYNAMIC(KeyboardDlg, CDialogEx)

KeyboardDlg::KeyboardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(KeyboardDlg::IDD, pParent)
{
	parent_wnd_ = NULL;
	keyboard_type_ = KEYBOARD_TYPE::KB_ALPHANUMERIC;
	holder_ = L"";
	disable_edit_box_ = FALSE;
	multilingual_ = FALSE;
	language_view_ = FALSE;
	show_history_view_ = FALSE;
	history_view_ = FALSE;
	password_style_ = FALSE;
	show_password_ = FALSE;
	ok_on_keypress_ = FALSE;
	text_limit_ = -1;
	show_password_rect_.SetRectEmpty();
	show_password_button_clicked_ = FALSE;
	language_view_rect_.SetRectEmpty();
	last_chars_array_index_ = 0;
	history_view_rect_.SetRectEmpty();
	history_view_ = FALSE;
}

KeyboardDlg::~KeyboardDlg()
{
	ctrEdit.DestroyWindow();

	language_list_in_eng_.RemoveAll();
	language_list_.RemoveAll();
	lang_id_list_.RemoveAll();
	sublang_id_list_.RemoveAll();
	space_text_list_.RemoveAll();
}

void KeyboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDE_KEYBOARD_TEXT, ctrEdit);
}

BEGIN_MESSAGE_MAP(KeyboardDlg, CDialogEx)
	ON_MESSAGE(WM_KEYBOARD_EDIT_CHARCHANGE, OnEditCharChangeMessage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// KeyboardDlg message handlers

LRESULT KeyboardDlg::OnEditCharChangeMessage(WPARAM wparam, LPARAM lparam) {

	if (parent_wnd_) {
		parent_wnd_->SendMessageW(WM_KEYBOARD_EDIT_CHARCHANGE, wparam, lparam);
	}

	return 0;
}

BOOL KeyboardDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
	if (pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		if (!close_button_clicked_) {
			CPoint p;
			GetCursorPos(&p);
			CRect r;
			GetWindowRect(&r);
			CRect t = r;
			t.bottom = t.top + Formation::control_height();
			if (t.PtInRect(p)) {
				ReleaseCapture();
				SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
				SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
				return TRUE;
			}
		}
	}
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_F1 || pMsg->wParam == VK_F2 ||
			pMsg->wParam == VK_F3 || pMsg->wParam == VK_F4 || pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6 ||
			pMsg->wParam == VK_F7 || pMsg->wParam == VK_F8 || pMsg->wParam == VK_F9 || pMsg->wParam == VK_F10 ||
			pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 || pMsg->wParam == VK_END || pMsg->wParam == VK_HOME) {
			return FALSE;
		}
		if (pMsg->wParam == VK_RETURN) {

			OkClicked();
			return FALSE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL KeyboardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(LABEL_COLOR3);
	//ModifyStyle(WS_CAPTION, 0);

	GetClientRect(&client_rect_);

	title_rect_ = close_rect_ = client_rect_;
	title_rect_.right = client_rect_.right - Formation::control_height() - Formation::spacing();
	close_rect_.bottom = title_rect_.bottom = title_rect_.top + Formation::control_height();
	close_rect_.left = title_rect_.right;

	client_rect_.top = title_rect_.bottom;
	client_rect_.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());

	keydown_ = FALSE;
	close_button_clicked_ = FALSE;

	//Language//
	language_list_in_eng_.RemoveAll();
	language_list_.RemoveAll();
	lang_id_list_.RemoveAll();
	sublang_id_list_.RemoveAll();
	space_text_list_.RemoveAll();

	//ADD NEW LANGUAGE HERE

	//Czech (Czech Republic)
	language_list_in_eng_.Add(_T("Czech (Czech Republic)"));
	language_list_.Add(_T("Češka (Česká republika)"));
	lang_id_list_.Add(LANG_CZECH);
	sublang_id_list_.Add(SUBLANG_CZECH_CZECH_REPUBLIC);
	space_text_list_.Add(L"mezera");

	//English (US)
	language_list_in_eng_.Add(_T("English (US)"));
	language_list_.Add(_T("English (US)"));
	lang_id_list_.Add(LANG_ENGLISH);
	sublang_id_list_.Add(SUBLANG_ENGLISH_US);
	space_text_list_.Add(_T("space"));

	//German
	language_list_in_eng_.Add(_T("German (Germany)"));
	language_list_.Add(_T("Deutsche (Deutschland)"));
	lang_id_list_.Add(LANG_GERMAN);
	sublang_id_list_.Add(SUBLANG_GERMAN);
	space_text_list_.Add(_T("leerstelle"));

	//Hindi
	language_list_in_eng_.Add(_T("Hindi (India)"));
	language_list_.Add(_T("हिन्दी (भारत)"));
	lang_id_list_.Add(LANG_HINDI);
	sublang_id_list_.Add(SUBLANG_HINDI_INDIA);
	space_text_list_.Add(_T("जगह"));

	//Italian
	language_list_in_eng_.Add(_T("Italian (Italy)"));
	language_list_.Add(_T("Italiano (Italia)"));
	lang_id_list_.Add(LANG_ITALIAN);
	sublang_id_list_.Add(SUBLANG_ITALIAN);
	space_text_list_.Add(_T("spazio"));

	//Polish (Poland)
	language_list_in_eng_.Add(_T("Polish (Poland)"));
	language_list_.Add(_T("Polskie (Polska)"));
	lang_id_list_.Add(LANG_POLISH);
	sublang_id_list_.Add(SUBLANG_POLISH_POLAND);
	space_text_list_.Add(_T("odstęp"));

	//Portuguese (Brazil)
	language_list_in_eng_.Add(_T("Portuguese (Brazil)"));
	language_list_.Add(_T("Português (Brasil)"));
	lang_id_list_.Add(LANG_PORTUGUESE);
	sublang_id_list_.Add(SUBLANG_PORTUGUESE_BRAZILIAN);
	space_text_list_.Add(_T("espaço"));

	//Romanian (Romania)
	language_list_in_eng_.Add(_T("Romanian (Romania)"));
	language_list_.Add(_T("Română (România)"));
	lang_id_list_.Add(LANG_ROMANIAN);
	sublang_id_list_.Add(SUBLANG_ROMANIAN_ROMANIA);
	space_text_list_.Add(_T("spațiu"));

	//Russian (Russia)
	language_list_in_eng_.Add(_T("Russian (Russia)"));
	language_list_.Add(_T("Pусский (Россия)"));
	lang_id_list_.Add(LANG_RUSSIAN);
	sublang_id_list_.Add(SUBLANG_RUSSIAN_RUSSIA);
	space_text_list_.Add(_T("пространство"));

	//Spanish (Mexico)
	language_list_in_eng_.Add(_T("Spanish (Mexico)"));
	language_list_.Add(_T("Türkçe (Türkiye)"));
	lang_id_list_.Add(LANG_SPANISH);
	sublang_id_list_.Add(SUBLANG_SPANISH_MEXICAN);
	space_text_list_.Add(_T("boşluk"));

	//Turkish (Turkey)
	language_list_in_eng_.Add(_T("Turkish (Turkey)"));
	language_list_.Add(_T("Türkçe (Türkiye)"));
	lang_id_list_.Add(LANG_TURKISH);
	sublang_id_list_.Add(SUBLANG_TURKISH_TURKEY);
	space_text_list_.Add(_T("boşluk"));

	language_view_ = FALSE;
	selected_lang_index_ = 0;
	for (INT_PTR lang_index = 0; lang_index < language_list_in_eng_.GetSize(); lang_index++) {
		if (language_list_in_eng_[lang_index] == Language::GetLanguageInEnglish()) {
			selected_lang_index_ = (int)lang_index;
			break;
		}
	}
	prev_lang_index_ = selected_lang_index_;
	lang_id_ = Language::GetLangId();
	sublang_id_ = Language::GetSubLangId();
	//Language

	//History//
	CString history_text;
	HKEY key = NULL;
	if (RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SPAN", &key) == ERROR_SUCCESS) {
		DWORD key_type = REG_SZ;
		DWORD size = MAX_PATH;
		WCHAR sz_text[255] = L"\0";
		RegQueryValueEx(key, L"FullKBHistory", NULL, &key_type, (LPBYTE)sz_text, &size);
		history_text.Format(L"%s", sz_text);
		RegCloseKey(key);
	}
	history_view_ = FALSE;
	selected_history_index_ = -1;
	history_view_rect_.SetRectEmpty();
	history_rect_list_.RemoveAll();
	history_text_list_.RemoveAll();
	for (int index = 0; index < 20; index++) {
		history_text_list_.Add(L"-");
	}
	int start = 0;
	while(1) {
		int find = history_text.Find(L",", start);
		if (find == -1) {
			break;
		}
		CString text = history_text.Mid(start, find - start).Trim();
		history_text_list_.RemoveAt(0);
		history_text_list_.Add(text);
		start = find + 1;
	}
	//History

	if (disable_edit_box_) {
		ctrEdit.EnableWindow(FALSE);
	}
	if (password_style_ && !show_password_) {
		//ctrEdit.SetPasswordChar('*');
		ctrEdit.SetPasswordChar(0x25CF);
	}
	ctrEdit.SetFont(&Formation::font(Formation::BIG_FONT));
	ctrEdit.SetWindowTextW(edit_text_);
	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
		if (text_limit_ == -1) {
			text_limit_ = FULL_KEY_LENGTH;
		}
	} else {
		if (text_limit_ == -1) {
			text_limit_ = NUM_KEY_LENGTH;
		}
	}
	ctrEdit.SetLimitText(text_limit_);

	PrepareKeys(TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void KeyboardDlg::PrepareKeys(BOOL fresh_popup) {

	possible_chars_.RemoveAll();
	possible_chars_rect_.RemoveAll();

	chars_array_index_ = last_chars_array_index_;
	button_index_ = -1;

	int button_size = Formation::control_height() + Formation::spacing4();
	int spacing = Formation::spacing();
	int button_window_width = 0, button_window_height = 0;
	upper_rect_ = bottom_rect_ = client_rect_;
	bottom_rect_.top = upper_rect_.bottom = upper_rect_.top + Formation::control_height();

	CRect caps_lock_rect, back_space_rect, ok_button_rect;
	alphabets_rect_.RemoveAll();

	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {

		FillPossibleChars();

		int max_buttons_in_row = FillChars();

		button_window_width = spacing * (max_buttons_in_row + 1) + button_size * max_buttons_in_row;
		button_window_height = upper_rect_.bottom + spacing * 6 + button_size * 5;

		//LINE 1
		int top = bottom_rect_.top + spacing;
		int left = ((max_buttons_in_row - buttons_in_row_[0]) * button_size / 2) + spacing;
		for (int i = 0; i < buttons_in_row_[0]; i++) {
			alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
			left += button_size + spacing;
		}
		//LINE 2
		top += button_size + spacing;
		left = ((max_buttons_in_row - buttons_in_row_[1]) * button_size / 2) + spacing;
		for (int i = 0; i < buttons_in_row_[1]; i++) {
			alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
			left += button_size + spacing;
		}
		//LINE 3
		top += button_size + spacing;
		left = ((max_buttons_in_row - buttons_in_row_[2]) * button_size / 2) + spacing;
		for (int i = 0; i < buttons_in_row_[2]; i++) {
			alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
			left += button_size + spacing;
		}
		//LINE 4
		top += button_size + spacing;
		left = ((max_buttons_in_row - buttons_in_row_[3]) * button_size / 2) + spacing;
		caps_lock_rect = CRect(left, top, left + button_size, top + button_size);
		for (int i = 0; i < buttons_in_row_[3]; i++) {
			alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
			left += button_size + spacing;
		}
		back_space_rect = alphabets_rect_[alphabets_rect_.GetSize() - 1];
		//LINE 5
		int space_button_size = max_buttons_in_row - 5;
		top += button_size + spacing;
		left = button_size / 2 + spacing;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
		left += button_size + spacing;// + button_size/4;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
		left += button_size + spacing;// + button_size/4;
		alphabets_rect_.Add(CRect(left, top, left + button_size * space_button_size + (spacing * (space_button_size - 1)), top + button_size));
		left += button_size * space_button_size + spacing * space_button_size;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
		left += button_size + spacing;// + button_size/4;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
		ok_button_rect = alphabets_rect_[alphabets_rect_.GetSize() - 1];

		//
		caps_lock_points_[0].X = caps_lock_points_[1].X = caps_lock_rect.left + (caps_lock_rect.Width() * 35 / 100);
		caps_lock_points_[0].Y = caps_lock_points_[6].Y = (caps_lock_rect.bottom - caps_lock_rect.Height() * 35 / 100);
		caps_lock_points_[1].Y = caps_lock_points_[2].Y = caps_lock_points_[4].Y = caps_lock_points_[5].Y = (caps_lock_rect.bottom - caps_lock_rect.Height() * 50 / 100);
		caps_lock_points_[2].X = caps_lock_rect.left + (caps_lock_rect.Width() * 25 / 100);
		caps_lock_points_[3].X = caps_lock_rect.left + (caps_lock_rect.Width() * 50 / 100);
		caps_lock_points_[3].Y = caps_lock_rect.bottom - (caps_lock_rect.Height() * 75 / 100);
		caps_lock_points_[4].X = caps_lock_rect.left + (caps_lock_rect.Width() * 75 / 100);
		caps_lock_points_[5].X = caps_lock_points_[6].X = caps_lock_rect.left + (caps_lock_rect.Width() * 65 / 100);

	} else {

		chars_array[0].Add(L"1"); chars_array[0].Add(L"2"); chars_array[0].Add(L"3"); chars_array[0].Add(L"4"); chars_array[0].Add(L"5");
		chars_array[0].Add(L"6"); chars_array[0].Add(L"7"); chars_array[0].Add(L"8"); chars_array[0].Add(L"9"); chars_array[0].Add(L"-");
		chars_array[0].Add(L"0"); chars_array[0].Add(L"."); chars_array[0].Add(L"BACK"); chars_array[0].Add(L"OK");

		button_window_width = spacing * 5 + button_size * 4;
		button_window_height = upper_rect_.bottom + spacing * 5 + button_size * 4;

		int top = bottom_rect_.top + spacing;
		int left = bottom_rect_.left;
		for (int i = 0; i < 12; i++) {
			alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
			left += button_size + spacing;
			if ((i + 1) % 3 == 0) {
				top += button_size + spacing;
				left = bottom_rect_.left;
			}
		}

		left = (button_size + spacing) * 3 + spacing;
		top = bottom_rect_.top + spacing;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size));
		back_space_rect = alphabets_rect_[alphabets_rect_.GetSize() - 1];

		top += button_size + spacing;
		alphabets_rect_.Add(CRect(left, top, left + button_size, top + button_size * 3 + spacing * 2));
		ok_button_rect = alphabets_rect_[alphabets_rect_.GetSize() - 1];

		if (keyboard_type_ == KEYBOARD_TYPE::KB_NUMERIC) {
			alphabets_rect_[9].SetRectEmpty();	// - button
			alphabets_rect_[11].SetRectEmpty(); // . button
		} else if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
			alphabets_rect_[9].SetRectEmpty();	// - button
		} else if (keyboard_type_ == KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
			alphabets_rect_[11].SetRectEmpty(); // . button
		}
	}

	back_space_points_[0].X = back_space_rect.left + (back_space_rect.Width() * 25 / 100);
	back_space_points_[0].Y = back_space_rect.top + (back_space_rect.Height() * 50 / 100);
	back_space_points_[1].X = back_space_points_[4].X = back_space_rect.left + (back_space_rect.Width() * 40 / 100);
	back_space_points_[2].X = back_space_points_[3].X = back_space_rect.left + (back_space_rect.Width() * 75 / 100);
	back_space_points_[1].Y = back_space_points_[2].Y = back_space_rect.top + (back_space_rect.Height() * 35 / 100);
	back_space_points_[3].Y = back_space_points_[4].Y = back_space_rect.top + (back_space_rect.Height() * 65 / 100);

	ok_button_points_[0].X = ok_button_points_[2].X = ok_button_rect.left + ok_button_rect.Width() * 40 / 100;
	ok_button_points_[1].X = ok_button_rect.left + ok_button_rect.Width() * 20 / 100;
	ok_button_points_[0].Y = ok_button_rect.CenterPoint().y - ok_button_rect.Width() * 10 / 100;
	ok_button_points_[1].Y = ok_button_rect.CenterPoint().y + ok_button_rect.Width() * 5 / 100;
	ok_button_points_[2].Y = ok_button_rect.CenterPoint().y + ok_button_rect.Width() * 20 / 100;

	client_rect_.right = client_rect_.left + button_window_width;
	client_rect_.bottom = client_rect_.top + button_window_height;
	upper_rect_.right = bottom_rect_.right = client_rect_.right - Formation::spacing();
	bottom_rect_.bottom = button_window_height;

	CDC* dc = GetDC();
	CDC cdc;
	cdc.CreateCompatibleDC(dc);
	cdc.SelectObject(ctrEdit.GetFont());
	TEXTMETRIC tm;
	cdc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight + tm.tmExternalLeading;
	DeleteDC(cdc);
	ReleaseDC(dc);

	int offset = (upper_rect_.Height() - font_height) / 2;
	CRect head_rect(upper_rect_.left + offset, upper_rect_.top + offset, upper_rect_.right, upper_rect_.top + Formation::control_height() - offset);
	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
		if (multilingual_) {
			language_view_rect_.SetRect(head_rect.right - Formation::control_height() + offset, head_rect.top, head_rect.right - offset, head_rect.bottom);
		} else {
			language_view_rect_.SetRect(head_rect.right, head_rect.top, head_rect.right, head_rect.bottom);
		}
		if (password_style_ || !show_history_view_) {
			show_password_rect_.SetRect(language_view_rect_.left - Formation::control_height() + offset - Formation::spacing(), head_rect.top, language_view_rect_.left - offset - Formation::spacing(), head_rect.bottom);
			history_view_rect_.left = history_view_rect_.right = language_view_rect_.left;
			head_rect.right = show_password_rect_.left;
		} else{
			show_password_rect_.left = show_password_rect_.right = language_view_rect_.left;
			history_view_rect_.SetRect(language_view_rect_.left - Formation::control_height() + offset - Formation::spacing(), head_rect.top, language_view_rect_.left - offset - Formation::spacing(), head_rect.bottom);
			head_rect.right = history_view_rect_.left;
		}
	}
	edit_rect_.SetRect(head_rect.left, head_rect.top, head_rect.right - offset, head_rect.bottom);
	ctrEdit.MoveWindow(edit_rect_);
	ctrEdit.SetFocus();

	CRect wnd_rect = bottom_rect_;
	offset = edit_rect_.left - 2;
	wnd_rect.DeflateRect(offset, offset, offset, offset);

	//Language//
	int row_count = int(ceil((double)language_list_.GetSize() / 2));
	int button_height = (wnd_rect.Height() - (row_count * 1)) / row_count;
	if (button_height > wnd_rect.Height() / 4) {
		button_height = wnd_rect.Height() / 4;
	}
	int button_width = (wnd_rect.Width() - 1) / 2;

	lang_rect_list_.RemoveAll();
	CRect item_rect = wnd_rect;
	item_rect.bottom = item_rect.top;
	for (int item_index = 0; item_index < int(language_list_in_eng_.GetSize()); item_index++) {
		if (item_index % 2 == 0) {
			item_rect.left = wnd_rect.left;
			item_rect.top = item_rect.bottom + 1;
		} else {
			item_rect.left = item_rect.right + 1;
		}
		item_rect.right = item_rect.left + button_width;
		item_rect.bottom = item_rect.top + button_height;
		lang_rect_list_.Add(item_rect);
	}
	//Language

	//History//
	int label_height = wnd_rect.Height() / 10;
	int label_width = wnd_rect.Width() / 2;

	history_rect_list_.RemoveAll();
	item_rect = wnd_rect;
	item_rect.bottom = item_rect.top + label_height;
	item_rect.right = item_rect.left + label_width;
	for (int item_index = 0; item_index < int(history_text_list_.GetSize()); item_index++) {

		if (item_index == 10) {
			item_rect.left = item_rect.right - 1;
			item_rect.right = item_rect.left + label_width;
			item_rect.top = wnd_rect.top;
			item_rect.bottom = item_rect.top + label_height;
		}

		history_rect_list_.Add(item_rect);

		item_rect.top = item_rect.bottom - 1;
		item_rect.bottom = item_rect.top + label_height;
	}
	//History

	int screen_width = GetSystemMetrics(SM_CXSCREEN); //Get the system metrics - VERT
	int screen_height = GetSystemMetrics(SM_CYSCREEN); //Get the system metrics - HORZ
	int window_width = client_rect_.Width();
	int window_height = client_rect_.Height();
	int offset_x = (screen_width - window_width) / 2;
	int offset_y = (screen_height - window_height) / 2;
	title_rect_.right = client_rect_.right - Formation::control_height() - Formation::spacing();
	close_rect_.left = title_rect_.right;
	close_rect_.right = client_rect_.right;

	if (language_view_) {
		CRect rect;
		GetWindowRect(rect);
		keyboard_position_.x = rect.left;
		keyboard_position_.y = rect.top;
	} else { 
		//QUERY LAST KEYBOARD POSITION FROM THE REGISTRY IF OPENED FIRST TIME
		HKEY key = NULL;
		if (RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SPAN", &key) == ERROR_SUCCESS) {
			DWORD key_type = REG_DWORD;
			DWORD size = sizeof(DWORD);
			if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
				RegQueryValueEx(key, L"FullKBL", NULL, &key_type, (LPBYTE)&keyboard_position_.x, &size);
				RegQueryValueEx(key, L"FullKBT", NULL, &key_type, (LPBYTE)&keyboard_position_.y, &size);
			} else {
				RegQueryValueEx(key, L"NumKBL", NULL, &key_type, (LPBYTE)&keyboard_position_.x, &size);
				RegQueryValueEx(key, L"NumKBT", NULL, &key_type, (LPBYTE)&keyboard_position_.y, &size);
			}
			RegCloseKey(key);
		}
	}
	if (keyboard_position_.x + window_width > screen_width) {
		keyboard_position_.x = 0;
	}
	if (keyboard_position_.y + window_height > screen_height) {
		keyboard_position_.y = 0;
	}
	if (keyboard_position_.x == 0 && keyboard_position_.y == 0) {
		keyboard_position_.SetPoint(offset_x, offset_y);
	}

	if (fresh_popup) {
		SetWindowPos(&CWnd::wndTop, keyboard_position_.x, keyboard_position_.y, client_rect_.Width() + GetSystemMetrics(SM_CXDLGFRAME) + Formation::spacing2(), client_rect_.Height() + GetSystemMetrics(SM_CYDLGFRAME) + Formation::spacing2(), SWP_HIDEWINDOW);
		AnimateWindow(200, AW_CENTER);
	} else {
		MoveWindow(keyboard_position_.x, keyboard_position_.y, client_rect_.Width() + GetSystemMetrics(SM_CXDLGFRAME) + Formation::spacing2(), client_rect_.Height() + GetSystemMetrics(SM_CYDLGFRAME) + Formation::spacing2());
	}

	Invalidate();
}

void KeyboardDlg::DrawRoundedRectangle(Gdiplus::Graphics& g, CRect r, int d, Gdiplus::Brush* brush) {

	Gdiplus::GraphicsPath gp;

	gp.AddArc(r.left, r.top, d, d, 180, 90);
	gp.AddArc(r.left + r.Width() - d, r.top, d, d, 270, 90);
	gp.AddArc(r.left + r.Width() - d, r.top + r.Height() - d, d, d, 0, 90);
	gp.AddArc(r.left, r.top + r.Height() - d, d, d, 90, 90);
	gp.AddLine(r.left, r.top + r.Height() - d, r.left, r.top + d / 2);

	if (brush == Formation::whitecolor_brush_gdi()) {
		g.DrawPath(Formation::spancolor1_pen_gdi(), &gp);
	} else {
		g.FillPath(brush, &gp);
	}
}

void KeyboardDlg::DrawRoundedRectangle(Gdiplus::Graphics& g, CRect r, int d, Gdiplus::Pen* pen) {

	Gdiplus::GraphicsPath gp;

	gp.AddArc(r.left, r.top, d, d, 180, 90);
	gp.AddArc(r.left + r.Width() - d, r.top, d, d, 270, 90);
	gp.AddArc(r.left + r.Width() - d, r.top + r.Height() - d, d, d, 0, 90);
	gp.AddArc(r.left, r.top + r.Height() - d, d, d, 90, 90);
	gp.AddLine(r.left, r.top + r.Height() - d, r.left, r.top + d / 2);

	g.DrawPath(pen, &gp);
}

void KeyboardDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	
	CRect client_rect;
	GetClientRect(&client_rect);

	COfflineDC offdc(&dc, &client_rect);
	offdc.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	CGdiPlusBitmapResource bitmap_gdi;
	int icon_size = Formation::icon_size(Formation::BIG_ICON);

	//LANGUAGE VIEW
	if (language_view_ || history_view_) { 
		offdc.FillSolidRect(client_rect, LABEL_COLOR3);
	}

	//TITLE
	offdc.FillSolidRect(title_rect_, BACKCOLOR1);
	offdc.FillSolidRect(close_rect_, close_button_clicked_ ? BACKCOLOR1_SEL : BACKCOLOR1);
	offdc.SetTextColor(WHITE_COLOR);
	offdc.SelectObject(Formation::font(Formation::BIG_FONT));
	offdc.DrawText(L" " + title_text_, title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	int x = close_rect_.left + (close_rect_.Width() - icon_size) / 2;
	int y = close_rect_.top + (close_rect_.Height() - icon_size) / 2;
	if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_KEYBOARD_CLOSE), L"USERICN", g_resource_handle)) {
		graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
		bitmap_gdi.Empty();
	}
	
	icon_size = Formation::icon_size(Formation::SMALL_ICON);

	//PASSWORD BUTTON
	if (password_style_ && !language_view_) { //If current view is language view then no need to draw Password button
		int x = show_password_rect_.left + (show_password_rect_.Width() - icon_size) / 2;
		int y = show_password_rect_.top + (show_password_rect_.Height() - icon_size) / 2;
		if (bitmap_gdi.Load(show_password_ ? MAKEINTRESOURCE(IDI_KEYBOARD_HIDEPASSWORD) : MAKEINTRESOURCE(IDI_KEYBOARD_SHOWPASSWORD), L"USERICN", g_resource_handle)) {
			graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
			bitmap_gdi.Empty();
		}
	}

	//HISTORY BUTTON
	if(show_history_view_ && !password_style_) { //If password style then no need to draw History button

		if (history_view_) {
			offdc.FillSolidRect(history_view_rect_, BACKCOLOR1_SEL);
		}

		int x = history_view_rect_.left + (history_view_rect_.Width() - icon_size) / 2;
		int y = history_view_rect_.top + (history_view_rect_.Height() - icon_size) / 2;
		if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_KEYBOARD_HISTORY), L"USERICN", g_resource_handle)) {
			graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
			bitmap_gdi.Empty();
		}

		if (language_view_ || history_view_) {
			offdc.MoveTo(CPoint(history_view_rect_.left - Formation::spacing2() + 1, y));
			offdc.LineTo(CPoint(history_view_rect_.left - Formation::spacing2() + 1, y + icon_size));
		}
	}

	//LANGUAGE BUTTON
	if (multilingual_) {

		if (language_view_) {
			offdc.FillSolidRect(language_view_rect_, BACKCOLOR1_SEL);
		}

		int x = language_view_rect_.left + (language_view_rect_.Width() - icon_size) / 2;
		int y = language_view_rect_.top + (language_view_rect_.Height() - icon_size) / 2;
		if (bitmap_gdi.Load(MAKEINTRESOURCE(IDI_KEYBOARD_LANGUAGE), L"USERICN", g_resource_handle)) {
			graphics.DrawImage(bitmap_gdi, x, y, icon_size, icon_size);
			bitmap_gdi.Empty();
		}

		offdc.SelectObject(Formation::scrollcolor_pen());
		offdc.MoveTo(CPoint(language_view_rect_.left - Formation::spacing2() + 1, y));
		offdc.LineTo(CPoint(language_view_rect_.left - Formation::spacing2() + 1, y + icon_size));
	}

	if (language_view_) { //LANGUAGE VIEW

		offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));

		CRect rect = edit_rect_;
		if (password_style_) {
			rect.right = language_view_rect_.left - Formation::spacing2();
		}
		rect.right -= Formation::spacing();
		offdc.SetTextColor(BLACK_COLOR);
		offdc.DrawText(Language::GetString(IDSTRINGT_LANGUAGE), rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		offdc.SelectObject(Formation::font(Formation::SMALL_FONT));
		offdc.SelectObject(Formation::spancolor1_pen());
		offdc.SetTextColor(WHITE_COLOR);

		for (INT_PTR item_index = 0; item_index < lang_rect_list_.GetSize(); item_index++) {

			//Item Rect Selection
			if (selected_lang_index_ == item_index) {
				offdc.SelectObject(Formation::spancolor1_brush_for_selection());
			} else {
				offdc.SelectObject(Formation::spancolor1_brush());
			}
			offdc.Rectangle(lang_rect_list_[item_index]);

			//Text Rect
			CRect text_rect1(lang_rect_list_[item_index]);
			CRect text_rect2(lang_rect_list_[item_index]);
			text_rect1.left = text_rect2.left = lang_rect_list_[item_index].left + Formation::spacing4();
			if (lang_id_list_[item_index] == LANG_ENGLISH) {
				offdc.DrawText(language_list_in_eng_[item_index], text_rect2, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			} else {
				text_rect1.bottom = text_rect1.top + text_rect1.Height() / 2;
				text_rect2.top = text_rect1.bottom;
				offdc.DrawText(language_list_[item_index], text_rect1, DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
				offdc.DrawText(language_list_in_eng_[item_index], text_rect2, DT_LEFT | DT_SINGLELINE | DT_TOP);
			}
		}

	}  else if (history_view_) { //HISTORY VIEW

		offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));

		CRect rect = edit_rect_;
		rect.right -= Formation::spacing();
		offdc.SetTextColor(BLACK_COLOR);
		offdc.DrawText(Language::GetString(IDSTRINGT_HISTORY), rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		offdc.SelectObject(Formation::spancolor1_pen());

		//int text_index = int(history_text_list_.GetSize()) - 1;
		for (INT_PTR item_index = 0; item_index < history_text_list_.GetSize(); item_index++) {

			//Item Rect Selection
			if (selected_history_index_ == item_index) {
				offdc.SelectObject(Formation::spancolor1_brush_for_selection());
				offdc.SetTextColor(WHITE_COLOR);
			} else {
				offdc.SelectObject(GetStockObject(NULL_BRUSH));
				offdc.SetTextColor(BLACK_COLOR);
			}
			offdc.Rectangle(history_rect_list_[item_index]);

			//Text Rect
			CRect text_rect(history_rect_list_[item_index]);
			text_rect.left = history_rect_list_[item_index].left + Formation::spacing();
			offdc.DrawText(history_text_list_[item_index], text_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		}

	} else { //KEYBOARD

		CRect rect(edit_rect_.left - 2, edit_rect_.top - 2, edit_rect_.right + 2, edit_rect_.bottom + 2);
		offdc.SelectObject(Formation::spancolor1_pen());
		offdc.Rectangle(rect);

		offdc.SelectObject(Formation::font(Formation::BIG_FONT));

		for (int index = 0; index < chars_array[chars_array_index_].GetSize(); index++) {

			if (!alphabets_rect_[index].IsRectEmpty()) {

				offdc.SetTextColor(WHITE_COLOR);
				if (button_index_ == index) {
					DrawRoundedRectangle(graphics, alphabets_rect_[index], Formation::control_height() / 2, Formation::spancolor1_brush_for_selection_gdi());
				} else {
					if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC && index < 10 || 
						(GetSpecialKeyIndex(index) != KEYBOARD_SPECIAL_KEYS::KB_NONE && GetSpecialKeyIndex(index) != KEYBOARD_SPECIAL_KEYS::KB_OK)) {
						offdc.SetTextColor(BACKCOLOR1);
						DrawRoundedRectangle(graphics, alphabets_rect_[index], Formation::control_height() / 2, Formation::whitecolor_brush_gdi());
					} else if (GetSpecialKeyIndex(index) == KEYBOARD_SPECIAL_KEYS::KB_OK) {
						DrawRoundedRectangle(graphics, alphabets_rect_[index], Formation::control_height() / 2, Formation::spancolor2_brush_gdi());
					} else {
						DrawRoundedRectangle(graphics, alphabets_rect_[index], Formation::control_height() / 2, Formation::spancolor1_brush_gdi());
					}
				}
				
				if (GetSpecialKeyIndex(index) == KEYBOARD_SPECIAL_KEYS::KB_CAPS) {
					graphics.DrawPolygon(Formation::spancolor1_pen_gdi(), caps_lock_points_, 7);
				} else if (GetSpecialKeyIndex(index) == KEYBOARD_SPECIAL_KEYS::KB_BACK) {
					graphics.DrawPolygon(Formation::spancolor1_pen_gdi(), back_space_points_, 5);
					offdc.SetTextColor(BACKCOLOR1);
					CRect rectx(back_space_points_[1].X, back_space_points_[1].Y, back_space_points_[3].X, back_space_points_[3].Y);
					offdc.DrawText(L"×", rectx, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				} else if (GetSpecialKeyIndex(index) == KEYBOARD_SPECIAL_KEYS::KB_SPACE) {
					offdc.DrawText(space_text_list_[selected_lang_index_], space_text_list_[selected_lang_index_].GetLength(), alphabets_rect_[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				} else if (GetSpecialKeyIndex(index) == KEYBOARD_SPECIAL_KEYS::KB_OK) {
					graphics.FillPolygon(Formation::whitecolor_brush_gdi(), ok_button_points_, 3);
					offdc.SelectObject(&Formation::whitecolor_pen2());
					offdc.MoveTo(ok_button_points_[2].X, ok_button_points_[1].Y + 1);
					offdc.LineTo(alphabets_rect_[index].right - Formation::spacing3(), ok_button_points_[1].Y + 1);
					offdc.LineTo(alphabets_rect_[index].right - Formation::spacing3(), ok_button_points_[1].Y - Formation::spacing2());
				} else {
					if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
						if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
							if (chars_array_index_ == 0 || chars_array_index_ == 1) {
								if ((index >= 10 && index <= 20) || index == 21 || index == 31) {
									if (chars_array[chars_array_index_].GetAt(10) != L"अ") {
										offdc.SetTextColor(BACKCOLOR2);
									}
								}
							}
						}
					}
					offdc.DrawText(chars_array[chars_array_index_].GetAt(index), chars_array[chars_array_index_].GetAt(index).GetLength(), alphabets_rect_[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				}
			}
		}

		//FOR POSSIBLE CHARACTERS
		offdc.SetTextColor(BACKCOLOR2);
		for (int index = 0; index < possible_chars_.GetSize(); index++) {
			if (possible_char_button_index_ == index) {
				DrawRoundedRectangle(graphics, possible_chars_rect_[index], Formation::control_height() / 2, Formation::spancolor1_brush_for_selection_gdi());
			} else {
				DrawRoundedRectangle(graphics, possible_chars_rect_[index], Formation::control_height() / 2, Formation::spancolor1_brush_gdi());
			}
			offdc.DrawText(possible_chars_.GetAt(index), possible_chars_.GetAt(index).GetLength(), possible_chars_rect_[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
}

void CALLBACK TimerFunction(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2) {

	KeyboardDlg* keyboard_dlg = (KeyboardDlg*)dwUser;
	keyboard_dlg->TimerFired(wTimerID);
}

void KeyboardDlg::TimerFired(UINT wTimerID)
{
	if (keyup_) return; //If timer fires on mouse up event

	ReleaseCapture();

	LASTINPUTINFO li;
	li.cbSize = sizeof(LASTINPUTINFO);
	::GetLastInputInfo(&li);

	// Calculate the time elapsed in seconds.
	int elapsed = (GetTickCount() - li.dwTime) / 1000;
	if (elapsed >= 0) {
		if (possible_chars_timer_event_ == wTimerID) {
					
			if (button_index_ != -1) {
				CString button_text = chars_array[chars_array_index_].GetAt(button_index_);
			
				possible_chars_.RemoveAll();
				possible_chars_rect_.RemoveAll();
				for (int possible_char_index = 0; possible_char_index < possible_chars_array_.size(); possible_char_index++) {
					if (button_text == possible_chars_array_[possible_char_index].first) {
						possible_chars_.Add(possible_chars_array_[possible_char_index].second);
					}
				}
				if (possible_chars_.GetSize() > 0) {

					keydown_ = FALSE;

					int start_index = button_index_;
					int end_index = buttons_in_row_[0] + buttons_in_row_[1] + (buttons_in_row_[2] - 2/*Caps and Back Keys*/) + (buttons_in_row_[3] - 3/*Symbol, space and enter keys*/) - 1;
					if (start_index + possible_chars_.GetSize() > end_index) {
						for (INT_PTR possible_char_index = 0; possible_char_index < possible_chars_.GetSize(); possible_char_index++) {
							--start_index;
							while (GetSpecialKeyIndex(start_index) != KeyboardDlg::KEYBOARD_SPECIAL_KEYS::KB_NONE) {
								--start_index;
							}
						}
						start_index = start_index + 1;
					}
					int rect_index = start_index;
					int index = 0;
					while (index < possible_chars_.GetSize()) {
						if (GetSpecialKeyIndex(rect_index) == KeyboardDlg::KEYBOARD_SPECIAL_KEYS::KB_NONE) {
							possible_chars_rect_.Add(alphabets_rect_[rect_index]);
							++index;
						}
						++rect_index;
					}

					button_index_ = -1;
					possible_char_button_index_ = -1;
					InvalidateRect(bottom_rect_, FALSE);
				}
			}

		} else if (back_key_timer_event_ == wTimerID) {

			BackKeyClicked();
		}
	}
}

void KeyboardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	//CLOSE BUTTON
	close_button_clicked_ = FALSE;
	if (close_rect_.PtInRect(point)) {
		close_button_clicked_ = TRUE;
		InvalidateRect(close_rect_, FALSE);
		return;
	}

	//LANGUAGE VIEW
	if (language_view_) {
		for (INT_PTR item_index = 0; item_index < lang_rect_list_.GetSize(); item_index++) {
			if (lang_rect_list_[item_index].PtInRect(point)) {
				selected_lang_index_ = (int)item_index;
				InvalidateRect(client_rect_, FALSE);
				break;
			}
		}
		return;
	}

	//HISTORY VIEW
	if (history_view_) {
		for (INT_PTR item_index = 0; item_index < history_rect_list_.GetSize(); item_index++) {
			if (history_rect_list_[item_index].PtInRect(point)) {
				selected_history_index_ = (int)item_index;
				InvalidateRect(client_rect_, FALSE);
				break;
			}
		}
		return;
	}

	//PASSWORD BUTTON
	show_password_button_clicked_ = FALSE;
	if (show_password_rect_.PtInRect(point)) {
		show_password_button_clicked_ = TRUE;
		InvalidateRect(show_password_rect_, FALSE);
		return;
	}

	keydown_ = FALSE;

	button_index_ = -1;
	for (int i = 0; i < alphabets_rect_.GetSize(); i++) {
		if (alphabets_rect_[i].PtInRect(point)) {
			if (GetSpecialKeyIndex(i) == KEYBOARD_SPECIAL_KEYS::KB_CAPS) {
				if (chars_array_index_ == 0 || chars_array_index_ == 1) {
					chars_array_index_ = !chars_array_index_;
				} else {
					chars_array_index_ = 0;
				}
			}
			if (GetSpecialKeyIndex(i) == KEYBOARD_SPECIAL_KEYS::KB_SYMBOL) {
				if (chars_array_index_ == 0 || chars_array_index_ == 1) {
					chars_array_index_ = 2;
				} else {
					chars_array_index_ = 0;
				}
			}
			button_index_ = i;
			break;
		}
	}
	possible_char_button_index_ = -1;
	for (int i = 0; i < possible_chars_rect_.GetSize(); i++) {
		if (possible_chars_rect_[i].PtInRect(point)) {
			button_index_ = -1;
			possible_char_button_index_ = i;
			break;
		}
	}

	if (keyboard_type_ == KEYBOARD_TYPE::KB_NUMERIC) {
		if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NUMERIC && button_index_ == 9 || //Pressed - button
			keyboard_type_ == KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC && button_index_ == 11) { //Pressed . button
			button_index_ = -1;
			return;
		}
	}

	InvalidateRect(bottom_rect_, FALSE);

	keydown_ = TRUE;
	keyup_ = FALSE;

	last_chars_array_index_ = chars_array_index_;

	down_point_ = point;

	KEYBOARD_SPECIAL_KEYS special_key = GetSpecialKeyIndex(button_index_);
	if (special_key == KEYBOARD_SPECIAL_KEYS::KB_BACK) {
		current_caret_pos_ = ctrEdit.CharFromPos(ctrEdit.GetCaretPos());
		BackKeyClicked();
		timeKillEvent(back_key_timer_event_);
		back_key_timer_event_ = timeSetEvent(100, 0, (LPTIMECALLBACK)TimerFunction, (DWORD_PTR)this, TIME_PERIODIC);
	} else {
		timeKillEvent(possible_chars_timer_event_);
		possible_chars_timer_event_ = timeSetEvent(400, 0, (LPTIMECALLBACK)TimerFunction, (DWORD_PTR)this, TIME_ONESHOT);
	}

	//CDialogEx::OnLButtonDown(nFlags, point);
}

void KeyboardDlg::CharClicked(CString character) {

	if (character == L"&&") character = L"&";

	CString button_text = character;
	ctrEdit.GetWindowText(edit_text_);
	int text_len = edit_text_.GetLength();
	
	int startpos = 0, endpos = 0;
	ctrEdit.GetSel(startpos, endpos);

	if (startpos != endpos) {

		CString temp_string  = _T("");
		for (int j = 0; j < startpos; j++) {
			temp_string.AppendChar(edit_text_.GetAt(j));
		}
		temp_string.AppendChar(button_text.GetAt(0));
		for (int i = endpos; i < text_len; i++) {
			temp_string.AppendChar(edit_text_.GetAt(i));
		}
		edit_text_.Format(_T("%s"), temp_string);
		ctrEdit.SetWindowText(edit_text_);
		ctrEdit.SetSel(startpos + 1, startpos + 1);
		OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);

	} else {

		if (text_len >= text_limit_) {
			return;
		}
		int char_pos = ctrEdit.CharFromPos(ctrEdit.GetCaretPos());
		//wprintf(L"CHAR POS %d\n", char_pos);
		CString temp_string = button_text;
		if (char_pos != text_len) {
			int i = 0, j = 0;
			CString edit_string = edit_text_;
			edit_text_ = _T("");
			while (edit_string.GetAt(i) != '\0' && i < text_limit_) {
				if (i == (char_pos)) {
					//edit_text_.AppendChar(temp_string.GetAt(0));
					edit_text_.Append(temp_string);
				} else {
					if (i > char_pos) {
						edit_text_.AppendChar(edit_string.GetAt(j - 1));
					} else {
						edit_text_.AppendChar(edit_string.GetAt(j));
					}
				}
				i++;
				j++;
			}
			edit_text_.AppendChar(edit_string.GetAt(j - 1));
		} else {
			//edit_text_.AppendChar(temp_string.GetAt(0));
			edit_text_.Append(temp_string);
		}
		ctrEdit.SetWindowText(edit_text_);
		ctrEdit.SetSel(char_pos + 1, char_pos + 1);
		OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);
	}
	
	possible_chars_.RemoveAll();
	possible_chars_rect_.RemoveAll();

	button_index_ = -1;
	InvalidateRect(bottom_rect_, FALSE);
}

void KeyboardDlg::BackKeyClicked() {

	ctrEdit.GetWindowText(edit_text_);
	int char_pos = --current_caret_pos_;// ctrEdit.CharFromPos(ctrEdit.GetCaretPos()) - 1;
	if (char_pos >= 0) {
		int i = 0;
		//Fill till last of matched character
		CString temp_string = _T("");
		while (edit_text_.GetAt(i) != '\0') {
			if (i != char_pos) {
				temp_string.AppendChar(edit_text_.GetAt(i));
			} else {
				if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
					if (temp_string.GetLength() > 0) {
						CString str_alphabet = CString(edit_text_.GetAt(i));
						if (!str_alphabet.IsEmpty() && str_alphabet != L" " && hindi_chars_.Find(str_alphabet) != -1) {
							//Erase all filled characters and fill till second last of matched character
							--char_pos;
							temp_string = _T("");
							int j = 0;
							while (edit_text_.GetAt(j) != '\0') {
								if (j != char_pos) {
									temp_string.AppendChar(edit_text_.GetAt(j));
								} else {
									break;
								}
								j++;
							}
						}
					}
				}
			}
			i++;
		}
		edit_text_ = temp_string;
		ctrEdit.SetWindowText(edit_text_);
		ctrEdit.SetSel(char_pos, char_pos);
		OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);
	}

	if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
		if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
			SetCharsForButtons10to20Hindi(L"अ");
		}
	}
}

void KeyboardDlg::SetCharsForButtons10to20Hindi(CString alphabet) {
		
	alphabet.Trim();
	if (alphabet.IsEmpty() || alphabet == L"अ" || hindi_chars_.Find(alphabet) != -1) { //Replace text of buttons 10 to 20, 21, 31 with L"अआइईउऊएऐओऔअंअ़अृअ्"
		for (int row_index = 0; row_index < 2; row_index++) {
			chars_array[row_index].SetAt(10, CString(hindi_chars_vowels_.GetAt(0)));
			chars_array[row_index].SetAt(11, CString(hindi_chars_vowels_.GetAt(1)));
			chars_array[row_index].SetAt(12, CString(hindi_chars_vowels_.GetAt(2)));
			chars_array[row_index].SetAt(13, CString(hindi_chars_vowels_.GetAt(3)));
			chars_array[row_index].SetAt(14, CString(hindi_chars_vowels_.GetAt(4)));
			chars_array[row_index].SetAt(15, CString(hindi_chars_vowels_.GetAt(5)));
			chars_array[row_index].SetAt(16, CString(hindi_chars_vowels_.GetAt(6)));
			chars_array[row_index].SetAt(17, CString(hindi_chars_vowels_.GetAt(7)));
			chars_array[row_index].SetAt(18, CString(hindi_chars_vowels_.GetAt(8)));
			chars_array[row_index].SetAt(19, CString(hindi_chars_vowels_.GetAt(9)));
			chars_array[row_index].SetAt(20, CString(hindi_chars_vowels_.GetAt(10)) + CString(hindi_chars_vowels_.GetAt(11))); //अं
			chars_array[row_index].SetAt(21, CString(hindi_chars_vowels_.GetAt(12)) + CString(hindi_chars_vowels_.GetAt(13))); //अृ
			chars_array[row_index].SetAt(31, CString(hindi_chars_vowels_.GetAt(14)) + CString(hindi_chars_vowels_.GetAt(15))); //अ्
		}									 
	} else { //Replace text of buttons 10 to 20, 21, 31 with L"ककाकिकीकुकूकेकैकोकौकं", etc.
		for (int row_index = 0; row_index < 2; row_index++) {
			chars_array[row_index].SetAt(10, alphabet);
			chars_array[row_index].SetAt(11, alphabet + CString(hindi_chars_.GetAt(0)));
			chars_array[row_index].SetAt(12, alphabet + CString(hindi_chars_.GetAt(1)));
			chars_array[row_index].SetAt(13, alphabet + CString(hindi_chars_.GetAt(2)));
			chars_array[row_index].SetAt(14, alphabet + CString(hindi_chars_.GetAt(3)));
			chars_array[row_index].SetAt(15, alphabet + CString(hindi_chars_.GetAt(4)));
			chars_array[row_index].SetAt(16, alphabet + CString(hindi_chars_.GetAt(5)));
			chars_array[row_index].SetAt(17, alphabet + CString(hindi_chars_.GetAt(6)));
			chars_array[row_index].SetAt(18, alphabet + CString(hindi_chars_.GetAt(7)));
			chars_array[row_index].SetAt(19, alphabet + CString(hindi_chars_.GetAt(8)));
			chars_array[row_index].SetAt(20, alphabet + CString(hindi_chars_.GetAt(9)));
			chars_array[row_index].SetAt(21, alphabet + CString(hindi_chars_.GetAt(10)));
			chars_array[row_index].SetAt(31, alphabet + CString(hindi_chars_.GetAt(11)));
		}
	}
}

void KeyboardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	keyup_ = TRUE;

	timeKillEvent(possible_chars_timer_event_);
	timeKillEvent(back_key_timer_event_);

	ReleaseCapture();

	//CLOSE BUTTON
	if (close_button_clicked_) {
		close_button_clicked_ = FALSE;
		CloseKeyboard(IDCANCEL);
		return;
	}

	//LANGUAGE BUTTON
	if (multilingual_ && language_view_rect_.PtInRect(point)) {
		history_view_ = FALSE;
		language_view_ = !language_view_;
		Invalidate();
		if (language_view_) {
			ctrEdit.MoveWindow(CRect());
			ctrEdit.EnableWindow(FALSE);
		} else {
			ctrEdit.MoveWindow(edit_rect_);
			ctrEdit.EnableWindow();
			ctrEdit.SetFocus();
		}
		return;
	}

	//LANGUAGE VIEW
	if (language_view_) {
		if (prev_lang_index_ != selected_lang_index_) {
			prev_lang_index_ = selected_lang_index_;
			lang_id_ = lang_id_list_[selected_lang_index_];
			sublang_id_ = sublang_id_list_[selected_lang_index_];
			PrepareKeys(FALSE);
			language_view_ = FALSE;
			ctrEdit.SetWindowTextW(L"");
			ctrEdit.MoveWindow(edit_rect_);
			ctrEdit.EnableWindow();
			OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);
			ctrEdit.SetFocus();
			Invalidate();
			return;
		}
	}

	//HISTORY BUTTON
	if (show_history_view_ && history_view_rect_.PtInRect(point)) {
		language_view_ = FALSE;
		history_view_ = !history_view_;
		Invalidate();
		if (history_view_) {
			ctrEdit.MoveWindow(CRect());
			ctrEdit.EnableWindow(FALSE);
		} else {
			ctrEdit.MoveWindow(edit_rect_);
			ctrEdit.EnableWindow();
			ctrEdit.SetFocus();
		}
		return;
	}

	//HISTORY VIEW
	if (history_view_) {
		if (selected_history_index_ != -1) {
			CString text = history_text_list_[selected_history_index_];
			history_view_ = FALSE;
			ctrEdit.SetWindowTextW(text);
			ctrEdit.MoveWindow(edit_rect_);
			ctrEdit.EnableWindow();
			ctrEdit.SetSel(text.GetLength() + 1, text.GetLength() + 1);
			OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);
			ctrEdit.SetFocus();
		}
		selected_history_index_ = -1;
		Invalidate();
		return;
	}

	//PASSWORD BUTTON
	if (show_password_button_clicked_) {
		show_password_ = !show_password_;
		if (show_password_) {
			ctrEdit.SetPasswordChar(0);
		} else {
			//ctrEdit.SetPasswordChar('*');
			ctrEdit.SetPasswordChar(0x25CF);
		}
		ctrEdit.RedrawWindow();
		ctrEdit.SetSel(-1);
		show_password_button_clicked_ = FALSE;
		InvalidateRect(show_password_rect_, FALSE);
		return;
	}

	if (!keydown_) return; //If timer fires on mouse down event

	KEYBOARD_SPECIAL_KEYS special_key = GetSpecialKeyIndex(button_index_);

	if (special_key == KEYBOARD_SPECIAL_KEYS::KB_NONE) {
		
		if (button_index_ != -1 || possible_char_button_index_ != -1) {
			if (button_index_ != -1) {
				CString str_alphabet = chars_array[chars_array_index_].GetAt(button_index_);
				if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
					if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
						if (chars_array_index_ == 0 || chars_array_index_ == 1) {
							if (button_index_ >= 22 && button_index_ <= 30 || button_index_ >= 33 && button_index_ <= 41) {
								SetCharsForButtons10to20Hindi(str_alphabet);
								str_alphabet.Empty();
							} else if ((button_index_ >= 10 && button_index_ <= 20) || button_index_ == 21 || button_index_ == 31) {
								if (chars_array[chars_array_index_].GetAt(10) != L"अ") {
									SetCharsForButtons10to20Hindi(L"अ");
								}
							}
						}
					}
				}
				if (!str_alphabet.IsEmpty()) {
					CharClicked(str_alphabet);
				}
			} else if (possible_char_button_index_ != -1) {
				CharClicked(possible_chars_.GetAt(possible_char_button_index_));
			}

			if (ok_on_keypress_) {
				CloseKeyboard(IDOK);
			}
		}

	} else if (special_key == KEYBOARD_SPECIAL_KEYS::KB_SPACE) {

		ctrEdit.GetWindowText(edit_text_);
		int text_len = edit_text_.GetLength();
		if (text_len < text_limit_) {
			int char_pos = ctrEdit.CharFromPos(ctrEdit.GetCaretPos());
			CString temp_string = _T(" ");

			if (char_pos != text_len) {
				int i = 0, j = 0;
				CString edit_string = edit_text_;
				edit_text_.Format(_T("%s"), _T(""));
				while (edit_string.GetAt(i) != '\0' && i < text_limit_) {
					if (i == (char_pos))
						edit_text_.AppendChar(temp_string.GetAt(0));
					else
					if (i>char_pos)
						edit_text_.AppendChar(edit_string.GetAt(j - 1));
					else
						edit_text_.AppendChar(edit_string.GetAt(j));
					i++;
					j++;
				}
				edit_text_.AppendChar(edit_string.GetAt(j - 1));
			} else {
				edit_text_.AppendChar(temp_string.GetAt(0));
			}
			ctrEdit.SetWindowText(edit_text_);
			ctrEdit.SetSel(char_pos + 1, char_pos + 1);
			OnEditCharChangeMessage(WPARAM(&ctrEdit), 0);
		}
	
		if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
			if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
				SetCharsForButtons10to20Hindi(L"अ");
			}
		}

	} else if (special_key == KEYBOARD_SPECIAL_KEYS::KB_OK) {

		OkClicked();
	}

	possible_chars_.RemoveAll();
	possible_chars_rect_.RemoveAll();

	button_index_ = -1;
	InvalidateRect(bottom_rect_, FALSE);

	keyup_ = FALSE;

	//CDialogEx::OnLButtonUp(nFlags, point);
}

void KeyboardDlg::OkClicked() {

	ctrEdit.GetWindowTextW(edit_text_);
	CString entered_text = edit_text_;
	entered_text.Trim();
	if (entered_text.IsEmpty()) {
		edit_text_.Empty();
	}

	BOOL end_dialog = TRUE;
	if (!holder_.IsEmpty()) {
		if (edit_text_.GetLength() > 0 && holder_.Find(edit_text_.GetAt(edit_text_.GetLength() - 1)) != -1) {
			Formation::MsgBox(Language::GetString(IDSTRINGM_FOLLOWING_CHARACTERS, holder_));
			return;
		}
	/*	for (int i = 0; i < edit_text_.GetLength(); i++) {
			if (holder_.Find(edit_text_.GetAt(i)) != -1) {
				end_dialog = FALSE;
				Formation::MsgBox(Language::GetString(IDSTRINGM_FOLLOWING_CHARACTERS, holder_));
				break;
			}
		}*/
	}
	if (end_dialog) {
		CloseKeyboard(IDOK);
	}
}

KeyboardDlg::KEYBOARD_SPECIAL_KEYS KeyboardDlg::GetSpecialKeyIndex(int button_index) {

	if (button_index >= 0 && button_index < chars_array[chars_array_index_].GetSize()) {
		if (chars_array[chars_array_index_][button_index] == L"CAPS") {
			return KEYBOARD_SPECIAL_KEYS::KB_CAPS;
		} else if (chars_array[chars_array_index_][button_index] == L"BACK") {
			return KEYBOARD_SPECIAL_KEYS::KB_BACK;
		} else if (chars_array[chars_array_index_][button_index] == L"+?=" || chars_array[chars_array_index_][button_index] == L"abc" ||
			chars_array[chars_array_index_][button_index] == L"कख") {
			return KEYBOARD_SPECIAL_KEYS::KB_SYMBOL;
		} else if (chars_array[chars_array_index_][button_index] == L"SPACE") {
			return KEYBOARD_SPECIAL_KEYS::KB_SPACE;
		} else if (chars_array[chars_array_index_][button_index] == L"OK") {
			return KEYBOARD_SPECIAL_KEYS::KB_OK;
		}
	}
	return KEYBOARD_SPECIAL_KEYS::KB_NONE;
}

void KeyboardDlg::CloseKeyboard(int okcancel) {

	//UPDATE KEYBOARD POSITION INTO REGISTRY
	CRect rect;
	GetWindowRect(rect);
	keyboard_position_.x = rect.left;
	keyboard_position_.y = rect.top	;

	HKEY key = NULL;
	if (RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SPAN", &key) == ERROR_SUCCESS) {
		DWORD size = sizeof(DWORD);
		if (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) {
			RegSetValueEx(key, L"FullKBL", NULL, REG_DWORD, (LPBYTE)&keyboard_position_.x, size);
			RegSetValueEx(key, L"FullKBT", NULL, REG_DWORD, (LPBYTE)&keyboard_position_.y, size);
		} else {
			RegSetValueEx(key, L"NumKBL", NULL, REG_DWORD, (LPBYTE)&keyboard_position_.x, size);
			RegSetValueEx(key, L"NumKBT", NULL, REG_DWORD, (LPBYTE)&keyboard_position_.y, size);
		}
		RegCloseKey(key);
	}

	if (okcancel == IDOK && keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC && !password_style_) {

		ctrEdit.GetWindowText(edit_text_);
		edit_text_.Trim();
		int remove_text_index = 0;
		for (int index = 0; index < int(history_text_list_.GetSize()); index++) {
			if (history_text_list_[index] == edit_text_) {
				remove_text_index = index;
			}
		}
		history_text_list_.RemoveAt(remove_text_index);
		history_text_list_.Add(edit_text_);

		CString history_text;
		for (int index = 0; index < int(history_text_list_.GetSize()); index++) {
			history_text += history_text_list_[index];
			history_text += L",";
		}

		HKEY key = NULL;
		if (RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SPAN", &key) == ERROR_SUCCESS) {
			RegSetValueEx(key, L"FullKBHistory", NULL, REG_SZ, (LPBYTE)history_text.GetBuffer(), ((DWORD)wcslen(history_text) + 1) * sizeof(wchar_t));
			RegCloseKey(key);
		}
	}

	//AnimateWindow(200, AW_HIDE | AW_CENTER);
	EndDialog(okcancel);
}

void KeyboardDlg::SetKeyboardType(KEYBOARD_TYPE keyboard_type) {

	keyboard_type_ = keyboard_type;
	if (keyboard_type_ == KEYBOARD_TYPE::KB_NUMERIC) {
		holder_ = L".-"; //holder_ = L"1234567890"; //`~!@#$%^&*()_+|=qwertyuiop[]asdfghjkl;"'zxcvbnm,./ QWERTYUIOP{}ASDFGHJKL:/\ZXCVBNM<>?;
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
		holder_ = L"-"; //holder_ = L".1234567890";
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
		holder_ = L"."; //holder_ = L"-1234567890";
	} else if (keyboard_type_ == KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC) {
		holder_ = L""; //holder_ = L"-.1234567890";
	}
}

int KeyboardDlg::OpenKeyboard(CString title_text, CString edit_text, BOOL password_style, BOOL disable_edit_box, BOOL multilingual) {

	title_text_ = title_text.Trim();
	edit_text_ = edit_text;
	password_style_ = password_style;
	disable_edit_box_ = disable_edit_box;
	multilingual_ = multilingual;
	show_history_view_ = (keyboard_type_ == KEYBOARD_TYPE::KB_ALPHANUMERIC) ? TRUE : FALSE;

	HINSTANCE hinstance = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	int result = Formation::OpenDialog(this); //DoModal();
	AfxSetResourceHandle(hinstance);
	
	return result;
}

int KeyboardDlg::FillChars() {

	chars_array[0].RemoveAll();
	chars_array[1].RemoveAll();
	chars_array[2].RemoveAll();

	if (lang_id_ == LANG_ENGLISH && sublang_id_ == SUBLANG_ENGLISH_US ||
		lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
		lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
		lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
		lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
		lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
		lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
		lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN ||
		lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {

		//LINE 1
		chars_array[0].Add(L"1"); chars_array[0].Add(L"2"); chars_array[0].Add(L"3"); chars_array[0].Add(L"4"); chars_array[0].Add(L"5");
		chars_array[0].Add(L"6"); chars_array[0].Add(L"7"); chars_array[0].Add(L"8"); chars_array[0].Add(L"9"); chars_array[0].Add(L"0");
		//LINE 2
		chars_array[0].Add(L"q"); chars_array[0].Add(L"w"); chars_array[0].Add(L"e"); chars_array[0].Add(L"r"); chars_array[0].Add(L"t");
		chars_array[0].Add(L"y"); chars_array[0].Add(L"u"); chars_array[0].Add(L"i"); chars_array[0].Add(L"o"); chars_array[0].Add(L"p");
		//LINE 3
		chars_array[0].Add(L"a"); chars_array[0].Add(L"s"); chars_array[0].Add(L"d"); chars_array[0].Add(L"f"); chars_array[0].Add(L"g");
		chars_array[0].Add(L"h"); chars_array[0].Add(L"j"); chars_array[0].Add(L"k"); chars_array[0].Add(L"l");
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			chars_array[0].Add(L"ñ");
		}
		//LINE 4
		chars_array[0].Add(L"CAPS"); chars_array[0].Add(L"z"); chars_array[0].Add(L"x"); chars_array[0].Add(L"c"); chars_array[0].Add(L"v");
		chars_array[0].Add(L"b"); chars_array[0].Add(L"n"); chars_array[0].Add(L"m"); chars_array[0].Add(L"."); chars_array[0].Add(L"BACK");
		//LINE 5
		chars_array[0].Add(L"+?="); chars_array[0].Add(L"-"); chars_array[0].Add(L"SPACE"); chars_array[0].Add(L"/"); chars_array[0].Add(L"OK");

		//LINE 1
		chars_array[1].Add(L"1"); chars_array[1].Add(L"2"); chars_array[1].Add(L"3"); chars_array[1].Add(L"4"); chars_array[1].Add(L"5");
		chars_array[1].Add(L"6"); chars_array[1].Add(L"7"); chars_array[1].Add(L"8"); chars_array[1].Add(L"9"); chars_array[1].Add(L"0");
		//LINE 2
		chars_array[1].Add(L"Q"); chars_array[1].Add(L"W"); chars_array[1].Add(L"E"); chars_array[1].Add(L"R"); chars_array[1].Add(L"T");
		chars_array[1].Add(L"Y"); chars_array[1].Add(L"U"); chars_array[1].Add(L"I"); chars_array[1].Add(L"O"); chars_array[1].Add(L"P");
		//LINE 3
		chars_array[1].Add(L"A"); chars_array[1].Add(L"S"); chars_array[1].Add(L"D"); chars_array[1].Add(L"F"); chars_array[1].Add(L"G");
		chars_array[1].Add(L"H"); chars_array[1].Add(L"J"); chars_array[1].Add(L"K"); chars_array[1].Add(L"L");
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			chars_array[1].Add(L"Ñ");
		}
		//LINE 4
		chars_array[1].Add(L"CAPS"); chars_array[1].Add(L"Z"); chars_array[1].Add(L"X"); chars_array[1].Add(L"C"); chars_array[1].Add(L"V");
		chars_array[1].Add(L"B"); chars_array[1].Add(L"N"); chars_array[1].Add(L"M"); chars_array[1].Add(L"."); chars_array[1].Add(L"BACK");
		//LINE 5
		chars_array[1].Add(L"+?="); chars_array[1].Add(L"-"); chars_array[1].Add(L"SPACE"); chars_array[1].Add(L"/"); chars_array[1].Add(L"OK");

		//LINE 1
		chars_array[2].Add(L"1"); chars_array[2].Add(L"2"); chars_array[2].Add(L"3"); chars_array[2].Add(L"4"); chars_array[2].Add(L"5");
		chars_array[2].Add(L"6"); chars_array[2].Add(L"7"); chars_array[2].Add(L"8"); chars_array[2].Add(L"9"); chars_array[2].Add(L"0");
		//LINE 2
		chars_array[2].Add(L"~"); chars_array[2].Add(L"^"); chars_array[2].Add(L"&&"); chars_array[2].Add(L"\""); chars_array[2].Add(L"{");
		chars_array[2].Add(L"}"); chars_array[2].Add(L"["); chars_array[2].Add(L"]"); chars_array[2].Add(L"<"); chars_array[2].Add(L">");
		//LINE 3
		chars_array[2].Add(L"@"); chars_array[2].Add(L"#"); chars_array[2].Add(L"$"); chars_array[2].Add(L"%"); chars_array[2].Add(L"*");
		chars_array[2].Add(L"("); chars_array[2].Add(L")"); chars_array[2].Add(L"-"); chars_array[2].Add(L"+");
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			chars_array[2].Add(L"×");
		}
		//LINE 4
		chars_array[2].Add(L"CAPS"); chars_array[2].Add(L"_"); chars_array[2].Add(L"|"); chars_array[2].Add(L"?"); chars_array[2].Add(L"!");
		chars_array[2].Add(L"="); chars_array[2].Add(L"\\"); chars_array[2].Add(L","); chars_array[2].Add(L"°"); chars_array[2].Add(L"BACK");
		//LINE 5
		chars_array[2].Add(L"abc"); chars_array[2].Add(L";"); chars_array[2].Add(L"SPACE"); chars_array[2].Add(L":"); chars_array[2].Add(L"OK");

		if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			chars_array[0][15] = _T("z"); //Replace y with z
			chars_array[1][15] = _T("Z"); //Replace Y with Z
			chars_array[0][30] = _T("y"); //Replace z with y
			chars_array[1][30] = _T("Y"); //Replace Z with Y
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			chars_array[1][17] = _T("İ"); //Replace I with İ
			chars_array[2][22] = _T("₺"); //Replace $ with ₺
		}

		buttons_in_row_[0] = 10;
		buttons_in_row_[1] = 10;
		buttons_in_row_[2] = 9;
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			buttons_in_row_[2] = 10;
		}
		buttons_in_row_[3] = 10;
		buttons_in_row_[4] = 5;

		return 10; //Number of max keys in row
	
	} else if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
		
		//hindi_chars_ = L"ािीुूेैोौं़ृ्";
		//hindi_chars_vowels_ = L"अआइईउऊएऐओऔअंअ़अृअ्";
		hindi_chars_ = L"ािीुूेैोौंृ्";
		hindi_chars_vowels_ = L"अआइईउऊएऐओऔअंअृअ्";
		hindi_chars_consonants_ = L"कखगघचछजझटठडढणतथदधनपफबभमयरलवशषसहळक्षज्ञत्रश्र";
		
		//LINE 1
		chars_array[0].Add(L"1"); chars_array[0].Add(L"2"); chars_array[0].Add(L"3"); chars_array[0].Add(L"4"); chars_array[0].Add(L"5");
		chars_array[0].Add(L"6"); chars_array[0].Add(L"7"); chars_array[0].Add(L"8"); chars_array[0].Add(L"9"); chars_array[0].Add(L"0");
		//LINE 2 : Index 10-20
		//chars_array[0].Add(L"ा"); chars_array[0].Add(L"ि"); chars_array[0].Add(L"ी"); chars_array[0].Add(L"ु"); chars_array[0].Add(L"ू"); 
		//chars_array[0].Add(L"े"); chars_array[0].Add(L"ै"); chars_array[0].Add(L"ो"); chars_array[0].Add(L"ौ"); chars_array[0].Add(L"ं");
		//chars_array[0].Add(L"ृ"); chars_array[0].Add(L"्");
		chars_array[0].Add(L"अ"); chars_array[0].Add(L"आ"); chars_array[0].Add(L"इ"); chars_array[0].Add(L"ई"); chars_array[0].Add(L"उ"); 
		chars_array[0].Add(L"ऊ"); chars_array[0].Add(L"ए"); chars_array[0].Add(L"ऐ"); chars_array[0].Add(L"ओ"); chars_array[0].Add(L"औ"); 
		chars_array[0].Add(L"अं");
		//LINE 3 : Index 21-31
		chars_array[0].Add(L"अृ"); chars_array[0].Add(L"क"); chars_array[0].Add(L"ख"); chars_array[0].Add(L"ग"); chars_array[0].Add(L"घ"); 
		chars_array[0].Add(L"च"); chars_array[0].Add(L"छ"); chars_array[0].Add(L"ज"); chars_array[0].Add(L"झ"); chars_array[0].Add(L"ट"); 
		chars_array[0].Add(L"अ्");
		//LINE 4 : Index 32-42
		chars_array[0].Add(L"CAPS"); chars_array[0].Add(L"ठ"); chars_array[0].Add(L"ड"); chars_array[0].Add(L"ढ"); chars_array[0].Add(L"ण"); 
		chars_array[0].Add(L"त"); chars_array[0].Add(L"थ"); chars_array[0].Add(L"द"); chars_array[0].Add(L"ध"); chars_array[0].Add(L"न"); 
		chars_array[0].Add(L"BACK");
		//LINE 5
		chars_array[0].Add(L"+?="); chars_array[0].Add(L"-"); chars_array[0].Add(L"SPACE"); chars_array[0].Add(L"|"); chars_array[0].Add(L"OK");

		//LINE 1
		chars_array[1].Add(L"1"); chars_array[1].Add(L"2"); chars_array[1].Add(L"3"); chars_array[1].Add(L"4"); chars_array[1].Add(L"5");
		chars_array[1].Add(L"6"); chars_array[1].Add(L"7"); chars_array[1].Add(L"8"); chars_array[1].Add(L"9"); chars_array[1].Add(L"0");
		//LINE 2
		chars_array[1].Add(L"अ"); chars_array[1].Add(L"आ"); chars_array[1].Add(L"इ"); chars_array[1].Add(L"ई"); chars_array[1].Add(L"उ");
		chars_array[1].Add(L"ऊ"); chars_array[1].Add(L"ए"); chars_array[1].Add(L"ऐ"); chars_array[1].Add(L"ओ"); chars_array[1].Add(L"औ");
		chars_array[1].Add(L"अं");
		//LINE 3
		chars_array[1].Add(L"अृ"); chars_array[1].Add(L"प"); chars_array[1].Add(L"फ"); chars_array[1].Add(L"ब"); chars_array[1].Add(L"भ");
		chars_array[1].Add(L"म"); chars_array[1].Add(L"य"); chars_array[1].Add(L"र"); chars_array[1].Add(L"ल"); chars_array[1].Add(L"व"); 
		chars_array[1].Add(L"अ्");
		//LINE 4
		chars_array[1].Add(L"CAPS"); chars_array[1].Add(L"श"); chars_array[1].Add(L"ष"); chars_array[1].Add(L"स"); chars_array[1].Add(L"ह"); 
		chars_array[1].Add(L"ळ"); chars_array[1].Add(L"क्ष"); chars_array[1].Add(L"ज्ञ"); chars_array[1].Add(L"त्र"); chars_array[1].Add(L"श्र"); 
		chars_array[1].Add(L"BACK");
		//LINE 5
		chars_array[1].Add(L"+?="); chars_array[1].Add(L"-"); chars_array[1].Add(L"SPACE"); chars_array[1].Add(L"|"); chars_array[1].Add(L"OK");

		//LINE 1
		chars_array[2].Add(L"1"); chars_array[2].Add(L"2"); chars_array[2].Add(L"3"); chars_array[2].Add(L"4"); chars_array[2].Add(L"5");
		chars_array[2].Add(L"6"); chars_array[2].Add(L"7"); chars_array[2].Add(L"8"); chars_array[2].Add(L"9"); chars_array[2].Add(L"0");
		//LINE 2
		chars_array[2].Add(L"~"); chars_array[2].Add(L"^"); chars_array[2].Add(L"&&"); chars_array[2].Add(L"\""); chars_array[2].Add(L"{");
		chars_array[2].Add(L"}"); chars_array[2].Add(L"["); chars_array[2].Add(L"]"); chars_array[2].Add(L"<"); chars_array[2].Add(L">");
		chars_array[2].Add(L"·");
		//LINE 3
		chars_array[2].Add(L"@"); chars_array[2].Add(L"#"); chars_array[2].Add(L"$"); chars_array[2].Add(L"₹"); chars_array[2].Add(L"%"); 
		chars_array[2].Add(L"*"); chars_array[2].Add(L"("); chars_array[2].Add(L")"); chars_array[2].Add(L"-"); chars_array[2].Add(L"+");
		chars_array[2].Add(L"/");
		//LINE 4
		chars_array[2].Add(L"CAPS"); chars_array[2].Add(L"_"); chars_array[2].Add(L"|"); chars_array[2].Add(L"?"); chars_array[2].Add(L"!");
		chars_array[2].Add(L"="); chars_array[2].Add(L"\\"); chars_array[2].Add(L","); chars_array[2].Add(L"°"); chars_array[2].Add(L"ऋ");
		chars_array[2].Add(L"BACK");
		//LINE 5
		chars_array[2].Add(L"कख"); chars_array[2].Add(L";"); chars_array[2].Add(L"SPACE"); chars_array[2].Add(L":"); chars_array[2].Add(L"OK");

		buttons_in_row_[0] = 10;
		buttons_in_row_[1] = 11;
		buttons_in_row_[2] = 11;
		buttons_in_row_[3] = 11;
		buttons_in_row_[4] = 5;

		return 11; //Number of max keys in row
	
	} else if (lang_id_ == LANG_RUSSIAN && sublang_id_ == SUBLANG_RUSSIAN_RUSSIA) {
		
		//LINE 1
		chars_array[0].Add(L"1"); chars_array[0].Add(L"2"); chars_array[0].Add(L"3"); chars_array[0].Add(L"4"); chars_array[0].Add(L"5");
		chars_array[0].Add(L"6"); chars_array[0].Add(L"7"); chars_array[0].Add(L"8"); chars_array[0].Add(L"9"); chars_array[0].Add(L"0");
		//LINE 2
		chars_array[0].Add(L"й"); chars_array[0].Add(L"ц"); chars_array[0].Add(L"у"); chars_array[0].Add(L"к"); chars_array[0].Add(L"e");
		chars_array[0].Add(L"н"); chars_array[0].Add(L"г"); chars_array[0].Add(L"ш"); chars_array[0].Add(L"щ"); chars_array[0].Add(L"з");
		chars_array[0].Add(L"х");
		//LINE 3
		chars_array[0].Add(L"ɸ"); chars_array[0].Add(L"ы"); chars_array[0].Add(L"в"); chars_array[0].Add(L"а"); chars_array[0].Add(L"п");
		chars_array[0].Add(L"р"); chars_array[0].Add(L"о"); chars_array[0].Add(L"л"); chars_array[0].Add(L"д"); chars_array[0].Add(L"ж");
		chars_array[0].Add(L"э");
		//LINE 4
		chars_array[0].Add(L"CAPS"); chars_array[0].Add(L"я"); chars_array[0].Add(L"ч"); chars_array[0].Add(L"с"); chars_array[0].Add(L"м");
		chars_array[0].Add(L"и"); chars_array[0].Add(L"т"); chars_array[0].Add(L"ь"); chars_array[0].Add(L"б"); chars_array[0].Add(L"ю");
		chars_array[0].Add(L"BACK");
		//LINE 5
		chars_array[0].Add(L"+?="); chars_array[0].Add(L"-"); chars_array[0].Add(L"SPACE"); chars_array[0].Add(L"/"); chars_array[0].Add(L"OK");

		//LINE 1
		chars_array[1].Add(L"1"); chars_array[1].Add(L"2"); chars_array[1].Add(L"3"); chars_array[1].Add(L"4"); chars_array[1].Add(L"5");
		chars_array[1].Add(L"6"); chars_array[1].Add(L"7"); chars_array[1].Add(L"8"); chars_array[1].Add(L"9"); chars_array[1].Add(L"0");
		//LINE 2
		chars_array[1].Add(L"Й"); chars_array[1].Add(L"Ц"); chars_array[1].Add(L"У"); chars_array[1].Add(L"K"); chars_array[1].Add(L"E");
		chars_array[1].Add(L"H"); chars_array[1].Add(L"Г"); chars_array[1].Add(L"Ш"); chars_array[1].Add(L"Щ"); chars_array[1].Add(L"З");
		chars_array[1].Add(L"Х");
		//LINE 3
		chars_array[1].Add(L"Ф"); chars_array[1].Add(L"Ы"); chars_array[1].Add(L"B"); chars_array[1].Add(L"A"); chars_array[1].Add(L"П");
		chars_array[1].Add(L"P"); chars_array[1].Add(L"O"); chars_array[1].Add(L"Л"); chars_array[1].Add(L"Д"); chars_array[1].Add(L"Ж");
		chars_array[1].Add(L"Э");
		//LINE 4
		chars_array[1].Add(L"CAPS"); chars_array[1].Add(L"Я"); chars_array[1].Add(L"Ч"); chars_array[1].Add(L"C"); chars_array[1].Add(L"М");
		chars_array[1].Add(L"И"); chars_array[1].Add(L"T"); chars_array[1].Add(L"Ь"); chars_array[1].Add(L"Б"); chars_array[1].Add(L"Ю");
		chars_array[1].Add(L"BACK");
		//LINE 5
		chars_array[1].Add(L"+?="); chars_array[1].Add(L"-"); chars_array[1].Add(L"SPACE"); chars_array[1].Add(L"/"); chars_array[1].Add(L"OK");

		//LINE 1
		chars_array[2].Add(L"1"); chars_array[2].Add(L"2"); chars_array[2].Add(L"3"); chars_array[2].Add(L"4"); chars_array[2].Add(L"5");
		chars_array[2].Add(L"6"); chars_array[2].Add(L"7"); chars_array[2].Add(L"8"); chars_array[2].Add(L"9"); chars_array[2].Add(L"0");
		//LINE 2
		chars_array[2].Add(L"~"); chars_array[2].Add(L"^"); chars_array[2].Add(L"&&"); chars_array[2].Add(L"\""); chars_array[2].Add(L"{");
		chars_array[2].Add(L"}"); chars_array[2].Add(L"["); chars_array[2].Add(L"]"); chars_array[2].Add(L"<"); chars_array[2].Add(L">");
		chars_array[2].Add(L"@"); 
		//LINE 3
		chars_array[2].Add(L"#"); chars_array[2].Add(L"$"); chars_array[2].Add(L"%"); chars_array[2].Add(L"*"); chars_array[2].Add(L"†"); 
		chars_array[2].Add(L"‡"); chars_array[2].Add(L"("); chars_array[2].Add(L")"); chars_array[2].Add(L"-"); chars_array[2].Add(L"+"); 
		chars_array[2].Add(L"±"); 
		//LINE 4
		chars_array[2].Add(L"CAPS"); chars_array[2].Add(L"_"); chars_array[2].Add(L"|"); chars_array[2].Add(L"?"); chars_array[2].Add(L"¿"); 
		chars_array[2].Add(L"!"); chars_array[2].Add(L"="); chars_array[2].Add(L"\\"); chars_array[2].Add(L","); chars_array[2].Add(L"°");
		chars_array[2].Add(L"BACK");
		//LINE 5
		chars_array[2].Add(L"abc"); chars_array[2].Add(L";"); chars_array[2].Add(L"SPACE"); chars_array[2].Add(L":"); chars_array[2].Add(L"OK");

		buttons_in_row_[0] = 10;
		buttons_in_row_[1] = 11;
		buttons_in_row_[2] = 11;
		buttons_in_row_[3] = 11;
		buttons_in_row_[4] = 5;

		return 11; //Number of max keys in row
	}

	return 1;
}

void KeyboardDlg::FillPossibleChars() {

	possible_chars_array_.clear();

	//-----------------------------ALPHABETS

	if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
		lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
		lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
		lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
		lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
		lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
		lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN ||
		lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {

		//e
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ē"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ę"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"è"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ê"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ė"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ē"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ę"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"è"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ě"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ê"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ė"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ē"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ę"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"è"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ӗ"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ě"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ê"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ė"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ə"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ē"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"è"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ê"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		} else if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"è"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ê"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ė"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"e", L"e"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"é"));
			possible_chars_array_.push_back(std::make_pair(L"e", L"ə"));
		}
		//r
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"r", L"r"));
			possible_chars_array_.push_back(std::make_pair(L"r", L"ř"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"r", L"r"));
			possible_chars_array_.push_back(std::make_pair(L"r", L"ŕ"));
			possible_chars_array_.push_back(std::make_pair(L"r", L"ř"));
		}
		//t
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"t", L"t"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"ť"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"t", L"t"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"þ"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"ť"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"ț"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"ţ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"t", L"t"));
			possible_chars_array_.push_back(std::make_pair(L"t", L"ț"));
		}
		//y
		if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"y", L"y"));
			possible_chars_array_.push_back(std::make_pair(L"y", L"ý"));
		} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"y", L"y"));
			possible_chars_array_.push_back(std::make_pair(L"y", L"ý"));
			possible_chars_array_.push_back(std::make_pair(L"y", L"ÿ"));
		}
		//u
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"u", L"u"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ū"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ù"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ú"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"û"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ü"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ů"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"u", L"u"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ū"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ù"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ú"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"û"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ü"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ů"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ų"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ű"));
		} else {
			possible_chars_array_.push_back(std::make_pair(L"u", L"u"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ū"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ù"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ú"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"û"));
			possible_chars_array_.push_back(std::make_pair(L"u", L"ü"));
		}
		//i
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"i", L"i")); 
			possible_chars_array_.push_back(std::make_pair(L"i", L"ī"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"į"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ì"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"í"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"î"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ï"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"i", L"i"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ī"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"į"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ì"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"í"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"î"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ï"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ı"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"i", L"i"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ī"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ì"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"í"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"î"));
			possible_chars_array_.push_back(std::make_pair(L"i", L"ï"));
		}
		//o
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN) {
			possible_chars_array_.push_back(std::make_pair(L"o", L"o"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"°"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ō"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ò"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ó"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ô"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"õ"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ö"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"œ"));
		} else if (lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN) {
			possible_chars_array_.push_back(std::make_pair(L"o", L"o"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"°"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ø"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ō"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ò"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ó"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ô"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"õ"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ö"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"œ"));
		} else if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"o", L"o"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ø"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ō"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ò"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ó"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ô"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"õ"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ö"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"œ"));
		} else if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"o", L"o"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ō"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ò"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ó"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ô"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"õ"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ö"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"œ"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"o", L"o"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ø"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ō"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ò"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ó"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ô"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"õ"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ö"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"ő"));
			possible_chars_array_.push_back(std::make_pair(L"o", L"œ"));
		}
		//a
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"à"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ã"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"å"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"æ"));
		} else if (lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ā"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"à"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ã"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"å"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"æ"));
		} else if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ą"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ā"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"à"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ã"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"å"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"æ"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ą"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ā"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"à"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ă"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ã"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"å"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"æ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"a", L"a"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ā"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"à"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"á"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ă"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"â"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ã"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"å"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"ä"));
			possible_chars_array_.push_back(std::make_pair(L"a", L"æ"));
		}
		//s
		if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"s", L"s"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"š"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ś"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"s", L"s"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ś"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ş"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"š"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"s", L"s"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"§"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ś"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ş"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"š"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"s", L"s"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"s", L"ş"));
		}
		//d
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"d", L"d"));
			possible_chars_array_.push_back(std::make_pair(L"d", L"ď"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"d", L"d"));
			possible_chars_array_.push_back(std::make_pair(L"d", L"ď"));
			possible_chars_array_.push_back(std::make_pair(L"d", L"đ"));
		}
		//g
		if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"g", L"g"));
			possible_chars_array_.push_back(std::make_pair(L"g", L"ǧ"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"g", L"g"));
			possible_chars_array_.push_back(std::make_pair(L"g", L"ǧ"));
			possible_chars_array_.push_back(std::make_pair(L"g", L"ģ"));
		}
		//k
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"k", L"k"));
			possible_chars_array_.push_back(std::make_pair(L"kɭ", L"ķ"));
		}
		//l
		if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"l", L"l"));
			possible_chars_array_.push_back(std::make_pair(L"l", L"ł"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"l", L"l"));
			possible_chars_array_.push_back(std::make_pair(L"l", L"ł"));
			possible_chars_array_.push_back(std::make_pair(L"l", L"ľ"));
			possible_chars_array_.push_back(std::make_pair(L"l", L"ļ"));
			possible_chars_array_.push_back(std::make_pair(L"l", L"ĺ"));
		}
		//z
		if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"z", L"z"));
			possible_chars_array_.push_back(std::make_pair(L"z", L"ž"));
			possible_chars_array_.push_back(std::make_pair(L"z", L"ż"));
			possible_chars_array_.push_back(std::make_pair(L"z", L"ź"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"z", L"z"));
			possible_chars_array_.push_back(std::make_pair(L"z", L"ž"));
		}
		//c
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"c", L"c"));
			possible_chars_array_.push_back(std::make_pair(L"c", L"ç"));
			possible_chars_array_.push_back(std::make_pair(L"c", L"ć"));
			possible_chars_array_.push_back(std::make_pair(L"c", L"č"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"c", L"c"));
			possible_chars_array_.push_back(std::make_pair(L"c", L"ç"));
		}
		//n
		if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"n", L"n"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ñ"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ń"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"n", L"n"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ň"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ñ"));
		} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"n", L"n"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ň"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ñ"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ń"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"n", L"n"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ň"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ñ"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ń"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ņ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"n", L"n"));
			possible_chars_array_.push_back(std::make_pair(L"n", L"ñ"));
		}
		//E
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ē"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ę"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"È"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ê"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ė"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ē"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ę"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"È"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ě"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ê"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ė"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ē"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ę"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"È"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ĕ"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ě"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ê"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ė"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ə"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ē"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"È"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ê"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		} else if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"È"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ê"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ė"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"E", L"E"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"É"));
			possible_chars_array_.push_back(std::make_pair(L"E", L"Ə"));
		}
		//R
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"R", L"R"));
			possible_chars_array_.push_back(std::make_pair(L"R", L"Ř"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"R", L"R"));
			possible_chars_array_.push_back(std::make_pair(L"R", L"Ŕ"));
			possible_chars_array_.push_back(std::make_pair(L"R", L"Ř"));		}
		//T
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"T", L"T"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"Ť"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"T", L"T"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"þ"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"Ť"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"Ț"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"Ţ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"T", L"T"));
			possible_chars_array_.push_back(std::make_pair(L"T", L"Ț"));
		}
		//Y
		if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"Y", L"Y"));
			possible_chars_array_.push_back(std::make_pair(L"Y", L"Ý"));
		} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"Y", L"Y"));
			possible_chars_array_.push_back(std::make_pair(L"Y", L"Ý"));
			possible_chars_array_.push_back(std::make_pair(L"Y", L"Ÿ"));
		}
		//U
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"U", L"U"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ū"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ù"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ú"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Û"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ü"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ů"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"U", L"U"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ū"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ù"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ú"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Û"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ü"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ů"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ű"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ų"));
		} else {
			possible_chars_array_.push_back(std::make_pair(L"U", L"U")); 
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ū"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ù"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ú"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Û"));
			possible_chars_array_.push_back(std::make_pair(L"U", L"Ü"));
		}
		//I
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"I", L"I"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ī"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Į"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ì"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Í"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Î"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ï"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"I", L"I"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ī"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Į"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ì"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Í"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Î"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ï"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"İ"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) { //İ
			possible_chars_array_.push_back(std::make_pair(L"İ", L"İ"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Ī"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Į"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Ì"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Í"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Î"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"Ï"));
			possible_chars_array_.push_back(std::make_pair(L"İ", L"І"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"I", L"I"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ī"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ì"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Í"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Î"));
			possible_chars_array_.push_back(std::make_pair(L"I", L"Ï"));
		}
		//O
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN) {
			possible_chars_array_.push_back(std::make_pair(L"O", L"O"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"°"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ō"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ò"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ó"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ô"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Õ"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ö"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Œ"));
		} else if (lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN) {
			possible_chars_array_.push_back(std::make_pair(L"O", L"O"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"°"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ø"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ō"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ò"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ó"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ô"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Õ"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ö"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Œ"));
		} else if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"O", L"O"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ø"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ō"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ò"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ó"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ô"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Õ"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ö"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Œ"));
		} else if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"O", L"O"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ō"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ò"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ó"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ô"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Õ"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ö"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Œ"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"O", L"O"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ø"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ō"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ò"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ó"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ô"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Õ"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ö"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Ő"));
			possible_chars_array_.push_back(std::make_pair(L"O", L"Œ"));
		}
		//A
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"À"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ã"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Å"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Æ"));
		} else if (lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ā"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"À"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ã"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Å"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Æ"));
		} else if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ā"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ą"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"À"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ã"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Å"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Æ"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ā"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ą"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"À"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ă"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ã"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Å"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Æ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"A", L"A"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ā"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"À"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Á"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ă"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Â"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ã"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Å"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Ä"));
			possible_chars_array_.push_back(std::make_pair(L"A", L"Æ"));
		}
		//S
		if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"S", L"S"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Š"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ś"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"S", L"S"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ś"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ş"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Š"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"S", L"S"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"§"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ś"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ş"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Š"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"S", L"S"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"ß"));
			possible_chars_array_.push_back(std::make_pair(L"S", L"Ş"));
		}
		//D
		if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"D", L"D"));
			possible_chars_array_.push_back(std::make_pair(L"D", L"Ď"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"D", L"D"));
			possible_chars_array_.push_back(std::make_pair(L"D", L"Ď"));
			possible_chars_array_.push_back(std::make_pair(L"D", L"Đ"));
		}
		//G
		if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"G", L"G"));
			possible_chars_array_.push_back(std::make_pair(L"G", L"Ǧ"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"G", L"G"));
			possible_chars_array_.push_back(std::make_pair(L"G", L"Ǧ"));
			possible_chars_array_.push_back(std::make_pair(L"G", L"Ģ"));
		}
		//K
		if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"K", L"K"));
			possible_chars_array_.push_back(std::make_pair(L"K", L"Ķ"));
		}
		//L
		if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"L", L"L"));
			possible_chars_array_.push_back(std::make_pair(L"L", L"Ł"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"L", L"L"));
			possible_chars_array_.push_back(std::make_pair(L"L", L"Ł"));
			possible_chars_array_.push_back(std::make_pair(L"L", L"Ľ"));
			possible_chars_array_.push_back(std::make_pair(L"L", L"Ļ"));
			possible_chars_array_.push_back(std::make_pair(L"L", L"Ĺ"));
		}
		//Z
		if (lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Z"));
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Ž"));
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Ż"));
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Ź"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Z"));
			possible_chars_array_.push_back(std::make_pair(L"Z", L"Ž"));
		}
		//C
		if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
			lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
			lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
			lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"C", L"C"));
			possible_chars_array_.push_back(std::make_pair(L"C", L"Ç"));
			possible_chars_array_.push_back(std::make_pair(L"C", L"Ć"));
			possible_chars_array_.push_back(std::make_pair(L"C", L"Č"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"C", L"C"));
			possible_chars_array_.push_back(std::make_pair(L"C", L"Ç"));
		}
		//N
		if (lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
			lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND) {
			possible_chars_array_.push_back(std::make_pair(L"N", L"N")); 
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ñ"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ń"));
		} else if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
			possible_chars_array_.push_back(std::make_pair(L"N", L"N"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ň"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ñ"));
		} else if(lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC) {
			possible_chars_array_.push_back(std::make_pair(L"N", L"N"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ň"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ñ"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ń"));
		} else if (lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
			possible_chars_array_.push_back(std::make_pair(L"N", L"N"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ň"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ñ"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ń"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ņ"));
		} else if (lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
			possible_chars_array_.push_back(std::make_pair(L"N", L"N"));
			possible_chars_array_.push_back(std::make_pair(L"N", L"Ñ"));
		}

	} else if (lang_id_ == LANG_RUSSIAN && sublang_id_ == SUBLANG_RUSSIAN_RUSSIA) {
		
		//e
		possible_chars_array_.push_back(std::make_pair(L"e", L"e")); 
		possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		//E
		possible_chars_array_.push_back(std::make_pair(L"E", L"E")); 
		possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		//Ь
		possible_chars_array_.push_back(std::make_pair(L"ь", L"ь")); 
		possible_chars_array_.push_back(std::make_pair(L"ь", L"ъ"));

	} else if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {
		
		possible_chars_array_.push_back(std::make_pair(L"e", L"e")); 
		possible_chars_array_.push_back(std::make_pair(L"e", L"ë"));
		possible_chars_array_.push_back(std::make_pair(L"E", L"E")); 
		possible_chars_array_.push_back(std::make_pair(L"E", L"Ë"));
		possible_chars_array_.push_back(std::make_pair(L"ь", L"ь")); 
		possible_chars_array_.push_back(std::make_pair(L"ь", L"ъ"));
	}

	//-----------------------------SPECIAL CHARACTERS

	//#
	possible_chars_array_.push_back(std::make_pair(L"#", L"#")); 
	possible_chars_array_.push_back(std::make_pair(L"#", L"№"));
	//$
	if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
		lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
		lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
		lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
		lang_id_ == LANG_RUSSIAN && sublang_id_ == SUBLANG_RUSSIAN_RUSSIA ||
		lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
		possible_chars_array_.push_back(std::make_pair(L"$", L"$"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"₹"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"₱"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"£"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"Ȼ"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"¥"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"€"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"₽"));
	} else if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
		lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN) {
		possible_chars_array_.push_back(std::make_pair(L"$", L"$"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"£"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"Ȼ"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"¥"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"€"));
		possible_chars_array_.push_back(std::make_pair(L"$", L"₱"));
	}
	//₺
	if (lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY) {
		possible_chars_array_.push_back(std::make_pair(L"₺", L"₺"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"$"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"₱"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"£"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"¥"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"€"));
		possible_chars_array_.push_back(std::make_pair(L"₺", L"₽"));
	}
	//-
	possible_chars_array_.push_back(std::make_pair(L"-", L"-")); 
	possible_chars_array_.push_back(std::make_pair(L"-", L"_"));
	possible_chars_array_.push_back(std::make_pair(L"-", L"–"));
	possible_chars_array_.push_back(std::make_pair(L"-", L"—"));
	if (lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
		lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {
		possible_chars_array_.push_back(std::make_pair(L"-", L"·"));
	}
	//'
	possible_chars_array_.push_back(std::make_pair(L"'", L"'")); 
	possible_chars_array_.push_back(std::make_pair(L"'", L"`"));
	possible_chars_array_.push_back(std::make_pair(L"'", L"‘"));
	possible_chars_array_.push_back(std::make_pair(L"'", L"’"));
	possible_chars_array_.push_back(std::make_pair(L"'", L"‹"));
	possible_chars_array_.push_back(std::make_pair(L"'", L"›"));

	if (lang_id_ == LANG_PORTUGUESE && sublang_id_ == SUBLANG_PORTUGUESE_BRAZILIAN ||
		lang_id_ == LANG_GERMAN && sublang_id_ == SUBLANG_GERMAN ||
		lang_id_ == LANG_POLISH && sublang_id_ == SUBLANG_POLISH_POLAND ||
		lang_id_ == LANG_ITALIAN && sublang_id_ == SUBLANG_ITALIAN ||
		lang_id_ == LANG_TURKISH && sublang_id_ == SUBLANG_TURKISH_TURKEY ||
		lang_id_ == LANG_CZECH && sublang_id_ == SUBLANG_CZECH_CZECH_REPUBLIC ||
		lang_id_ == LANG_SPANISH && sublang_id_ == SUBLANG_SPANISH_MEXICAN ||
		lang_id_ == LANG_ROMANIAN && sublang_id_ == SUBLANG_ROMANIAN_ROMANIA) {

		//*
		possible_chars_array_.push_back(std::make_pair(L"*", L"*")); 
		possible_chars_array_.push_back(std::make_pair(L"*", L"†"));
		possible_chars_array_.push_back(std::make_pair(L"*", L"‡"));
		//+
		possible_chars_array_.push_back(std::make_pair(L"+", L"+")); 
		possible_chars_array_.push_back(std::make_pair(L"+", L"±"));
		//!
		possible_chars_array_.push_back(std::make_pair(L"!", L"!"));
		possible_chars_array_.push_back(std::make_pair(L"!", L"i"));
		//?
		possible_chars_array_.push_back(std::make_pair(L"?", L"?")); 
		possible_chars_array_.push_back(std::make_pair(L"?", L"¿"));

	} else if (lang_id_ == LANG_RUSSIAN && sublang_id_ == SUBLANG_RUSSIAN_RUSSIA) {

	} else if (lang_id_ == LANG_HINDI && sublang_id_ == SUBLANG_HINDI_INDIA) {

		possible_chars_array_.push_back(std::make_pair(L"क", L"क"));
		possible_chars_array_.push_back(std::make_pair(L"क", L"क्"));
		possible_chars_array_.push_back(std::make_pair(L"क", L"कृ"));
		possible_chars_array_.push_back(std::make_pair(L"क", L"क़"));
		possible_chars_array_.push_back(std::make_pair(L"क", L"क्र"));
		possible_chars_array_.push_back(std::make_pair(L"क", L"क्र"));
	}
}

BOOL KeyboardDlg::OnNcActivate(BOOL bActive)
{
	if (!bActive && IsWindowVisible()) {
		
	//	Formation::FlashWnd(this, TRUE);
	}

	return CDialogEx::OnNcActivate(bActive);
}

void KeyboardDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	if (WA_INACTIVE == nState) {
		activate_ = FALSE;
	}
}

void KeyboardDlg::OnTimer(UINT_PTR nIDEvent)
{
	Formation::FlashWnd(this, FALSE);

	CDialogEx::OnTimer(nIDEvent);
}