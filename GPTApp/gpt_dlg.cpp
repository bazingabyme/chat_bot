// gpt_dlg.cpp : implementation file - Enhanced with Modern Touches
//

#include "gpt_dlg.h"

static const UINT SPANMsgGPTApp = ::RegisterWindowMessage(L"SPANMsgGPTApp");

#include <curl\curl.h>
#include <nlohmann\json.hpp>

#pragma comment(lib, "libcurl_imp.lib")
using json = nlohmann::json;

// GPTDlg dialog

IMPLEMENT_DYNAMIC(GPTDlg, CDialogEx)

GPTDlg::GPTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(GPTDlg::IDD, pParent)
{
}

GPTDlg::~GPTDlg()
{
	/*ctrTitle.Destroy();
	ctrQuestion.Destroy();
	ctrAnswer.Destroy();
	ctrCloseButton.Destroy();
	ctrClearButton.Destroy();
	ctrSubmitButton.Destroy();*/
}

void GPTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDL_GPT_TITLE, ctrTitle);
	DDX_Control(pDX, IDEC_GPT_QUESTION, ctrQuestion);
	DDX_Control(pDX, IDL_GPT_ANSWER, ctrAnswer);
	DDX_Control(pDX, IDL_GPT_ANSWER_TINYVIEW, ctrAnswerTinyView);
	DDX_Control(pDX, IDEC_GPT_PATH, ctrPath);
	DDX_Control(pDX, IDB_GPT_CLOSE, ctrCloseButton);
	DDX_Control(pDX, IDB_GPT_CLEAR, ctrClearButton);
	DDX_Control(pDX, IDB_GPT_MINMAX, ctrMinMaxButton);
	DDX_Control(pDX, IDB_GPT_BROWSE, ctrBrowseButton);
}

BEGIN_MESSAGE_MAP(GPTDlg, CDialogEx)
	ON_REGISTERED_MESSAGE(SPANMsgGPTApp, CallbackMsgGPTApp)
	ON_MESSAGE(WM_EDIT_ENTERPRESSED, OnEditEnterPressedMessage)
	ON_BN_CLICKED(IDB_GPT_CLOSE, &GPTDlg::OnBnClickedGPTClose)
	ON_BN_CLICKED(IDB_GPT_CLEAR, &GPTDlg::OnBnClickedGPTClear)
	ON_BN_CLICKED(IDB_GPT_BROWSE, &GPTDlg::OnBnClickedGPTBrowse)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// GPTDlg message handlers

BOOL GPTDlg::PreTranslateMessage(MSG* pMsg)
{
	if (!full_view_ && pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		CPoint p;
		GetCursorPos(&p);
		CRect r;
		ctrTitle.GetWindowRect(&r);
		if (r.PtInRect(p)) {
			ReleaseCapture();
			SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
			return TRUE;
		}
	}
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_F1 || pMsg->wParam == VK_F2 ||
		pMsg->wParam == VK_F3 || pMsg->wParam == VK_F4 || pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6 ||
		pMsg->wParam == VK_F7 || pMsg->wParam == VK_F8 || pMsg->wParam == VK_F9 || pMsg->wParam == VK_F10 ||
		pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 || pMsg->wParam == VK_END || pMsg->wParam == VK_HOME) {
		return FALSE;
	}
	if (pMsg->wParam == VK_RETURN) {
		OnEditEnterPressedMessage(0, 0);
		return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string GPTDlg::MakeQueryRequest(const std::string& url, const std::string& query) {

	CURL* curl;
	CURLcode res;
	std::string response;

	std::string json_payload = "{\"query\": \"" + query + "\"}";

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		// Set headers
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Perform the request
		res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			throw std::runtime_error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res));
		}

		// Cleanup
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
	}
	else {
		throw std::runtime_error("Failed to initialize curl");
	}

	return response;
}

std::string GPTDlg::Inference_SPAN_GPT(const std::string& query) {
	std::string url = "http://localhost:9094/execute_span_gpt_inference/";
	return MakeQueryRequest(url, query);
}

