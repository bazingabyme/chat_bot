// language_dlg.cpp : implementation file
//

#include "stdafx.h"

// LanguageDlg dialog

IMPLEMENT_DYNAMIC(LanguageDlg, CDialogEx)

LanguageDlg::LanguageDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(LanguageDlg::IDD, pParent)
{
	lang_id_ = LANG_ENGLISH;
	sublang_id_ = SUBLANG_ENGLISH_US;
	language_ = language_in_english_ = _T("English (US)");
	foreign_language_ = FALSE;
	enable_ = TRUE;
	lang_index_ = 0;
}

LanguageDlg::~LanguageDlg()
{
}

void LanguageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LanguageDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// LanguageDlg message handlers

BOOL LanguageDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_F1 || pMsg->wParam == VK_F2 ||
		pMsg->wParam == VK_F3 || pMsg->wParam == VK_F4 || pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6 ||
		pMsg->wParam == VK_F7 || pMsg->wParam == VK_F8 || pMsg->wParam == VK_F9 || pMsg->wParam == VK_F10 ||
		pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 || pMsg->wParam == VK_END || pMsg->wParam == VK_HOME) {
		return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL LanguageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(WHITE_COLOR);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void LanguageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(Formation::font(Formation::BIG_FONT));
	offdc.SetTextColor(WHITE_COLOR);
	offdc.FillSolidRect(title_rect_, BACKCOLOR1);
	offdc.DrawText(L"  " + title_text_list_[lang_index_], title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));
	if (enable_ && language_in_english_ != language_list_in_eng_[lang_index_]) {
		offdc.FillSolidRect(save_rect_, BACKCOLOR2);
	} else {
		offdc.FillSolidRect(save_rect_, DISABLE_COLOR);
	}
	offdc.DrawText(save_text_list_[lang_index_], save_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	Gdiplus::Graphics graphics(offdc);

	for (INT_PTR item_index = 0; item_index < lang_rect_list_.GetSize(); item_index++) {

		//Item Rect Selection
		if (enable_) {
			offdc.SelectObject(Formation::spancolor1_pen());
			if (lang_index_ == item_index) {
				offdc.SelectObject(Formation::spancolor1_brush_for_selection());
			} else {
				offdc.SelectObject(Formation::spancolor1_brush());
			}
		} else {
			offdc.SelectObject(Formation::disablecolor_pen());
			if (lang_index_ == item_index) {
				offdc.SelectObject(GetStockObject(NULL_BRUSH));
			} else {
				offdc.SelectObject(Formation::disablecolor_brush());
			}
		}
		offdc.SetTextColor(WHITE_COLOR);
		offdc.Rectangle(lang_rect_list_[item_index]);
		
		//Icon Rect
		CRect icon_rect(lang_rect_list_[item_index]);
		icon_rect.right = lang_rect_list_[item_index].CenterPoint().x;
		icon_rect.DeflateRect(Formation::spacing2(), Formation::spacing2(), Formation::spacing4(), Formation::spacing2());
		if (icon_rect.Width() > Formation::control_height() * 3) {
			icon_rect.left = icon_rect.right - (Formation::control_height() * 3);
		}
		int icon_height = (icon_rect.Width() / 3);
		icon_rect.top = icon_rect.CenterPoint().y - icon_height;
		icon_rect.bottom = icon_rect.top + icon_height * 2;
		BITMAP bm;
		GetObject(lang_bmp_list_[item_index], sizeof(BITMAP), (LPSTR)&bm);
		Formation::GetRectInCenterAndLeftAlligned(icon_rect, CSize(bm.bmWidth, bm.bmHeight));
		Gdiplus::Bitmap bitmap_gdi(bm.bmWidth, bm.bmHeight, PixelFormatDontCare);
		Gdiplus::Image* image = bitmap_gdi.FromHBITMAP(lang_bmp_list_[item_index], 0);
		graphics.DrawImage(image, icon_rect.left, icon_rect.top, icon_rect.Width(), icon_rect.Height());
		delete image;

		//Text Rect
		CRect text_rect1(lang_rect_list_[item_index]);
		CRect text_rect2(lang_rect_list_[item_index]);
		text_rect1.left = text_rect2.left = lang_rect_list_[item_index].CenterPoint().x;
		offdc.SetTextColor(WHITE_COLOR);
		if (lang_id_list_[item_index] == LANG_ENGLISH) {
			if (!enable_) {
				offdc.SetTextColor(DISABLE_COLOR);
			}
			offdc.DrawText(language_list_in_eng_[item_index], text_rect2, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		} else {
			text_rect1.bottom = text_rect1.top + text_rect1.Height() / 2;
			text_rect2.top = text_rect1.bottom;
			offdc.DrawText(language_list_[item_index], text_rect1, DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
			offdc.DrawText(language_list_in_eng_[item_index], text_rect2, DT_LEFT | DT_SINGLELINE | DT_TOP);
		}
	}
}

void LanguageDlg::Initialize(CString language) {

	lang_rect_list_.RemoveAll();
	language_list_in_eng_.RemoveAll();
	language_list_.RemoveAll();
	title_text_list_.RemoveAll();
	save_text_list_.RemoveAll();
	lang_id_list_.RemoveAll();
	sublang_id_list_.RemoveAll();
	lang_bmp_list_.RemoveAll();

	//ADD NEW LANGUAGE HERE

	//Czech (Czech Republic)
	language_list_in_eng_.Add(_T("Czech (Czech Republic)"));
	language_list_.Add(_T("Češka (Česká republika)"));
	lang_id_list_.Add(LANG_CZECH);
	sublang_id_list_.Add(SUBLANG_CZECH_CZECH_REPUBLIC);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_CZECH_CZECHREPUBLIC)));
	title_text_list_.Add(L"Jazyk (Language)");
	save_text_list_.Add(L"Uložit (Save)");

	//English (US)
	language_list_in_eng_.Add(_T("English (US)"));
	language_list_.Add(_T("English (US)"));
	lang_id_list_.Add(LANG_ENGLISH);
	sublang_id_list_.Add(SUBLANG_ENGLISH_US);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_ENGLISH_US)));
	title_text_list_.Add(L"Language");
	save_text_list_.Add(L"Save");

	//German
	language_list_in_eng_.Add(_T("German (Germany)"));
	language_list_.Add(_T("Deutsche (Deutschland)"));
	lang_id_list_.Add(LANG_GERMAN);
	sublang_id_list_.Add(SUBLANG_GERMAN);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_GERMAN)));
	title_text_list_.Add(L"Sprache (Language)");
	save_text_list_.Add(L"Speichern (Save)");

	//Hindi
	language_list_in_eng_.Add(_T("Hindi (India)"));
	language_list_.Add(_T("हिन्दी (भारत)"));
	lang_id_list_.Add(LANG_HINDI);
	sublang_id_list_.Add(SUBLANG_HINDI_INDIA);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_HINDI_INDIA)));
	title_text_list_.Add(L"भाषा (Language)");
	save_text_list_.Add(L"संचय (Save)");

	//Italian
	language_list_in_eng_.Add(_T("Italian (Italy)"));
	language_list_.Add(_T("Italiano (Italia)"));
	lang_id_list_.Add(LANG_ITALIAN);
	sublang_id_list_.Add(SUBLANG_ITALIAN);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_ITALIAN)));
	title_text_list_.Add(L"Lingua (Language)");
	save_text_list_.Add(L"Memorizzare (Save)");

	//Polish (Poland)
	language_list_in_eng_.Add(_T("Polish (Poland)"));
	language_list_.Add(_T("Polskie (Polska)"));
	lang_id_list_.Add(LANG_POLISH);
	sublang_id_list_.Add(SUBLANG_POLISH_POLAND);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_POLISH_POLAND)));
	title_text_list_.Add(L"Język (Language)");
	save_text_list_.Add(L"Zapisać (Save)");

	//Portuguese (Brazil)
	language_list_in_eng_.Add(_T("Portuguese (Brazil)"));
	language_list_.Add(_T("Português (Brasil)"));
	lang_id_list_.Add(LANG_PORTUGUESE);
	sublang_id_list_.Add(SUBLANG_PORTUGUESE_BRAZILIAN);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_PORTUGUESE_BRAZIL)));
	title_text_list_.Add(L"Língua (Language)");
	save_text_list_.Add(L"Guardar (Save)");
	
	//Romanian (Romania)
	language_list_in_eng_.Add(_T("Romanian (Romania)"));
	language_list_.Add(_T("Română (România)"));
	lang_id_list_.Add(LANG_ROMANIAN);
	sublang_id_list_.Add(SUBLANG_ROMANIAN_ROMANIA);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_ROMANIAN_ROMANIA)));
	title_text_list_.Add(L"Limbă (Language)");
	save_text_list_.Add(L"Salvează (Save)");

	//Russian (Russia)
	language_list_in_eng_.Add(_T("Russian (Russia)"));
	language_list_.Add(_T("Pусский (Россия)"));
	lang_id_list_.Add(LANG_RUSSIAN);
	sublang_id_list_.Add(SUBLANG_RUSSIAN_RUSSIA);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_RUSSIAN_RUSSIA)));
	title_text_list_.Add(L"Язык (Language)");
	save_text_list_.Add(L"Сохранить (Save)");
	
	//Spanish (Mexico)
	language_list_in_eng_.Add(_T("Spanish (Mexico)"));
	language_list_.Add(_T("Español (México)"));
	lang_id_list_.Add(LANG_SPANISH);
	sublang_id_list_.Add(SUBLANG_SPANISH_MEXICAN);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_SPANISH_MEXICO)));
	title_text_list_.Add(L"Idioma (Language)");
	save_text_list_.Add(L"Guardar (Save)");

	//Turkish (Turkey)
	language_list_in_eng_.Add(_T("Turkish (Turkey)"));
	language_list_.Add(_T("Türkçe (Türkiye)"));
	lang_id_list_.Add(LANG_TURKISH);
	sublang_id_list_.Add(SUBLANG_TURKISH_TURKEY);
	lang_bmp_list_.Add(LoadBitmap(g_resource_handle, MAKEINTRESOURCE(IDB_TURKISH_TURKEY)));
	title_text_list_.Add(L"Dil (Language)");
	save_text_list_.Add(L"Kaydet (Save)");

	lang_index_ = 0;
	
	SetLanguage(language);

	if (GetSafeHwnd() != NULL) {
	
		GetClientRect(client_rect_);
		
		title_rect_ = client_rect_;
		title_rect_.bottom = title_rect_.top + Formation::control_height();

		wnd_rect_ = client_rect_;
		wnd_rect_.top = title_rect_.bottom;
		wnd_rect_.bottom -= (Formation::control_height() + Formation::spacing2()); //Save button
		wnd_rect_.DeflateRect(Formation::control_height(), Formation::control_height(), Formation::control_height(), Formation::control_height());
		
		int columns = 2;
		if (language_list_.GetSize() > 8) { //11) {
			columns = (language_list_.GetSize() % 4 == 0) ? 4 : 3;
		}
		int row_count = (int)ceil((double)language_list_.GetSize() / columns);
		int button_height = (wnd_rect_.Height() - row_count) / row_count;
		if (button_height > Formation::control_height() * 4) {
			button_height = Formation::control_height() * 4;
		}
		int button_width = (wnd_rect_.Width() - columns) / columns;
		if (button_width > button_height * 4) {
			button_width = button_height * 4;
		}

		CRect control_rect = wnd_rect_;
		control_rect.bottom = wnd_rect_.CenterPoint().y - (button_height * row_count / 2) - row_count / 2;
		control_rect.left = control_rect.CenterPoint().x - (button_width * columns / 2) - columns / 2;
		CRect item_rect = control_rect;
		item_rect.bottom = control_rect.bottom;
		for (int item_index = 0; item_index < (int)language_list_.GetSize(); item_index++) {
			if (item_index % columns == 0) {
				item_rect.left = control_rect.left;
				item_rect.top = item_rect.bottom + 1;
			} else {
				item_rect.left = item_rect.right + 1;
			}
			item_rect.right = item_rect.left + button_width;
			item_rect.bottom = item_rect.top + button_height;
			lang_rect_list_.Add(item_rect);
		}

		control_rect.top = client_rect_.bottom - Formation::control_height() - Formation::spacing();//item_rect.Height();
		control_rect.bottom = control_rect.top + Formation::control_height();
		control_rect.left = client_rect_.right - Formation::control_height() * 6;
		control_rect.right = client_rect_.right - Formation::spacing();
		save_rect_ = control_rect;
	}
}

