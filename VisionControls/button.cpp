#include "stdafx.h"

#define INCLUDE_BUTTON __declspec(dllexport)
#include "button.h"

//Button

Button::Button()
{
	button_pressed_ = FALSE;
	prev_button_pressed_ = FALSE;
	button_disabled_ = FALSE;
	button_down_ = FALSE;
	button_clicked_ = FALSE;
	highlight_changed_value_ = FALSE;
	button_type_ = BUTTON_TYPE::BUTTON_REGULAR;
	
	text_ = _T("");
	text_on_button_click_ = _T("");

	text_color_ = -1;
	text_flash_color_ = RGB(1,1,1);
	disable_color_ = DISABLE_COLOR;

	image_org_pt_.x = 5;
	image_org_pt_.y = 5;

	icon_alignment_ = ICON_ALIGNMENT::ST_ALIGN_OVERLAP;
	icon_info_[0].hIcon = NULL;
	icon_info_[1].hIcon = NULL;
}

Button::~Button() {

	Destroy();
}

IMPLEMENT_DYNAMIC(Button, CButton)

BEGIN_MESSAGE_MAP(Button, CButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void Button::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
}

void Button::Destroy() {
	 
	if (icon_info_[0].hIcon != NULL) {
		DestroyIcon(icon_info_[0].hIcon);
		icon_info_[0].hIcon = NULL;
	}
	if (icon_info_[1].hIcon != NULL) {
		DestroyIcon(icon_info_[1].hIcon);
		icon_info_[1].hIcon = NULL;
	}
}

void Button::Create(BUTTON_TYPE button_type, COLORREF back_color, COLORREF back_color_on_click, BOOL highlight_changed_value) {

	button_type_ = button_type;
	back_color_ = back_color;
	back_color_on_click_ = back_color_on_click;
	highlight_changed_value_ = highlight_changed_value;
	border_color_ = back_color;

	button_pressed_ = FALSE;
	prev_button_pressed_ = FALSE;
	button_disabled_ = FALSE;
	button_down_ = FALSE;
	button_clicked_ = FALSE;

	GetClientRect(&client_rect_);

	button_down_ = FALSE;
	text_flash_flag_ = FALSE;

	if (text_flash_color_ != RGB(1, 1, 1)) {
		SetTimer(1, 1000, NULL);
	}
}

void Button::SetText(CString text, CString text_on_button_pressed) {

	text_ = text;
	text_on_button_click_ = text_on_button_pressed;
	if (text_on_button_click_.IsEmpty()) {
		text_on_button_click_ = text;
	}

	Invalidate(FALSE);
}

CString Button::GetText() {

	if (button_pressed_) {
		return text_on_button_click_;
	} else {
		return text_;
	}
}

CString Button::GetText(BOOL pressed_off) {

	if (pressed_off) {
		return text_;
	} else {
		return text_on_button_click_;
	}
}

void Button::SetIcon(HICON icon, HICON icon_on_button_click, ICON_ALIGNMENT alignment) {

	icon_alignment_ = alignment;

	ICONINFO ii;
	if (icon) {
		icon_info_[0].hIcon = icon;
		::ZeroMemory(&ii, sizeof(ICONINFO));
		::GetIconInfo(icon, &ii);
		icon_info_[0].dwWidth = (DWORD)(ii.xHotspot * 2);
		icon_info_[0].dwHeight = (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);
	}
	if (icon_on_button_click) {
		icon_info_[1].hIcon = icon_on_button_click;
		::ZeroMemory(&ii, sizeof(ICONINFO));
		::GetIconInfo(icon_on_button_click, &ii);
		icon_info_[1].dwWidth = (DWORD)(ii.xHotspot * 2);
		icon_info_[1].dwHeight = (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);
	}

	if (alignment == ST_ALIGN_HORIZ) {
		image_org_pt_.x = icon_info_[0].dwWidth * 40 / 100;
	}

	Invalidate();
}

void Button::SetBackColor(COLORREF back_color) {

	back_color_ = back_color;
}

void Button::SetBorderColor(COLORREF border_color) {

	border_color_ = border_color;
}

void Button::SetTextColor(COLORREF text_color) {

	text_color_ = text_color;
}

void Button::SetTextFlashColor(COLORREF text_flash_color) {

	text_flash_color_ = text_flash_color;
}

