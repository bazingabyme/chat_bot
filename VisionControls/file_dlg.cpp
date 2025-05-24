// file_dlg.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_FILEDLG __declspec(dllexport)
#include "file_dlg.h"

// FileDlg

IMPLEMENT_DYNAMIC(FileDlg, CDialogEx)

FileDlg::FileDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(FileDlg::IDD, pParent)
{
}

FileDlg::~FileDlg()
{
}

void FileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDMST_FILE_MFCSHELLTREE, ctrShellTree);
	DDX_Control(pDX, IDLC_FILE_LIST, ctrList);
}

BEGIN_MESSAGE_MAP(FileDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDMST_FILE_MFCSHELLTREE, &FileDlg::OnTvnSelchangedMfcshelltree)
	ON_NOTIFY(NM_CLICK, IDLC_FILE_LIST, &FileDlg::OnNMClickFilelist)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// FileDlg message handlers

INT_PTR FileDlg::Open(BOOL file_selection, CString extention) {

	file_selection_ = file_selection;
	extention_list_.RemoveAll();
	extention_list_.Add(extention);

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	INT_PTR result = DoModal();
	AfxSetResourceHandle(old_resource_handle);
	return result;
}

INT_PTR FileDlg::Open(BOOL file_selection, CStringArray& extention) {

	file_selection_ = file_selection;
	extention_list_.RemoveAll();
	for (int index = 0; index < int(extention.GetSize()); index++) {
		extention_list_.Add(extention[index]);
	}

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	INT_PTR result = DoModal();
	AfxSetResourceHandle(old_resource_handle);
	return result;
}

