#pragma once

#include "gpt_dlg.h"
#include "resource.h"		// main symbols

#include "..\VisionControls\vision_controls.h"
#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Debug\\VisionControls.lib")
	#else
		#pragma comment(lib, "..\\Debug\\VisionControls.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Release\\VisionControls.lib")
	#else
		#pragma comment(lib, "..\\Release\\VisionControls.lib")
	#endif
#endif

#include "..\VisionBuilder\formation.h"
#ifdef _DEBUG
	#ifdef _M_X64 
		#pragma comment(lib, "..\\x64\\Debug\\VisionBuilder.lib")
	#else 
		#pragma comment(lib, "..\\Debug\\VisionBuilder.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Release\\VisionBuilder.lib")
	#else
		#pragma comment(lib, "..\\Release\\VisionBuilder.lib")
	#endif
#endif

#include "..\Language\language.h"
#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Debug\\Language.lib")
	#else
		#pragma comment(lib, "..\\Debug\\Language.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "..\\x64\\Release\\Language.lib")
	#else
		#pragma comment(lib, "..\\Release\\Language.lib")
	#endif
#endif

// GPTDlg dialog

class GPTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(GPTDlg)

public:
	enum { IDD = IDD_GPT_DLG };

	GPTDlg(CWnd* pParent = NULL);
	virtual ~GPTDlg();

	void Show(DWORD show);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:	
	friend DWORD WINAPI inference_gpt(LPVOID args);

	void RefreshPage();
	//SPAN-GPT CURL
	//bool Initialize_SPANGPT(const std::string& vector_data_path);
	//bool ReRAG_SPANGPT(const std::string& path);
	std::string Inference_SPAN_GPT(const std::string& query);
	//std::string MakePathRequest(const std::string& url, const std::string& data);
	std::string MakeQueryRequest(const std::string& url, const std::string& query);
	CString FormatChatMessage(const CString& message, BOOL isUser);
	void UpdateChatDisplay();

	CString chat_history_;
	LabelCtrl ctrTitle;
	LabelCtrl ctrAnswer, ctrAnswerTinyView;
	EditCtrl ctrQuestion, ctrPath;
	Button ctrMinMaxButton, ctrCloseButton;
	Button ctrClearButton;
	Button ctrBrowseButton;

	CRect full_rect_, tiny_rect_;
	CRect min_max_rect_;
	CString question_text_;
	CString answer_text_;
	int position_;
	CStringArray history_list_;
	CStringArray history_content_list_;
	CRect history_rect_;
	int history_selection_index_;
	
	BOOL m_bModernUI;
	BOOL m_bModernStyle;
	int m_nCornerRadius;
	int m_nShadowSize;
	int m_nButtonHeight;
	int m_nSpacing;
	int m_nTitleBarHeight;

	BOOL min_max_button_clicked_, full_view_;

	DECLARE_MESSAGE_MAP()

public:
	LRESULT CallbackMsgGPTApp(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnEditEnterPressedMessage(WPARAM wparam, LPARAM lparam);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedGPTClose();
	afx_msg void OnBnClickedGPTClear();
	afx_msg void OnBnClickedGPTBrowse();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
