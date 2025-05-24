#pragma once

#ifndef INCLUDE_IMAGECTRL
#define INCLUDE_IMAGECTRL __declspec(dllimport)
#endif //INCLUDE_IMAGECTRL

// ImageCtrl

class INCLUDE_IMAGECTRL ImageCtrl : public CStatic
{
	DECLARE_DYNAMIC(ImageCtrl)

public:
	ImageCtrl();
	virtual ~ImageCtrl();

	void Create(long image_width, long image_height, BOOL color_image);
	void SetImage(HINSTANCE hinstance, LPCWSTR name, UINT type, int bitmap_width, int bitmap_height);
	void SetImage(cv::Mat& buffer);
	void Destroy();
	
protected:
	virtual void PreSubclassWindow();

private:
	CRect client_rect_;
	int	img_width_;
	int	img_height_;
	int wnd_width_;
	int wnd_height_;
	HBITMAP hbitmap_;

	CDC	main_dc_;
	CBitmap main_bitmap_;
	CBitmap* old_main_bitmap_;
	BITMAPINFO* bitmap_info_;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
};