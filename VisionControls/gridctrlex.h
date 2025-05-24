#pragma once

#ifndef INCLUDE_GRIDCTRLEX
#define INCLUDE_GRIDCTRLEX __declspec(dllimport)
#endif //INCLUDE_GRIDCTRLEX

#include "button.h"
#include "edit.h"
#include "dropdownlist.h"
#include "dropdownlistex.h"

#define WM_GRIDCTRLEX_SELCHANGE					WM_USER + 402
#define WM_GRIDCTRLEX_BUTTON_CLICKED			WM_USER + 430
#define WM_GRIDCTRLEX_DROPDOWNLIST_SELCHANGE	WM_USER + 435
#define WM_GRIDCTRLEX_ONOFFCTRL_CHECKONOFF		WM_USER + 436
#define WM_GRIDCTRLEX_EDIT_CHARCHANGE			WM_USER + 437
#define WM_GRIDCTRLEX_EDIT_ENTERPRESSED			WM_USER + 438
#define WM_GRIDCTRLEX_SPINCTRL_SELCHANGE		WM_USER + 439
#define WM_GRIDCTRLEX_CREATED					WM_USER + 440
#define WM_GRIDCTRLEX_DROPDOWNLISTEX_SELCHANGE	WM_USER + 450

// GridCtrlEx

class INCLUDE_GRIDCTRLEX GridCtrlEx : public CStatic
{
	DECLARE_DYNAMIC(GridCtrlEx)

public:
	enum CELL_TYPE { CT_LABEL, CT_TEXT, CT_EDIT, CT_ONOFF, CT_DROPDOWN, CT_DROPDOWNEX, CT_SPIN, CT_BUTTON };
	enum CELL_BUTTON_TYPE { CT_BUTTON_REGULAR, CT_BUTTON_CHECK_BOX, CT_BUTTON_CHECK_BOX_PUSH_LIKE, CT_BUTTON_RADIO_BUTTON, CT_BUTTON_RADIO_BUTTON_PUSH_LIKE };

	enum ICON_ALIGNMENT {
		ST_ALIGN_HORIZ = 0,			// Icon/bitmap on the left, text on the right
		ST_ALIGN_HORIZ_RIGHT,		// Icon/bitmap on the right, text on the left
		ST_ALIGN_VERT,				// Icon/bitmap on the top, text on the bottom (both are center aligned)
		ST_ALIGN_CENTER,			// Icon/bitmap on the top in the center, text on the bottom
		ST_ALIGN_OVERLAP			// Icon/bitmap on the same space as text
	};

	struct ICON_INFO {
		HICON hIcon;		// Handle to icon
		DWORD dwWidth;		// Width of icon
		DWORD dwHeight;		// Height of icon
	};

	class GridHeadingCell {
	public:
		GridHeadingCell::GridHeadingCell(GridCtrlEx* gridctrlex) {

			gridctrlex_ = gridctrlex;
			font_ = gridctrlex_->GetFont();

			text_.Empty();
			data1_.Empty();
			data2_.Empty();
			data3_.Empty();
			back_color_ = WHITE_COLOR;
			border_color_ = BLACK_COLOR;
			text_color_ = BLACK_COLOR;
			disable_ = FALSE;
		}

		CFont* GetFont() { return font_; }
		void SetFont(CFont* font) { font_ = font; }
		CString GetText() { return text_; }
		void SetText(CString text) { text_ = text; }
		void SetData(int data1, int data2 = 0, int data3 = 0) {
			data1_.Format(L"%d", data1);
			data2_.Format(L"%d", data2);
			data3_.Format(L"%d", data3);
		}
		void SetData(CString data1, CString data2 = L"", CString data3 = L"") {
			data1_ = data1;
			data2_ = data2;
			data3_ = data3;
		}
		CString GetData1() { return data1_; }
		CString GetData2() { return data2_; }
		CString GetData3() { return data3_; }
		void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
		void SetBorderColor(COLORREF border_color) { border_color_ = border_color; }
		void SetTextColor(COLORREF text_color) { text_color_ = text_color; }

