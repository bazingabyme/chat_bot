#include "stdafx.h"

#define INCLUDE_IMAGECTRL __declspec(dllexport)
#include "imagectrl.h"

//ImageCtrl

IMPLEMENT_DYNAMIC(ImageCtrl, CStatic)

ImageCtrl::ImageCtrl()
{
	old_main_bitmap_ = NULL;
	bitmap_info_ = NULL;
}

ImageCtrl::~ImageCtrl()
{
	Destroy();
}

BEGIN_MESSAGE_MAP(ImageCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// ImageCtrl message handlers

void ImageCtrl::Destroy() {

	if (bitmap_info_ != NULL) {
		delete bitmap_info_;
		bitmap_info_ = NULL;
	}
	if (old_main_bitmap_ != NULL) {
		main_dc_.SelectObject(old_main_bitmap_);
		main_bitmap_.DeleteObject();
		old_main_bitmap_ = NULL;
	}
	main_dc_.DeleteDC();
}

void ImageCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}

void ImageCtrl::Create(long image_width, long image_height, BOOL color_image) {
	
	img_width_ = image_width;
	img_height_ = image_height;
	
	GetClientRect(client_rect_);
	CRect wnd_rect = Formation::GetRectInCenterAndLeftAlligned(client_rect_, CSize(img_width_, img_height_));
	wnd_width_ = wnd_rect.Width();
	wnd_height_ = wnd_rect.Height();

	CDC* dc = GetDC();
	/*if (old_main_bitmap_ != NULL) {
		main_dc_.SelectObject(old_main_bitmap_);
	}*/
	main_dc_.DeleteDC();
	main_dc_.CreateCompatibleDC(dc);
	main_dc_.SetStretchBltMode(COLORONCOLOR);
	main_bitmap_.DeleteObject();
	main_bitmap_.CreateCompatibleBitmap(dc, wnd_width_ - 2, wnd_height_ - 2);
	old_main_bitmap_ = main_dc_.SelectObject(&main_bitmap_);
	main_dc_.BitBlt(0, 0, wnd_width_, wnd_height_, NULL, 0, 0, WHITENESS);
	ReleaseDC(dc);

	if (color_image) {
		//GENERATE BITMAP INFO HEADER FOR 24BIT COLOR
		long bmpsize = sizeof(BITMAPINFO);
		bitmap_info_ = (BITMAPINFO *)calloc(1, bmpsize);
		bitmap_info_->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info_->bmiHeader.biWidth = image_width;
		bitmap_info_->bmiHeader.biHeight = -image_height;
		bitmap_info_->bmiHeader.biPlanes = 1;
		bitmap_info_->bmiHeader.biBitCount = 24;
		bitmap_info_->bmiHeader.biCompression = BI_RGB;
		bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(image_width * image_height * 3);
	} else {
		//GENERATE BITMAP INFO HEADER FOR FOR 8BIT GRAY
		long bmpsize = sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD));
		bitmap_info_ = (BITMAPINFO *)calloc(1, bmpsize);
		bitmap_info_->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info_->bmiHeader.biWidth = image_width;
		bitmap_info_->bmiHeader.biHeight = -image_height;
		bitmap_info_->bmiHeader.biPlanes = 1;
		bitmap_info_->bmiHeader.biBitCount = 8;
		bitmap_info_->bmiHeader.biCompression = BI_RGB;
		bitmap_info_->bmiHeader.biSizeImage = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+(256 * sizeof(RGBQUAD)) + (image_width * image_height);
		for (int counter = 0; counter < 256; counter++) {
			bitmap_info_->bmiColors[counter].rgbBlue = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbGreen = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbRed = BYTE(counter);
			bitmap_info_->bmiColors[counter].rgbReserved = BYTE(0);
		}
	}
}

void ImageCtrl::SetImage(HINSTANCE hinstance, LPCWSTR name, UINT type, int bitmap_width, int bitmap_height) {

	hbitmap_ = (HBITMAP)LoadImage(hinstance, name, type, bitmap_width, bitmap_height, LR_DEFAULTCOLOR);
	Gdiplus::Bitmap image(hbitmap_, NULL);
	//Gdiplus::Bitmap *bm_photo = new Gdiplus::Bitmap(bitmap_width, bitmap_height, PixelFormat24bppRGB);
	//bm_photo->SetResolution(image.GetHorizontalResolution(), image.GetVerticalResolution());
	Gdiplus::Graphics gr_photo(main_dc_);// = Gdiplus::Graphics::FromImage(bm_photo);
	Gdiplus::Color colorW(255, 255, 255, 255);
	gr_photo.Clear(colorW);
	gr_photo.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	gr_photo.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	gr_photo.DrawImage(&image, Gdiplus::Rect(0, 0, bitmap_width, bitmap_height));

	Invalidate(FALSE);
}

void ImageCtrl::SetImage(cv::Mat& buffer) {

	StretchDIBits(main_dc_.GetSafeHdc(), 0, 0, wnd_width_, wnd_height_, 0, 0, img_width_, img_height_, buffer.data, bitmap_info_, DIB_RGB_COLORS, SRCCOPY);
	Invalidate(FALSE);
}

void ImageCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	COfflineDC offdc(&dc, &client_rect_);

	offdc.BitBlt(1, 1, wnd_width_, wnd_height_, &main_dc_, 0, 0, SRCCOPY);
}

void ImageCtrl::OnEnable(BOOL bEnable)
{
	InvalidateRect(client_rect_, FALSE);

	//CStatic::OnEnable(bEnable);
}