void Button::SetDisableColor(COLORREF disable_color) {

	disable_color_ = disable_color;
	Invalidate(FALSE);
}

BOOL Button::PreTranslateMessage(MSG* pMsg)
{
	return CButton::PreTranslateMessage(pMsg);
}

void Button::DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled) {

	CRect ImgRect = rpItem;

	DWORD Width = icon_info_[bIsPressed].dwWidth;
	DWORD Height = icon_info_[bIsPressed].dwHeight;

	switch (icon_alignment_) {
		case ST_ALIGN_HORIZ: {
			if (bHasTitle == FALSE) {
				// Center image horizontally
				ImgRect.left += ((ImgRect.Width() - Width) / 2);
			} else {
				// Image must be placed just inside the focus rect
				ImgRect.left += ((ImgRect.Height() - Height) / 2);
				rpCaption->left = ImgRect.left + Height + (ImgRect.Height() - Height) / 2;
			}
			// Center image vertically
			ImgRect.top += ((ImgRect.Height() - Height) / 2);
			break;
		}
		case ST_ALIGN_HORIZ_RIGHT: {
			CRect rBtn;
			GetClientRect(&rBtn);
			if (bHasTitle == FALSE) {
				// Center image horizontally
				ImgRect.left += ((ImgRect.Width() - Width) / 2);
			} else {
				// Image must be placed just inside the focus rect
				rpCaption->right = rpCaption->Width() - Width - image_org_pt_.x;
				rpCaption->left = image_org_pt_.x;
				ImgRect.left = rBtn.right - Width - image_org_pt_.x;
				// Center image vertically
				ImgRect.top += ((ImgRect.Height() - Height) / 2);
			}
			break;
		}
		case ST_ALIGN_VERT: {
			// Center image horizontally
			ImgRect.left += ((ImgRect.Width() - Width) / 2);
			if (bHasTitle == FALSE) {
				// Center image vertically
				ImgRect.top += ((ImgRect.Height() - Height) / 2);
			} else {
				CRect calc_rect;
				pDC->DrawText(text_, &calc_rect, DT_CALCRECT);
				ImgRect.top += ((ImgRect.Height() - calc_rect.Height() - Formation::spacing() - Height) / 2);
				rpCaption->top += Height;
			}
			break;
		}
		case ST_ALIGN_CENTER: {
			// Center image horizontally
			ImgRect.left += ((ImgRect.Width() - Width) / 2);
			if (bHasTitle == FALSE) {
				// Center image vertically
				ImgRect.top += ((ImgRect.Height() - Height) / 2);
			} else {
				ImgRect.top += ((ImgRect.Height() - Height) / 2);
				rpCaption->top += (ImgRect.top + Height);
			}
			break;
		}
		case ST_ALIGN_OVERLAP: {
			ImgRect.left += (ImgRect.Width() - Width) / 2;
			ImgRect.top += (ImgRect.Height() - Height) / 2;
			if (ImgRect.left <= 0) ImgRect.left = 0;
			if (ImgRect.top <= 0) ImgRect.top = 0;
			break;
		}
	}

	pDC->DrawState(ImgRect.TopLeft(), ImgRect.Size(), icon_info_[bIsPressed].hIcon, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), (CBrush*)NULL);
}