		GridCtrlEx* gridctrlex_;
		CFont* font_;
		CString text_;
		CString data1_;
		CString data2_;
		CString data3_;
		DWORD align_;
		COLORREF back_color_;
		COLORREF border_color_;
		COLORREF text_color_;
		BOOL disable_;
		CRect rect_;
	};

	class GridColumnCell {
	public:
		GridColumnCell::GridColumnCell(GridCtrlEx* gridctrlex) {
			
			gridctrlex_ = gridctrlex;
			font_ = gridctrlex_->GetFont();

			text_.Empty();
			data1_.Empty();
			data2_.Empty();
			data3_.Empty();
			col_no_ = 0;
			col_width_ = 0;
			back_color_ = WHITE_COLOR;
			border_color_ = BLACK_COLOR;
			text_color_ = BLACK_COLOR;
			disable_ = FALSE;
		}

		CFont* GetFont() { return font_; }
		void SetFont(CFont* font) { font_ = font; }
		CString GetText() { return text_; }
		void SetText(CString text);
		void SetData(int data1, int data2 = 0, int data3 = 0) {
			data1_.Format(L"%d", data1);
			data2_.Format(L"%d", data2);
			data3_.Format(L"%d", data3);
		}
		void SetData(CString data1, CString data2 = L"", CString data3 = L"") {
			data1_ = data1;
			data2_ = data2;
			data3_ = data3;
		}
		CString GetData1() { return data1_; }
		CString GetData2() { return data2_; }
		CString GetData3() { return data3_; }
		void SetBackColor(COLORREF back_color);
		void SetBorderColor(COLORREF border_color);
		void SetTextColor(COLORREF text_color);

		GridCtrlEx* gridctrlex_;
		CFont* font_;
		int col_no_;
		int col_width_;
		CString text_;
		CString data1_;
		CString data2_;
		CString data3_;
		DWORD align_;
		COLORREF back_color_;
		COLORREF border_color_;
		COLORREF text_color_;
		BOOL disable_;
		CRect rect_;
	};

	class INCLUDE_GRIDCTRLEX GridCell {
	public:
		GridCell::GridCell() {}
		GridCell::GridCell(GridCtrlEx* gridctrlex) {
		
			gridctrlex_ = gridctrlex;

			font_ = gridctrlex_->GetFont();
			index_ = int(gridctrlex_->GetGridCells().GetSize());

			data1_.Empty();
			data2_.Empty();
			data3_.Empty();

			associated_parent_cell_ = NULL;
			associated_cells_.RemoveAll();
			back_color_ = BACKCOLOR1;
			border_color_ = BACKCOLOR1;
			text_color_ = WHITE_COLOR;
			sel_back_color_ = BACKCOLOR1_SEL;
			sel_border_color_ = BACKCOLOR1_SEL;
			sel_text_color_ = WHITE_COLOR;
			disable_back_color_ = DISABLE_COLOR;
			disable_border_color_ = DISABLE_COLOR;
			disable_text_color_ = WHITE_COLOR;
			joint_ = FALSE;
			disable_row_ = FALSE;
			disable_ = FALSE;
			visible_ = TRUE;
			value_changed_ = FALSE;
		}

