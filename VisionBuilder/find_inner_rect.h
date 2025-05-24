#pragma once

#include <vector>

class InnerRect {

public:
	InnerRect() {}
	~InnerRect() {}

	long DivideInSquare(long width, long height, long n, int& rows, int& cols);
	void GetInsideBox(std::vector<POINT> &subRectInfo, RECT mainRect, long sW, int allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect);
	void GetStrokeImagePosition(long window_width, long window_height, long spacing_between_images, long image_width, long image_height, long total_images, std::vector<RECT> &image_rect, BOOL top_to_bottom = FALSE);
	CRect GetRectInCenter(CRect ParentRect, CSize ImgSize);
	CRect GetRectInCenterAndLeftAlligned(CRect ParentRect, CSize ImgSize);
	CRect GetRectInCenterAndBottomAlligned(CRect ParentRect, CSize ImgSize);

private:
	void findFloorRect(long lHorDiv, long lNumberOfDivision, long sW, long h, long lTotalProcessedRect, POINT mainRectInfo, std::vector<POINT> subRectInfo, std::vector<float> a, std::vector<POINT> &floorRectInfo);
	void findRect(std::vector<std::vector<POINT>> &bestfloorWiseRectSize, RECT mainRect, long sW, long allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect);
	void normalizeSize(std::vector<float> &a, std::vector<POINT> &bestRectInfo, std::vector<POINT> &subRectInfo, std::vector<std::vector<POINT>> &bestfloorWiseRectSize);
};
