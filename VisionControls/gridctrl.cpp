// sliderctrl.cpp : implementation file
//

#include "stdafx.h"

#define INCLUDE_GRIDCTRL __declspec(dllexport)
#include "gridctrl.h"

// GridCtrl

IMPLEMENT_DYNAMIC(GridCtrl, CStatic)

GridCtrl::GridCtrl()
{
	allow_selection_ = FALSE;
	row_height_ = 0;
}

GridCtrl::~GridCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(GridCtrl, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// GridCtrl message handlers

void GridCtrl::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);

	CStatic::PreSubclassWindow();
}

void GridCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	
	COfflineDC offdc(&dc, &client_rect_);
	offdc.SetBkMode(TRANSPARENT);
	offdc.SetTextColor(WHITE_COLOR);

	//CRect rect;
	for (int i = 0; i < cells_.GetSize(); i++) {

		GridCell* cell = cells_.GetAt(i);

		CFont* old_font = NULL;
		if (cell->heading) {
			cell->rect.right = client_rect_.right;
			old_font = offdc.SelectObject(&Formation::font(Formation::MEDIUM_FONT));
		} else if (font_.GetSafeHandle() == NULL) {
			old_font = offdc.SelectObject(&Formation::font(Formation::SMALL_FONT));
		} else {
			old_font = offdc.SelectObject(&font_);
		}
		HPEN pen = CreatePen(PS_SOLID, 1, cell->border_color);
		HBRUSH brush = NULL;
		if (cell->set_as == SET_AS::BUTTON && button_clicked_ && selected_index_ == i) {
			brush = CreateSolidBrush(BACKCOLOR1_SEL);
		} else {
			brush = CreateSolidBrush(cell->back_color);
		}
		HPEN old_pen = (HPEN)offdc.SelectObject(pen);
		HBRUSH old_brush = (HBRUSH)offdc.SelectObject(brush);
		if (selected_index_ == i) {
			offdc.SelectObject(Formation::blackcolor_pen());
			//offdc.SelectObject(GetStockObject(NULL_BRUSH));
		}
		offdc.Rectangle(cell->rect);
		offdc.SelectObject(old_pen);
		offdc.SelectObject(old_brush);
		DeleteObject(pen);
		DeleteObject(brush);

		offdc.SetTextColor(cell->text_color);
		offdc.DrawText(Formation::PrepareString(offdc, cell->text, cell->rect), cell->rect, cell->format | DT_SINGLELINE | DT_VCENTER);

		offdc.SelectObject(old_font);

		if (cell->set_as == SET_AS::BUTTON && cell->icon != NULL) {
			ICONINFO iconinfo;
			::GetIconInfo(cell->icon, &iconinfo);
			DWORD dw_width = (DWORD)(iconinfo.xHotspot * 2);
			DWORD dw_height = (DWORD)(iconinfo.yHotspot * 2);
			::DeleteObject(iconinfo.hbmMask);
			::DeleteObject(iconinfo.hbmColor);
			CRect icon_rect(cell->rect);
			icon_rect.top = icon_rect.CenterPoint().y - (dw_height / 2);
			icon_rect.bottom = icon_rect.top + dw_height;
			icon_rect.left = icon_rect.CenterPoint().x - (dw_width / 2);
			icon_rect.right = icon_rect.left + dw_width;
			offdc.DrawState(icon_rect.TopLeft(), CSize(icon_rect.Width(), icon_rect.Height()), cell->icon, DSS_NORMAL, (CBrush*)NULL);
		}
	}

	if (select_row_no_ != -1) {
		CRect select_rect(0xFFFF, 0, 0, 0);
		for (int i = 0; i < cells_.GetSize(); i++) {
			GridCell* cell = cells_.GetAt(i);
			if (select_row_no_ == cell->row_no) {
				if (select_rect.left > cell->rect.left) {
					select_rect.left = cell->rect.left;
				}
				if (select_rect.right < cell->rect.right) {
					select_rect.right = cell->rect.right;
				}
				select_rect.top = cell->rect.top;
				select_rect.bottom = cell->rect.bottom;
			}
		}
		if (select_rect.top != 0) {
			HPEN pen = CreatePen(PS_SOLID, 1, selected_row_border_color_);
			HPEN old_pen = (HPEN)offdc.SelectObject(pen);
			HBRUSH old_brush = (HBRUSH)offdc.SelectObject(GetStockObject(NULL_BRUSH));
			offdc.Rectangle(select_rect);
			offdc.SelectObject(old_pen);
			offdc.SelectObject(old_brush);
			DeleteObject(pen);
		}
	}

	if (border_) {
		offdc.SelectObject(GetStockObject(NULL_BRUSH));
		offdc.SelectObject(Formation::labelcolor_pen());
		offdc.Rectangle(client_rect_);
	}
}

