#pragma once

#ifndef INCLUDE_GRIDCTRL
#define INCLUDE_GRIDCTRL __declspec(dllimport)
#endif //INCLUDE_GRIDCTRL

#define WM_GRIDCTRL_SELCHANGE		WM_USER + 402
#define WM_GRIDCTRL_BUTTON_CLICKED	WM_USER + 430

// GridCtrl

class INCLUDE_GRIDCTRL GridCtrl : public CStatic
{
	DECLARE_DYNAMIC(GridCtrl)

public:
	enum SET_AS { LABEL, BUTTON };

	struct GridCell {
		SET_AS set_as;
		HICON icon;
		CString text;
		int row_no;
		int col_no;
		int row_count;
		int col_count;
		CRect rect;
		COLORREF back_color;
		COLORREF border_color;
		COLORREF text_color;
		DWORD format;
		BOOL heading;
		BOOL joint;
		BOOL selected;
	};

	GridCtrl();
	virtual ~GridCtrl();

	void Create(std::vector<double>& col_width, int rows, int spacing, BOOL border, BOOL allow_scrolling = FALSE);
	void Create(int cols, int rows, int spacing, BOOL dynamic_font, BOOL allow_scrolling = FALSE);
	void AllowSelection(BOOL allow) { allow_selection_ = allow; }
	void SetRowHeight(int row_height) { row_height_ = row_height; }
	void Destroy();
	void AddCell(CString text, int row_no, int col_no, int row_count, int col_count, COLORREF back_color, COLORREF border_color, COLORREF text_color, DWORD format, BOOL heading, BOOL joint = FALSE);
	void SetCellAs(SET_AS set_as, int row_no, int col_no);
	void SetCellIcon(HICON icon, int row_no, int col_no);
	void SetCellText(CString text, int row_no, int col_no);
	void SetCellBackColor(COLORREF backcolor, int row_no, int col_no);
	void SetCellBorderColor(COLORREF bordercolor, int row_no, int col_no);
	void SetCellTextColor(COLORREF textcolor, int row_no, int col_no);
	SET_AS GetCellAs(int row_no, int col_no);
	CString GetCellText(int row_no, int col_no);
	COLORREF GetCellBackColor(int row_no, int col_no);
	COLORREF GetCellBorderColor(int row_no, int col_no);
	COLORREF GetCellTextColor(int row_no, int col_no);
	CArray<GridCell*>& cells() { return cells_; };
	void SetSelectionIndex(int index) { selected_index_ = index; }
	void SelectRow(int row_no, COLORREF selected_row_border_color = BACKCOLOR1_SEL);

protected:
	virtual void PreSubclassWindow();

private:
	CFont font_;

	CRect client_rect_;
	int selected_index_;
	int row_height_;
	BOOL border_;
	CDWordArray col_width_;
	CDWordArray col_start_;
	CArray<GridCell*> cells_;
	int select_row_no_;
	COLORREF selected_row_border_color_;
	BOOL button_clicked_;

	BOOL allow_selection_;

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};