BOOL GPTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(L"SPAN GPT App");

	Formation::Instance();

	Language::Instance(NULL, CRect(), _T("English (US)"));

	// Enhanced background with modern color
	SetBackgroundColor(MODERN_GRAY);
	ModifyStyle(WS_CAPTION, 0);

	CRect client_rect;
	client_rect.left = 0;
	client_rect.right = GetSystemMetrics(SM_CXSCREEN);
	client_rect.top = 0;
	client_rect.bottom = GetSystemMetrics(SM_CYSCREEN) - Formation::taskbar_height() + Formation::spacing4();

	full_rect_ = client_rect;

	tiny_rect_.left = 0;
	tiny_rect_.right = tiny_rect_.left + (full_rect_.Width() * 30 / 100);
	tiny_rect_.bottom = full_rect_.bottom;
	tiny_rect_.top = tiny_rect_.bottom - (full_rect_.Height() * 25 / 100);

	// title bar positioning
	CRect title_rect = client_rect;
	title_rect.bottom = title_rect.top + Formation::control_height();
	title_rect.right = title_rect.right - (Formation::control_height() * 2);
	ctrTitle.MoveWindow(title_rect);
	ctrTitle.SetFont(&Formation::font(Formation::BIG_FONT));
	ctrTitle.SetBackColor(BACKCOLOR1);  // Your requested blue color
	ctrTitle.SetBorderColor(BACKCOLOR1);
	ctrTitle.Create(DT_LEFT);
	ctrTitle.SetText(L"   SPAN GPT");  // Added emoji for modern touch

	CRect control_rect = title_rect;
	control_rect.left = title_rect.right;
	control_rect.right = control_rect.left + Formation::control_height();
	min_max_rect_ = control_rect;
	ctrMinMaxButton.Destroy();
	ctrMinMaxButton.MoveWindow(control_rect);
	HICON min_icon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
	HICON max_icon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
	ctrMinMaxButton.SetIcon(min_icon, max_icon, Button::ST_ALIGN_VERT);
	ctrMinMaxButton.Create(Button::BUTTON_TYPE::BUTTON_CHECK_BOX_PUSH_LIKE, BACKCOLOR1, MODERN_LIGHT_BLUE);  // Enhanced colors
	ctrMinMaxButton.ShowWindow(SW_HIDE);

	control_rect.left = control_rect.right;
	control_rect.right = control_rect.left + Formation::control_height();
	ctrCloseButton.Destroy();
	ctrCloseButton.MoveWindow(control_rect);
	HICON close_icon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
	ctrCloseButton.SetIcon(close_icon, close_icon, Button::ST_ALIGN_VERT);
	ctrCloseButton.Create(Button::BUTTON_TYPE::BUTTON_REGULAR, BACKCOLOR1, MODERN_LIGHT_BLUE);;  // Enhanced colors

	// Enhanced tiny view with better styling
	control_rect.left = client_rect.left + Formation::spacing();
	control_rect.right = control_rect.left + tiny_rect_.Width() - Formation::spacing3();
	control_rect.top = title_rect.bottom + Formation::spacing();
	control_rect.bottom = control_rect.top + tiny_rect_.Height() - title_rect.Height() - Formation::spacing3();
	ctrAnswerTinyView.MoveWindow(control_rect);
	ctrAnswerTinyView.SetFont(&Formation::font(Formation::SMALL_FONT));
	ctrAnswerTinyView.SetBorderColor(WHITE_COLOR);
	ctrAnswerTinyView.SetBackColor(WHITE_COLOR);
	ctrAnswerTinyView.SetTextColor(BLACK_COLOR);
	ctrAnswerTinyView.Create(DT_TOP, TRUE);
	ctrAnswerTinyView.ShowWindow(SW_HIDE);

	// Enhanced main answer area
	control_rect.left = client_rect.left + Formation::spacing4();
	control_rect.right = client_rect.right - (Formation::control_height() * 9) - Formation::spacing4();
	control_rect.top = title_rect.bottom + Formation::spacing4();
	control_rect.bottom = client_rect.bottom - Formation::control_height() - (Formation::spacing4() * 2) - Formation::spacing3();

	CRect answer_rect = control_rect;
	answer_rect.DeflateRect(Formation::spacing3(), Formation::spacing2(), Formation::spacing3(), Formation::spacing());
	ctrAnswer.MoveWindow(answer_rect);
	ctrAnswer.SetFont(&Formation::font(Formation::BIG_FONT));
	ctrAnswer.SetBorderColor(WHITE_COLOR);
	ctrAnswer.SetBackColor(WHITE_COLOR);
	ctrAnswer.SetTextColor(BLACK_COLOR);
	ctrAnswer.Create(DT_TOP, TRUE);

	// Enhanced question input
	control_rect.top = client_rect.bottom - Formation::control_height() - Formation::spacing4();
	control_rect.bottom = control_rect.top + Formation::control_height();
	control_rect.left = client_rect.left + Formation::spacing4();
	control_rect.right = client_rect.right - (Formation::control_height() * 9) - Formation::spacing4() - (Formation::control_height() * 2) - Formation::spacing(); // Reduced width to make space for clear button

	ctrQuestion.MoveWindow(control_rect);
	ctrQuestion.SetFont(&Formation::font(Formation::BIG_FONT));
	ctrQuestion.SetLabelBorderColor(WHITE_COLOR);
	ctrQuestion.SetLabelBackColor(WHITE_COLOR);
	ctrQuestion.SetEditBackColor(GRAY_COLOR);
	ctrQuestion.SetEditTextColor(BLACK_COLOR);
	ctrQuestion.Create(0, FALSE, TRUE, TRUE, 255);
	ctrQuestion.SetText("  Ask");
	ctrQuestion.SetEditText(L"  Ask anything...");
	ctrQuestion.ShowMultilingualKeyboard(TRUE);


	// Enhanced buttons with modern colors
	CRect clear_button_rect;
	clear_button_rect.top = control_rect.top;
	clear_button_rect.bottom = control_rect.bottom;
	clear_button_rect.left = control_rect.right + Formation::spacing();
	clear_button_rect.right = clear_button_rect.left + (Formation::control_height() * 2);
	ctrClearButton.MoveWindow(clear_button_rect);
	ctrClearButton.SetFont(&Formation::font(Formation::MEDIUM_FONT));
	HICON clear_icon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NEW), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
	ctrClearButton.SetIcon(clear_icon, clear_icon, Button::ST_ALIGN_VERT);
	ctrClearButton.Create(Button::BUTTON_TYPE::BUTTON_REGULAR, WHITE_COLOR, MODERN_LIGHT_BLUE);

	control_rect.left = control_rect.right + Formation::spacing();
	control_rect.right = answer_rect.right - (Formation::control_height() + Formation::spacing());
	ctrPath.MoveWindow(control_rect);
	ctrPath.SetFont(&Formation::font(Formation::MEDIUM_FONT));
	ctrPath.Create(0);
	ctrPath.ShowWindow(SW_HIDE);

	control_rect.left = control_rect.right + Formation::spacing();
	control_rect.right = control_rect.left + Formation::control_height();
	ctrBrowseButton.MoveWindow(control_rect);
	ctrBrowseButton.SetFont(&Formation::font(Formation::MEDIUM_FONT));
	ctrBrowseButton.SetText(_T("📁"));  // Modern file icon
	ctrBrowseButton.Create(Button::BUTTON_TYPE::BUTTON_REGULAR, WHITE_COLOR, MODERN_LIGHT_BLUE);
	ctrBrowseButton.ShowWindow(SW_HIDE);

	control_rect.top = title_rect.bottom + Formation::spacing4();
	control_rect.left = control_rect.right + Formation::spacing4() + Formation::spacing();
	control_rect.right = control_rect.left + (Formation::control_height() * 9) - Formation::spacing4();
	history_rect_ = control_rect;

	ctrQuestion.SetFocus();

	history_list_.RemoveAll();
	history_content_list_.RemoveAll();
	for (int index = 0; index < 20; index++) {
		history_list_.Add(L"");
		history_content_list_.Add(L"");
	}
	min_max_button_clicked_ = FALSE;
	full_view_ = TRUE;
	history_selection_index_ = -1;
	question_text_.Empty();
	OnBnClickedGPTClear();

	CString user_name;
	//QUERY LAST USER NAME FROM THE REGISTRY
	HKEY key = NULL;
	if (RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SPAN", &key) == ERROR_SUCCESS) {
		DWORD key_type = REG_SZ;
		DWORD size = MAX_PATH;
		WCHAR sz_user_name[255] = L"\0";
		RegQueryValueEx(key, L"UserName", NULL, &key_type, (LPBYTE)sz_user_name, &size);
		user_name.Format(L"%s", sz_user_name);
		RegCloseKey(key);
	}

	//SPAN-GPT CURL
	//const char* command = "start cmd /c python LLM01v1.py";
	//int result = std::system(command);

	int result = 0;
	if (result == 0) {
		std::cout << "Python script executed successfully." << std::endl;
	}
	else {
		std::cout << "Failed to execute Python script." << std::endl;
	}

	SetWindowPos(&CWnd::wndTop, full_rect_.left, full_rect_.top, full_rect_.Width(), full_rect_.Height(), SWP_SHOWWINDOW);

	return TRUE;
}