void LanguageDlg::SetLanguage(CString language) {

	language_ = language_in_english_ = language;
	
	SetUILanguage();
}

void LanguageDlg::UserOrRightsChanged(BOOL enable) {

	enable_ = enable;

	Invalidate(FALSE);
}

void LanguageDlg::Show(DWORD show) {

	if (show == SW_SHOW) {
		//SET CURRENT LANGUAGE SELECTED
		for (INT_PTR lang_index = 0; lang_index < language_list_in_eng_.GetSize(); lang_index++) {
			if (language_list_in_eng_[lang_index] == language_in_english_) {
				lang_index_ = (int)lang_index;
				break;
			}
		}
	}

	ShowWindow(show);
}

void LanguageDlg::SetUILanguage() {

	for (INT_PTR lang_index = 0; lang_index < language_list_in_eng_.GetSize(); lang_index++) {
		if (language_list_in_eng_[lang_index] == language_in_english_) {
			language_ = language_list_[lang_index];
			lang_id_ = lang_id_list_[lang_index];
			sublang_id_ = sublang_id_list_[lang_index];
			lang_index_ = (int)lang_index;
			break;
		}
	}

	foreign_language_ = (lang_id_ == LANG_ENGLISH) ? FALSE : TRUE;

	SetThreadUILanguage();
}