		GridCtrlEx* GetGridCtrlEx() { return gridctrlex_; }
		CELL_TYPE GetCellType() { return cell_type_; }
		int GetIndex() { return index_; }
		int GetRowNo() { return row_no_; }
		int GetColNo() { return col_no_; }
		int GetRowCount() { return row_count_; }
		int GetColCount() { return col_count_; }
		void SetData(int data1, int data2 = 0, int data3 = 0) { 
			data1_.Format(L"%d", data1);
			data2_.Format(L"%d", data2);
			data3_.Format(L"%d", data3);
		}
		void SetData(CString data1, CString data2 = L"", CString data3 = L"") { 
			data1_ = data1; 
			data2_ = data2;
			data3_ = data3;
		}
		CString GetData1() { return data1_; }
		CString GetData2() { return data2_; }
		CString GetData3() { return data3_; }
		DWORD GetAlign() { return align_; }
		COLORREF& GetBackColor() { return back_color_; }
		COLORREF& GetBorderColor() { return border_color_; }
		COLORREF& GetTextColor() { return text_color_; }
		void SetBackColor(COLORREF back_color) { back_color_ = back_color; }
		void SetBorderColor(COLORREF border_color) { border_color_ = border_color; }
		void SetTextColor(COLORREF text_color) { text_color_ = text_color; }
		void SetSelBackColor(COLORREF back_color) { sel_back_color_ = back_color; }
		void SetSelBorderColor(COLORREF border_color) { sel_border_color_ = border_color; }
		void SetSelTextColor(COLORREF text_color) { sel_text_color_ = text_color; }
		void SetDisableBackColor(COLORREF back_color) { disable_back_color_ = back_color; }
		void SetDisableBorderColor(COLORREF border_color) { disable_border_color_ = border_color; }
		void SetDisableTextColor(COLORREF text_color) { disable_text_color_ = text_color; }
		CFont* GetFont() { return font_; }
		void SetFont(CFont* font) { font_ = font; }
		void DisableRow(BOOL disable);
		void DisableCell(BOOL disable);
		void VisibleCell(BOOL visible);
		BOOL IsJointed() { return joint_; }
		BOOL IsDisabled() { return (disable_ || disable_row_); }
		BOOL IsVisibled() { return visible_; }
		void SetRect(CRect rect) { rect_ = rect; }
		CRect& GetRect() { return rect_; }

		GridCell* GetAssociatedParentCell();
		void SetAssociatedParentCell(GridCell* cell);
		GridCell* GetAssociatedCell(int index);
		void SetAssociatedCell(GridCell* cell);
		int AssociatedChildCellCount() { return (int)associated_cells_.GetSize(); }
		void ValueChanged(BOOL change);
		BOOL IsValueChanged() { return value_changed_; }

		virtual void PrepareCell() = 0;
		virtual void DrawCell(COfflineDC& offdc) = 0;
		virtual void LButtonDown(UINT nFlags, CPoint point) = 0;
		virtual void MouseMove(UINT nFlags, CPoint point) = 0;
		virtual void LButtonUp(UINT nFlags, CPoint point) = 0;
		virtual void OnTimer(UINT_PTR nIDEvent) = 0;
		virtual void Destroy() = 0;

	protected:
		GridCtrlEx* gridctrlex_;
		CArray<GridCell*> associated_cells_;
		GridCell* associated_parent_cell_;
		CFont* font_;
		CELL_TYPE cell_type_;
		int index_;
		int row_no_;
		int col_no_;
		int row_count_;
		int col_count_;
		CString data1_;
		CString data2_;
		CString data3_;
		DWORD align_;
		COLORREF back_color_;
		COLORREF border_color_;
		COLORREF text_color_;
		COLORREF sel_back_color_;
		COLORREF sel_border_color_;
		COLORREF sel_text_color_;
		COLORREF disable_back_color_;
		COLORREF disable_border_color_;
		COLORREF disable_text_color_;
		BOOL joint_;
		BOOL disable_row_;
		BOOL disable_;
		BOOL visible_;
		BOOL value_changed_;
		CRect rect_;
		LPARAM param_;
	};

	class INCLUDE_GRIDCTRLEX GridCellLabel : public GridCell {
	public:
		GridCellLabel::GridCellLabel(GridCtrlEx* gridctrlex, int row_no, CString text, DWORD align, BOOL allow_selection):GridCtrlEx::GridCell(gridctrlex) {
			
			row_no_ = row_no;
			text_ = text;
			align_ = align;
			allow_selection_ = allow_selection;

			cell_type_ = CELL_TYPE::CT_LABEL;
			back_color_ = LABEL_COLOR;
			border_color_ = LABEL_COLOR;
			text_color_ = WHITE_COLOR;
			col_no_ = 0;
			row_count_ = 0;
			col_count_ = 0;
		}