void GPTDlg::RefreshPage() {

	if (full_view_) {
		GetWindowRect(&tiny_rect_);
		ModifyStyle(WS_DLGFRAME, 0);
		MoveWindow(full_rect_);
		ctrAnswerTinyView.ShowWindow(SW_HIDE);
		ctrAnswer.ShowWindow(SW_SHOW);
		ctrQuestion.ShowWindow(SW_SHOW);
		ctrClearButton.ShowWindow(SW_SHOW);
		ctrPath.ShowWindow(SW_HIDE);
		ctrBrowseButton.ShowWindow(SW_HIDE);
	}
	else {
		ModifyStyle(0, WS_DLGFRAME);
		MoveWindow(tiny_rect_);
		ctrAnswerTinyView.ShowWindow(SW_SHOW);
		ctrAnswer.ShowWindow(SW_HIDE);
		ctrQuestion.ShowWindow(SW_HIDE);
		ctrClearButton.ShowWindow(SW_HIDE);
		ctrPath.ShowWindow(SW_HIDE);
		ctrBrowseButton.ShowWindow(SW_HIDE);
	}

	CRect client_rect;
	GetClientRect(&client_rect);

	CRect title_rect = client_rect;
	title_rect.bottom = title_rect.top + Formation::control_height();
	title_rect.right = title_rect.right - (Formation::control_height() * 2);
	ctrTitle.MoveWindow(title_rect);
	ctrTitle.SetFont(&Formation::font(Formation::BIG_FONT));
	ctrTitle.SetBackColor(BACKCOLOR1);
	ctrTitle.SetBorderColor(BACKCOLOR1);
	ctrTitle.Create(DT_LEFT | DT_VCENTER);
	ctrTitle.SetText(L"  SPAN GPT");
	CRect control_rect = title_rect;
	control_rect.left = title_rect.right;
	control_rect.right = control_rect.left + Formation::control_height();
	min_max_rect_ = control_rect;
	control_rect.left = control_rect.right;
	control_rect.right = control_rect.left + Formation::control_height();
	ctrCloseButton.Destroy();
	ctrCloseButton.MoveWindow(control_rect);
	HICON close_icon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, Formation::icon_size(Formation::MEDIUM_ICON), Formation::icon_size(Formation::MEDIUM_ICON), LR_DEFAULTCOLOR);
	ctrCloseButton.SetIcon(close_icon, close_icon, Button::ST_ALIGN_VERT);
	ctrCloseButton.Create(Button::BUTTON_TYPE::BUTTON_REGULAR, BACKCOLOR1, BACKCOLOR1_SEL);

	Invalidate(FALSE);
}

