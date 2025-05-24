#pragma once

#ifndef INCLUDE_LANGUAGE
#define INCLUDE_LANGUAGE __declspec(dllimport)
#endif //INCLUDE_LANGUAGE

#include "resource.h"

#define WM_LANGUAGE_CHANGED     WM_USER + 50

class LanguageDlg;

class INCLUDE_LANGUAGE Language
{
	public:
		Language();
		~Language();

		static Language* Instance(CWnd* parent_wnd = NULL, CRect parent_rect = CRect(0, 0, 0, 0), CString language = L"");
		static void SetLanguage(CString language);
		static void Show(DWORD show);
		static void UserOrRightsChanged(BOOL enable);
		static CString GetString(UINT id, CString text1 = L"", CString text2 = L"", CString text3 = L"");
		static CString GetString(UINT id, int value);
		static CString GetString(UINT id, int value1, int value2);
		static CString GetString(UINT id, int value1, int value2, int value3);
		static void SetUILanguage();
		static void SetThreadUILanguage();
		static BOOL IsForeignLanguage();
		static CString GetLanguageInEnglish();
		static CString GetLanguage();
		static DWORD GetLangId();
		static DWORD GetSubLangId();

	private:
		static Language* language_ptr_;
		LanguageDlg* language_dlg_;
};