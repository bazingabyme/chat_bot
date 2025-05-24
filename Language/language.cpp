	// Language.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INCLUDE_LANGUAGE __declspec(dllexport)
#include "language.h"

Language* Language::language_ptr_ = NULL;

Language::Language() {

	language_ptr_ = NULL;
}

Language::~Language() {

	if (language_ptr_ != NULL) {
		if (language_ptr_->language_dlg_ != NULL) {
			language_ptr_->language_dlg_->DestroyWindow();
			delete language_ptr_->language_dlg_;
			language_ptr_->language_dlg_ = NULL;
		}
		delete language_ptr_;
		language_ptr_ = NULL;
	}
}

Language* Language::Instance(CWnd* parent_wnd, CRect parent_rect, CString language) {

	if (language_ptr_ == NULL) {
		language_ptr_ = new Language();

		language_ptr_->language_dlg_ = new LanguageDlg();
		if (parent_wnd != NULL) {
			language_ptr_->language_dlg_->Create(IDD_LANGUAGE_DLG, parent_wnd);
			language_ptr_->language_dlg_->MoveWindow(parent_rect);
		}
		language_ptr_->language_dlg_->Initialize(language);
	}
	return language_ptr_;
}

void Language::SetLanguage(CString language) {

	if (language_ptr_) {
		language_ptr_->language_dlg_->SetLanguage(language);
	}
}

CString Language::GetString(UINT id, CString text1, CString text2, CString text3) {

	CString str = _T("");
	if (language_ptr_ != NULL) {
		str.LoadStringW(g_resource_handle, id);
	}
	if (!text1.IsEmpty()) {
		str.Replace(L"(100)", text1);
	}
	if (!text2.IsEmpty()) {
		str.Replace(L"(200)", text2);
	}
	if (!text3.IsEmpty()) {
		str.Replace(L"(300)", text3);
	}
	return str;
}

CString Language::GetString(UINT id, int value) {

	CString text1;
	text1.Format(L"%d", value);

	return GetString(id, text1);
}

CString Language::GetString(UINT id, int value1, int value2) {

	CString text1;
	text1.Format(L"%d", value1);
	CString text2;
	text2.Format(L"%d", value2);

	return GetString(id, text1, text2);
}

CString Language::GetString(UINT id, int value1, int value2, int value3) {

	CString text1;
	text1.Format(L"%d", value1);
	CString text2;
	text2.Format(L"%d", value2);
	CString text3;
	text3.Format(L"%d", value3);

	return GetString(id, text1, text2, text3);
}

void Language::Show(DWORD show) {

	if (language_ptr_ != NULL) {
		language_ptr_->language_dlg_->Show(show);
	}
}

void Language::UserOrRightsChanged(BOOL enable) {

	if (language_ptr_ != NULL) {
		language_ptr_->language_dlg_->UserOrRightsChanged(enable);
	}
}

void Language::SetUILanguage() {

	if (language_ptr_ != NULL) {
		language_ptr_->language_dlg_->SetUILanguage();
	}
}

void Language::SetThreadUILanguage() {

	if (language_ptr_ != NULL) {
		language_ptr_->language_dlg_->SetThreadUILanguage();
	}
}

BOOL Language::IsForeignLanguage() {

	if (language_ptr_ != NULL) {
		return language_ptr_->language_dlg_->IsForeignLanguage();
	}
	return FALSE;
}

CString Language::GetLanguageInEnglish() {

	if (language_ptr_ != NULL) {
		return language_ptr_->language_dlg_->GetLanguageInEnglish();
	}
	return _T("");
}

CString Language::GetLanguage() {

	if (language_ptr_ != NULL) {
		return language_ptr_->language_dlg_->GetLanguage();
	}
	return _T("");
}

DWORD Language::GetLangId() { 

	if (language_ptr_ != NULL) {
		return language_ptr_->language_dlg_->GetLangId();
	}
	return LANG_ENGLISH;
}

DWORD Language::GetSubLangId() {

	if (language_ptr_ != NULL) {
		return language_ptr_->language_dlg_->GetSubLangId();
	}
	return SUBLANG_ENGLISH_US;
}
