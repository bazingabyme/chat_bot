#pragma once

#ifndef INCLUDE_QUEUECTRL
#define INCLUDE_QUEUECTRL __declspec(dllimport)
#endif //INCLUDE_QUEUECTRL

#define WM_QUEUECTRL_ADDITEM		WM_USER + 409

// QueueCtrl

class INCLUDE_QUEUECTRL QueueCtrl : public CStatic
{
	DECLARE_DYNAMIC(QueueCtrl)

public:
	QueueCtrl();
	virtual ~QueueCtrl();

	void Create(CString title, int count);
	void Update();
	void Destroy();
	void Reset();
	void SetAsTinyPage(BOOL set_as_tiny_page) { set_as_tiny_page_ = set_as_tiny_page; }
	LRESULT Add(WPARAM wparam, LPARAM lparam);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CRITICAL_SECTION cs_;

	CDC cdc_;
	CBitmap bitmap_;
	BOOL status_changed_;
	Gdiplus::Point points_for_in_[3];
	Gdiplus::Point points_for_out_[3];

	CDWordArray queue_array_;
	CRect client_rect_;
	CRect title_rect_;
	CRect queue_rect_;
	CRect border_rect_;
	CRect cell_rect_;
	CString text_;
	BOOL push_from_left_;
	int cols_;
	int rows_;
	int count_;

	BOOL set_as_tiny_page_;
	
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