		CString GetText() { return text_; }
		void SetText(CString text);
		void SetBackColor(COLORREF back_color);
		void SetBorderColor(COLORREF border_color);
		void SetTextColor(COLORREF text_color);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		CString text_;
		BOOL allow_selection_;
	};
	
	class INCLUDE_GRIDCTRLEX GridCellText : public GridCell {
	public:
		GridCellText::GridCellText(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL multiline, BOOL allow_selection, BOOL joint):GridCtrlEx::GridCell(gridctrlex) {
			
			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			text_ = text;
			align_ = align;
			multiline_ = multiline;
			allow_selection_ = allow_selection;
			joint_ = joint;

			cell_type_ = CELL_TYPE::CT_TEXT;
			back_color_ = WHITE_COLOR;
			border_color_ = BACKCOLOR1;
			text_color_ = BLACK_COLOR;
			sel_back_color_ = WHITE_COLOR;
			sel_border_color_ = BLACK_COLOR;
			sel_text_color_ = BLACK_COLOR;
			disable_back_color_ = WHITE_COLOR;
			disable_border_color_ = DISABLE_COLOR;
			disable_text_color_ = DISABLE_COLOR;

			show_balloon_ = FALSE;
			balloon_rect_.SetRectEmpty();
			balloon_data_.Empty();
		}

		CString GetText() { return text_; }
		void SetText(CString text);
		void SetBackColor(COLORREF back_color, BOOL update = FALSE);
		void SetBorderColor(COLORREF border_color, BOOL update = FALSE);
		void SetTextColor(COLORREF text_color, BOOL update = FALSE);
		COLORREF GetBackColor() { return back_color_; }
		void SetSelBackColor(COLORREF sel_back_color = FALSE);
		void SetSelBorderColor(COLORREF sel_border_color = FALSE);
		void SetSelTextColor(COLORREF sel_text_color = FALSE);
		void SetBallonData(int data, COLORREF color);
		void SetBallonData(CString data, COLORREF color);
		void ShowBalloon(BOOL show);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		CString text_;
		BOOL multiline_;
		BOOL allow_selection_;

		BOOL show_balloon_;
		CString balloon_data_;
		CRect balloon_rect_;
		COLORREF balloon_color_;
	};

	class INCLUDE_GRIDCTRLEX GridCellEdit : public GridCell {
	public:
		GridCellEdit::GridCellEdit(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL joint):GridCtrlEx::GridCell(gridctrlex) {
		
			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			text_ = text;
			prev_text_ = text;
			align_ = align;
			joint_ = joint;

			back_color_ = WHITE_COLOR;
			border_color_ = BACKCOLOR1;
			text_color_ = BLACK_COLOR;
			sel_back_color_ = WHITE_COLOR;
			sel_border_color_ = BLACK_COLOR;
			sel_text_color_ = BLACK_COLOR;
			disable_back_color_ = WHITE_COLOR;
			disable_border_color_ = DISABLE_COLOR;
			disable_text_color_ = DISABLE_COLOR;

			cell_type_ = CELL_TYPE::CT_EDIT;
			text_limit_ = -1;
			keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_ALPHANUMERIC;
			keyboard_captionbar_text_ = _T("");
			multilingual_ = FALSE;
			holder_ = L"\\'";
			read_only_ = FALSE;
			use_floating_ = FALSE;
		}

		CString GetText();
		int GetValue();
		CString GetPrevText();
		int GetPrevValue();
		void SetText(CString text, BOOL highlight = FALSE);
		void SetValue(int value, BOOL highlight = FALSE);
		void SetKeyBoardType(KeyboardDlg::KEYBOARD_TYPE keyboard_type);
		void SetKeyBoardCaptionBarText(CString keyboard_captionbar_text);
		void ShowMultilingualKeyboard(BOOL show);
		void SetTextLimit(int limit);
		void SetHolder(CString holder);
		void SetReadOnly(BOOL readonly);
		void UseFloating(BOOL floating);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		CRect edit_rect_;
		CPoint down_point_;
		CString text_;
		CString prev_text_;
		CString holder_;
		int text_limit_;
		KeyboardDlg::KEYBOARD_TYPE keyboard_type_;
		CString keyboard_captionbar_text_;
		BOOL multilingual_;
		BOOL read_only_;
		BOOL use_floating_;
	};

