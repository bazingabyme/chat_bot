#pragma once

#include <afxdialogex.h>
#include "resource.h"

// WaitControl dialog

class WaitControl : public CDialogEx
{
	DECLARE_DYNAMIC(WaitControl)

public:
	enum { IDD = IDD_WAIT_DLG };

	WaitControl(CWnd* pParent = NULL);
	virtual ~WaitControl();

	void Open(CString title, CString text);
	void Close();
	void Hide(DWORD hide);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CRect client_rect_;
	CRect title_rect_;
	CRect text_rect_;
	CString title_;
	CString text_;

public:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};
