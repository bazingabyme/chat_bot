#pragma once

// LanguageDlg dialog

class LanguageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LanguageDlg)

public:
	enum { IDD = IDD_LANGUAGE_DLG };

	LanguageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~LanguageDlg();

	void Initialize(CString language);
	void SetLanguage(CString language);
	void SetUILanguage();
	void SetThreadUILanguage();
	BOOL IsForeignLanguage();
	CString GetLanguageInEnglish();
	CString GetLanguage();
	DWORD GetLangId();
	DWORD GetSubLangId();
	void Show(DWORD show);
	void UserOrRightsChanged(BOOL enable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CRect client_rect_;
	CRect wnd_rect_;
	CRect title_rect_;
	CRect save_rect_;
	BOOL enable_;
	CArray<CRect> lang_rect_list_;
	CStringArray language_list_in_eng_;
	CStringArray language_list_;
	CStringArray title_text_list_;
	CStringArray save_text_list_;
	CDWordArray lang_id_list_;
	CDWordArray sublang_id_list_;
	CArray<HBITMAP> lang_bmp_list_;
	CString language_in_english_;
	CString language_;
	DWORD lang_id_;
	DWORD sublang_id_;
	BOOL foreign_language_;
	int lang_index_;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};