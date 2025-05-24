#include "stdafx.h"

#define INCLUDE_ONOFFCTRL __declspec(dllexport)
#include "onoffctrl.h"

//OnOffCtrl

IMPLEMENT_DYNAMIC(OnOffCtrl, CStatic)

OnOffCtrl::OnOffCtrl()
{
	text_[0].Empty();
	text_[1].Empty();
	highlight_changed_value_ = FALSE;
}

OnOffCtrl::~OnOffCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(OnOffCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// OnOffCtrl message handlers

void OnOffCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void OnOffCtrl::Create(int text_area, DWORD text_aligment, BOOL highlight_changed_value) {
	
	highlight_changed_value_ = highlight_changed_value;
	text_aligment_ = text_aligment;

	GetClientRect(client_rect_);

	int width = client_rect_.Width() * text_area / 100;

	checked_ = prev_checked_ = FALSE;

	caption_rect_.SetRect(client_rect_.left, client_rect_.top, width, client_rect_.bottom);

	CRect rect(caption_rect_.right + Formation::spacing(), client_rect_.top + Formation::spacing(), client_rect_.right - Formation::spacing(), client_rect_.bottom - Formation::spacing());

	rect_[0].SetRect(rect.left, rect.top, rect.left + rect.Width() / 2, rect.bottom);
	rect_[1].SetRect(rect_[0].right, rect_[0].top, rect_[0].right + rect_[0].Width(), rect_[0].bottom);

	if (text_[0].IsEmpty() && text_[1].IsEmpty()) {
		text_[0] = Language::GetString(IDSTRINGT_ON);
		text_[1] = Language::GetString(IDSTRINGT_OFF);
	}
}

void OnOffCtrl::Destroy() {

}

void OnOffCtrl::SetText(CString text, CString on_text, CString off_text) {

	caption_text_ = L" " + text + L" ";

	text_[0] = on_text;
	text_[1] = off_text;

	Invalidate(FALSE);
}

CString OnOffCtrl::GetText(BOOL right) {

	return text_[right];
}

BOOL OnOffCtrl::GetCheck() {

	return checked_;
}

void OnOffCtrl::SetCheck(BOOL check, BOOL update, BOOL highlight) {

	checked_ = check;

	if (update) {
		GetParent()->SendMessageW(WM_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
	}
	InvalidateRect(rect_[!checked_], FALSE);
	InvalidateRect(rect_[checked_], FALSE);

	if (!highlight) {
		prev_checked_ = check;
	}

	//TO RESET L"*" IN TEXT
	InvalidateRect(caption_rect_, FALSE);
}

void OnOffCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());

	offdc.SelectObject(GetStockObject(NULL_BRUSH));

	BOOL window_enabled = IsWindowEnabled();

	window_enabled ? offdc.SelectObject(Formation::spancolor1_pen()) : offdc.SelectObject(Formation::disablecolor_pen());
	offdc.Rectangle(client_rect_);
	window_enabled ? offdc.SetTextColor(BLACK_COLOR) : offdc.SetTextColor(DISABLE_COLOR);

	CString text = caption_text_;
	if (highlight_changed_value_ && checked_ != prev_checked_) {
		text.Trim();
		if (text_aligment_ == DT_LEFT) {
			text = L" *" + text;
		} else {
			text = L"*" + text + L" ";
		}
	}
	offdc.DrawText(Formation::PrepareString(offdc, text, caption_rect_), caption_rect_, text_aligment_ | DT_SINGLELINE | DT_VCENTER);

	window_enabled ? offdc.SelectObject(Formation::spancolor1_brush()) : offdc.SelectObject(Formation::disablecolor_brush());
	offdc.Rectangle(CRect(caption_rect_.right, client_rect_.top, client_rect_.right, client_rect_.bottom));

	offdc.SetTextColor(WHITE_COLOR);
	offdc.SelectObject(Formation::whitecolor_pen());

	if (rect_[0].PtInRect(down_point_) && !checked_) {
		offdc.SelectObject(Formation::spancolor1_pen_for_selection());
	} else {
		checked_ ? offdc.SelectObject(Formation::whitecolor_pen()) : offdc.SelectObject(GetStockObject(NULL_PEN));
	}
	offdc.Rectangle(rect_[0]);
	offdc.DrawText(Formation::PrepareString(offdc, text_[0], rect_[0]), rect_[0], DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if (rect_[1].PtInRect(down_point_) && checked_) {
		offdc.SelectObject(Formation::spancolor1_pen_for_selection());
	} else {
		checked_ ? offdc.SelectObject(GetStockObject(NULL_PEN)) : offdc.SelectObject(Formation::whitecolor_pen());
	}
	offdc.Rectangle(rect_[1]);
	offdc.DrawText(Formation::PrepareString(offdc, text_[1], rect_[1]), rect_[1], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void OnOffCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	down_point_ = point;
	InvalidateRect(rect_[!checked_], FALSE);
	InvalidateRect(rect_[checked_], FALSE);
	SetCapture();
}

void OnOffCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (rect_[0].PtInRect(down_point_) && !checked_) {
		checked_ = TRUE;
		GetParent()->SendMessageW(WM_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
		Invalidate(FALSE);
	}
	if (rect_[1].PtInRect(down_point_) && checked_) {
		checked_ = FALSE;
		GetParent()->SendMessageW(WM_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
		Invalidate(FALSE);
	}
	down_point_.SetPoint(-1, -1);
	ReleaseCapture();
}

void OnOffCtrl::OnEnable(BOOL bEnable)
{
	Invalidate(FALSE);

	//CStatic::OnEnable(bEnable);
}