void GridCtrl::Create(std::vector<double>& col_width, int rows, int spacing, BOOL border, BOOL allow_scrolling) {

	GetClientRect(client_rect_);

	if (rows <= 0 || col_width.size() <= 0) {
		return;
	}
	if (row_height_ == 0) row_height_ = Formation::heading_height();
	int row_height = (client_rect_.Height() - ((rows - 1) * spacing)) / rows;
	if (row_height > row_height_) {
		row_height = row_height_;
	}
	int heading_height = row_height;// *75 / 100;
	int width = (client_rect_.Width() - (((int)col_width.size() - 1) * spacing));
	col_width_.RemoveAll();
	col_start_.RemoveAll();
	int start = 0;
	for (int i = 0; i < col_width.size(); i++) {
		int temp_width = (int)((width * col_width[i]) / 100);
		col_width_.Add(temp_width);
		col_start_.Add(start);
		start += temp_width;
	}
	
	font_.DeleteObject();
	border_ = border;
	selected_index_ = -1;
	select_row_no_ = -1;
	button_clicked_ = FALSE;

	CRect rect;
	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		rect.top = cell->row_no * row_height + cell->row_no * spacing;
		if (border_ && cell->row_no == 0 && !cell->heading) {
			rect.top += spacing;
		}
		if (cell->heading) {
			rect.bottom = rect.top + cell->row_count * heading_height + (cell->row_count - 1) * spacing;
		} else {
			rect.bottom = rect.top + cell->row_count * row_height + (cell->row_count - 1) * spacing;
		}
		rect.left = col_start_.GetAt(cell->col_no) + cell->col_no * spacing;

		//if cols are merged
		int col_width = 0;
		int col_start = cell->col_no;
		for (int j = col_start; j < col_start + cell->col_count; j++) {
			col_width += col_width_.GetAt(j);
		}
		if (cell->joint) {
			rect.right = rect.left + col_width + (cell->col_count) * spacing;
		} else {
			rect.right = rect.left + col_width + (cell->col_count - 1) * spacing;
		}
		if (rect.right >= client_rect_.right) {
			rect.right = client_rect_.right - 1;
		}

		//if rows are merged
		int row_width = 0;
		int row_start = cell->row_no;
		for (int j = row_start; j < row_start + cell->row_count; j++) {
			row_width += row_height;
		}
		/*if (cell->joint) {
			rect.bottom = rect.top + row_width + (cell->row_count) * spacing;
		} else {*/
			rect.bottom = rect.top + row_width + (cell->row_count - 1) * spacing;
		//}
		if (rect.bottom >= client_rect_.bottom) {
			rect.bottom = client_rect_.bottom - 1;
		}

		cell->rect = rect;
	}
}