BOOL FileDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK) {
		pMsg->message = WM_LBUTTONDOWN;
	}
	if (pMsg->message == WM_MOUSEMOVE && (pMsg->wParam & MK_LBUTTON)) {
		CPoint p;
		GetCursorPos(&p);
		CRect r;
		GetWindowRect(&r);
		CRect t = r;
		t.bottom = t.top + Formation::heading_height();
		if (t.PtInRect(p)) {
			ReleaseCapture();
			SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SendMessage(WM_NCLBUTTONUP, HTCAPTION, 0);
			return TRUE;
		}
	}
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_F1 || pMsg->wParam == VK_F2 ||
		pMsg->wParam == VK_F3 || pMsg->wParam == VK_F4 || pMsg->wParam == VK_F5 || pMsg->wParam == VK_F6 ||
		pMsg->wParam == VK_F7 || pMsg->wParam == VK_F8 || pMsg->wParam == VK_F9 || pMsg->wParam == VK_F10 ||
		pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 || pMsg->wParam == VK_END || pMsg->wParam == VK_HOME) {
		return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL FileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(WHITE_COLOR);
	//ModifyStyle(WS_CAPTION, 0);
	
	GetClientRect(client_rect_);
	if (file_selection_) {
		client_rect_.right = client_rect_.left + Formation::control_height() * 32;
	} else {
		client_rect_.right = client_rect_.left + Formation::control_height() * 16;
	}

	CRect control_rect = client_rect_;
	control_rect.bottom = control_rect.top + Formation::control_height();
	title_rect_ = control_rect;

	control_rect.DeflateRect(Formation::spacing(), 0, Formation::spacing(), 0);
	control_rect.top = title_rect_.bottom + Formation::spacing2();
	control_rect.bottom = control_rect.top + Formation::heading_height();
	control_rect.top = control_rect.bottom;
	control_rect.bottom = control_rect.top + Formation::control_height() * 16;
	control_rect.right = Formation::control_height() * 16 - Formation::spacing2();
	ctrShellTree.MoveWindow(control_rect);
	ctrShellTree.SetFont(&Formation::font(Formation::MEDIUM_FONT));
	ctrShellTree.SetFlags(ctrShellTree.GetFlags() | SHCONTF_INCLUDEHIDDEN | SHCONTF_INCLUDESUPERHIDDEN);
	HTREEITEM hParentItem = ctrShellTree.GetRootItem();
	ctrShellTree.SelectItem(hParentItem);
	ctrShellTree.Expand(hParentItem, TVE_EXPAND);
	ctrList.ShowWindow(SW_HIDE);
	if (file_selection_) {
		control_rect.left = control_rect.right + Formation::spacing2();
		control_rect.right = client_rect_.right - Formation::spacing2();
		ctrList.MoveWindow(control_rect);
		ctrList.InsertColumn(0, L"", 0, control_rect.Width() * 96 / 100);
		ctrList.SetFont(&Formation::font(Formation::MEDIUM_FONT));
		ctrList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
		ctrList.ShowWindow(SW_SHOW);
	}
	control_rect.top = control_rect.bottom + Formation::spacing2();
	control_rect.bottom = control_rect.top + Formation::heading_height();
	control_rect.left = client_rect_.left + Formation::spacing();
	control_rect.right = client_rect_.right - Formation::spacing2();
	path_rect_ = control_rect;
	control_rect.top = control_rect.bottom + Formation::spacing2();
	control_rect.bottom = control_rect.top + Formation::control_height();
	control_rect.left = control_rect.right - Formation::control_height() * 10 - Formation::spacing();
	control_rect.right = control_rect.left + Formation::control_height() * 5;
	open_rect_ = control_rect;
	control_rect.left = control_rect.right + Formation::spacing();
	control_rect.right = control_rect.left + Formation::control_height() * 5;
	close_rect_ = control_rect;

	client_rect_.bottom = control_rect.bottom + Formation::spacing3();

	client_rect_.right += GetSystemMetrics(SM_CXDLGFRAME);
	client_rect_.bottom += GetSystemMetrics(SM_CYDLGFRAME);
	int screen_width = GetSystemMetrics(SM_CXSCREEN); //Get the system metrics - VERT
	int screen_height = GetSystemMetrics(SM_CYSCREEN); //Get the system metrics - HORZ
	int window_width = client_rect_.Width();
	int window_height = client_rect_.Height();
	int offset_x = (screen_width - window_width) / 2;
	int offset_y = (screen_height - window_height) / 2;

	SetWindowPos(&CWnd::wndTop, client_rect_.left + offset_x, client_rect_.top + offset_y, window_width, window_height, SWP_SHOWWINDOW);

	open_button_clicked_ = close_button_clicked_ = FALSE;
	
	FillList();
	SetFilePath(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void FileDlg::FillList() {

	if (!file_selection_) {
		return;
	}

	HTREEITEM hItem = ctrShellTree.GetSelectedItem();
	CString folder_path;
	if (ctrShellTree.GetItemPath(folder_path, hItem)) {
		//ctrList.DisplayFolder(folder_path);
		ctrList.DeleteAllItems();
		for (int index = 0; index < int(extention_list_.GetSize()); index++) {
			WIN32_FIND_DATA findfiledata = { 0 };
			folder_path = folder_path + L"\\" + extention_list_[index];
			HANDLE findfilehandle = FindFirstFile(folder_path, &findfiledata);
			if (findfilehandle != INVALID_HANDLE_VALUE) {
				int index = 0;
				do {
					if (wcscmp(findfiledata.cFileName, L".") != 0 && wcscmp(findfiledata.cFileName, L"..") != 0) {
						ctrList.InsertItem(index, CString(findfiledata.cFileName), index);
						++index;
					}
					if (index == 1000) {
						break;
					}
				} while (FindNextFile(findfilehandle, &findfiledata) != 0);
				FindClose(findfilehandle);
			}
		}
		if (ctrList.GetItemCount() > 0) {
			ctrList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

void FileDlg::SetFilePath(int index) {

	file_name_ = _T("");
	ctrShellTree.GetItemPath(file_path_, ctrShellTree.GetSelectedItem());
	if (file_selection_) {
		if (index != -1 && index < ctrList.GetItemCount()) {
			file_name_ = ctrList.GetItemText(index, 0);
			file_path_ = file_path_ + L"\\" + file_name_;
		}
	}
	InvalidateRect(path_rect_, FALSE);
}

void FileDlg::OnTvnSelchangedMfcshelltree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	FillList();
	SetFilePath(0);

	*pResult = 0;
}

void FileDlg::OnNMClickFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	SetFilePath(pNMListView->iItem);

	*pResult = 0;
}

void FileDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(WHITE_COLOR);

	offdc.SelectObject(Formation::font(Formation::BIG_FONT));
	offdc.FillSolidRect(title_rect_, BACKCOLOR1);
	offdc.DrawText(L" " + Language::GetString(IDSTRINGT_SELECT_FILE), title_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	offdc.SelectObject(Formation::font(Formation::MEDIUM_FONT));

	offdc.SelectObject(GetStockObject(NULL_BRUSH));
	offdc.SelectObject(Formation::labelcolor_pen());
	CRect wnd_rect;
	ctrShellTree.GetWindowRect(&wnd_rect);
	ScreenToClient(&wnd_rect);
	//wnd_rect.InflateRect(2, 1, 0, 1);
	wnd_rect.InflateRect(1, 2, 1, 2);
	offdc.Rectangle(wnd_rect);
	wnd_rect.bottom = wnd_rect.top;
	wnd_rect.top = wnd_rect.bottom - Formation::heading_height();
	offdc.FillSolidRect(wnd_rect, LABEL_COLOR);
	offdc.DrawText(L" " + Language::GetString(IDSTRINGT_DIRECTORIES), wnd_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	if (file_selection_) {
		ctrList.GetWindowRect(&wnd_rect);
		ScreenToClient(&wnd_rect);
		wnd_rect.InflateRect(1, 2, 1, 2);
		offdc.Rectangle(wnd_rect);
		wnd_rect.bottom = wnd_rect.top;
		wnd_rect.top = wnd_rect.bottom - Formation::heading_height();
		offdc.FillSolidRect(wnd_rect, LABEL_COLOR);
		CString extention;
		for (int index = 0; index < int(extention_list_.GetSize()); index++) {
			if (index > 0) extention += L" | ";
			extention += extention_list_[index];
		}
		offdc.DrawText(L" " + Language::GetString(IDSTRINGT_FILES) + L" (" + extention + L")", wnd_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	//offdc.FillSolidRect(path_rect_, LABEL_COLOR2);
	offdc.SetTextColor(BLACK_COLOR);
	offdc.DrawText(L" " + Language::GetString(IDSTRINGT_PATH) + L": " + file_path_, path_rect_, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	offdc.SetTextColor(WHITE_COLOR);
	if (open_button_clicked_) {
		offdc.FillSolidRect(open_rect_, BACKCOLOR1_SEL);
	} else {
		offdc.FillSolidRect(open_rect_, BACKCOLOR1);
	}
	offdc.DrawText(Language::GetString(IDSTRINGT_OK), open_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (close_button_clicked_) {
		offdc.FillSolidRect(close_rect_, BACKCOLOR1_SEL);
	} else {
		offdc.FillSolidRect(close_rect_, BACKCOLOR1);
	}
	offdc.DrawText(Language::GetString(IDSTRINGT_CANCEL), close_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void FileDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	//OK BUTTON
	open_button_clicked_ = FALSE;
	if (open_rect_.PtInRect(point)) {
		open_button_clicked_ = TRUE;
		InvalidateRect(open_rect_, FALSE);
		return;
	}
	//CLOSE BUTTON
	close_button_clicked_ = FALSE;
	if (close_rect_.PtInRect(point)) {
		close_button_clicked_ = TRUE;
		InvalidateRect(close_rect_, FALSE);
		return;
	}
}

void FileDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	//OK BUTTON
	if (open_button_clicked_) {
		open_button_clicked_ = FALSE;
		if (file_selection_) {
			POSITION pos = ctrList.GetFirstSelectedItemPosition();
			int index = ctrList.GetNextSelectedItem(pos);
			if (pos < 0 || index == -1) {
				Formation::MsgBox(Language::GetString(IDSTRINGM_SELECT_FILE_FROM_LIST));
				InvalidateRect(open_rect_, FALSE);
				return;
			}
		}
		EndDialog(1);
		return;
	}
	//CLOSE BUTTON
	if (close_button_clicked_) {
		close_button_clicked_ = FALSE;
		EndDialog(0);
	}
}