	class INCLUDE_GRIDCTRLEX GridCellSpin : public GridCell {
	public:
		GridCellSpin::GridCellSpin(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, BOOL joint):GridCtrlEx::GridCell(gridctrlex) {

			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			joint_ = joint;

			cell_type_ = CELL_TYPE::CT_SPIN;
			keyboard_type_ = KeyboardDlg::KEYBOARD_TYPE::KB_NUMERIC;
			keyboard_captionbar_text_ = _T("");
			read_only_ = FALSE;

			use_floating_ = FALSE;
			double_precesion_ = FALSE;
			spin_index_ = -1;
		}

		void SetKeyBoardCaptionBarText(CString keyboard_captionbar_text);
		void SetReadOnly(BOOL readonly);
		void SetRange(int min, int max, int freq);
		void SetPos(int pos, BOOL highlight = FALSE);
		int GetPos();
		int GetPrevPos();
		int GetMin() { return min_pos_; }
		int GetMax() { return max_pos_; }
		void SetRangeF(double min, double max, double freq, BOOL double_precision = FALSE);
		void SetPosF(double pos, BOOL highlight = FALSE);
		double GetPosF();
		double GetPrevPosF();
		double GetMinF() { return f_min_pos_; }
		double GetMaxF() { return f_max_pos_; }
		BOOL IsFloating() { return use_floating_; }

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent);
		void Destroy();
		void EditEnterPressedMessage();
		void SetPosition(int pos);
		void SetPositionF(double pos);