void GridCtrl::Create(int cols, int rows, int spacing, BOOL dynamic_font, BOOL allow_scrolling) {

	GetClientRect(client_rect_);

	if (rows > 0 && cols > 0) {
		
		if (row_height_ == 0) row_height_ = Formation::control_height();
		int row_height = (client_rect_.Height() - ((rows - 1) * spacing)) / rows;
		if (row_height > row_height_) {
			row_height = row_height_;
		}
		int heading_height = row_height * 75 / 100;
		int width = (client_rect_.Width() - ((cols - 1) * spacing)) / cols;
		if (width > row_height_) {
			width = row_height_;
		}
		col_width_.RemoveAll();
		col_start_.RemoveAll();
		int start = spacing;
		for (int i = 0; i < cols; i++) {
			int temp_width = width;
			col_width_.Add(temp_width);
			col_start_.Add(start);
			start += temp_width;
		}

		border_ = TRUE;
		selected_index_ = -1;
		select_row_no_ = -1;
		button_clicked_ = FALSE;

		CRect rect;
		for (int i = 0; i < cells_.GetSize(); i++) {
			GridCell* cell = cells_.GetAt(i);
			rect.top = cell->row_no * row_height + cell->row_no * spacing;
			if (border_ && cell->row_no == 0) {
				rect.top += spacing;
			}
			if (cell->heading) {
				rect.bottom = rect.top + cell->row_count * heading_height + (cell->row_count - 1) * spacing;
			} else {
				rect.bottom = rect.top + cell->row_count * row_height + (cell->row_count - 1) * spacing;
			}
			rect.left = col_start_.GetAt(cell->col_no) + cell->col_no * spacing;

			//if cols are merged
			int col_width = 0;
			int col_start = cell->col_no;
			for (int j = col_start; j < col_start + cell->col_count; j++) {
				col_width += col_width_.GetAt(j);
			}
			if (cell->joint) {
				rect.right = rect.left + col_width + (cell->col_count) * spacing;
			} else {
				rect.right = rect.left + col_width + (cell->col_count - 1) * spacing;
			}
			if (rect.right >= client_rect_.right) {
				rect.right = client_rect_.right - 1;
			}

			//if rows are merged
			int row_length = 0;
			int row_start = cell->row_no;
			for (int j = row_start; j < row_start + cell->row_count; j++) {
				row_length += row_height;
			}
			/*if (cell->joint) {
				rect.bottom = rect.top + row_width + (cell->row_count) * spacing;
			} else {*/
				if (cell->heading) {
					rect.bottom = rect.top + heading_height + (cell->row_count - 1) * spacing;
				} else {
					rect.bottom = rect.top + row_length + (cell->row_count - 1) * spacing;
				}
			//}
			if (rect.bottom >= client_rect_.bottom) {
				rect.bottom = client_rect_.bottom - 1;
			}

			cell->rect = rect;
		}

		if (dynamic_font) {
			for (int i = 0; i < cells_.GetSize(); i++) {
				GridCell* cell = cells_.GetAt(i);
				if (!cell->heading) {

					font_.DeleteObject();
					if (cell->rect.Width() >= 8 && cell->rect.Width() <= 18) {
						font_.CreateFontW(-cell->rect.Width(), 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial Unicode MS"));
					} else {
						font_.CreateFontW(-18, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial Unicode MS"));
					}
					//cell->text = "W";

					//CDC* dc = GetDC();

					//font_.DeleteObject();
					//font_.CreateFontW(-18, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial Unicode MS"));
					//CFont* old_font = dc->SelectObject(&font_);
					//int text_width = dc->GetTextExtent(cell->text).cx;
					//int rect_width = cell->rect.Width();
					//dc->SelectObject(old_font);
					//if (text_width > rect_width) {
					//	font_.DeleteObject();
					//	font_.CreateFontW(-rect_width, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial Unicode MS"));
					//	//CFont* old_font = dc->SelectObject(&font_);
					//	//int text_width = dc->GetTextExtent(cell->text).cx;
					//	//int rect_width = cell->rect.Width();
					//	//dc->SelectObject(old_font);
					//}

					//ReleaseDC(dc);
					break;
				}
			}
		}
	}

	Invalidate(FALSE);
}

void GridCtrl::Destroy() {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->icon != NULL) {
			DestroyIcon(cell->icon);
			cell->icon = NULL;
		}
		delete cell;
	}
	cells_.RemoveAll();
	col_start_.RemoveAll();
	col_width_.RemoveAll();

	font_.DeleteObject();
}