LRESULT GPTDlg::CallbackMsgGPTApp(WPARAM wparam, LPARAM lparam) {

	if (wparam == 0) {
		ShowWindow(SW_HIDE);
	}
	else if (wparam == 1) {
		if (full_view_) {
			SetWindowPos(&CWnd::wndTop, full_rect_.left, full_rect_.top, full_rect_.Width(), full_rect_.Height(), SWP_SHOWWINDOW);
		}
		else {
			SetWindowPos(&CWnd::wndTop, tiny_rect_.left, tiny_rect_.top, tiny_rect_.Width(), tiny_rect_.Height(), SWP_SHOWWINDOW);
		}
	}
	else if (wparam == 2) {
		if (IsWindowVisible()) {
			if (full_view_) {
				full_view_ = FALSE;
				RefreshPage();
				Formation::CloseAllPopups();
			}
		}
	}

	return 0;
}

void GPTDlg::Show(DWORD show) {

	ShowWindow(show);
}

void GPTDlg::OnBnClickedGPTClear()
{
	ctrQuestion.SetEditText(L"Ask me anything...");

	position_ = 0;
	answer_text_.Empty();
	answer_text_ = L"🚀 Welcome to SPAN GPT! How can I help you today?\r\n\r\n";

	ctrAnswer.SetText(answer_text_);
}

