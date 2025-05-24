#pragma once

#include "listctrl.h"

#ifndef INCLUDE_FILEDLG
#define INCLUDE_FILEDLG __declspec(dllimport)
#endif //INCLUDE_FILEDLG

// FileDlg

class INCLUDE_FILEDLG FileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FileDlg)

public:
	enum { IDD = IDD_FILE_DLG };

	FileDlg(CWnd* pParent = NULL);
	virtual ~FileDlg();

	INT_PTR Open(BOOL file_selection, CString extention = _T("*.*"));
	INT_PTR Open(BOOL file_selection, CStringArray& extention);
	CString GetFilePath() { return file_path_; }
	CString GetFileName() { return file_name_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	void FillList();
	void SetFilePath(int index);

	CRect client_rect_;
	CRect title_rect_;
	CRect path_rect_;
	CRect open_rect_;
	CRect close_rect_;

	CString file_path_;
	CString file_name_;
	BOOL file_selection_;
	CStringArray extention_list_;
	BOOL open_button_clicked_;
	BOOL close_button_clicked_;

	CMFCShellTreeCtrl	ctrShellTree;
	CListCtrl ctrList;
	
public:
	afx_msg void OnTvnSelchangedMfcshelltree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};