#pragma once

#include <afxdialogex.h>
#include "resource.h"

// MessageBoxDlg dialog

class MessageBoxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MessageBoxDlg)

public:
	enum { IDD = IDD_MESSAGEBOX_DLG };

	MessageBoxDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MessageBoxDlg();
	
	void SetButtonsText(CStringArray& button_text);
	int Open(CString text, UINT type, BOOL remain_popup = FALSE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CString text_;
	UINT type_;

	CRect client_rect_;
	CRect title_rect_;
	CRect icon_rect_;
	CRect text_rect_;

	CStringArray button_text_;
	CArray<CRect> button_rect_;
	int button_index_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