void GPTDlg::OnBnClickedGPTClose()
{
	if (!full_view_) {
		GetWindowRect(&tiny_rect_);
	}
	ShowWindow(SW_HIDE);
}

void GPTDlg::OnBnClickedGPTBrowse()
{
	OnBnClickedGPTClear();

	FileDlg fileDlg;
	if (fileDlg.Open(TRUE, _T("*.docx")) == IDOK) {
		CString text = fileDlg.GetFilePath();
		ctrPath.SetEditText(text);
	}
}

DWORD WINAPI inference_gpt(LPVOID args) {


	Language::SetThreadUILanguage();

	GPTDlg* gpt_dlg = (GPTDlg*)args;

	//std::string query = CT2A(gpt_dlg->question_text_).m_psz;
	//std::string response = gpt_dlg->Inference_SPAN_GPT(query);
	std::string response = "Hello!";
	gpt_dlg->answer_text_ = response.c_str();
	gpt_dlg->answer_text_.Replace(L"\\n", L"\n");
	gpt_dlg->answer_text_.Replace(L"\"", L"");

	Sleep(1);

	WaitModelClose(Language::GetString(IDSTRINGT_PROCESSING));

	return 0;
}


LRESULT GPTDlg::OnEditEnterPressedMessage(WPARAM wparam, LPARAM lparam) {
	CString question_text = ctrQuestion.GetEditText();
	if (question_text.IsEmpty()) return 0;

	if (question_text_ != question_text || ctrAnswer.GetText().IsEmpty()) {
		question_text_ = question_text;
		KillTimer(1);
		position_ = 0;
		answer_text_.Empty();
		ctrAnswer.SetText(L"🤔 Thinking...");

		if (history_selection_index_ != -1) {
			answer_text_ = history_content_list_[history_selection_index_];
		}
		else {
			CreateThread(NULL, 0, inference_gpt, this, 0, NULL);
			WaitModelOpen(Language::GetString(IDSTRINGT_PLEASE_WAIT), Language::GetString(IDSTRINGT_PROCESSING));
		}

		// Update history
		BOOL question_exist = FALSE;
		for (int index = 0; index < history_list_.GetSize(); index++) {
			if (question_text_ == history_list_[index]) {
				question_exist = TRUE;
				break;
			}
		}

		if (!question_exist) {
			history_list_.RemoveAt(0);
			history_list_.Add(question_text_);
			history_content_list_.RemoveAt(0);
			history_content_list_.Add(answer_text_);
			InvalidateRect(history_rect_, FALSE);
		}

		ctrAnswerTinyView.SetText(answer_text_);
		SetTimer(1, 50, NULL); // Faster typing animation
	}
	return 0;
}

void GPTDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect client_rect;
	GetClientRect(&client_rect);
	COfflineDC offdc(&dc, &client_rect);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetStockObject(NULL_BRUSH));

	// Enhanced background colors
	offdc.FillSolidRect(client_rect, WHITE_COLOR);
	offdc.FillSolidRect(min_max_rect_, min_max_button_clicked_ ? MODERN_LIGHT_BLUE : BACKCOLOR1);
	 
	// Enhanced minimize/maximize button
	CRect min_max_rect = min_max_rect_;
	min_max_rect.left = min_max_rect_.CenterPoint().x - 6;
	min_max_rect.right = min_max_rect_.CenterPoint().x + 6;
	if (full_view_) {
		min_max_rect.top = min_max_rect_.CenterPoint().y - 1;
		min_max_rect.bottom = min_max_rect_.CenterPoint().y + 1;
	}
	else {
		min_max_rect.top = min_max_rect_.CenterPoint().y - 6;
		min_max_rect.bottom = min_max_rect_.CenterPoint().y + 6;
	}
	CPen white_pen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen* old_pen = offdc.SelectObject(&white_pen);
	offdc.Rectangle(min_max_rect);
	offdc.SelectObject(old_pen);

	if (full_view_) {

		// Enhanced answer area border
		CRect answer_rect;
		ctrAnswer.GetWindowRect(answer_rect);
		ScreenToClient(&answer_rect);
		answer_rect.InflateRect(Formation::spacing2(), Formation::spacing2(), Formation::spacing2(), Formation::spacing4());
		CPen pen(PS_SOLID, 2, DARK_GRAY);  // Thicker border with your blue color
		CPen* old_pen = offdc.SelectObject(&pen);
		offdc.Rectangle(answer_rect);
		offdc.SelectObject(old_pen);
		pen.DeleteObject();

		CRect question_rect;
		ctrQuestion.GetWindowRect(question_rect);
		offdc.FillSolidRect(question_rect, GRAY_COLOR);

		// Enhanced history panel
		CRect history_bg_rect = history_rect_;
		history_bg_rect.bottom = client_rect.bottom - Formation::spacing4();
		offdc.FillSolidRect(history_bg_rect, WHITE_COLOR);

		// History header
		CRect history_header = history_bg_rect;
		history_header.bottom = history_header.top + Formation::control_height();
		offdc.FillSolidRect(history_header, LABEL_COLOR);
		offdc.SetTextColor(RGB(255, 255, 255));
		offdc.SelectObject(Formation::font(Formation::BIG_FONT));
		offdc.DrawText(L"  Recent Queries", history_header, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		// Enhanced history items
		offdc.SelectObject(Formation::font(Formation::BIG_FONT));
		CRect text_rect = history_bg_rect;
		text_rect.top = history_header.bottom + Formation::spacing();
		text_rect.DeflateRect(Formation::spacing(), 0, Formation::spacing(), Formation::spacing());

		for (int index = int(history_list_.GetSize() - 1); index >= 0; index--) {
			if (!history_list_[index].IsEmpty()) {
				text_rect.bottom = text_rect.top + Formation::heading_height();

				// Highlight selected item
				if (history_selection_index_ == index) {
					CRect highlight_rect = text_rect;
					highlight_rect.InflateRect(Formation::spacing(), 2, Formation::spacing(), 2);
					offdc.FillSolidRect(highlight_rect, MODERN_LIGHT_BLUE);
					offdc.SetTextColor(BACKCOLOR1);
				}
				else {
					offdc.SetTextColor(RGB(64, 64, 64));
				}

				// Truncate long text for better display
				CString display_text = history_list_[index];	
				if (display_text.GetLength() > 40) {
					display_text = display_text.Left(37) + L"...";
				}

				offdc.DrawText("  " + display_text, text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				text_rect.top = text_rect.bottom + 2;
			}
		}

		// Draw border around history panel
		CPen history_pen(PS_SOLID, 2, LABEL_COLOR);
		old_pen = offdc.SelectObject(&history_pen);
		offdc.Rectangle(history_bg_rect);
		offdc.SelectObject(old_pen);
	}
}

void GPTDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		position_ = answer_text_.Find(L" ", position_);
		if (position_ == -1) {
			ctrAnswer.SetText(answer_text_);
			KillTimer(1);
		}
		else {
			CString text = answer_text_.Mid(0, position_);
			ctrAnswer.SetText(text);
			position_++;
		}
	}
	else if (nIDEvent == 2) {
		//Formation::CloseAllPopups();

	}
	CDialogEx::OnTimer(nIDEvent);
}

void GPTDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	min_max_button_clicked_ = FALSE;

	if (history_rect_.PtInRect(point)) {
		// Calculate which history item was clicked with better precision
		CRect history_content_rect = history_rect_;
		history_content_rect.top += Formation::control_height() + Formation::spacing(); // Account for header

		int item_height = Formation::heading_height() + 2;
		int clicked_item = (point.y - history_content_rect.top) / item_height;

		// Convert to actual history index
		history_selection_index_ = (int(history_list_.GetSize()) - 1) - clicked_item;

		// Validate selection
		if (history_selection_index_ >= 0 && history_selection_index_ < history_list_.GetSize() &&
			!history_list_[history_selection_index_].IsEmpty()) {
			InvalidateRect(history_rect_, FALSE);
		}
		else {
			history_selection_index_ = -1;
		}

	}
	else if (min_max_rect_.PtInRect(point)) {
		min_max_button_clicked_ = TRUE;
		InvalidateRect(min_max_rect_, FALSE);
	}

	SetCapture();
}

void GPTDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (history_selection_index_ >= 0 && history_selection_index_ < history_list_.GetSize()) {

		ctrQuestion.SetEditText(history_list_[history_selection_index_]);
		OnEditEnterPressedMessage(0, 0);

		history_selection_index_ = -1;
		InvalidateRect(history_rect_, FALSE);

	}
	else if (min_max_button_clicked_) {

		full_view_ = !full_view_;

		min_max_button_clicked_ = FALSE;
		RefreshPage();
	}
}