	private:
		CRect edit_rect_;
		CRect spin_rect_[2];
		CRect left_text_rect_;
		CRect right_text_rect_;
		CPoint down_point_;
		CString left_text_;
		CString right_text_;
		BOOL use_floating_;
		BOOL double_precesion_;
		BOOL button_down_[2];
		int timer_executed_;
		int min_pos_;
		int max_pos_;
		int cur_pos_;
		int prev_pos_;
		int steps_[2];
		int spin_index_;
		double f_min_pos_;
		double f_max_pos_;
		double f_cur_pos_;
		double f_prev_pos_;
		double f_steps_[2];
		CString text_;
		CString keyboard_captionbar_text_;
		BOOL read_only_;
		KeyboardDlg::KEYBOARD_TYPE keyboard_type_;
	};

	class INCLUDE_GRIDCTRLEX GridCellOnOff : public GridCell {
	public:
		GridCellOnOff::GridCellOnOff(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, CString on_text, CString off_text, BOOL joint):GridCtrlEx::GridCell(gridctrlex) {

			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			on_text_ = on_text;
			off_text_ = off_text;
			joint_ = joint;
			checked_ = FALSE;
			prev_checked_ = FALSE;

			cell_type_ = CELL_TYPE::CT_ONOFF;
		}

		void SetText(CString on_text, CString off_text);
		CString GetOnText() { return on_text_; }
		CString GetOffText() { return off_text_; }
		BOOL GetCheck();
		BOOL GetPrevCheck();
		void SetCheck(BOOL check, BOOL update = TRUE, BOOL highlight = FALSE);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		CRect rect1_;
		CRect rect2_;
		CString on_text_;
		CString off_text_;
		CPoint down_point_;
		BOOL checked_;
		BOOL prev_checked_;
	};

	class INCLUDE_GRIDCTRLEX GridCellButton : public GridCell {
	public:
		GridCellButton::GridCellButton(GridCtrlEx* gridctrlex, CELL_BUTTON_TYPE cell_button_type, int row_no, int col_no, int row_count, int col_count, CString on_text, CString off_text, DWORD align, BOOL joint) :GridCtrlEx::GridCell(gridctrlex) {

			cell_button_type_ = cell_button_type;
			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			on_text_ = on_text;
			off_text_ = off_text.IsEmpty() ? on_text : off_text;
			align_ = align;
			joint_ = joint;
			button_pressed_ = FALSE;
			prev_button_pressed_ = FALSE;

			cell_type_ = CELL_TYPE::CT_BUTTON;
	
			image_org_pt_.x = 5;
			image_org_pt_.y = 5;
			icon_alignment_ = ICON_ALIGNMENT::ST_ALIGN_OVERLAP;
			icon_info_[0].hIcon = NULL;
			icon_info_[1].hIcon = NULL;

			box_color_ = disable_box_color_ = RGB(1, 1, 1);
		}
		
		CELL_BUTTON_TYPE GetCellButtonType() { return cell_button_type_; }
		void SetBackColor(COLORREF back_color, BOOL update = FALSE);
		void SetBorderColor(COLORREF border_color, BOOL update = FALSE);
		void SetTextColor(COLORREF text_color, BOOL update = FALSE);
		void SetSelBackColor(COLORREF sel_back_color, BOOL update = FALSE);
		void SetSelBorderColor(COLORREF sel_border_color, BOOL update = FALSE);
		void SetSelTextColor(COLORREF sel_text_color, BOOL update = FALSE);
		void SetDisableBackColor(COLORREF disable_back_color, BOOL update = FALSE);
		void SetDisableBorderColor(COLORREF disable_border_color, BOOL update = FALSE);
		void SetDisableTextColor(COLORREF disable_text_color, BOOL update = FALSE);
		void SetBoxColor(COLORREF box_color, BOOL update = FALSE);
		void SetDisableBoxColor(COLORREF disable_box_color, BOOL update = FALSE);
		void SetText(CString text);
		void SetText(CString on_text, CString off_text);
		CString GetText() { return on_text_; }
		CString GetOnText() { return on_text_; }
		CString GetOffText() { return off_text_; }
		void SetIcon(HICON icon, ICON_ALIGNMENT alignment);
		void SetIcon(HICON on_icon, HICON off_icon, ICON_ALIGNMENT alignment);
		void SetCheck(BOOL check, BOOL highlight = FALSE);
		BOOL GetCheck() { return button_pressed_; }
		BOOL GetPrevCheck() { return prev_button_pressed_; }

	protected:
		virtual void DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		CELL_BUTTON_TYPE cell_button_type_;
		CString on_text_;
		CString off_text_;
		BOOL button_pressed_;
		BOOL prev_button_pressed_;
		POINT image_org_pt_;
		ICON_ALIGNMENT icon_alignment_;
		ICON_INFO icon_info_[2];
		COLORREF box_color_;
		COLORREF disable_box_color_;
	};

	class INCLUDE_GRIDCTRLEX GridCellDropDown : public GridCell {
	public:
		GridCellDropDown::GridCellDropDown(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, DWORD align, BOOL joint):GridCtrlEx::GridCell(gridctrlex) {

			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			align_ = align;
			joint_ = joint;

			cell_type_ = CELL_TYPE::CT_DROPDOWN;
			dropdown_text_.RemoveAll();
			items_to_display_ = 5;
		}
		
		void SetItemsToDisplay(int items_to_display);
		void SetText(int index, CString text);
		CString GetText(int index);
		void AddItem(CString str);
		void DeleteItem(int index);
		void ResetContents();
		void SetCurSel(int index, BOOL highlight = FALSE);
		int GetCurSel();
		int GetPrevSel();
		CString GetCurSelText();
		int FindString(CString str);
		int GetItemCount();
		void SetBackColor(COLORREF back_color);
		void SetBorderColor(COLORREF border_color);
		void SetTextColor(COLORREF text_color);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		DDList ctrDDList_;
		CStringArray dropdown_text_;
		CRect list_window_rect_;
		int items_to_display_;
	};

	class INCLUDE_GRIDCTRLEX GridCellDropDownEx : public GridCell {
	public:
		GridCellDropDownEx::GridCellDropDownEx(GridCtrlEx* gridctrlex, int row_no, int col_no, int row_count, int col_count, CString text, DWORD align, BOOL joint) :GridCtrlEx::GridCell(gridctrlex) {

			row_no_ = row_no;
			col_no_ = col_no;
			row_count_ = row_count;
			col_count_ = col_count;
			text_ = text;
			align_ = align;
			joint_ = joint;

			cell_type_ = CELL_TYPE::CT_DROPDOWNEX;
			items_list_.RemoveAll();

			selected_index_ = prev_selected_index_ = -1;
			down_point_.SetPoint(-1, -1);
		}

		void SetText(int index, CString text);
		CString GetText(int index);
		void AddItem(CString str);
		void DeleteItem(int index);
		void ResetContents();
		void SetCurSel(int index, BOOL highlight = FALSE);
		int GetCurSel();
		int GetPrevSel();
		CString GetCurSelText();
		int FindString(CString str);
		int GetItemCount();
		void SetBackColor(COLORREF back_color);
		void SetBorderColor(COLORREF border_color);
		void SetTextColor(COLORREF text_color);
		void SelChangeMessage(WPARAM wparam, LPARAM lparam);

	protected:
		void PrepareCell();
		void DrawCell(COfflineDC& offdc);
		void LButtonDown(UINT nFlags, CPoint point);
		void MouseMove(UINT nFlags, CPoint point);
		void LButtonUp(UINT nFlags, CPoint point);
		void OnTimer(UINT_PTR nIDEvent) {}
		void Destroy();

	private:
		DDListEx ddlistex_;
		
		CStringArray items_list_;
		CPoint down_point_;
		CString text_;
		int selected_index_;
		int prev_selected_index_;
	};

	GridCtrlEx();
	virtual ~GridCtrlEx();

	void Create(int spacing, BOOL columns_in_percentage = TRUE);
	void Destroy();
	CRect CalcGridDocumentWH(int spacing, BOOL columns_in_percentage = TRUE);
	int GetGridDocumentHeight() { return document_height_; }
	int GetGridDocumentWidth() { return document_width_; }
	void DisableScrollbar(UINT sbar, BOOL disable);
	GridCtrlEx::GridHeadingCell* InsertHeading(CString text, DWORD align);
	GridCtrlEx::GridColumnCell* InsertColumn(int col_no, int col_width, CString text, DWORD align);
	GridCtrlEx::GridCellLabel* AddCellLabel(int row_no, CString text, DWORD align = DT_LEFT, BOOL allow_selection = FALSE);
	GridCtrlEx::GridCellText* AddCellText(int row_no, int col_no, int row_count = 1, int col_count = 1, CString text = L"", DWORD align = DT_LEFT, BOOL multiline = FALSE, BOOL allow_selection = FALSE, BOOL joint = FALSE);
	GridCtrlEx::GridCellEdit* AddCellEdit(int row_no, int col_no, int row_count = 1, int col_count = 1, CString text = L"", DWORD align = DT_RIGHT, BOOL joint = FALSE);
	GridCtrlEx::GridCellSpin* AddCellSpin(int row_no, int col_no, int row_count = 1, int col_count = 1, BOOL joint = FALSE);
	GridCtrlEx::GridCellOnOff* AddCellOnOff(int row_no, int col_no, int row_count = 1, int col_count = 1, CString on_text = L"On", CString off_text = L"Off", BOOL joint = FALSE);
	GridCtrlEx::GridCellButton* AddCellButton(int row_no, int col_no, int row_count = 1, int col_count = 1, CString text = L"", DWORD align = DT_CENTER, BOOL joint = FALSE);
	GridCtrlEx::GridCellButton* AddCellButton(CELL_BUTTON_TYPE cell_button_type, int row_no, int col_no, int row_count = 1, int col_count = 1, CString on_text = L"On", CString off_text = L"Off", DWORD align = DT_CENTER, BOOL joint = FALSE);
	GridCtrlEx::GridCellDropDown* AddCellDropDown(int row_no, int col_no, int row_count = 1, int col_count = 1, DWORD align = DT_RIGHT, BOOL joint = FALSE);
	GridCtrlEx::GridCellDropDownEx* AddCellDropDownEx(int row_no, int col_no, int row_count = 1, int col_count = 1, CString text = L"", DWORD align = DT_LEFT, BOOL joint = FALSE);
	BOOL RemoveCell(int row_no, int col_no);

	CArray<GridCell*>& GetGridCells() { return cells_; };
	int GetTotalRows();
	int GetTotalCols() { return (int)column_cells_.GetSize(); }
	GridColumnCell* GetGridColumnCell(int col_no);
	GridCtrlEx::CELL_TYPE GetCellType(int row_no, int col_no);
	GridCellLabel* GetGridCellLabel(int row_no, int col_no);
	GridCellText* GetGridCellText(int row_no, int col_no);
	GridCellEdit* GetGridCellEdit(int row_no, int col_no);
	GridCellSpin* GetGridCellSpin(int row_no, int col_no);
	GridCellOnOff* GetGridCellOnOff(int row_no, int col_no);
	GridCellDropDown* GetGridCellDropDown(int row_no, int col_no);
	GridCellDropDownEx* GetGridCellDropDownEx(int row_no, int col_no);
	GridCellButton* GetGridCellButton(int row_no, int col_no);

	void SetBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color);
	void SetBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color);
	void SetTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color);
	void SetSelBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color);
	void SetSelBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color);
	void SetSelTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color);
	void SetDisableBackColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF back_color);
	void SetDisableBorderColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF border_color);
	void SetDisableTextColor(GridCtrlEx::CELL_TYPE cell_type, COLORREF text_color);
	void SetBackColor(COLORREF back_color);
	void SetBorderColor(COLORREF border_color);
	void SetTextColor(COLORREF text_color);
	COLORREF GetBackColor() { return back_color_; }
	void SetSelectionIndex(int index) { selected_index_ = index; }
	int GetSelectedIndex() { return selected_index_; }
	void SetHeadingRowHeight(int height) { heading_row_height_ = height; }
	int GetHeadingRowHeight() { return heading_row_height_; }
	void SetColumnRowHeight(int height) { column_row_height_ = height; }
	int GetColumnRowHeight() { return column_row_height_; }
	void SetLabelRowHeight(int height) { label_row_height_ = height; }
	int GetLabelRowHeight() { return label_row_height_; }
	void SetRowHeight(int height) { row_height_ = height; }
	int GetRowHeight() { return row_height_; }
	void SelectRow(int row_no, COLORREF selected_row_border_color = BACKCOLOR1_SEL);
	void DisableRow(int row_no, BOOL disable);
	void DisableAllRows(BOOL disable);
	void SetAssociatedButtons(CArray<Button*>& associated_buttons);
	void ResetValuesOfAllCells(BOOL with_prev_values);
	void RemoveAllRows();
	void RefreshGrid();

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void GridCellUpdated();
	void ResetScrollBar();

	CRect wnd_rect_;
	CRect client_rect_;
	int	document_height_;
	int	document_width_;
	int total_rows_;
	int selected_index_;
	int spacing_;
	BOOL columns_in_percentage_;
	BOOL border_;
	COLORREF back_color_;
	BOOL button_down_;

	int heading_row_height_;
	GridHeadingCell* heading_cell_;

	int column_row_height_;
	CArray<GridColumnCell*> column_cells_;

	int label_row_height_;
	int row_height_;
	CArray<GridCell*> cells_;

	int select_row_no_;
	COLORREF selected_row_border_color_;

	GridCell* gridcell_;
	GridCellDropDown* gridcelldropdown_;

	CArray<Button*> associated_buttons_;

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	LRESULT OnListCtrlExSelChangeMessage(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};