void Button::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (client_rect_.IsRectEmpty()) return;

	COfflineDC cdc(&dc, &client_rect_);
	cdc.SelectObject(GetFont());

	CRect rect = client_rect_;
	CRect text_rect(rect.left + Formation::spacing(), rect.top + Formation::spacing(), rect.right - Formation::spacing(), rect.bottom - Formation::spacing());

	int border_size = (button_type_ == BUTTON_RADIO_PUSH_LIKE || button_type_ == BUTTON_CHECK_BOX_PUSH_LIKE) ? 1 : 0;
	if (back_color_ == WHITE_COLOR) {
		if(!text_.IsEmpty()) {
			if (!button_disabled_) {
				if (button_pressed_ && (button_type_ == BUTTON_REGULAR || button_type_ == BUTTON_RADIO_PUSH_LIKE || button_type_ == BUTTON_CHECK_BOX_PUSH_LIKE)) {
					cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), back_color_on_click_);
				} else {
					cdc.SelectObject(Formation::spancolor1_pen());
					cdc.Rectangle(rect);
				}
			} else {
				if (back_color_ == WHITE_COLOR && button_down_ && (button_type_ == BUTTON_RADIO || button_type_ == BUTTON_CHECK_BOX)) { //Border color and text color should remain as it is
					cdc.SelectObject(Formation::spancolor1_pen());
				} else {
					cdc.SelectObject(Formation::disablecolor_pen());
				}
				cdc.Rectangle(rect);
			}
		}
	} else {
		if (button_disabled_) {
			if (back_color_ == WHITE_COLOR) {
				cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), WHITE_COLOR);
			} else {
				cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), disable_color_);
			}
		} else {
			if (!button_pressed_) {
				cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), back_color_);
			} else {
				if (border_size > 0) {
					cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), border_color_);
					rect.DeflateRect(border_size, border_size, border_size, border_size);
				}
				cdc.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), back_color_on_click_);
			}
		}
	}

	if (icon_info_[0].hIcon == NULL) {

		if (button_type_ == BUTTON_RADIO || button_type_ == BUTTON_CHECK_BOX) {

			icon_alignment_ = ST_ALIGN_HORIZ;
			icon_info_[0].dwWidth = rect.Height();
			icon_info_[0].dwHeight = rect.Height();
			icon_info_[1].dwWidth = rect.Height();
			icon_info_[1].dwHeight = rect.Height();

			int offset = client_rect_.Height() / 4;
			CRect on_rect(client_rect_.left + offset, client_rect_.top + offset, client_rect_.left + offset * 2 + offset, client_rect_.top + offset * 2 + offset);
			CRect off_rect(on_rect.left + offset / 2, on_rect.top + offset / 2, on_rect.right - offset / 2, on_rect.bottom - offset / 2);
			Gdiplus::Graphics graphics(cdc);
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

			if (button_type_ == BUTTON_RADIO) {

				if (back_color_ == WHITE_COLOR) {

					if (button_disabled_) {
						graphics.DrawEllipse(Formation::disablecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
						if (button_pressed_) {
							graphics.FillEllipse(Formation::disablecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
						}
					} else {
						graphics.DrawEllipse(Formation::spancolor1_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
						if (button_pressed_) {
							graphics.FillEllipse(Formation::spancolor1_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
						}
					}

				} else {

					graphics.DrawEllipse(Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
					if (button_pressed_) {
						graphics.FillEllipse(Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					}
				}

			} else if (button_type_ == BUTTON_CHECK_BOX) {

				if (back_color_ == WHITE_COLOR) {

					if (button_disabled_) {

						graphics.DrawRectangle(Formation::disablecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
						if (button_pressed_) {
							graphics.FillRectangle(Formation::disablecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
						}

					} else {

						if (highlight_changed_value_ && prev_button_pressed_ != button_pressed_ && text_.IsEmpty()) { //TO HIGHLIGHT CHANGE WITH BACKCOLOR2
							graphics.DrawRectangle(Formation::spancolor2_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
							if (button_pressed_) {
								graphics.FillRectangle(Formation::spancolor2_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
							}
						} else {
							graphics.DrawRectangle(Formation::spancolor1_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
							if (button_pressed_) {
								graphics.FillRectangle(Formation::spancolor1_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
							}
						}
					}

				} else {

					if (highlight_changed_value_ && prev_button_pressed_ != button_pressed_ && text_.IsEmpty()) { //TO HIGHLIGHT CHANGE WITH BACKCOLOR2
						graphics.DrawRectangle(Formation::spancolor2_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
						if (button_pressed_) {
							graphics.FillRectangle(Formation::spancolor2_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
						}
					} else {
						graphics.DrawRectangle(Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
						if (button_pressed_) {
							graphics.FillRectangle(Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
						}
					}
				}
			}
		}
	}
	bool has_title = !text_.IsEmpty();
	CRect caption_rect(rect);
	DrawTheIcon(&cdc, has_title, rect, &caption_rect, button_pressed_, button_disabled_);

	DWORD text_alignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	if (icon_alignment_ == ST_ALIGN_VERT) {
		text_alignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
		text_rect.top = caption_rect.top;
	} else if (icon_alignment_ == ST_ALIGN_CENTER) {
		text_alignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
		text_rect.top = caption_rect.top;
	} else if (icon_alignment_ == ST_ALIGN_HORIZ) {
		text_alignment = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
		text_rect.left = caption_rect.left;
	} else if (icon_alignment_ == ST_ALIGN_HORIZ_RIGHT) {
		text_alignment = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	}

	if (button_disabled_) {
		if (back_color_ == WHITE_COLOR && button_down_ && (button_type_ == BUTTON_RADIO || button_type_ == BUTTON_CHECK_BOX)) { //Border color and text color should remain as it is
			cdc.SetTextColor(BLACK_COLOR);
		} else {
			(back_color_ == WHITE_COLOR) ? cdc.SetTextColor(DISABLE_COLOR) : cdc.SetTextColor(WHITE_COLOR);
		}
	} else {
		if (back_color_ == WHITE_COLOR) {
			cdc.SetTextColor(BLACK_COLOR);
		} else {
			if (text_flash_flag_) {
				cdc.SetTextColor(text_flash_color_);
			} else {
				cdc.SetTextColor(text_color_ == -1 ? WHITE_COLOR : text_color_);
			}
		}
	}
	CString text = button_pressed_ ? text_on_button_click_ : text_;
	if (highlight_changed_value_ && prev_button_pressed_ != button_pressed_ && !text_.IsEmpty()) {
		text = L"*" + text;
	}
	cdc.DrawText(Formation::PrepareString(cdc, text, text_rect), text_rect, text_alignment);
}

void Button::SetCheck(BOOL check, BOOL highlight) {

	button_pressed_ = check;

	if (!highlight) {
		prev_button_pressed_ = check;
	}

	Invalidate(FALSE);
}

void Button::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (button_down_) return;

	button_down_ = TRUE;

	if (IsWindowEnabled()) {

		SetCapture();

		if (text_flash_color_ != RGB(1, 1, 1)) {
			text_flash_flag_ = FALSE;
			KillTimer(1);
		}
	
		if (button_type_ == BUTTON_TYPE::BUTTON_REGULAR) {
			button_pressed_ = TRUE;
			Invalidate(FALSE);
		} else if (button_type_ == BUTTON_TYPE::BUTTON_CHECK_BOX || button_type_ == BUTTON_CHECK_BOX_PUSH_LIKE) {
			button_disabled_ = TRUE;
			Invalidate(FALSE);
		}
	}
//	CButton::OnLButtonDown(nFlags, point);
}

void Button::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (button_down_ && IsWindowEnabled()) {

		if (button_type_ == BUTTON_TYPE::BUTTON_RADIO || button_type_ == BUTTON_RADIO_PUSH_LIKE) {
			if (button_pressed_ == TRUE) {
				button_down_ = FALSE;
				return;
			}
		}

		EnableWindow(FALSE);

		if (button_type_ != BUTTON_TYPE::BUTTON_REGULAR) {
			button_pressed_ = !button_pressed_;
		}

		button_status_on_click_ = TRUE;
		button_clicked_ = TRUE;
		if (GetParent() != NULL) {
			GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
		}
		if (GetParent() != NULL) {
			GetParent()->SendMessageW(WM_BN_CLICKED, (WPARAM)this, (LPARAM)(button_pressed_));
		}
		button_clicked_ = FALSE;

		if (button_type_ == BUTTON_TYPE::BUTTON_REGULAR) {
			button_pressed_ = FALSE;
		} else {
			//SetCheck(button_pressed_);
			Invalidate(FALSE);
		}

		EnableWindow(button_status_on_click_);

		if (!button_pressed_ && text_flash_color_ != RGB(1, 1, 1)) {
			SetTimer(1, 1000, NULL);
		}
	}
	//CButton::OnLButtonUp(nFlags, point);

	button_down_ = FALSE;
}

void Button::EnableWnd(BOOL bEnable) {

	if (button_clicked_) {
		button_status_on_click_ = bEnable; //If disabled from outside in BN_CLICKED handler
	} else {
		BOOL result = EnableWindow(bEnable);
		if ((!bEnable && result) || (bEnable && !result)) {
			OnEnable(bEnable);
		}
	}
}

void Button::OnEnable(BOOL bEnable)
{
	button_disabled_ = !bEnable;

	Invalidate(FALSE);

	//CButton::OnEnable(bEnable);
}

void Button::OnTimer(UINT_PTR nIDEvent)
{
	text_flash_flag_ = !text_flash_flag_;

	Invalidate(FALSE);

	CButton::OnTimer(nIDEvent);
}


void Button::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//CButton::OnLButtonDblClk(nFlags, point);
}
