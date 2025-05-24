// gridctrlex.cpp : implementation file
//

#include "stdafx.h"
#include "keyboard_dlg.h"

#define INCLUDE_GRIDCTRLEX __declspec(dllexport)
#include "gridctrlex.h"

//GridColumnCell

void GridCtrlEx::GridColumnCell::SetText(CString text) { 
	
	text_ = text; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridColumnCell::SetBackColor(COLORREF back_color) {

	back_color_ = back_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridColumnCell::SetBorderColor(COLORREF border_color) {

	border_color_ = border_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridColumnCell::SetTextColor(COLORREF text_color) {

	text_color_ = text_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

//GridCell

void GridCtrlEx::GridCell::DisableRow(BOOL disable) {

	disable_row_ = disable;

	if (associated_parent_cell_) {
		associated_parent_cell_->disable_row_ = disable;
		gridctrlex_->InvalidateRect(associated_parent_cell_->rect_, FALSE);
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCell::DisableCell(BOOL disable) {

	disable_ = disable;
	
	if(associated_parent_cell_) {
		associated_parent_cell_->disable_ = disable;
		gridctrlex_->InvalidateRect(associated_parent_cell_->rect_, FALSE);
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCell::VisibleCell(BOOL visible) {

	visible_ = visible;

	if (associated_parent_cell_) {
		associated_parent_cell_->visible_ = visible;
		gridctrlex_->InvalidateRect(associated_parent_cell_->rect_, FALSE);
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
}

GridCtrlEx::GridCell* GridCtrlEx::GridCell::GetAssociatedParentCell() {

	return associated_parent_cell_;
}

void GridCtrlEx::GridCell::SetAssociatedParentCell(GridCell* cell) {

	associated_parent_cell_ = cell;
}

GridCtrlEx::GridCell* GridCtrlEx::GridCell::GetAssociatedCell(int index) {

	if (index < associated_cells_.GetSize()) {
		return associated_cells_[index];
	}
	return NULL;
}

void GridCtrlEx::GridCell::SetAssociatedCell(GridCell* cell) {

	if (cell) {
		associated_cells_.Add(cell);
		cell->associated_parent_cell_ = this;

		//IF CURRENT CELL IS TEXT THEN SET KEYBOARD CAPTION 
		if (cell_type_ == CELL_TYPE::CT_TEXT) {
			GridCellText* cell_text = (GridCellText*)this;
			if (cell_text) {
				if (cell->GetCellType() == CELL_TYPE::CT_EDIT) {
					GridCellEdit* cell_edit = (GridCellEdit*)cell;
					if (cell_edit) {
						cell_edit->SetKeyBoardCaptionBarText(cell_text->GetText());
					}
				} else if (cell->GetCellType() == CELL_TYPE::CT_SPIN) {
					GridCellSpin* cell_spin = (GridCellSpin*)cell;
					if (cell_spin) {
						cell_spin->SetKeyBoardCaptionBarText(cell_text->GetText());
					}
				}
			}
		}
	}
}

void GridCtrlEx::GridCell::ValueChanged(BOOL change) { 
	
	value_changed_ = change; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

//GridCellLabel

void GridCtrlEx::GridCellLabel::SetText(CString text) {
	
	text_ = text; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellLabel::SetBackColor(COLORREF back_color) { 
	
	back_color_ = back_color; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellLabel::SetBorderColor(COLORREF border_color) { 

	border_color_ = border_color; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellLabel::SetTextColor(COLORREF text_color) { 

	text_color_ = text_color; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellLabel::PrepareCell() {

}

void GridCtrlEx::GridCellLabel::DrawCell(COfflineDC& offdc) {

	if (disable_ || disable_row_) {
		offdc.FillSolidRect(rect_, DISABLE_COLOR);
	} else {
		HPEN pen = NULL;
		HPEN old_pen = NULL;
		HBRUSH brush = CreateSolidBrush(back_color_);
		if (allow_selection_ && gridctrlex_->GetSelectedIndex() == index_) {
			offdc.SelectObject(Formation::blackcolor_pen());
		} else {
			pen = CreatePen(PS_SOLID, 1, border_color_);
			old_pen = (HPEN)offdc.SelectObject(pen);
		}
		HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);

		offdc.Rectangle(rect_);

		if (old_pen != NULL) {
			offdc.SelectObject(old_pen);
			DeleteObject(pen);
		}
		if (old_brush != NULL) {
			offdc.SelectObject(old_brush);
			DeleteObject(brush);
		}
	}

	CString text = text_;
	if (align_ == DT_LEFT) {
		text = L" " + text;
	} else if (align_ == DT_RIGHT) {
		text = text + L" ";
	}
	offdc.SetTextColor((disable_ || disable_row_) ? WHITE_COLOR : text_color_);
	offdc.DrawText(Formation::PrepareString(offdc, text, rect_), rect_, align_ | DT_SINGLELINE | DT_VCENTER);
}

void GridCtrlEx::GridCellLabel::LButtonDown(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellLabel::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellLabel::LButtonUp(UINT nFlags, CPoint point) {

	if (allow_selection_) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SELCHANGE, (WPARAM)this, (LPARAM)(index_));
	}
}

void GridCtrlEx::GridCellLabel::Destroy() {

}

//GridCellText

void GridCtrlEx::GridCellText::SetText(CString text) { 
	
	text_ = text; 
	gridctrlex_->InvalidateRect(rect_, FALSE);

	for (int index = 0; index < int(associated_cells_.GetSize()); index++) { //To set the keyboard text of associated cell if text changed
		GridCell* cell = associated_cells_[index];
		if (cell->GetCellType() == CELL_TYPE::CT_EDIT) {
			GridCellEdit* cell_edit = (GridCellEdit*)cell;
			if (cell_edit) {
				cell_edit->SetKeyBoardCaptionBarText(text_);
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_SPIN) {
			GridCellSpin* cell_spin = (GridCellSpin*)cell;
			if (cell_spin) {
				cell_spin->SetKeyBoardCaptionBarText(text_);
			}
		}
	}
}

void GridCtrlEx::GridCellText::SetBackColor(COLORREF back_color, BOOL update) {

	back_color_ = back_color;
	if (update) gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetBorderColor(COLORREF border_color, BOOL update) {

	border_color_ = border_color;
	if (update) gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetTextColor(COLORREF text_color, BOOL update) {

	text_color_ = text_color;
	if(update) gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetSelBackColor(COLORREF sel_back_color) {

	sel_back_color_ = sel_back_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetSelBorderColor(COLORREF sel_border_color) {

	sel_border_color_ = sel_border_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetSelTextColor(COLORREF sel_text_color) {

	sel_text_color_ = sel_text_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::SetBallonData(CString data, COLORREF color) {

	balloon_color_ = color;
	balloon_data_ = data;

	int find = balloon_data_.Find(L".");
	if (find != -1) {
		find += 1;
		BOOL double_precision = (balloon_data_.GetLength() - find) == 2 ? TRUE : FALSE;
		if (double_precision) {
			CString text = balloon_data_.Mid(find, 2); //If it is 15.00 then should show 15 only
			if (text == L"00") {
				balloon_data_ = balloon_data_.Mid(0, balloon_data_.GetLength() - 3);
			}
		} else {
			CString text = balloon_data_.Mid(find, 1); //If it is 15.0 then should show 15 only
			if (text == L"0") {
				balloon_data_ = balloon_data_.Mid(0, balloon_data_.GetLength() - 2);
			}
		}
	}

	ShowBalloon(TRUE);
}

void GridCtrlEx::GridCellText::SetBallonData(int data, COLORREF color) {

	balloon_data_.Format(L"%d", data);
	balloon_color_ = color;

	SetBallonData(balloon_data_, balloon_color_);
}

void GridCtrlEx::GridCellText::ShowBalloon(BOOL show) {

	show_balloon_ = show;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellText::PrepareCell() {

}

void GridCtrlEx::GridCellText::DrawCell(COfflineDC& offdc) {

	CRect text_rect = rect_;
	CRect balloon_rect;
	if (show_balloon_) {
		text_rect.right = rect_.right - (rect_.Width() * 40 / 100);
		balloon_rect = rect_;
		balloon_rect.left = text_rect.right;
		balloon_rect.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
	}
	if (disable_ || disable_row_) {

		if (border_color_ != WHITE_COLOR && gridctrlex_->spacing_ > 0) {

			HPEN pen = CreatePen(PS_SOLID, 1, disable_border_color_);
			HBRUSH brush = CreateSolidBrush(disable_back_color_);
			HPEN old_pen = (HPEN)offdc.SelectObject(pen);
			HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);
			offdc.Rectangle(rect_);
			if (old_pen != NULL) {
				offdc.SelectObject(old_pen);
				DeleteObject(pen);
			}
			if (old_brush != NULL) {
				offdc.SelectObject(old_brush);
				DeleteObject(brush);
			}
		}

	} else {

		if(gridctrlex_->spacing_ > 0) {

			HPEN pen = NULL;
			HPEN old_pen = NULL;
			HBRUSH brush = NULL;
			HBRUSH old_brush = NULL;
			if (allow_selection_ && gridctrlex_->GetSelectedIndex() == index_) {
				pen = CreatePen(PS_SOLID, 1, sel_border_color_);
				brush = CreateSolidBrush(sel_back_color_);
			} else {
				pen = CreatePen(PS_SOLID, 1, border_color_);
				brush = CreateSolidBrush(back_color_);
			}
			old_brush = (HBRUSH)offdc.SelectObject(brush);
			old_pen = (HPEN)offdc.SelectObject(pen);
		
			offdc.Rectangle(rect_);

			if (old_pen != NULL) {
				offdc.SelectObject(old_pen);
				DeleteObject(pen);
			}
			if (old_brush != NULL) {
				offdc.SelectObject(old_brush);
				DeleteObject(brush);
			}

		} else {

			if (allow_selection_ && gridctrlex_->GetSelectedIndex() == index_) {
				offdc.FillSolidRect(rect_, sel_back_color_);
			} else {
				offdc.FillSolidRect(rect_, back_color_);
			}
		}
	}

	if (disable_ || disable_row_) {
		offdc.SetTextColor(disable_text_color_);
	} else if (allow_selection_ && gridctrlex_->GetSelectedIndex() == index_) {
		offdc.SetTextColor(sel_text_color_);
	} else {
		offdc.SetTextColor(text_color_);
	}

	if (multiline_) {
		CString text = text_;
		Formation::PrepareMultilineString(offdc, text, text_rect);
		CRect calc_rect;
		offdc.DrawText(text, &calc_rect, DT_CALCRECT);
		if (calc_rect.Height() < rect_.Height()) {
			text_rect.MoveToY(rect_.top + (rect_.Height() - calc_rect.Height()) / 2);
		}
		if (calc_rect.Width() < rect_.Width()) {
			if (align_ == DT_CENTER) {
				text_rect.MoveToX(rect_.left + (rect_.Width() - calc_rect.Width()) / 2);
			}
		}
		offdc.DrawText(Formation::PrepareMultilineString(offdc, text_, text_rect), text_rect, align_ | DT_WORDBREAK);
	} else {
		CString text = value_changed_ ? (L" *" + text_ + L" ") : (L" " + text_ + L" ");
		offdc.DrawText(Formation::PrepareString(offdc, text, rect_), rect_, align_ | DT_SINGLELINE | DT_VCENTER);
	}

	if (show_balloon_ && !balloon_data_.IsEmpty()) {
		Gdiplus::Graphics graphics(offdc);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		offdc.DrawText(balloon_data_, balloon_rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		graphics.DrawEllipse(&Gdiplus::Pen(Gdiplus::Color(GetRValue(balloon_color_), GetGValue(balloon_color_), GetBValue(balloon_color_)), 1), balloon_rect.left, balloon_rect.top, balloon_rect.Width(), balloon_rect.Height());
	}
}

void GridCtrlEx::GridCellText::LButtonDown(UINT nFlags, CPoint point) {

	if (allow_selection_) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
		gridctrlex_->SetCapture();
	}
}

void GridCtrlEx::GridCellText::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellText::LButtonUp(UINT nFlags, CPoint point) {

	ReleaseCapture();

	if (allow_selection_) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SELCHANGE, (WPARAM)this, (LPARAM)(index_));
	}
}

void GridCtrlEx::GridCellText::Destroy() {

}

//GridCellEdit

void GridCtrlEx::GridCellEdit::PrepareCell() {

}

void GridCtrlEx::GridCellEdit::DrawCell(COfflineDC& offdc) {

	edit_rect_ = rect_;
	edit_rect_.DeflateRect(Formation::spacing(), Formation::spacing(), Formation::spacing(), Formation::spacing());
	
	if (disable_ || disable_row_) {

		HPEN pen = CreatePen(PS_SOLID, 1, disable_border_color_);
		HBRUSH brush = CreateSolidBrush(disable_back_color_);
		HPEN old_pen = (HPEN)offdc.SelectObject(pen);
		HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);
		offdc.SetTextColor(disable_text_color_);
		offdc.Rectangle(rect_);
		if (old_pen != NULL) {
			offdc.SelectObject(old_pen);
			DeleteObject(pen);
		}
		if (old_brush != NULL) {
			offdc.SelectObject(old_brush);
			DeleteObject(brush);
		}
		offdc.SelectObject(Formation::disablecolor_pen());

	} else {

		HPEN pen = NULL;
		HPEN old_pen = NULL;
		HBRUSH brush = NULL;
		HBRUSH old_brush = NULL;
		if (gridctrlex_->GetSelectedIndex() == index_) {
			pen = CreatePen(PS_SOLID, 1, sel_border_color_);
			brush = CreateSolidBrush(sel_back_color_);
			offdc.SetTextColor(sel_text_color_);
		} else {
			pen = CreatePen(PS_SOLID, 1, border_color_);
			brush = CreateSolidBrush(back_color_);
			offdc.SetTextColor(text_color_);
		}
		old_brush = (HBRUSH)offdc.SelectObject(brush);
		old_pen = (HPEN)offdc.SelectObject(pen);

		offdc.Rectangle(rect_);

		if (old_pen != NULL) {
			offdc.SelectObject(old_pen);
			DeleteObject(pen);
		}
		if (old_brush != NULL) {
			offdc.SelectObject(old_brush);
			DeleteObject(brush);
		}
	}

	if (disable_ || disable_row_) {
		offdc.FillSolidRect(edit_rect_, DISABLE_COLOR);
		offdc.SetTextColor(WHITE_COLOR);
	} else {
		if (gridctrlex_->GetSelectedIndex() == index_) {
			offdc.FillSolidRect(edit_rect_, BACKCOLOR1_SEL);
		} else {
			offdc.FillSolidRect(edit_rect_, BACKCOLOR1);
		}
		if (read_only_) {
			offdc.SetTextColor(DISABLE_COLOR);
		} else {
			offdc.SetTextColor(WHITE_COLOR);
		}
	}
	offdc.DrawText(Formation::PrepareString(offdc, L" " + text_ + L" ", edit_rect_), edit_rect_, DT_SINGLELINE | align_ | DT_VCENTER);
}

void GridCtrlEx::GridCellEdit::LButtonDown(UINT nFlags, CPoint point) {

	if (edit_rect_.PtInRect(point) && !read_only_) {
		down_point_ = point;
		gridctrlex_->InvalidateRect(edit_rect_, FALSE);
		gridctrlex_->SetCapture();
	}
}

void GridCtrlEx::GridCellEdit::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellEdit::LButtonUp(UINT nFlags, CPoint point) {

	ReleaseCapture();

	if (edit_rect_.PtInRect(down_point_) && !read_only_) {

		KeyboardDlg keyboard;
	
		KeyboardDlg::KEYBOARD_TYPE keyboard_type;
		if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC) {
			keyboard_type = KeyboardDlg::KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC;
		} else if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_NUMERIC) {
			keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_NUMERIC;
		} else if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
			keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NUMERIC;
		} else if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
			keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC;
		} else if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC) {
			keyboard_type = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC;
		}
		keyboard.SetKeyboardType(keyboard_type);
		if (keyboard_type_ == KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC) {
			if (text_limit_ == -1) {
				text_limit_ = FULL_KEY_LENGTH;
			}
		} else {
			if (text_limit_ == -1) {
				text_limit_ = NUM_KEY_LENGTH;
			}
		}
		keyboard.SetLimitText(text_limit_);
		keyboard.SetHolder(holder_);
		if (keyboard.OpenKeyboard(keyboard_captionbar_text_, text_, FALSE, FALSE, multilingual_) == IDOK) {

			text_ = keyboard.GetEditText();
			gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_EDIT_ENTERPRESSED, (WPARAM)this);
			gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
		}
		//gridctrlex_->InvalidateRect(edit_rect_, FALSE);
	}
	down_point_.SetPoint(-1, -1);
}

void GridCtrlEx::GridCellEdit::Destroy() {

}

CString GridCtrlEx::GridCellEdit::GetText() {

	return text_;
}

int GridCtrlEx::GridCellEdit::GetValue() {

	return _wtoi(text_);
}

CString GridCtrlEx::GridCellEdit::GetPrevText() {

	return prev_text_;
}

int GridCtrlEx::GridCellEdit::GetPrevValue() {

	return _wtoi(prev_text_);
}

void GridCtrlEx::GridCellEdit::SetText(CString text, BOOL highlight) {
	
	text_ = text;

	if(use_floating_) {
		int find = text_.Find(L".");
		if (find != -1) {
			find += 1;
			BOOL double_precision = (text_.GetLength() - find) == 2 ? TRUE : FALSE;
			if (double_precision) {
				CString text = text_.Mid(find, 2); //If it is 15.00 then should show 15 only
				if (text == L"00") {
					text_ = text_.Mid(0, text_.GetLength() - 3);
				}
			} else {
				CString text = text_.Mid(find, 1); //If it is 15.0 then should show 15 only
				if (text == L"0") {
					text_ = text_.Mid(0, text_.GetLength() - 2);
				}
			}
		}
	}

	if (!highlight) {
		prev_text_ = text;
	}

	gridctrlex_->InvalidateRect(edit_rect_, FALSE);
	gridctrlex_->GridCellUpdated();
}

void GridCtrlEx::GridCellEdit::SetValue(int value, BOOL highlight) {

	CString text;
	text.Format(L"%d", value);

	SetText(text, highlight);
}

void GridCtrlEx::GridCellEdit::SetKeyBoardType(KeyboardDlg::KEYBOARD_TYPE keyboard_type) {

	keyboard_type_ = keyboard_type;
	if (keyboard_type_ != keyboard_type) {
		keyboard_type_ = keyboard_type;
		if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_NUMERIC) {
			holder_ = L".-";
		} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_FLOATING_NUMERIC) {
			holder_ = L"-";
		} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC) {
			holder_ = L".";
		} else if (keyboard_type_ == Edit::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC) {
			holder_ = L"";
		}
	}
}

void GridCtrlEx::GridCellEdit::SetKeyBoardCaptionBarText(CString keyboard_captionbar_text) {

	keyboard_captionbar_text_ = keyboard_captionbar_text;
}

void GridCtrlEx::GridCellEdit::ShowMultilingualKeyboard(BOOL show) {

	multilingual_ = show;
}

void GridCtrlEx::GridCellEdit::SetTextLimit(int limit) {

	text_limit_ = limit;
}

void GridCtrlEx::GridCellEdit::SetHolder(CString holder) {

	holder_ = holder;
}

void GridCtrlEx::GridCellEdit::SetReadOnly(BOOL readonly) {

	read_only_ = readonly;
	gridctrlex_->InvalidateRect(edit_rect_, FALSE);
}

void GridCtrlEx::GridCellEdit::UseFloating(BOOL floating) {

	use_floating_ = floating;
}

// GridCellSpin

void GridCtrlEx::GridCellSpin::PrepareCell() {

}

void GridCtrlEx::GridCellSpin::DrawCell(COfflineDC& offdc) {

	int edit_width = rect_.Width() * 25 / 100;
	int spin_width = (rect_.Width() - edit_width) / 2;
	spin_rect_[0].SetRect(rect_.left, rect_.top, rect_.left + spin_width, rect_.bottom);
	edit_rect_.SetRect(spin_rect_[0].right, rect_.top, spin_rect_[0].right + edit_width, rect_.bottom);
	spin_rect_[1].SetRect(edit_rect_.right, rect_.top, edit_rect_.right + spin_width, rect_.bottom);

	left_text_rect_ = spin_rect_[0];
	left_text_rect_.DeflateRect(0, 0, spin_rect_[0].Height() * 30 / 100, 0);
	right_text_rect_ = spin_rect_[1];
	right_text_rect_.DeflateRect(spin_rect_[1].Height() * 30 / 100, 0, 0, 0);

	Gdiplus::Point point_for_spin_btn[6];
	point_for_spin_btn[0].X = point_for_spin_btn[2].X = spin_rect_[0].left + spin_rect_[0].Height() * 30 / 100;
	point_for_spin_btn[1].X = spin_rect_[0].left;
	point_for_spin_btn[3].X = point_for_spin_btn[5].X = spin_rect_[1].right - spin_rect_[1].Height() * 30 / 100;
	point_for_spin_btn[4].X = spin_rect_[1].right - 1;
	point_for_spin_btn[0].Y = point_for_spin_btn[5].Y = spin_rect_[0].bottom - 1;
	point_for_spin_btn[2].Y = point_for_spin_btn[3].Y = spin_rect_[0].top;
	point_for_spin_btn[1].Y = point_for_spin_btn[4].Y = rect_.top + spin_rect_[0].Height() / 2;

	Gdiplus::Point point_for_arrow[2][3];
	point_for_arrow[0][0].X = point_for_arrow[0][2].X = spin_rect_[0].right - 1 - spin_rect_[0].Height() * 20 / 100;
	point_for_arrow[0][1].X = spin_rect_[0].right - 1 - spin_rect_[0].Height() * 45 / 100;
	point_for_arrow[0][0].Y = spin_rect_[0].top + spin_rect_[0].Height() * 20 / 100;
	point_for_arrow[0][1].Y = spin_rect_[0].top + spin_rect_[0].Height() * 50 / 100;
	point_for_arrow[0][2].Y = spin_rect_[0].top + spin_rect_[0].Height() * 80 / 100;
	point_for_arrow[1][0].X = point_for_arrow[1][2].X = spin_rect_[1].left + spin_rect_[0].Height() * 20 / 100;
	point_for_arrow[1][1].X = spin_rect_[1].left + spin_rect_[1].Height() * 45 / 100;
	point_for_arrow[1][0].Y = spin_rect_[1].top + spin_rect_[1].Height() * 20 / 100;
	point_for_arrow[1][1].Y = spin_rect_[1].top + spin_rect_[1].Height() * 50 / 100;
	point_for_arrow[1][2].Y = spin_rect_[1].top + spin_rect_[1].Height() * 80 / 100;

	//Edit box
	if (disable_ || disable_row_) {
		offdc.FillSolidRect(edit_rect_, DISABLE_COLOR);
		offdc.SetTextColor(WHITE_COLOR);
	} else {
		if (read_only_) {
			offdc.FillSolidRect(edit_rect_, LABEL_COLOR3);
		} else {
			if (gridctrlex_->GetSelectedIndex() == index_ && edit_rect_.PtInRect(down_point_)) {
				offdc.FillSolidRect(edit_rect_, BACKCOLOR1_SEL);
			} else {
				offdc.FillSolidRect(edit_rect_, BACKCOLOR1);
			}
		}
		offdc.SetTextColor(text_color_);
	}
	//Edit box text
	offdc.DrawText(Formation::PrepareString(offdc, text_, edit_rect_), edit_rect_, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	//Draw left and right text
	offdc.SetTextColor(!(disable_ || disable_row_) && !read_only_ ? BLACK_COLOR : DISABLE_COLOR);
	//left_text_.Format(L"%d", min_pos_);
	offdc.DrawText(left_text_, left_text_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//right_text_.Format(L"%d", max_pos_);
	offdc.DrawText(right_text_, right_text_rect_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	//Draw outer polygon <=>
	Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	if (!(disable_ || disable_row_)) {
		graphics.DrawPolygon(Formation::spancolor1_pen_gdi(), point_for_spin_btn, 6);
	} else {
		graphics.DrawPolygon(Formation::disablecolor_pen_gdi(), point_for_spin_btn, 6);
	}
	
	//Draw left and right arrow near to Edit box
	for (int i = 0; i < 2; i++)	{
		if (!(disable_ || disable_row_)) {
			if (!read_only_) {
				bool update = true;
				if (use_floating_) {
					if ((i == 0 && f_cur_pos_ == f_min_pos_) || (i == 1 && f_cur_pos_ == f_max_pos_)) {
						graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow[i], 3);
						update = false;
					}
				} else {
					if ((i == 0 && cur_pos_ == min_pos_) || (i == 1 && cur_pos_ == max_pos_)) {
						graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow[i], 3);
						update = false;
					}
				}
				if (update) {
					if (button_down_[i]) {
						graphics.FillPolygon(Formation::spancolor1_brush_for_selection_gdi(), point_for_arrow[i], 3);
					} else {
						graphics.FillPolygon(Formation::spancolor1_brush_gdi(), point_for_arrow[i], 3);
					}
				}
			} else {
				graphics.FillPolygon(Formation::labelcolor3_brush_gdi(), point_for_arrow[i], 3);
			}
		} else {
			graphics.FillPolygon(Formation::disablecolor_brush_gdi(), point_for_arrow[i], 3);
		}
	}
}

void GridCtrlEx::GridCellSpin::EditEnterPressedMessage() {

	CString edit_text = text_;
	timer_executed_ = 1;
	if (use_floating_) {
		SetPositionF(_wtof(edit_text));
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPosF());
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	} else {
		SetPosition(_wtoi(edit_text));
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPos());
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	}
	timer_executed_ = 0;

	gridctrlex_->InvalidateRect(spin_rect_[0], FALSE); //To update arrow color
	gridctrlex_->InvalidateRect(spin_rect_[1], FALSE); //To update arrow color
}

void GridCtrlEx::GridCellSpin::LButtonDown(UINT nFlags, CPoint point) {

	spin_index_ = -1;

	if (spin_rect_[0].PtInRect(point) && !read_only_) {
		if (use_floating_) {
			if (f_cur_pos_ == f_min_pos_) {
				return;
			}
		} else {
			if (cur_pos_ == min_pos_) {
				return;
			}
		}
		spin_index_ = 0;
	} else if (spin_rect_[1].PtInRect(point) && !read_only_) {
		if (use_floating_) {
			if (f_cur_pos_ == f_max_pos_) {
				return;
			}
		} else {
			if (cur_pos_ == max_pos_) {
				return;
			}
		}
		spin_index_ = 1;
	} else {
		if (edit_rect_.PtInRect(point) && !read_only_) {
			down_point_ = point;
			gridctrlex_->InvalidateRect(edit_rect_, FALSE);
			gridctrlex_->SetCapture();
		}
		return;
	}

	button_down_[spin_index_] = TRUE;
	//TO UPDATE ARROW COLORS
	gridctrlex_->InvalidateRect(spin_rect_[spin_index_], FALSE);

	timer_executed_ = 0;
	gridctrlex_->SetTimer(spin_index_, 100, NULL);
	gridctrlex_->SetCapture();
}

void GridCtrlEx::GridCellSpin::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellSpin::LButtonUp(UINT nFlags, CPoint point) {
	
	ReleaseCapture();

	if (spin_index_ == 0 || spin_index_ == 1) {

		button_down_[spin_index_] = FALSE;
		//TO UPDATE ARROW COLORS
		//gridctrlex_->InvalidateRect(spin_rect_[spin_index_], FALSE);

	} else {

		if (edit_rect_.PtInRect(down_point_) && !read_only_) {

			KeyboardDlg keyboard;
			keyboard.SetKeyboardType(keyboard_type_);
			if (keyboard.OpenKeyboard(keyboard_captionbar_text_, text_, FALSE, FALSE, FALSE) == IDOK) {

				text_ = keyboard.GetEditText();

				EditEnterPressedMessage();

				gridctrlex_->GetParent()->SendMessageW(WM_EDIT_ENTERPRESSED, (WPARAM)this);
				gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
			}
			//gridctrlex_->InvalidateRect(edit_rect_, FALSE);
		}
	}

	down_point_.SetPoint(-1, -1);
}

void GridCtrlEx::GridCellSpin::OnTimer(UINT_PTR nIDEvent) {

	if (button_down_[spin_index_] || timer_executed_ == 0) {
		++timer_executed_;

		if (use_floating_) {
			SetPositionF(f_cur_pos_ - f_steps_[spin_index_]);
			CString str_prev, str_cur;
			double_precesion_ ? str_prev.Format(L"%.2f", f_prev_pos_) : str_prev.Format(L"%.1f", f_prev_pos_);
			double_precesion_ ? str_cur.Format(L"%.2f", f_cur_pos_) : str_cur.Format(L"%.1f", f_cur_pos_);
			gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPosF());
			gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
		} else {
			SetPosition(cur_pos_ - steps_[spin_index_]);
			gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_SPINCTRL_SELCHANGE, (WPARAM)this, (LPARAM)GetPos());
			gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
		}
		
		gridctrlex_->InvalidateRect(spin_rect_[0], FALSE); //To update arrow color
		gridctrlex_->InvalidateRect(spin_rect_[1], FALSE); //To update arrow color
	}

	if (!button_down_[spin_index_]) {
		gridctrlex_->KillTimer(spin_index_);
	}
}

void GridCtrlEx::GridCellSpin::Destroy() {

}

void GridCtrlEx::GridCellSpin::SetRange(int min, int max, int freq) {

	use_floating_ = FALSE;
	keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_NUMERIC;
	if (min < 0)		keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_NEGATIVE_NUMERIC;

	min_pos_ = min;
	max_pos_ = max;
	if (min > max) {
		max_pos_ = min;
		min_pos_ = max;
	}
	steps_[0] = freq;
	steps_[1] = -freq;

	left_text_.Format(L"%d", min_pos_);
	right_text_.Format(L"%d", max_pos_);

	gridctrlex_->InvalidateRect(left_text_rect_, FALSE);
	gridctrlex_->InvalidateRect(right_text_rect_, FALSE);
}

void GridCtrlEx::GridCellSpin::SetRangeF(double min, double max, double freq, BOOL double_precesion) {

	use_floating_ = TRUE;
	keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NUMERIC;
	if (min < 0)		keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_FLOATING_NEGATIVE_NUMERIC;

	double_precesion_ = double_precesion;

	f_min_pos_ = min;
	f_max_pos_ = max;
	if (min > max) {
		f_max_pos_ = min;
		f_min_pos_ = max;
	}
	f_steps_[0] = freq;
	f_steps_[1] = -freq;

	if (f_min_pos_ == 0) {
		left_text_ = L"0";
	} else {
		if (double_precesion_) {
			left_text_.Format(L"%.2f", f_min_pos_);
			CString text = left_text_.Mid(left_text_.GetLength() - 2, 2); //If it is 15.00 then should show 15 only
			if (text == L"00") {
				left_text_ = left_text_.Mid(0, left_text_.GetLength() - 3);
			}
		} else {
			left_text_.Format(L"%.1f", f_min_pos_);
			CString text = left_text_.Mid(left_text_.GetLength() - 1, 1); //If it is 15.0 then should show 15 only
			if (text == L"0") {
				left_text_ = left_text_.Mid(0, left_text_.GetLength() - 2);
			}
		}
	}

	int temp_max = (int)f_max_pos_;
	if (((f_max_pos_ * 100) / temp_max) == 100) {
		right_text_.Format(L"%.0f", f_max_pos_);
	} else {
		if (double_precesion_) {
			right_text_.Format(L"%.2f", f_max_pos_);
			CString text = right_text_.Mid(right_text_.GetLength() - 2, 2); //If it is 15.00 then should show 15 only
			if (text == L"00") {
				right_text_ = right_text_.Mid(0, right_text_.GetLength() - 3);
			}
		} else {
			right_text_.Format(L"%.1f", f_max_pos_);
			CString text = right_text_.Mid(right_text_.GetLength() - 1, 1); //If it is 15.0 then should show 15 only
			if (text == L"0") {
				right_text_ = right_text_.Mid(0, right_text_.GetLength() - 2);
			}
		}
	}

	gridctrlex_->InvalidateRect(left_text_rect_, FALSE);
	gridctrlex_->InvalidateRect(right_text_rect_, FALSE);
}

int GridCtrlEx::GridCellSpin::GetPos() {

	return cur_pos_;
}

int GridCtrlEx::GridCellSpin::GetPrevPos() {

	return prev_pos_;
}

double GridCtrlEx::GridCellSpin::GetPosF() {

	return f_cur_pos_;
}

double GridCtrlEx::GridCellSpin::GetPrevPosF() {

	return f_prev_pos_;
}

void GridCtrlEx::GridCellSpin::SetPosition(int pos) {

	if (pos < min_pos_) {
		pos = min_pos_;
	}
	if (pos > max_pos_) {
		pos = max_pos_;
	}

	cur_pos_ = pos;
	text_.Format(L"%d", cur_pos_);

	gridctrlex_->InvalidateRect(edit_rect_, FALSE);
	gridctrlex_->InvalidateRect(spin_rect_[0], FALSE);
	gridctrlex_->InvalidateRect(spin_rect_[1], FALSE);
}

void GridCtrlEx::GridCellSpin::SetPos(int pos, BOOL highlight) {

	SetPosition(pos);

	if (!highlight) {
		prev_pos_ = cur_pos_;
	}

	gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
}

void GridCtrlEx::GridCellSpin::SetPositionF(double pos) {

	if (pos < f_min_pos_) {
		pos = f_min_pos_;
	}
	if (pos > f_max_pos_) {
		pos = f_max_pos_;
	}
	
	f_cur_pos_ = roundf(float(pos) * 100) / 100.0; //To set edit value unchanged when match with original value. It does not change if value is changed by keyboard bcoz of precision digits
	if (f_cur_pos_ == 0) {
		text_.Format(L"%d", f_cur_pos_);
	} else {
		if (double_precesion_) {
			text_.Format(L"%.2f", f_cur_pos_);
			CString text = text_.Mid(text_.GetLength() - 2, 2); //If it is 15.00 then should show 15 only
			if (text == L"00") {
				text_ = text_.Mid(0, text_.GetLength() - 3);
			}
		} else {
			text_.Format(L"%.1f", f_cur_pos_);
			CString text = text_.Mid(text_.GetLength() - 1, 1); //If it is 15.0 then should show 15 only
			if (text == L"0") {
				text_ = text_.Mid(0, text_.GetLength() - 2);
			}
		}
	}
	gridctrlex_->InvalidateRect(edit_rect_, FALSE);
	gridctrlex_->InvalidateRect(spin_rect_[0], FALSE);
	gridctrlex_->InvalidateRect(spin_rect_[1], FALSE);
}

void GridCtrlEx::GridCellSpin::SetPosF(double pos, BOOL highlight) {

	SetPositionF(pos);

	if (!highlight) {
		f_prev_pos_ = f_cur_pos_;
	}

	gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
}

void GridCtrlEx::GridCellSpin::SetKeyBoardCaptionBarText(CString keyboard_captionbar_text) {

	keyboard_captionbar_text_ = keyboard_captionbar_text;
}

void GridCtrlEx::GridCellSpin::SetReadOnly(BOOL readonly) {

	read_only_ = readonly;
	gridctrlex_->InvalidateRect(edit_rect_, FALSE);
}

//GridCellOnOff

void GridCtrlEx::GridCellOnOff::PrepareCell() {

}

void GridCtrlEx::GridCellOnOff::DrawCell(COfflineDC& offdc) {

	offdc.FillSolidRect(rect_, (disable_ || disable_row_) ? DISABLE_COLOR : BACKCOLOR1);

	offdc.SelectObject(GetStockObject(NULL_BRUSH));
	offdc.SelectObject(Formation::whitecolor_pen());

	CRect rect(rect_.left + Formation::spacing(), rect_.top + Formation::spacing(), rect_.right - Formation::spacing(), rect_.bottom - Formation::spacing());
	rect1_.SetRect(rect.left, rect.top, rect.left + rect.Width() / 2, rect.bottom);
	rect2_.SetRect(rect1_.right, rect1_.top, rect1_.right + rect1_.Width(), rect1_.bottom);

	if (on_text_.IsEmpty() && off_text_.IsEmpty()) {
		on_text_ = Language::GetString(IDSTRINGT_ON);
		off_text_ = Language::GetString(IDSTRINGT_OFF);
	}

	offdc.SetTextColor(text_color_);
	if (gridctrlex_->GetSelectedIndex() == index_ && rect1_.PtInRect(down_point_) && !checked_) {
		offdc.SelectObject(Formation::spancolor1_pen_for_selection());
	} else {
		checked_ ? offdc.SelectObject(Formation::whitecolor_pen()) : offdc.SelectObject(GetStockObject(NULL_PEN));
	}
	offdc.Rectangle(rect1_);
	offdc.DrawText(Formation::PrepareString(offdc, on_text_, rect1_), rect1_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (gridctrlex_->GetSelectedIndex() == index_ && rect2_.PtInRect(down_point_) && checked_) {
		offdc.SelectObject(Formation::spancolor1_pen_for_selection());
	} else {
		!checked_ ? offdc.SelectObject(Formation::whitecolor_pen()) : offdc.SelectObject(GetStockObject(NULL_PEN));
	}	
	offdc.Rectangle(rect2_);
	offdc.DrawText(Formation::PrepareString(offdc, off_text_, rect2_), rect2_, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void GridCtrlEx::GridCellOnOff::LButtonDown(UINT nFlags, CPoint point) {

	down_point_ = point;
	gridctrlex_->InvalidateRect(rect_, FALSE);
	gridctrlex_->SetCapture();
}

void GridCtrlEx::GridCellOnOff::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellOnOff::LButtonUp(UINT nFlags, CPoint point) {

	ReleaseCapture();

	if (rect1_.PtInRect(down_point_) && !checked_) {
		checked_ = TRUE;
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	}
	if (rect2_.PtInRect(down_point_) && checked_) {
		checked_ = FALSE;
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	}
	//gridctrlex_->InvalidateRect(rect_, FALSE);
	down_point_.SetPoint(-1, -1);
}

void GridCtrlEx::GridCellOnOff::Destroy() {

}

void GridCtrlEx::GridCellOnOff::SetText(CString on_text, CString off_text) {

	if (!on_text.IsEmpty()) {
		on_text_ = on_text;
	}
	if (!off_text.IsEmpty()) {
		off_text_ = off_text;
	}

	gridctrlex_->InvalidateRect(rect1_, FALSE);
	gridctrlex_->InvalidateRect(rect2_, FALSE);
}

BOOL GridCtrlEx::GridCellOnOff::GetCheck() {

	return checked_;
}

BOOL GridCtrlEx::GridCellOnOff::GetPrevCheck() {

	return prev_checked_;
}

void GridCtrlEx::GridCellOnOff::SetCheck(BOOL check, BOOL update, BOOL highlight) {

	checked_ = check;

	if (!highlight) {
		prev_checked_ = check;
	}

	if (update) {
		gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_ONOFFCTRL_CHECKONOFF, (WPARAM)this, (LPARAM)(checked_));
	}
	
	gridctrlex_->InvalidateRect(rect1_, FALSE);
	gridctrlex_->InvalidateRect(rect2_, FALSE);
	gridctrlex_->GridCellUpdated();
}

//GridCellButton

void GridCtrlEx::GridCellButton::SetBackColor(COLORREF back_color, BOOL update) {

	back_color_ = back_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetBorderColor(COLORREF border_color, BOOL update) {

	border_color_ = border_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetTextColor(COLORREF text_color, BOOL update) {

	text_color_ = text_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetSelBackColor(COLORREF sel_back_color, BOOL update) {

	sel_back_color_ = sel_back_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetSelBorderColor(COLORREF sel_border_color, BOOL update) {

	sel_border_color_ = sel_border_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetSelTextColor(COLORREF sel_text_color, BOOL update) {

	sel_text_color_ = sel_text_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetDisableBackColor(COLORREF disable_back_color, BOOL update) {

	disable_back_color_ = disable_back_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetDisableBorderColor(COLORREF disable_border_color, BOOL update) {

	disable_border_color_ = disable_border_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetDisableTextColor(COLORREF disable_text_color, BOOL update) {

	disable_text_color_ = disable_text_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetBoxColor(COLORREF box_color, BOOL update) {

	box_color_ = box_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetDisableBoxColor(COLORREF disable_box_color, BOOL update) {

	disable_box_color_ = disable_box_color;
	if (update) {
		gridctrlex_->InvalidateRect(rect_, FALSE);
	}
}

void GridCtrlEx::GridCellButton::SetText(CString text) { 
	
	on_text_ = off_text_ = text; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellButton::SetText(CString on_text, CString off_text) { 
	
	on_text_ = on_text; off_text_ = off_text; 
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellButton::SetIcon(HICON icon, ICON_ALIGNMENT alignment) {

	SetIcon(icon, NULL, alignment);
}

void GridCtrlEx::GridCellButton::SetIcon(HICON on_icon, HICON off_icon, ICON_ALIGNMENT alignment) {

	icon_alignment_ = alignment;

	ICONINFO ii;
	if (on_icon) {
		icon_info_[0].hIcon = on_icon;
		::ZeroMemory(&ii, sizeof(ICONINFO));
		::GetIconInfo(on_icon, &ii);
		icon_info_[0].dwWidth = (DWORD)(ii.xHotspot * 2);
		icon_info_[0].dwHeight = (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);
	}
	if (off_icon) {
		icon_info_[1].hIcon = off_icon;
		::ZeroMemory(&ii, sizeof(ICONINFO));
		::GetIconInfo(off_icon, &ii);
		icon_info_[1].dwWidth = (DWORD)(ii.xHotspot * 2);
		icon_info_[1].dwHeight = (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellButton::SetCheck(BOOL check, BOOL highlight) {

	button_pressed_ = check;

	if (!highlight) {
		prev_button_pressed_ = check;
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
	gridctrlex_->GridCellUpdated();
}

void GridCtrlEx::GridCellButton::DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled) {

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
				rpCaption->left = ImgRect.left + Width + ImgRect.left;
			}
			// Center image vertically
			ImgRect.top += ((ImgRect.Height() - Height) / 2);
			break;
		}
		case ST_ALIGN_HORIZ_RIGHT: {
			CRect rBtn = rect_;
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
				pDC->DrawText(on_text_, &calc_rect, DT_CALCRECT);
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

void GridCtrlEx::GridCellButton::PrepareCell() {

}

void GridCtrlEx::GridCellButton::DrawCell(COfflineDC& offdc) {

	HPEN old_pen = NULL;
	HBRUSH old_brush = NULL;
	HPEN pen = CreatePen(PS_SOLID, 1, border_color_);
	HBRUSH brush = CreateSolidBrush(back_color_);
	HPEN sel_pen = CreatePen(PS_SOLID, 1, sel_border_color_);
	HBRUSH sel_brush = CreateSolidBrush(sel_back_color_);
	HPEN disable_pen = CreatePen(PS_SOLID, 1, disable_border_color_);
	HBRUSH disable_brush = CreateSolidBrush(disable_back_color_);

	CRect text_rect(rect_);
	if (disable_ || disable_row_) {
		
		old_pen = (HPEN)offdc.SelectObject(disable_pen);
		if (back_color_ == WHITE_COLOR) {
			GetStockObject(NULL_BRUSH);
		} else {
			old_brush = (HBRUSH)offdc.SelectObject(disable_brush);
		}
		offdc.Rectangle(rect_);

	} else {

		if (((cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_CHECK_BOX_PUSH_LIKE || cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_RADIO_BUTTON_PUSH_LIKE) && button_pressed_) || (gridctrlex_->GetSelectedIndex() == index_)) {
			old_pen = (HPEN)offdc.SelectObject(sel_pen);
			old_brush = (HBRUSH)offdc.SelectObject(sel_brush);
		} else {
			old_pen = (HPEN)offdc.SelectObject(pen);
			old_brush = (HBRUSH)offdc.SelectObject(brush);
		}
		offdc.Rectangle(rect_);
	}

	DWORD text_alignment = align_ | DT_SINGLELINE | DT_VCENTER;
	if (cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
		if (icon_info_[0].hIcon != NULL) {
			CRect caption_rect(rect_);
			DrawTheIcon(&offdc, !on_text_.IsEmpty(), rect_, &caption_rect, FALSE, FALSE);// disable_);
			if (icon_alignment_ == ST_ALIGN_VERT) {
				text_alignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
				text_rect.top = caption_rect.top;
			} else if (icon_alignment_ == ST_ALIGN_CENTER) {
				text_alignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
				text_rect.top = caption_rect.top;
			} else if (icon_alignment_ == ST_ALIGN_HORIZ) {
				text_alignment = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
				text_rect.left = caption_rect.left - Formation::spacing();
			} else if (icon_alignment_ == ST_ALIGN_HORIZ_RIGHT) {
				text_alignment = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
			}
		}
	} else {
		if (cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_CHECK_BOX_PUSH_LIKE || cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_RADIO_BUTTON_PUSH_LIKE) {
			if (icon_info_[0].hIcon != NULL && icon_info_[1].hIcon != NULL) {
				CRect caption_rect(rect_);
				DrawTheIcon(&offdc, !on_text_.IsEmpty(), rect_, &caption_rect, button_pressed_, (disable_ || disable_row_));
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
			} else {
				text_rect.left += Formation::spacing();
			}
		} else {
			int offset = rect_.Height() / 4;
			CRect on_rect(rect_.left + offset, rect_.top + offset, rect_.left + offset * 3, rect_.top + offset * 3);
			if (align_ == DT_CENTER) {
				on_rect.SetRect(rect_.CenterPoint().x - offset, rect_.CenterPoint().y - offset, rect_.CenterPoint().x + offset, rect_.CenterPoint().y + offset);
			}
			CRect off_rect = on_rect;
			off_rect .DeflateRect(offset / 2, offset / 2, offset / 2, offset / 2);
			Gdiplus::Graphics graphics(offdc);
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			if (cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_CHECK_BOX) {
				if (disable_ || disable_row_) {
					graphics.DrawRectangle((back_color_ == WHITE_COLOR || disable_box_color_ != RGB(1, 1, 1)) ? Formation::disablecolor_pen_gdi() : Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else if (box_color_ != RGB(1, 1, 1)) {
					graphics.DrawRectangle(Formation::blackcolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else if (back_color_ == WHITE_COLOR) {
					graphics.DrawRectangle(Formation::spancolor1_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else {
					graphics.DrawRectangle(Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				}
				//graphics.DrawRectangle((disable_ || disable_row_) ? ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::disablecolor_pen_gdi() : Formation::whitecolor_pen_gdi()) : ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::spancolor1_pen_gdi() : Formation::whitecolor_pen_gdi()), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				if (button_pressed_) {
					if (disable_ || disable_row_) {
						graphics.FillRectangle((back_color_ == WHITE_COLOR || disable_box_color_ != RGB(1, 1, 1)) ? Formation::disablecolor_brush_gdi() : Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else if (box_color_ != RGB(1, 1, 1)) {
						graphics.FillRectangle(Formation::blackcolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else if (back_color_ == WHITE_COLOR) {
						graphics.FillRectangle(Formation::spancolor1_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else {
						graphics.FillRectangle(Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					}
					//graphics.FillRectangle((disable_ || disable_row_) ? ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::disablecolor_brush_gdi() : Formation::whitecolor_brush_gdi()) : ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::spancolor1_brush_gdi() : Formation::whitecolor_brush_gdi()), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
				}
			} else if (cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_RADIO_BUTTON) {
				if (disable_ || disable_row_) {
					graphics.DrawEllipse((back_color_ == WHITE_COLOR || disable_box_color_ != RGB(1, 1, 1)) ? Formation::disablecolor_pen_gdi() : Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else if (box_color_ != RGB(1, 1, 1)) {
					graphics.DrawEllipse(Formation::blackcolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else if (back_color_ == WHITE_COLOR) {
					graphics.DrawEllipse(Formation::spancolor1_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				} else {
					graphics.DrawEllipse(Formation::whitecolor_pen_gdi(), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				}
				//graphics.DrawEllipse((disable_ || disable_row_) ? ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::disablecolor_pen_gdi() : Formation::whitecolor_pen_gdi()) : ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::spancolor1_pen_gdi() : Formation::whitecolor_pen_gdi()), on_rect.left, on_rect.top, on_rect.Width(), on_rect.Height());
				if (button_pressed_) {
					if (disable_ || disable_row_) {
						graphics.FillEllipse((back_color_ == WHITE_COLOR || disable_box_color_ != RGB(1, 1, 1)) ? Formation::disablecolor_brush_gdi() : Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else if (box_color_ != RGB(1, 1, 1)) {
						graphics.FillEllipse(Formation::blackcolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else if (back_color_ == WHITE_COLOR) {
						graphics.FillEllipse(Formation::spancolor1_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					} else {
						graphics.FillEllipse(Formation::whitecolor_brush_gdi(), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
					}
					//graphics.FillEllipse((disable_ || disable_row_) ? ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::disablecolor_brush_gdi() : Formation::whitecolor_brush_gdi()) : ((back_color_ == WHITE_COLOR || border_color_ == WHITE_COLOR) ? Formation::spancolor1_brush_gdi() : Formation::whitecolor_brush_gdi()), off_rect.left, off_rect.top, off_rect.Width(), off_rect.Height());
				}
			}
			text_rect.left = on_rect.right + offset;
		}
	}

	if (((cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_CHECK_BOX_PUSH_LIKE || cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_RADIO_BUTTON_PUSH_LIKE) && button_pressed_) || (gridctrlex_->GetSelectedIndex() == index_)) {
		offdc.SetTextColor((disable_ || disable_row_) ? disable_text_color_ : sel_text_color_);
	} else {
		offdc.SetTextColor((disable_ || disable_row_) ? disable_text_color_ : text_color_);
	}
	CString text = IsValueChanged() ? L"*" + (button_pressed_ ? on_text_ : off_text_) : (button_pressed_ ? on_text_ : off_text_);
	text_rect.DeflateRect((align_ == DT_LEFT) ? Formation::spacingHalf() : 0, 0, (align_ == DT_RIGHT) ? Formation::spacingHalf() : 0, 0);
	offdc.DrawText(Formation::PrepareString(offdc, text, text_rect), text_rect, text_alignment);

	if (old_pen != NULL) {
		offdc.SelectObject(old_pen);
		DeleteObject(sel_pen);
		DeleteObject(disable_pen);
		DeleteObject(pen);
	}
	if (old_brush != NULL) {
		offdc.SelectObject(old_brush);
		DeleteObject(sel_brush);
		DeleteObject(disable_brush);
		DeleteObject(brush);
	}
}

void GridCtrlEx::GridCellButton::LButtonDown(UINT nFlags, CPoint point) {

	//DisableCell(TRUE);

	if (cell_button_type_ == CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
		button_pressed_ = !button_pressed_;
	}
	gridctrlex_->InvalidateRect(rect_, FALSE);
	gridctrlex_->SetCapture();
}

void GridCtrlEx::GridCellButton::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellButton::LButtonUp(UINT nFlags, CPoint point) {

	ReleaseCapture();

	button_pressed_ = !button_pressed_;
	gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_BUTTON_CLICKED, (WPARAM)this, (LPARAM)button_pressed_);
	//gridctrlex_->InvalidateRect(rect_, FALSE);

	if (cell_button_type_ != GridCtrlEx::CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	}
}

void GridCtrlEx::GridCellButton::Destroy() {

	if (icon_info_[0].hIcon != NULL) {
		DestroyIcon(icon_info_[0].hIcon);
		icon_info_[0].hIcon = NULL;
	}
	if (icon_info_[1].hIcon != NULL) {
		DestroyIcon(icon_info_[1].hIcon);
		icon_info_[1].hIcon = NULL;
	}
}

//GridCellDropDown

void GridCtrlEx::GridCellDropDown::PrepareCell() {

	CRect parent_rect = rect_;

	//Calucate offset with parent window rect
	CRect wnd_rect1, wnd_rect2;
	gridctrlex_->GetParent()->GetWindowRect(&wnd_rect1);
	gridctrlex_->GetWindowRect(&wnd_rect2);
	CRect client_rect;
	gridctrlex_->GetParent()->GetClientRect(&client_rect);
	parent_rect.top = wnd_rect2.top - wnd_rect1.top + rect_.top - 2;
	parent_rect.bottom = parent_rect.top + rect_.Height();
	parent_rect.left = wnd_rect2.left - wnd_rect1.left + rect_.left - 2;
	parent_rect.right = parent_rect.left + rect_.Width();

	LONG style = GetWindowLong(gridctrlex_->GetParent()->GetSafeHwnd(), GWL_EXSTYLE);
	if ((style & WS_DLGFRAME) == WS_DLGFRAME) {
		CRect wnd_rect;
		gridctrlex_->GetWindowRect(&wnd_rect);
		//parent_rect = CRect(wnd_rect.left + rect_.left - GetSystemMetrics(SM_CXDLGFRAME), wnd_rect.top + rect_.top - GetSystemMetrics(SM_CYDLGFRAME), wnd_rect.left + rect_.right - GetSystemMetrics(SM_CXDLGFRAME), wnd_rect.top + rect_.bottom - GetSystemMetrics(SM_CYDLGFRAME));
	}

	ctrDDList_.Create(_T("GroupBox"), WS_CHILD | SS_NOTIFY, CRect(parent_rect.left, parent_rect.bottom, parent_rect.right, parent_rect.bottom), gridctrlex_->GetParent());
	ctrDDList_.SetFont(gridctrlex_->GetFont());
	ctrDDList_.Initialize(parent_rect);
}

void GridCtrlEx::GridCellDropDown::DrawCell(COfflineDC& offdc) {

	Gdiplus::Point points_for_open_arrow[3], points_for_close_arrow[3];

	points_for_open_arrow[0].X = rect_.right - rect_.Height() * 60 / 100;
	points_for_open_arrow[1].X = rect_.right - rect_.Height() * 40 / 100;
	points_for_open_arrow[2].X = rect_.right - rect_.Height() * 20 / 100;
	points_for_open_arrow[0].Y = points_for_open_arrow[2].Y = rect_.top + rect_.Height() * 35 / 100;
	points_for_open_arrow[1].Y = rect_.bottom - rect_.Height() * 35 / 100;
	points_for_close_arrow[0].X = points_for_close_arrow[2].X = rect_.right - rect_.Height() * 20 / 100;
	points_for_close_arrow[1].X = rect_.right - rect_.Height() * 50 / 100;
	points_for_close_arrow[0].Y = rect_.top + rect_.Height() * 30 / 100;
	points_for_close_arrow[1].Y = rect_.top + rect_.Height() * 50 / 100;
	points_for_close_arrow[2].Y = rect_.top + rect_.Height() * 70 / 100;

	if (ctrDDList_.GetSafeHwnd() != NULL) {

		if (disable_ || disable_row_) {

			if (back_color_ == WHITE_COLOR) {
				offdc.SelectObject(Formation::disablecolor_pen());
				offdc.SelectObject(GetStockObject(NULL_BRUSH));
				offdc.Rectangle(rect_);
			} else {
				offdc.FillSolidRect(rect_, DISABLE_COLOR);
			}

		} else {

			if (ctrDDList_.IsWindowVisible()) {

				offdc.FillSolidRect(rect_, BACKCOLOR1_SEL);

			} else {

				HPEN pen = NULL;
				HPEN old_pen = NULL;
				HBRUSH brush = NULL;
				HBRUSH old_brush = NULL;

				pen = CreatePen(PS_SOLID, 1, border_color_);
				brush = CreateSolidBrush(back_color_);
				old_brush = (HBRUSH)offdc.SelectObject(brush);
				old_pen = (HPEN)offdc.SelectObject(pen);
				offdc.Rectangle(rect_);

				if (old_pen != NULL) {
					offdc.SelectObject(old_pen);
					DeleteObject(pen);
				}
				if (old_brush != NULL) {
					offdc.SelectObject(old_brush);
					DeleteObject(brush);
				}
			}
		}
								
		if (ctrDDList_.GetCurSel() != -1 && ctrDDList_.GetCurSel() < ctrDDList_.get_items_list().GetSize()) {
			CRect rect(rect_);
			rect.DeflateRect(0, 0, (rect_.Height() * 75 / 100), 0); //REMOVE ARROW PART
			if (disable_ || disable_row_) {
				if (back_color_ == WHITE_COLOR) {
					offdc.SetTextColor(DISABLE_COLOR);
				} else {
					offdc.SetTextColor(WHITE_COLOR);
				}
			} else {
				if (ctrDDList_.IsWindowVisible()) {
					offdc.SetTextColor(WHITE_COLOR);
				} else {
					offdc.SetTextColor(text_color_);
				}
			}
			offdc.DrawText(Formation::PrepareString(offdc, L" " + ctrDDList_.get_items_list().GetAt(ctrDDList_.GetCurSel()), rect), rect, align_ | DT_SINGLELINE | DT_VCENTER);
		}

		Gdiplus::Graphics graphics(offdc);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		if (ctrDDList_.IsWindowVisible()) {
			graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_close_arrow, 3);
		} else {
			if (back_color_ == BACKCOLOR1) {
				graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_open_arrow, 3);
			} else {
				if (disable_ || disable_row_) {
					graphics.FillPolygon(Formation::disablecolor_brush_gdi(), points_for_open_arrow, 3);
				} else {
					graphics.FillPolygon(Formation::spancolor1_brush_gdi(), points_for_open_arrow, 3);
				}
			}
		}
	}
}

void GridCtrlEx::GridCellDropDown::LButtonDown(UINT nFlags, CPoint point) {

	if(gridctrlex_->gridcelldropdown_ != NULL) {
		if (list_window_rect_.PtInRect(point)) {
			point.Offset(0, -rect_.bottom);
			ctrDDList_.LButtonDown(nFlags, point);
		} else {
			gridctrlex_->InvalidateRect(rect_, FALSE);
			ctrDDList_.ShowWindow(SW_HIDE);
		}
	} else {
		if (ctrDDList_.GetSafeHwnd() != NULL) {
			ctrDDList_.ResetPosition(-gridctrlex_->GetScrollPos(SB_VERT));
			ctrDDList_.GetWindowRect(list_window_rect_);
			gridctrlex_->ScreenToClient(list_window_rect_);
			gridctrlex_->InvalidateRect(rect_, FALSE);
			ctrDDList_.ShowWindow(SW_SHOW);
			gridctrlex_->gridcelldropdown_ = this;
			gridctrlex_->SetCapture();
		}
	}
}

void GridCtrlEx::GridCellDropDown::MouseMove(UINT nFlags, CPoint point) {

	if (list_window_rect_.PtInRect(point)) {
		point.Offset(0, -rect_.bottom);
		if (ctrDDList_.GetSafeHwnd() != NULL) {
			ctrDDList_.MouseMove(nFlags, point);
		}
	}
}

void GridCtrlEx::GridCellDropDown::LButtonUp(UINT nFlags, CPoint point) {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		if (list_window_rect_.PtInRect(point)) {
			point.Offset(0, -rect_.bottom);
			ctrDDList_.LButtonUp(nFlags, point);
			gridctrlex_->InvalidateRect(rect_, FALSE);
			if (!ctrDDList_.IsWindowVisible()) {
				gridctrlex_->GetParent()->SendMessageW(WM_GRIDCTRLEX_DROPDOWNLIST_SELCHANGE, (WPARAM)this, (LPARAM)gridctrlex_->GetSelectedIndex());
				gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
			}
		}
		if (!ctrDDList_.IsWindowVisible()) {
			gridctrlex_->gridcelldropdown_ = NULL;
			ReleaseCapture();
		}
	}
}

void GridCtrlEx::GridCellDropDown::Destroy() {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		ctrDDList_.Destroy();
		ctrDDList_.DestroyWindow();
	}
}

void GridCtrlEx::GridCellDropDown::SetItemsToDisplay(int items_to_display) {

	items_to_display_ = items_to_display;
}

void GridCtrlEx::GridCellDropDown::SetText(int index, CString text) {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		if (index < GetItemCount() && index >= 0) {
			ctrDDList_.get_items_list().SetAt(index, text);
			ctrDDList_.Invalidate(FALSE);
			gridctrlex_->InvalidateRect(rect_, FALSE);
		}
	}
}

CString GridCtrlEx::GridCellDropDown::GetText(int index) {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		if (index < GetItemCount() && index >= 0) {
			return ctrDDList_.get_items_list().GetAt(index);
		}
	}
	return _T("");
}

void GridCtrlEx::GridCellDropDown::AddItem(CString str) {

	ctrDDList_.get_items_list().Add(str);

	items_to_display_ = (int)(ctrDDList_.get_items_list().GetSize() < 5) ? (int)ctrDDList_.get_items_list().GetSize() : 5;
	if (items_to_display_ <= 5) {
		if (ctrDDList_.GetSafeHwnd() != NULL) {
			ctrDDList_.Update(items_to_display_);
		}
	}
}

void GridCtrlEx::GridCellDropDown::DeleteItem(int index) {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		if (index < ctrDDList_.get_items_list().GetSize() && index >= 0) {
			ctrDDList_.get_items_list().RemoveAt(index);
		}
	}
}

void GridCtrlEx::GridCellDropDown::ResetContents() {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		ctrDDList_.get_items_list().RemoveAll();
	}
}

void GridCtrlEx::GridCellDropDown::SetCurSel(int index, BOOL highlight) {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		if (index >= 0 && index < ctrDDList_.get_items_list().GetSize()) {
			ctrDDList_.SetCurSel(index, highlight);
		}

		gridctrlex_->InvalidateRect(rect_, FALSE);
		gridctrlex_->GridCellUpdated();
	}
}

int GridCtrlEx::GridCellDropDown::GetCurSel() {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		return ctrDDList_.GetCurSel();
	}
	return -1;
}

int GridCtrlEx::GridCellDropDown::GetPrevSel() {

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		return ctrDDList_.GetPrevSel();
	}
	return -1;
}

CString GridCtrlEx::GridCellDropDown::GetCurSelText() { 

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		return ctrDDList_.GetCurSelText();
	}
	return _T("");
}

int GridCtrlEx::GridCellDropDown::FindString(CString str) { 

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		return ctrDDList_.FindString(str);
	}
	return -1;
}

int GridCtrlEx::GridCellDropDown::GetItemCount() { 

	if (ctrDDList_.GetSafeHwnd() != NULL) {
		return (int)ctrDDList_.get_items_list().GetCount();
	}
	return 0;
}

void GridCtrlEx::GridCellDropDown::SetBackColor(COLORREF back_color) {

	back_color_ = back_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellDropDown::SetBorderColor(COLORREF border_color) {

	border_color_ = border_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellDropDown::SetTextColor(COLORREF text_color) {

	text_color_ = text_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

//GridCellDropDownEx

void GridCtrlEx::GridCellDropDownEx::PrepareCell() {

	HINSTANCE old_resource_handle = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
	ddlistex_.Create(IDD_DDLISTEX_DLG);
	AfxSetResourceHandle(old_resource_handle);
}

void GridCtrlEx::GridCellDropDownEx::DrawCell(COfflineDC& offdc) {

	offdc.SelectObject(GetFont());
	
	if (!(disable_ || disable_row_)) {
		offdc.FillSolidRect(rect_, ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? BACKCOLOR1_SEL : BACKCOLOR1);
	} else {
		offdc.FillSolidRect(rect_, DISABLE_COLOR);
	}
	CRect dd_text_rect(rect_);
	dd_text_rect.right -= (Formation::control_height() - Formation::spacing3());
	CRect option_rect(rect_);
	option_rect.left = dd_text_rect.right;

	if (selected_index_ >= 0 && selected_index_ < int(items_list_.GetSize())) {
		offdc.SetTextColor(WHITE_COLOR);
		offdc.DrawText(Formation::PrepareString(offdc, L" " + items_list_[selected_index_], dd_text_rect), dd_text_rect, align_ | DT_SINGLELINE | DT_VCENTER);
	}

	/*Gdiplus::Graphics graphics(offdc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	if (down_point_.x == -1) {
		graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_open_arrow_, 3);
	} else {
		graphics.FillPolygon(Formation::whitecolor_brush_gdi(), points_for_close_arrow_, 3);
	}*/
	CPoint point1, point2;
	point1.x = option_rect.left + Formation::spacing2();
	point2.x = option_rect.right - Formation::spacing2();
	point1.y = option_rect.CenterPoint().y - Formation::spacing() - 1;
	point2.y = option_rect.CenterPoint().y - Formation::spacing();
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
	point1.y = option_rect.CenterPoint().y - 1;
	point2.y = option_rect.CenterPoint().y + 1;
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
	point1.y = option_rect.CenterPoint().y + Formation::spacing();
	point2.y = option_rect.CenterPoint().y + Formation::spacing() + 1;
	offdc.FillSolidRect(CRect(point1, point2), ((down_point_.x != -1) || ddlistex_.IsWindowVisible()) ? DISABLE_COLOR : WHITE_COLOR);
}

void GridCtrlEx::GridCellDropDownEx::LButtonDown(UINT nFlags, CPoint point) {

	down_point_ = point;
	gridctrlex_->InvalidateRect(rect_, FALSE);
	gridctrlex_->SetCapture();
}

void GridCtrlEx::GridCellDropDownEx::MouseMove(UINT nFlags, CPoint point) {

}

void GridCtrlEx::GridCellDropDownEx::LButtonUp(UINT nFlags, CPoint point) {

	ReleaseCapture();

	CRect wnd_rect = rect_;
	gridctrlex_->ClientToScreen(&wnd_rect);

	ddlistex_.Open(gridctrlex_, wnd_rect, text_, items_list_, selected_index_, GetFont(), align_);

	down_point_.SetPoint(-1, -1);
}

void GridCtrlEx::GridCellDropDownEx::Destroy() {

	items_list_.RemoveAll();
	ddlistex_.DestroyWindow();
}

void GridCtrlEx::GridCellDropDownEx::SetText(int index, CString text) {

	if (index < GetItemCount() && index >= 0) {
		items_list_.SetAt(index, text);
	}
}

CString GridCtrlEx::GridCellDropDownEx::GetText(int index) {

	if (index < GetItemCount() && index >= 0) {
		return items_list_.GetAt(index);
	}
	return L"";
}

void GridCtrlEx::GridCellDropDownEx::AddItem(CString str) {

	items_list_.Add(str);
}

void GridCtrlEx::GridCellDropDownEx::DeleteItem(int index) {

	items_list_.RemoveAt(index);
}

void GridCtrlEx::GridCellDropDownEx::ResetContents() {

	items_list_.RemoveAll();
}

void GridCtrlEx::GridCellDropDownEx::SetCurSel(int index, BOOL highlight) {

	selected_index_ = index;

	if (!highlight) {
		prev_selected_index_ = index;
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
	gridctrlex_->GridCellUpdated();
}

int GridCtrlEx::GridCellDropDownEx::GetCurSel() {

	return selected_index_;
}

int GridCtrlEx::GridCellDropDownEx::GetPrevSel() {

	return prev_selected_index_;
}

CString GridCtrlEx::GridCellDropDownEx::GetCurSelText() {

	if (selected_index_ >= 0 && selected_index_ < int(items_list_.GetSize())) {
		return items_list_[selected_index_];
	}
	return L"";
}

int GridCtrlEx::GridCellDropDownEx::FindString(CString str) {

	for (int index = 0; index < int(items_list_.GetSize()); index++) {
		if (items_list_.GetAt(index) == str) {
			return index;
		}
	}
	return -1;
}

int GridCtrlEx::GridCellDropDownEx::GetItemCount() {

	return int(items_list_.GetCount());
}

void GridCtrlEx::GridCellDropDownEx::SetBackColor(COLORREF back_color) {

	back_color_ = back_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellDropDownEx::SetBorderColor(COLORREF border_color) {

	border_color_ = border_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellDropDownEx::SetTextColor(COLORREF text_color) {

	text_color_ = text_color;
	gridctrlex_->InvalidateRect(rect_, FALSE);
}

void GridCtrlEx::GridCellDropDownEx::SelChangeMessage(WPARAM wparam, LPARAM lparam) {

	int selected_index = int(lparam);

	if (selected_index != -1) {
		selected_index_ = selected_index;
		gridctrlex_->GetParent()->SendMessage(WM_GRIDCTRLEX_DROPDOWNLISTEX_SELCHANGE, (WPARAM)this, (LPARAM)selected_index_);
		gridctrlex_->GridCellUpdated(); //To place * in associated cell text and enable/disable cancel/apply button
	}

	gridctrlex_->InvalidateRect(rect_, FALSE);
}

// GridCtrlEx

IMPLEMENT_DYNAMIC(GridCtrlEx, CStatic)

GridCtrlEx::GridCtrlEx()
{
	heading_cell_ = NULL;

	heading_row_height_ = 0;
	column_row_height_ = 0;
	label_row_height_ = 0;
	row_height_ = 0;

	document_height_ = 0;
	
	total_rows_ = 0;

	button_down_ = FALSE;

	back_color_ = WHITE_COLOR;

	gridcell_ = NULL;
	gridcelldropdown_ = NULL;

	column_cells_.RemoveAll();
	cells_.RemoveAll();

	client_rect_.SetRectEmpty();
	wnd_rect_.SetRectEmpty();
}

GridCtrlEx::~GridCtrlEx()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(GridCtrlEx, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_LISTCTRLEX_SELCHANGE, OnListCtrlExSelChangeMessage)
	//ON_MESSAGE(WM_EDIT_ENTERPRESSED, OnEditEnterPressedMessage)
END_MESSAGE_MAP()

// GridCtrlEx message handlers

void GridCtrlEx::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	label_row_height_ = Formation::heading_height();
	row_height_ = Formation::control_height();

	CStatic::PreSubclassWindow();
}

BOOL GridCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK || (button_down_ && pMsg->message == WM_LBUTTONDOWN)) {
		pMsg->message = 0;
		return -1;
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void GridCtrlEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect control_rect = client_rect_;
	if (document_height_ > client_rect_.Height()) {
		control_rect.right -= GetSystemMetrics(SM_CXVSCROLL);
	}
	if (document_width_ > client_rect_.Width()) {
		control_rect.bottom -= GetSystemMetrics(SM_CYHSCROLL);
	}

	COfflineDC offdc(&dc, &control_rect);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SelectObject(GetFont());
	offdc.FillSolidRect(control_rect, back_color_);

	int top = -GetScrollPos(SB_VERT);
	if (heading_row_height_ > 0) {
		top += heading_row_height_ + spacing_;
	}
	if (column_row_height_ > 0) {
		top += column_row_height_ + spacing_;
	}
	int left = -GetScrollPos(SB_HORZ);

	GridCell* prev_cell = NULL;
	for (int i = 0; i < cells_.GetSize(); i++) {

		GridCell* cell = cells_.GetAt(i);
		if (cell->IsVisibled()) {

			//Find top
			if (prev_cell != NULL) {
				if (cell->GetRowNo() != prev_cell->GetRowNo()) {
					if (prev_cell->GetCellType() == CELL_TYPE::CT_LABEL) {
						top += label_row_height_;
					}
					else {
						top += row_height_;
					}
					top += spacing_;
				}
			}
			cell->GetRect().MoveToY(top);

			//Find left
			if (!columns_in_percentage_) {

				if (cell->GetColNo() == 0) {
					left = -GetScrollPos(SB_HORZ);
					prev_cell = NULL;
				}

				if (prev_cell != NULL) {
					if (cell->GetColNo() != prev_cell->GetColNo()) {
						if (cell->GetColNo() - 1 < column_cells_.GetSize()) {
							left += column_cells_[cell->GetColNo() - 1]->col_width_;
						}
						//left += spacing_;
					}
				}
				cell->GetRect().MoveToX(left);
			}

			offdc.SelectObject(cell->GetFont());
			cell->DrawCell(offdc);

			prev_cell = cell;
		}
	}

	//DRAW HEADING
	if (heading_cell_ != NULL) {

		HBRUSH brush = CreateSolidBrush(heading_cell_->back_color_);
		HPEN pen = CreatePen(PS_SOLID, 1, heading_cell_->border_color_);
		HPEN old_pen = (HPEN)offdc.SelectObject(pen);
		HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);

		offdc.Rectangle(heading_cell_->rect_);

		if (old_pen != NULL) {
			offdc.SelectObject(old_pen);
			DeleteObject(pen);
		}
		if (old_brush != NULL) {
			offdc.SelectObject(old_brush);
			DeleteObject(brush);
		}

		CFont* old_column_font = NULL;
		if (heading_cell_->GetFont() != NULL) {
			old_column_font = offdc.SelectObject(heading_cell_->GetFont());
		}

		CString text = heading_cell_->text_;
		if (heading_cell_->align_ == DT_LEFT) {
			text = L" " + text;
		} else if (heading_cell_->align_ == DT_RIGHT) {
			text = text + L" ";
		}
		offdc.SetTextColor(heading_cell_->text_color_);
		offdc.DrawText(Formation::PrepareString(offdc, text, heading_cell_->rect_), heading_cell_->rect_, heading_cell_->align_ | DT_SINGLELINE | DT_VCENTER);

		if (old_column_font != NULL) {
			offdc.SelectObject(old_column_font);
		}
	}

	//DRAW COLUMNS
	if (column_row_height_ > 0) {

		for (int i = 0; i < column_cells_.GetSize(); i++) {
			GridColumnCell* column_cell = column_cells_.GetAt(i);

			HBRUSH brush = CreateSolidBrush(column_cell->back_color_);
			HPEN pen = CreatePen(PS_SOLID, 1, column_cell->border_color_);
			HPEN old_pen = (HPEN)offdc.SelectObject(pen);
			HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);

			offdc.Rectangle(column_cell->rect_);

			if (old_pen != NULL) {
				offdc.SelectObject(old_pen);
				DeleteObject(pen);
			}
			if (old_brush != NULL) {
				offdc.SelectObject(old_brush);
				DeleteObject(brush);
			}

			CFont* old_column_font = NULL;
			if (column_cell->GetFont() != NULL) {
				old_column_font = offdc.SelectObject(column_cell->GetFont());
			}

			offdc.SetTextColor(column_cell->text_color_);
			offdc.DrawText(Formation::PrepareString(offdc, column_cell->text_, column_cell->rect_), column_cell->rect_, column_cell->align_ | DT_SINGLELINE | DT_VCENTER);

			if (old_column_font != NULL) {
				offdc.SelectObject(old_column_font);
			}

			if (i != 0) {
				offdc.SelectObject(Formation::labelcolor2_pen());
				offdc.MoveTo(column_cell->rect_.left, column_cell->rect_.top + column_cell->rect_.Height() * 20 / 100);
				offdc.LineTo(column_cell->rect_.left, column_cell->rect_.bottom - column_cell->rect_.Height() * 20 / 100);
			}
			offdc.FillSolidRect(CRect(column_cell->rect_.left, column_cell->rect_.bottom, column_cell->rect_.right, column_cell->rect_.bottom + spacing_), WHITE_COLOR);
		}
	}

	//SELECT ROW 
	if (select_row_no_ != -1) {
		CRect select_rect(0xFFFF, 0, 0, 0);
		for (int i = 0; i < cells_.GetSize(); i++) {
			GridCell* cell = cells_.GetAt(i);
			if (select_row_no_ == cell->GetRowNo()) {
				if (select_rect.left > cell->GetRect().left) {
					select_rect.left = cell->GetRect().left;
				}
				if (select_rect.right < cell->GetRect().right) {
					select_rect.right = cell->GetRect().right;
				}
				select_rect.top = cell->GetRect().top;
				select_rect.bottom = cell->GetRect().bottom;
			}
		}
		if (select_rect.top != 0) {
			HPEN pen = CreatePen(PS_SOLID, 1, selected_row_border_color_);
			HPEN old_pen = (HPEN)offdc.SelectObject(pen);
			offdc.SelectObject(GetStockObject(NULL_BRUSH));
			offdc.Rectangle(select_rect);
			offdc.SelectObject(old_pen);
			DeleteObject(pen);
		}
	}
}

void GridCtrlEx::GridCellUpdated() {

	for (int index = 0; index < (int)cells_.GetSize(); index++) {
		GridCell* cell = cells_.GetAt(index);
		BOOL value_changed = FALSE;
		for (int asso_cell_index = 0; asso_cell_index < cell->AssociatedChildCellCount(); asso_cell_index++) {
			GridCell* asso_cell = cell->GetAssociatedCell(asso_cell_index);
			if (asso_cell != NULL) {
				if (asso_cell->GetCellType() == CELL_TYPE::CT_EDIT) {
					GridCellEdit* cell_edit = (GridCellEdit*)asso_cell;
					if (cell_edit->GetPrevText() != cell_edit->GetText()) {
						value_changed = TRUE;
						break;
					}
				} else if (asso_cell->GetCellType() == CELL_TYPE::CT_ONOFF) {
					GridCellOnOff* cell_onoff = (GridCellOnOff*)asso_cell;
					if (cell_onoff->GetPrevCheck() != cell_onoff->GetCheck()) {
						value_changed = TRUE;
						break;
					}
				} else if (asso_cell->GetCellType() == CELL_TYPE::CT_DROPDOWN) {
					GridCellDropDown* cell_dropdown = (GridCellDropDown*)asso_cell;
					if (cell_dropdown->GetPrevSel() != cell_dropdown->GetCurSel()) {
						value_changed = TRUE;
						break;
					}
				} else if (asso_cell->GetCellType() == CELL_TYPE::CT_DROPDOWNEX) {
					GridCellDropDownEx* cell_dropdownex = (GridCellDropDownEx*)asso_cell;
					if (cell_dropdownex->GetPrevSel() != cell_dropdownex->GetCurSel()) {
						value_changed = TRUE;
						break;
					}
				} else if (asso_cell->GetCellType() == CELL_TYPE::CT_SPIN) {
					GridCellSpin* cell_spin = (GridCellSpin*)asso_cell;
					if (cell_spin->IsFloating()) {
						if (cell_spin->GetPrevPosF() != cell_spin->GetPosF()) {
							value_changed = TRUE;
							break;
						}
					} else {
						if (cell_spin->GetPrevPos() != cell_spin->GetPos()) {
							value_changed = TRUE;
							break;
						}
					}
				} else if (asso_cell->GetCellType() == CELL_TYPE::CT_BUTTON) {
					GridCellButton* cell_button = (GridCellButton*)asso_cell;
					if (cell_button->GetCellButtonType() != CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
						if (cell_button->GetPrevCheck() != cell_button->GetCheck()) {
							value_changed = TRUE;
							break;
						}
					}
				}
			}
		}
		cell->ValueChanged(value_changed);
	}

	BOOL enable_buttons = FALSE;
	for (int index = 0; index < (int)cells_.GetSize(); index++) {
		GridCell* cell = cells_.GetAt(index);
		if (cell->GetCellType() == CELL_TYPE::CT_EDIT) {
			GridCellEdit* cell_edit = (GridCellEdit*)cell;
			if (cell_edit->GetPrevText() != cell_edit->GetText()) {
				enable_buttons = TRUE;
				break;
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_ONOFF) {
			GridCellOnOff* cell_onoff = (GridCellOnOff*)cell;
			if (cell_onoff->GetPrevCheck() != cell_onoff->GetCheck()) {
				enable_buttons = TRUE;
				break;
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWN) {
			GridCellDropDown* cell_dropdown = (GridCellDropDown*)cell;
			if (cell_dropdown->GetPrevSel() != cell_dropdown->GetCurSel()) {
				enable_buttons = TRUE;
				break;
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWNEX) {
			GridCellDropDownEx* cell_dropdownex = (GridCellDropDownEx*)cell;
			if (cell_dropdownex->GetPrevSel() != cell_dropdownex->GetCurSel()) {
				enable_buttons = TRUE;
				break;
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_SPIN) {
			GridCellSpin* cell_spin = (GridCellSpin*)cell;
			if (cell_spin->IsFloating()) {
				if (cell_spin->GetPrevPosF() != cell_spin->GetPosF()) {
					enable_buttons = TRUE;
					break;
				}
			} else {
				if (cell_spin->GetPrevPos() != cell_spin->GetPos()) {
					enable_buttons = TRUE;
					break;
				}
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_BUTTON) {
			GridCellButton* cell_button = (GridCellButton*)cell;
			if (cell_button->GetCellButtonType() != CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
				if (cell_button->GetPrevCheck() != cell_button->GetCheck()) {
					enable_buttons = TRUE;
					break;
				}
			}
		}
	}
	for (int index = 0; index < (int)associated_buttons_.GetSize(); index++) {
		associated_buttons_[index]->EnableWnd(enable_buttons);
	}
}

void GridCtrlEx::RemoveAllRows() {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		cell->Destroy();
		delete cell;
	}
	cells_.RemoveAll();
	gridcell_ = NULL;
	gridcelldropdown_ = NULL;
}

void GridCtrlEx::ResetScrollBar() {

	SCROLLINFO	si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = client_rect_.Height();
	si.nPos = si.nMax = document_height_;// +8;
	si.nMin = 0;
	SetScrollInfo(SB_VERT, &si);
	if(client_rect_.Height() >= document_height_) {
		ShowScrollBar(SB_VERT, FALSE);
	} else {
		SetScrollPos(SB_VERT, 0);
		EnableScrollBarCtrl(SB_VERT, TRUE);
	}

	if(!columns_in_percentage_) {
		SCROLLINFO	si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = client_rect_.Width();
		si.nPos = si.nMax = document_width_;// +8;
		si.nMin = 0;
		SetScrollInfo(SB_HORZ, &si);
		if(client_rect_.Width() >= document_width_) {
			ShowScrollBar(SB_HORZ, FALSE);
		} else {
			SetScrollPos(SB_HORZ, 0);
			EnableScrollBarCtrl(SB_HORZ, TRUE);
		}
	}
}

CRect GridCtrlEx::CalcGridDocumentWH(int spacing, BOOL columns_in_percentage) {

	document_height_ = 0;
	document_width_ = 0;

	GetClientRect(client_rect_);

	CRect wnd_rect = client_rect_;

	total_rows_ = -1;
	for (int cell_index = 0; cell_index < cells_.GetSize(); cell_index++) {
		GridCell* cell = cells_.GetAt(cell_index);
		if (cell->GetRowNo() > total_rows_) {
			total_rows_ = cell->GetRowNo();
		}
	}

	if (total_rows_ < 0 && (int)column_cells_.GetSize() <= 0) {
		return CRect();
	}

	int row_height = 0;
	if (row_height_ == 0) {
		row_height = (client_rect_.Height() - ((total_rows_ - 1) * spacing)) / total_rows_;
		if (row_height > Formation::heading_height()) {
			row_height = Formation::heading_height();
		}
	} else {
		row_height = row_height_;
	}

	//CALCULATE DOCUMENT HEIGHT
	CRect rect;
	rect.top = heading_row_height_;
	if (heading_row_height_ > 0) {
		rect.top += spacing;
	}
	rect.top += column_row_height_;
	if (column_row_height_ > 0) {
		rect.top += spacing;
	}
	GridCell* prev_cell = NULL;
	for (int cell_index = 0; cell_index < cells_.GetSize(); cell_index++) {
		GridCell* cell = cells_.GetAt(cell_index);
		//Find top
		if (prev_cell != NULL) {
			if (cell->GetRowNo() != prev_cell->GetRowNo()) {
				if (prev_cell->GetCellType() == CELL_TYPE::CT_LABEL) {
					rect.top += label_row_height_;
				} else {
					rect.top += row_height_;
				}
				rect.top += spacing;
			}
		}
		if (cell->GetCellType() == CELL_TYPE::CT_LABEL) {
			rect.bottom = rect.top + label_row_height_;
		} else {
			rect.bottom = rect.top + (cell->GetRowCount() * row_height) + ((cell->GetRowCount() - 1) * spacing);
			//if rows are merged
			int cell_height = 0;
			int row_start = cell->GetRowNo();
			for (int j = row_start; j < row_start + cell->GetRowCount(); j++) {
				cell_height += row_height;
			}
			rect.bottom = rect.top + cell_height + (cell->GetRowCount() - 1) * spacing;
		}
		cell->SetRect(rect);
		prev_cell = cell;
	}
	//

	document_height_ = rect.bottom;

	if (!columns_in_percentage_) {
		for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
			document_width_ += column_cells_[cell_index]->col_width_;
		}
		if (document_width_ > wnd_rect.Width()) {
			document_height_ += (GetSystemMetrics(SM_CYHSCROLL));
		}
		if (document_height_ > wnd_rect.Height()) {
			document_width_ += (GetSystemMetrics(SM_CYHSCROLL));
		}
	}
	if (document_height_ > wnd_rect.Height() /*If vertical scroll bar exist*/ && document_width_ <= wnd_rect.Width() /*If horizontal scroll bar not exist*/) {
		wnd_rect.right -= (GetSystemMetrics(SM_CXVSCROLL) + Formation::spacing());
	}

	return wnd_rect;
}

void GridCtrlEx::Create(int spacing, BOOL columns_in_percentage) {

	spacing_ = spacing;
	columns_in_percentage_ = columns_in_percentage;

	CRect wnd_rect = CalcGridDocumentWH(spacing, columns_in_percentage);
	if (wnd_rect.IsRectEmpty()) {
		GetParent()->SendMessageW(WM_GRIDCTRLEX_CREATED, (WPARAM)this, (LPARAM)-1);
		return;
	}

	int row_height = 0;
	if (row_height_ == 0) {
		row_height = (client_rect_.Height() - ((total_rows_ - 1) * spacing_)) / total_rows_;
		if (row_height > Formation::heading_height()) {
			row_height = Formation::heading_height();
		}
	} else {
		row_height = row_height_;
	}

	int cols = 0;
	for (int col_index = 0; col_index < (int)column_cells_.GetSize(); col_index++) {
		if (column_cells_[col_index]->col_width_ != 0) {
			cols = 1;
		}
	}

	int width = wnd_rect.Width();
	if (cols == 0) {
		width = width / (int)column_cells_.GetSize();
		if (width > Formation::control_height()) {
			width = Formation::control_height();
		}
	}
	CDWordArray col_width;
	CDWordArray col_start;
	int start = 0;
	for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
		int temp_width = 0;
		if (columns_in_percentage_) {
			temp_width = (cols == 1) ? int(ceil((width * column_cells_[cell_index]->col_width_) / 100)) : width;
		} else {
			temp_width = column_cells_[cell_index]->col_width_;
		}
		col_width.Add(temp_width);
		col_start.Add(start);
		start += temp_width;
	}
	
	selected_index_ = -1;
	select_row_no_ = -1;

	//DRAW HEADING
	CRect heading_rect = wnd_rect;
	if(heading_cell_) {
		heading_rect.right = wnd_rect.right - 1;
		heading_rect.bottom = heading_rect.top + heading_row_height_;
		heading_cell_->rect_ = heading_rect;
	}

	//DRAW COLUMNS
	CRect column_rect = heading_rect;
	if (heading_row_height_ > 0) {
		column_rect.top = heading_rect.bottom;
		column_rect.top += spacing_;
	}
	for (int i = 0; i < column_cells_.GetSize(); i++) {
		GridColumnCell* column_cell = column_cells_.GetAt(i);
		column_rect.right = column_rect.left + col_width.GetAt(i);
		if (i == column_cells_.GetSize() - 1) {
			column_rect.right = wnd_rect.right - 1;
		}
		column_rect.bottom = column_rect.top + column_row_height_;
		column_cell->rect_ = column_rect;
		column_rect.left = column_rect.right;// +spacing_;
	}

	//DRAW CELLS
	CRect rect;
	rect.top = column_rect.bottom;
	if (column_row_height_ > 0) {
		rect.top += spacing_;
	}
	GridCell* prev_cell = NULL;
	for (int cell_index = 0; cell_index < cells_.GetSize(); cell_index++) {
		GridCell* cell = cells_.GetAt(cell_index);
		
		//Find top
		if (prev_cell != NULL) {
			if (cell->GetRowNo() != prev_cell->GetRowNo()) {
				if (prev_cell->GetCellType() == CELL_TYPE::CT_LABEL) {
					rect.top += label_row_height_;
				} else {
					rect.top += row_height;
				}
				rect.top += spacing_;
			}
		}
		
		if (cell->GetCellType() == CELL_TYPE::CT_LABEL) {
			rect.bottom = rect.top + label_row_height_;
			rect.left = wnd_rect.left;
			if (columns_in_percentage_) {
				rect.right = wnd_rect.right - 1;
			} else {
				int temp_width = 0;
				for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
					temp_width += column_cells_[cell_index]->col_width_;
				}
				rect.right = temp_width - 1;
			}
		} else {
			rect.left = col_start.GetAt(cell->GetColNo());

			//if cols are merged
			int cell_width = 0;
			int col_start = cell->GetColNo();
			for (int j = col_start; j < col_start + cell->GetColCount(); j++) {
				cell_width += col_width.GetAt(j);
			}
			if (cell->IsJointed()) {
				rect.right = rect.left + cell_width + 1;
			} else {
				rect.right = rect.left + cell_width - spacing_;
			}
			if (cell->GetColNo() + cell->GetColCount() == column_cells_.GetSize()) {
				if (columns_in_percentage_) {
					if (rect.right >= wnd_rect.right) {
						rect.right = wnd_rect.right - 1;
					}
				} else {
					int temp_width = 0;
					for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
						temp_width += column_cells_[cell_index]->col_width_;
					}
					rect.right = temp_width - 1;
				}
			}

			//if rows are merged
			int cell_height = 0;
			int row_start = cell->GetRowNo();
			for (int row_index = row_start; row_index < row_start + cell->GetRowCount(); row_index++) {
				cell_height += row_height_;
			}
			rect.bottom = rect.top + cell_height + (cell->GetRowCount() - 1) * spacing_;
		}
		cell->SetRect(rect);

		cell->PrepareCell();

		prev_cell = cell;
	}

	if (document_height_ != 0 && document_height_ < wnd_rect.Height()) {

		CRect control_rect;
		GetWindowRect(&control_rect);
		GetParent()->ScreenToClient(&control_rect);
		control_rect.bottom = control_rect.top + document_height_;

		MoveWindow(control_rect);

		client_rect_.bottom = control_rect.Height();
		client_rect_.right = control_rect.Width();
		//GetClientRect(&client_rect_);
	}

	ResetScrollBar();

	Invalidate(FALSE);

	gridcell_ = gridcelldropdown_ = NULL;

	GetParent()->SendMessageW(WM_GRIDCTRLEX_CREATED, (WPARAM)this, (LPARAM)0);
}

void GridCtrlEx::RefreshGrid() {

	CRect wnd_rect = CalcGridDocumentWH(spacing_, columns_in_percentage_);
	if (wnd_rect.IsRectEmpty()) {
		//GetParent()->SendMessageW(WM_GRIDCTRLEX_CREATED, (WPARAM)this, (LPARAM)-1);
		return;
	}
	
	GetClientRect(&client_rect_);

	int row_height = 0;
	if (row_height_ == 0) {
		row_height = (client_rect_.Height() - ((total_rows_ - 1) * spacing_)) / total_rows_;
		if (row_height > Formation::heading_height()) {
			row_height = Formation::heading_height();
		}
	}
	else {
		row_height = row_height_;
	}

	int cols = 0;
	for (int col_index = 0; col_index < (int)column_cells_.GetSize(); col_index++) {
		if (column_cells_[col_index]->col_width_ != 0) {
			cols = 1;
		}
	}

	int width = wnd_rect.Width();
	if (cols == 0) {
		width = width / (int)column_cells_.GetSize();
		if (width > Formation::control_height()) {
			width = Formation::control_height();
		}
	}
	CDWordArray col_width;
	CDWordArray col_start;
	int start = 0;
	for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
		int temp_width = 0;
		if (columns_in_percentage_) {
			temp_width = (cols == 1) ? int(ceil((width * column_cells_[cell_index]->col_width_) / 100)) : width;
		}
		else {
			temp_width = column_cells_[cell_index]->col_width_;
		}
		col_width.Add(temp_width);
		col_start.Add(start);
		start += temp_width;
	}

	selected_index_ = -1;
	select_row_no_ = -1;

	//DRAW HEADING
	CRect heading_rect = wnd_rect;
	if (heading_cell_) {
		heading_rect.right = wnd_rect.right - 1;
		heading_rect.bottom = heading_rect.top + heading_row_height_;
		heading_cell_->rect_ = heading_rect;
	}

	//DRAW COLUMNS
	CRect column_rect = heading_rect;
	if (heading_row_height_ > 0) {
		column_rect.top = heading_rect.bottom;
		column_rect.top += spacing_;
	}
	for (int i = 0; i < column_cells_.GetSize(); i++) {
		GridColumnCell* column_cell = column_cells_.GetAt(i);
		column_rect.right = column_rect.left + col_width.GetAt(i);
		if (i == column_cells_.GetSize() - 1) {
			column_rect.right = wnd_rect.right - 1;
		}
		column_rect.bottom = column_rect.top + column_row_height_;
		column_cell->rect_ = column_rect;
		column_rect.left = column_rect.right;// +spacing_;
	}

	//DRAW CELLS
	CRect rect;
	rect.top = column_rect.bottom;
	if (column_row_height_ > 0) {
		rect.top += spacing_;
	}
	GridCell* prev_cell = NULL;
	for (int cell_index = 0; cell_index < cells_.GetSize(); cell_index++) {
		GridCell* cell = cells_.GetAt(cell_index);
		if (!cell->IsVisibled()) {
			cell->SetRect(CRect());
		} else {
			//Find top
			if (prev_cell != NULL) {
				if (cell->GetRowNo() != prev_cell->GetRowNo()) {
					if (prev_cell->GetCellType() == CELL_TYPE::CT_LABEL) {
						rect.top += label_row_height_;
					} else {
						rect.top += row_height;
					}
					rect.top += spacing_;
				}
			}
		
			if (cell->GetCellType() == CELL_TYPE::CT_LABEL) {
				rect.bottom = rect.top + label_row_height_;
				rect.left = wnd_rect.left;
				if (columns_in_percentage_) {
					rect.right = wnd_rect.right - 1;
				} else {
					int temp_width = 0;
					for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
						temp_width += column_cells_[cell_index]->col_width_;
					}
					rect.right = temp_width - 1;
				}
			} else {
				rect.left = col_start.GetAt(cell->GetColNo());

				//if cols are merged
				int cell_width = 0;
				int col_start = cell->GetColNo();
				for (int j = col_start; j < col_start + cell->GetColCount(); j++) {
					cell_width += col_width.GetAt(j);
				}
				if (cell->IsJointed()) {
					rect.right = rect.left + cell_width + 1;
				} else {
					rect.right = rect.left + cell_width - spacing_;
				}
				if (cell->GetColNo() + cell->GetColCount() == column_cells_.GetSize()) {
					if (columns_in_percentage_) {
						if (rect.right >= wnd_rect.right) {
							rect.right = wnd_rect.right - 1;
						}
					} else {
						int temp_width = 0;
						for (int cell_index = 0; cell_index < column_cells_.GetSize(); cell_index++) {
							temp_width += column_cells_[cell_index]->col_width_;
						}
						rect.right = temp_width - 1;
					}
				}

				//if rows are merged
				int cell_height = 0;
				int row_start = cell->GetRowNo();
				for (int row_index = row_start; row_index < row_start + cell->GetRowCount(); row_index++) {
					cell_height += row_height_;
				}
				rect.bottom = rect.top + cell_height + (cell->GetRowCount() - 1) * spacing_;
			}

			cell->SetRect(rect);
		}

		cell->PrepareCell();

		prev_cell = cell;
	}

	if (document_height_ != 0 && document_height_ < wnd_rect.Height()) {

		CRect control_rect;
		GetWindowRect(&control_rect);
		GetParent()->ScreenToClient(&control_rect);
		control_rect.bottom = control_rect.top + document_height_;

		MoveWindow(control_rect);

		client_rect_.bottom = control_rect.Height();
		client_rect_.right = control_rect.Width();
		//GetClientRect(&client_rect_);
	}

	ResetScrollBar();

	Invalidate(FALSE);

	gridcell_ = gridcelldropdown_ = NULL;
}

void GridCtrlEx::Destroy() {

	delete heading_cell_;
	heading_cell_ = NULL;

	for (int i = 0; i < column_cells_.GetSize(); i++) {
		GridColumnCell* column_cell = column_cells_.GetAt(i);
		delete column_cell;
	}
	column_cells_.RemoveAll();

	RemoveAllRows();
}

void GridCtrlEx::DisableScrollbar(UINT sbar, BOOL disable) {

	if (sbar == SB_VERT) {
		if (client_rect_.Height() < document_height_) {
			SetScrollPos(SB_VERT, document_height_);
			Invalidate(FALSE);
		}
	} else {
		if (client_rect_.Width() < document_width_) {
			SetScrollPos(SB_HORZ, document_width_);
			Invalidate(FALSE);
		}
	}
	
	EnableScrollBar(sbar, disable ? ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);
}

GridCtrlEx::GridHeadingCell*  GridCtrlEx::InsertHeading(CString text, DWORD align) {

	heading_cell_ = new GridHeadingCell(this);
	heading_cell_->back_color_ = LABEL_COLOR;
	heading_cell_->border_color_ = LABEL_COLOR;
	heading_cell_->text_color_ = WHITE_COLOR;
	heading_cell_->text_ = text;
	heading_cell_->align_ = align;

	heading_row_height_ = Formation::heading_height();

	return heading_cell_;
}

GridCtrlEx::GridColumnCell* GridCtrlEx::InsertColumn(int col_no, int col_width, CString text, DWORD align) {

	GridColumnCell* cell = new GridColumnCell(this);
	cell->back_color_ = LABEL_COLOR;
	cell->border_color_ = LABEL_COLOR;
	cell->text_color_ = WHITE_COLOR;
	cell->col_no_ = col_no;
	cell->col_width_ = col_width;
	cell->text_ = text;
	cell->align_ = align;

	column_cells_.Add(cell);

	if (!text.IsEmpty()) {
		column_row_height_ = Formation::heading_height();
	}

	return cell;
}

GridCtrlEx::GridCellLabel* GridCtrlEx::AddCellLabel(int row_no, CString text, DWORD align, BOOL allow_selection) {
	
	GridCellLabel* cell = new GridCellLabel(this, row_no, text, align, allow_selection);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellText* GridCtrlEx::AddCellText(int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL multiline, BOOL allow_selection, BOOL joint) {

	GridCellText* cell = new GridCellText(this, row_no, col_no, row_count, col_count, text, align, multiline, allow_selection, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellEdit* GridCtrlEx::AddCellEdit(int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL joint) {

	GridCellEdit* cell = new GridCellEdit(this, row_no, col_no, row_count, col_count, text, align, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellSpin* GridCtrlEx::AddCellSpin(int row_no, int col_no, int row_count, int col_count, BOOL joint) {

	GridCellSpin* cell = new GridCellSpin(this, row_no, col_no, row_count, col_count, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellOnOff* GridCtrlEx::AddCellOnOff(int row_no, int col_no, int row_count, int col_count, CString on_text, CString off_text, BOOL joint) {

	GridCellOnOff* cell = new GridCellOnOff(this, row_no, col_no, row_count, col_count, on_text, off_text, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellButton* GridCtrlEx::AddCellButton(int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL joint) {

	return AddCellButton(CELL_BUTTON_TYPE::CT_BUTTON_REGULAR, row_no, col_no, row_count, col_count, text, text, align, joint);
}

GridCtrlEx::GridCellButton* GridCtrlEx::AddCellButton(CELL_BUTTON_TYPE cell_button_type, int row_no, int col_no, int row_count, int col_count, CString on_text, CString off_text, DWORD align, BOOL joint) {

	GridCellButton* cell = new GridCellButton(this, cell_button_type, row_no, col_no, row_count, col_count, on_text, off_text, align, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellDropDown* GridCtrlEx::AddCellDropDown(int row_no, int col_no, int row_count, int col_count, DWORD align, BOOL joint) {

	GridCellDropDown* cell = new GridCellDropDown(this, row_no, col_no, row_count, col_count, align, joint);
	cells_.Add(cell);
	return cell;
}

GridCtrlEx::GridCellDropDownEx* GridCtrlEx::AddCellDropDownEx(int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL joint) {

	GridCellDropDownEx* cell = new GridCellDropDownEx(this, row_no, col_no, row_count, col_count, text, align, joint);
	cells_.Add(cell);
	return cell;
}

BOOL GridCtrlEx::RemoveCell(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			cell->Destroy();
			delete cell;
			cells_.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}

int GridCtrlEx::GetTotalRows() {

	total_rows_ = 0;
	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetRowNo() + 1 > total_rows_) {
			total_rows_ = cell->GetRowNo() + 1;
		}
	}
	return total_rows_;
}

GridCtrlEx::GridColumnCell* GridCtrlEx::GetGridColumnCell(int col_no) {

	if(col_no < column_cells_.GetSize()) {
		return column_cells_.GetAt(col_no);
	}
	return NULL;
}

GridCtrlEx::CELL_TYPE GridCtrlEx::GetCellType(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return cell->GetCellType();
		}
	}
	return CELL_TYPE::CT_LABEL;
}

GridCtrlEx::GridCellLabel* GridCtrlEx::GetGridCellLabel(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_LABEL && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellLabel*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellText* GridCtrlEx::GetGridCellText(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_TEXT && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellText*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellEdit* GridCtrlEx::GetGridCellEdit(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_EDIT && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellEdit*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellSpin* GridCtrlEx::GetGridCellSpin(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_SPIN && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellSpin*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellOnOff* GridCtrlEx::GetGridCellOnOff(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_ONOFF && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellOnOff*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellDropDown* GridCtrlEx::GetGridCellDropDown(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWN && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellDropDown*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellDropDownEx* GridCtrlEx::GetGridCellDropDownEx(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWNEX && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellDropDownEx*)cell;
		}
	}
	return NULL;
}

GridCtrlEx::GridCellButton* GridCtrlEx::GetGridCellButton(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == CELL_TYPE::CT_BUTTON && cell->GetRowNo() == row_no && cell->GetColNo() == col_no) {
			return (GridCellButton*)cell;
		}
	}
	return NULL;
}

void GridCtrlEx::SetBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetBackColor(back_color);
		}
	}
}

void GridCtrlEx::SetBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetBorderColor(border_color);
		}
	}
}

void GridCtrlEx::SetTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetTextColor(text_color);
		}
	}
}

void GridCtrlEx::SetSelBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetSelBackColor(back_color);
		}
	}
}

void GridCtrlEx::SetSelBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetSelBorderColor(border_color);
		}
	}
}

void GridCtrlEx::SetSelTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetSelTextColor(text_color);
		}
	}
}

void GridCtrlEx::SetDisableBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetDisableBackColor(back_color);
		}
	}
}

void GridCtrlEx::SetDisableBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetDisableBorderColor(border_color);
		}
	}
}

void GridCtrlEx::SetDisableTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() == cell_type) {
			cell->SetDisableTextColor(text_color);
		}
	}
}

void GridCtrlEx::SetBackColor(COLORREF back_color) { 

	back_color_ = back_color; 
}

void GridCtrlEx::SetBorderColor(COLORREF border_color) {

	//border_color_ = border_color;
}

void GridCtrlEx::SetTextColor(COLORREF text_color) {
		
}

void GridCtrlEx::SelectRow(int row_no, COLORREF selected_row_border_color) {
	
	select_row_no_ = row_no; 
	selected_row_border_color_ = selected_row_border_color; 
}

void GridCtrlEx::DisableRow(int row_no, BOOL disable) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() != CELL_TYPE::CT_LABEL && cell->GetRowNo() == row_no) {
			cell->DisableRow(disable);
		}
	}
}

void GridCtrlEx::DisableAllRows(BOOL disable) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->GetCellType() != CELL_TYPE::CT_LABEL) {
			cell->DisableRow(disable);
		}
	}
}

void GridCtrlEx::SetAssociatedButtons(CArray<Button*>& associated_buttons) {

	associated_buttons_.RemoveAll();
	for (int index = 0; index < (int)associated_buttons.GetSize(); index++) {
		associated_buttons_.Add(associated_buttons[index]);
	}
	associated_buttons.RemoveAll();

	BOOL enable_buttons = FALSE;
	for (int index = 0; index < (int)associated_buttons_.GetSize(); index++) {
		associated_buttons_[index]->EnableWnd(enable_buttons);
	}
}

void GridCtrlEx::ResetValuesOfAllCells(BOOL with_prev_values) {

	for (int index = 0; index < (int)cells_.GetSize(); index++) {
		GridCell* cell = cells_.GetAt(index);
		if (cell->GetCellType() == CELL_TYPE::CT_EDIT) {
			GridCellEdit* cell_edit = (GridCellEdit*)cell;
			cell_edit->SetText(with_prev_values ? cell_edit->GetPrevText() : cell_edit->GetText());
		} else if (cell->GetCellType() == CELL_TYPE::CT_ONOFF) {
			GridCellOnOff* cell_onoff = (GridCellOnOff*)cell;
			cell_onoff->SetCheck(with_prev_values ? cell_onoff->GetPrevCheck() : cell_onoff->GetCheck(), FALSE);
		} else if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWN) {
			GridCellDropDown* cell_dropdown = (GridCellDropDown*)cell;
			cell_dropdown->SetCurSel(with_prev_values ? cell_dropdown->GetPrevSel() : cell_dropdown->GetCurSel());
		} else if (cell->GetCellType() == CELL_TYPE::CT_DROPDOWNEX) {
			GridCellDropDownEx* cell_dropdownex = (GridCellDropDownEx*)cell;
			cell_dropdownex->SetCurSel(with_prev_values ? cell_dropdownex->GetPrevSel() : cell_dropdownex->GetCurSel());
		} else if (cell->GetCellType() == CELL_TYPE::CT_SPIN) {
			GridCellSpin* cell_spin = (GridCellSpin*)cell;
			if (cell_spin->IsFloating()) {
				cell_spin->SetPosF(with_prev_values ? cell_spin->GetPrevPosF() : cell_spin->GetPosF());
			} else {
				cell_spin->SetPos(with_prev_values ? cell_spin->GetPrevPos() : cell_spin->GetPos());
			}
		} else if (cell->GetCellType() == CELL_TYPE::CT_BUTTON) {
			GridCellButton* cell_button = (GridCellButton*)cell;
			if (cell_button->GetCellButtonType() != CELL_BUTTON_TYPE::CT_BUTTON_REGULAR) {
				cell_button->SetCheck(with_prev_values ? cell_button->GetPrevCheck() : cell_button->GetCheck());
			}
		}
	}
}

void GridCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int pty = heading_row_height_;
	if (heading_row_height_ > 0) {
		pty += spacing_;
	}
	pty += column_row_height_;
	if (column_row_height_ > 0) {
		pty += spacing_;
	}
	if (point.y < pty || button_down_) {
		return;
	}

	button_down_ = TRUE;

	if (gridcelldropdown_) { //Required...If dropdown list is open and clicked other than cell

		GridCell* gridcell = (GridCell*)gridcelldropdown_;
		gridcell->LButtonDown(nFlags, point);

	} else {

		gridcell_ = NULL;

		for (int i = 0; i < (int)cells_.GetSize(); i++) {
			GridCell* gridcell = (GridCell*)cells_.GetAt(i);
			if (!gridcell->IsDisabled()) {
				if (gridcell->GetRect().PtInRect(point)) {
					gridcell_ = gridcell;
					break;
				}
			}
		}

		if (gridcell_ != NULL) {
			if (gridcell_->GetRect().top < (client_rect_.top + pty)) {
				int pos = gridcell_->GetRect().top - (client_rect_.top + pty);
				SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
				Invalidate(FALSE);
			}
			if (gridcell_->GetRect().bottom > client_rect_.bottom) {
				int pos = gridcell_->GetRect().bottom - client_rect_.bottom;
				SetScrollPos(SB_VERT, pos + GetScrollPos(SB_VERT));
				Invalidate(FALSE);
			}

			selected_index_ = gridcell_->GetIndex();
			
			gridcell_->LButtonDown(nFlags, point);
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void GridCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (gridcell_ != NULL) {
		gridcell_->MouseMove(nFlags, point);
	}

	CStatic::OnMouseMove(nFlags, point);
}

void GridCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	int pty = heading_row_height_;
	if (heading_row_height_ > 0) {
		pty += spacing_;
	}
	pty += column_row_height_;
	if (column_row_height_ > 0) {
		pty += spacing_;
	}
	if (point.y < pty) {
		return;
	}

	if (gridcell_ != NULL) {
		gridcell_->LButtonUp(nFlags, point);
	}

	selected_index_ = -1;

	if (gridcell_ != NULL) {
		InvalidateRect(gridcell_->GetRect(), FALSE);
	}

	button_down_ = FALSE;

	CStatic::OnLButtonUp(nFlags, point);
}

void GridCtrlEx::OnTimer(UINT_PTR nIDEvent)
{
	if (gridcell_ != NULL) {
		gridcell_->OnTimer(nIDEvent);
	}

	CStatic::OnTimer(nIDEvent);
}

void GridCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iScrollBarPos = GetScrollPos( SB_VERT );

	switch( nSBCode )
	{
		case SB_LINEUP:
			iScrollBarPos = std::max(iScrollBarPos - Formation::spacing(), 0);
		break;
		case SB_LINEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + Formation::spacing(), GetScrollLimit(SB_VERT) );
		break;
		case SB_PAGEUP:
			iScrollBarPos = std::max(iScrollBarPos - (client_rect_.Height() - heading_row_height_ - spacing_ - column_row_height_ - spacing_), 0);
		break;
		case SB_PAGEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + (client_rect_.Height() - heading_row_height_ - spacing_ - column_row_height_ - spacing_), GetScrollLimit(SB_VERT));
		break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			SCROLLINFO si;
			ZeroMemory( &si, sizeof(SCROLLINFO) );
			si.cbSize	= sizeof(SCROLLINFO);
			si.fMask	= SIF_TRACKPOS;

			if (GetScrollInfo(SB_VERT, &si, SIF_TRACKPOS)) {
				iScrollBarPos = si.nTrackPos;
			} else {
				iScrollBarPos = (UINT)nPos;
			}
			break;
		}
	}		
	
	SetScrollPos(SB_VERT, iScrollBarPos);
	Invalidate(FALSE);
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void GridCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iScrollBarPos = GetScrollPos( SB_HORZ );

	switch( nSBCode )
	{
		case SB_LINEUP:
			iScrollBarPos = std::max(iScrollBarPos - Formation::spacing(), 0);
		break;
		case SB_LINEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + Formation::spacing(), GetScrollLimit(SB_HORZ) );
		break;
		case SB_PAGEUP:
			iScrollBarPos = std::max(iScrollBarPos - (client_rect_.Width() - spacing_), 0);
		break;
		case SB_PAGEDOWN:
			iScrollBarPos = std::min(iScrollBarPos + (client_rect_.Width() - spacing_), GetScrollLimit(SB_HORZ));
		break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			SCROLLINFO si;
			ZeroMemory( &si, sizeof(SCROLLINFO) );
			si.cbSize	= sizeof(SCROLLINFO);
			si.fMask	= SIF_TRACKPOS;

			if (GetScrollInfo(SB_HORZ, &si, SIF_TRACKPOS)) {
				iScrollBarPos = si.nTrackPos;
			} else {
				iScrollBarPos = (UINT)nPos;
			}
			break;
		}
	}		

	SetScrollPos(SB_HORZ, iScrollBarPos);
	Invalidate(FALSE);
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT GridCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_NCHITTEST || message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK)
		return ::DefWindowProc(m_hWnd, message, wParam, lParam);
	
	return CStatic::WindowProc(message, wParam, lParam);
}

BOOL GridCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (!gridcelldropdown_) {
		//OnVScroll((zDelta > 0) ? SB_LINEUP : SB_LINEDOWN, 0, NULL);
	}

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT GridCtrlEx::OnListCtrlExSelChangeMessage(WPARAM wparam, LPARAM lparam) {

	if (gridcell_) {
		GridCtrlEx::GridCellDropDownEx* dropdownex = (GridCtrlEx::GridCellDropDownEx*)gridcell_;
		if (dropdownex) {
			dropdownex->SelChangeMessage(wparam, lparam);
		}
	}	

	return 0;
}

//
//LRESULT GridCtrlEx::OnEditEnterPressedMessage(WPARAM wparam, LPARAM lparam) {
//
//	GridCellSpin* cell = (GridCellSpin*)wparam;
//
//	for (int i = 0; i < cells_.GetSize(); i++) {
//		GridCell* cell = cells_.GetAt(i);
//		if (cell->GetCellType() == CELL_TYPE::CT_SPIN) {
//			GridCellSpin* spincell = (GridCellSpin*)cell;
//			if (spincell == cell) {
//				spincell->EditEnterPressedMessage();
//				break;
//			}
//		}
//	}
//
//	return 0;
//}