void LanguageDlg::SetThreadUILanguage() {

	::SetThreadUILanguage(MAKELCID(MAKELANGID(lang_id_, sublang_id_), SORT_DEFAULT));
}

BOOL LanguageDlg::IsForeignLanguage() {

	return foreign_language_;
}

CString LanguageDlg::GetLanguageInEnglish() {

	return language_in_english_;
}

CString LanguageDlg::GetLanguage() {

	return language_;
}

DWORD LanguageDlg::GetLangId() {

	return lang_id_;
}

DWORD LanguageDlg::GetSubLangId() {

	return sublang_id_;
}

void LanguageDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!enable_) {
		return;
	}

	int index = -1;
	for (INT_PTR item_index = 0; item_index < lang_rect_list_.GetSize(); item_index++) {
		if (lang_rect_list_[item_index].PtInRect(point)) {
			index = (int)item_index;
			InvalidateRect(client_rect_, FALSE);
			break;
		}
	}
	if (index != -1) {

		lang_index_ = index;

	} else {

		if (save_rect_.PtInRect(point)) {
			if (language_in_english_ != language_list_in_eng_[lang_index_]) {
				language_in_english_ = language_list_in_eng_[lang_index_];

				CString old_language = language_;

				SetUILanguage();
				InvalidateRect(client_rect_, FALSE);

				GetParent()->SendMessage(WM_USER + 50, (WPARAM)1, (LPARAM)old_language.GetString());
			}
		}
	}
}