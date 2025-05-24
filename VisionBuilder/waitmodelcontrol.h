#pragma once

#include <afxdialogex.h>
#include "resource.h"

#define WM_WAIT_MODEL_CLOSE		WM_USER + 600

// WaitModelControl dialog

class WaitModelControl : public CDialogEx
{
	DECLARE_DYNAMIC(WaitModelControl)

public:
	enum { IDD = IDD_WAIT_MODEL_DLG };

	WaitModelControl(CWnd* pParent = NULL);
	virtual ~WaitModelControl();

	//friend UINT ProgressBarThread(LPVOID param);

	void Open(CString title, CString text, BOOL show_progress);
	void Hide(DWORD hide);
	void ChangeText(CString text);
	//void Paint();
	CString& GetProgressText() { return text_; }
	float& GetProgressValue() { return percentage_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CWinThread* winthread_;

	CRect client_rect_;
	CRect wnd_rect_;
	CRect caption_rect_;
	CRect progress_rect_;
	CRect text_rect_;
	CString title_;
	CString text_;
	float percentage_;

	BOOL show_progress_bar_;
	DWORD interlock_open_;
	DWORD interlock_close_;

	HANDLE model_dialog_created_handle_;
	HANDLE close_modal_dialog_handle_;
	HANDLE close_thread_handle_;

public:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT Close(WPARAM wparam = 0, LPARAM lparam = 0);
};