void GridCtrl::AddCell(CString text, int row_no, int col_no, int row_count, int col_count, COLORREF back_color, COLORREF border_color, COLORREF text_color, DWORD format, BOOL heading, BOOL joint) {

	GridCell* cell = new GridCell();

	cell->set_as = SET_AS::LABEL;
	cell->icon = NULL;

	if (format == DT_LEFT) {
		text = L" " + text;
	} else if (format == DT_RIGHT) {
		text = text + L" ";
	}
	cell->text = text;
	cell->row_no = row_no;
	cell->col_no = col_no;
	cell->row_count = row_count;
	cell->col_count = col_count;
	cell->back_color = back_color;
	cell->border_color = border_color;
	cell->text_color = text_color;
	cell->format = format;
	cell->heading = heading;
	cell->joint = joint;
	cell->selected = FALSE;

	cells_.Add(cell);
}

void GridCtrl::SetCellAs(SET_AS set_as, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			cell->set_as = set_as;
			break;
		}
	}
}

void GridCtrl::SetCellIcon(HICON icon, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			cell->icon = icon;
			break;
		}
	}
}

void GridCtrl::SetCellText(CString text, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			if (cell->format == DT_LEFT) {
				text = L" " + text;
			} else if (cell->format == DT_RIGHT) {
				text = text + L" ";
			}
			cell->text = text;

			break;
		}
	}
}

void GridCtrl::SetCellBackColor(COLORREF backcolor, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			cell->back_color = backcolor;
			break;
		}
	}
}

void GridCtrl::SetCellBorderColor(COLORREF backcolor, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			cell->border_color = backcolor;
			break;
		}
	}
}

void GridCtrl::SetCellTextColor(COLORREF backcolor, int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			cell->text_color = backcolor;
			break;
		}
	}
}

GridCtrl::SET_AS GridCtrl::GetCellAs(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			return cell->set_as;
		}
	}

	return SET_AS::LABEL;
}

CString GridCtrl::GetCellText(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			return cell->text;
		}
	}

	return _T("");
}

COLORREF GridCtrl::GetCellBackColor(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			return cell->back_color;
		}
	}

	return NULL;
}

COLORREF GridCtrl::GetCellBorderColor(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			return cell->border_color;
		}
	}

	return NULL;
}

COLORREF GridCtrl::GetCellTextColor(int row_no, int col_no) {

	for (int i = 0; i < cells_.GetSize(); i++) {
		GridCell* cell = cells_.GetAt(i);
		if (cell->row_no == row_no && cell->col_no == col_no) {
			return cell->text_color;
		}
	}

	return NULL;
}

void GridCtrl::SelectRow(int row_no, COLORREF selected_row_border_color) { 
	
	select_row_no_ = row_no; 
	selected_row_border_color_ = selected_row_border_color; 
}

void GridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

void GridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	GridCell* gridcell = NULL;
	for (int i = 0; i < (int)cells_.GetSize(); i++) {
		gridcell = (GridCell*)cells_.GetAt(i);
		if (!gridcell->heading && gridcell->rect.PtInRect(point)) {
			if (allow_selection_) {
				selected_index_ = i;
				InvalidateRect(client_rect_);
				GetParent()->SendMessageW(WM_GRIDCTRL_SELCHANGE, (WPARAM)this, (LPARAM)(selected_index_));
				break;
			} else if (gridcell->set_as == SET_AS::BUTTON) {
				selected_index_ = i;
				button_clicked_ = TRUE;
				InvalidateRect(client_rect_);
				GetParent()->SendMessageW(WM_GRIDCTRL_BUTTON_CLICKED, (WPARAM)this, (LPARAM)(selected_index_));
				button_clicked_ = FALSE;
				InvalidateRect(client_rect_);
				break;
			}
		}
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}
