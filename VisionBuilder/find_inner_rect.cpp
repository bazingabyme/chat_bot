#include "stdafx.h"
#include "find_inner_rect.h"

long InnerRect::DivideInSquare(long width, long height, long n, int& rows, int& cols) {

	if (n == 0) n = 1;
	long area = width * height;
	long maxDivArea = area / n;
	long maxDivSide = (long)sqrt(float(maxDivArea));
	cols = width / maxDivSide;
	rows = height / maxDivSide;
	long maxSq = cols * rows;
	if (maxSq >= n){
		return maxDivSide;
	}
	bool change_col = true;
	for (int i = maxDivSide - 1; i >= 0; i--){
		if (change_col) {
			cols = width / i;
		} else {
			rows = height / i;
		}
		long maxSq = cols * rows;
		if (maxSq >= n){
			return i;
		}
		change_col = !change_col;
	}
	return 1;
}

void InnerRect::findFloorRect(long lHorDiv, long lNumberOfDivision, long sW, long h, long lTotalProcessedRect, POINT mainRectInfo, std::vector<POINT> subRectInfo, std::vector<float> a, std::vector<POINT> &floorRectInfo) {

	long n = (long)subRectInfo.size();
	//POINT floorInfo = { ((mainRectInfo.x - sW) - (sW * lHorDiv)), h };
	POINT floorInfo = { (mainRectInfo.x - (sW * (lHorDiv - 1))), h };
	long lTotalWidth = 0;
	for (long j = lTotalProcessedRect; j < lTotalProcessedRect + lHorDiv; j++){
		if (j < n){
			lTotalWidth += subRectInfo[j].x;
		}
	}
	long minHeight = 0xffffff;
	long maxHeight = 0;
	long lFloorWidth = floorInfo.x;
	long lFloorHeight = floorInfo.y;
	long sumWidth = 0;
	do{
		minHeight = 0xffffff;
		maxHeight = 0;
		floorRectInfo.clear();
		sumWidth = 0;
		for (long j = lTotalProcessedRect; j < lTotalProcessedRect + lHorDiv; j++){
			if (j < n){
				long height = lFloorHeight;
				long width = (long)(height * a[j]);
				POINT pt = { width, height };
				floorRectInfo.push_back(pt);
				sumWidth += width;
			}
		}
		lFloorHeight -= 5;
		if (lFloorHeight < 50)
			break;
	} while (sumWidth > floorInfo.x);
}

void InnerRect::findRect(std::vector<std::vector<POINT>> &bestfloorWiseRectSize, RECT mainRect, long sW, long allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect) {

	POINT mainRectInfo = { (mainRect.right - mainRect.left), (mainRect.bottom - mainRect.top) };
	long lDivisionHeight = (long)((float)mainRectInfo.y / bestfloorWiseRectSize.size());
	long lPrevRowBottom = mainRect.top;
	for (unsigned int i = 0; i < bestfloorWiseRectSize.size(); i++){
		RECT rect;
		std::vector<RECT> floorRect;
		long startY = (lDivisionHeight * i) + sW;
		if (allignment == 0){

			//Find the base line on each floor...
			long maxHeight_on_floor = 0;
			long bottomLine = 0;
			for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
				if (maxHeight_on_floor < bestfloorWiseRectSize[i][j].y){
					maxHeight_on_floor = bestfloorWiseRectSize[i][j].y;
				}
			}
			long temp = (lDivisionHeight - maxHeight_on_floor) / 2;
			bottomLine = ((lDivisionHeight * (i + 1)) - temp);

			long lPrevWidth = 0;
			for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
				rect.left = lPrevWidth + mainRect.left + (j == 0 ? 0 : sW);
				rect.right = rect.left + bestfloorWiseRectSize[i][j].x;
				rect.top = lPrevRowBottom + sW;
				rect.bottom = rect.top + bestfloorWiseRectSize[i][j].y;

				floorRect.push_back(rect);
				lPrevWidth = rect.right;
			}

		}
		else if (allignment == 1){

			//Find the base line on each floor...
			long maxHeight_on_floor = 0;
			long bottomLine = 0;
			long startX = 0;

			for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
				if (maxHeight_on_floor < bestfloorWiseRectSize[i][j].y){
					maxHeight_on_floor = bestfloorWiseRectSize[i][j].y;
				}
				startX += bestfloorWiseRectSize[i][j].x + (j == (bestfloorWiseRectSize[i].size() - 1) ? 0 : sW);
			}
			long temp = (lDivisionHeight - maxHeight_on_floor) / 2;
			bottomLine = ((lDivisionHeight * (i + 1)) - temp);
			startX = (mainRectInfo.x - startX) / 2;

			//Calculate the rectangle
			long lPrevWidth = startX;
			for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
				rect.left = lPrevWidth + mainRect.left + (j == 0 ? 0 : sW);
				rect.right = rect.left + bestfloorWiseRectSize[i][j].x;
				rect.top = lPrevRowBottom + sW;
				rect.bottom = rect.top + bestfloorWiseRectSize[i][j].y;
				//rect.bottom = bottomLine;
				//rect.top = rect.bottom - bestfloorWiseRectSize[i][j].y;

				floorRect.push_back(rect);
				lPrevWidth = rect.right;
			}
		}
		lPrevRowBottom = rect.bottom + offset;
		bestfloorWiseRect.push_back(floorRect);
	}
}

void InnerRect::normalizeSize(std::vector<float> &a, std::vector<POINT> &bestRectInfo, std::vector<POINT> &subRectInfo, std::vector<std::vector<POINT>> &bestfloorWiseRectSize) {

	long cnt = 0;
	long minHeightOnFloor = 0xffffff;
	for (unsigned int i = 0; i < bestfloorWiseRectSize.size(); i++){
		long startPos = cnt;
		for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
			if (minHeightOnFloor > bestRectInfo[cnt].y){
				minHeightOnFloor = bestRectInfo[cnt].y;
			}
			cnt++;
		}
		cnt = startPos;
		for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
			bestRectInfo[cnt].y = minHeightOnFloor;
			bestRectInfo[cnt].x = (long)(minHeightOnFloor * a[cnt]);
			cnt++;
		}
	}

	for (unsigned int i = 0; i < subRectInfo.size(); i++){
		POINT pt = subRectInfo[i];
		POINT minSize = { 0xfffffff, 0xfffffff };
		for (unsigned int j = i; j < subRectInfo.size(); j++){
			if (pt.x == subRectInfo[j].x && pt.y == subRectInfo[j].y){
				long height = bestRectInfo[j].y;
				if (minSize.y > height){
					minSize.y = height;
					minSize.x = bestRectInfo[j].x;
				}
			}
		}

		for (unsigned int j = i; j < subRectInfo.size(); j++){
			if (pt.x == subRectInfo[j].x && pt.y == subRectInfo[j].y){
				bestRectInfo[j] = minSize;
			}
		}
	}

	cnt = 0;
	for (unsigned int i = 0; i < bestfloorWiseRectSize.size(); i++){
		for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
			bestfloorWiseRectSize[i][j] = bestRectInfo[cnt];
			//printf("\n (%d, %d)", bestfloorWiseRectSize[i][j].x, bestfloorWiseRectSize[i][j].y);
			cnt++;
		}
	}

	cnt = 0;
	minHeightOnFloor = 0xffffff;
	for (unsigned int i = 0; i < bestfloorWiseRectSize.size(); i++){
		for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
			if (minHeightOnFloor > bestfloorWiseRectSize[i][j].y){
				minHeightOnFloor = bestfloorWiseRectSize[i][j].y;
			}
		}
		for (unsigned int j = 0; j < bestfloorWiseRectSize[i].size(); j++){
			bestfloorWiseRectSize[i][j].y = minHeightOnFloor;
			bestfloorWiseRectSize[i][j].x = (long)(minHeightOnFloor * a[cnt]);
			cnt++;
		}
	}
}

void InnerRect::GetInsideBox(std::vector<POINT> &subRectInfo, RECT mainRect, long sW, int allignment, int offset, std::vector<std::vector<RECT>> &bestfloorWiseRect) {

	std::vector<std::vector<POINT>> bestfloorWiseRectSize;
	std::vector<float> a;
	POINT mainRectInfo = { (mainRect.right - mainRect.left), (mainRect.bottom - mainRect.top) };
	long outerArea = mainRectInfo.x * mainRectInfo.y;
	long n = (long)subRectInfo.size();
	for (long i = 0; i < n; i++){
		float aspect = (float)subRectInfo[i].x / (subRectInfo[i].y == 0 ? 1 : subRectInfo[i].y);
		a.push_back(aspect);
	}

	long minDiff = 0xfffffff;
	long bestRowDivision = -1;
	std::vector<POINT> bestRectInfo;
	//This loop is for number of division in rows for specific application...
	for (long i = 1; i <= n; i++){
		std::vector<std::vector<POINT>> floorWiseRectSize;
		long h = ((mainRectInfo.y) - ((sW + offset) * i)) / i;
		long totalArea = 0;
		std::vector<POINT> rectInfo;

		//Smartness comes here...
		totalArea = 0;
		rectInfo.clear();
		long lNumberOfDivision = i;
		long lTotalProcessedRect = 0;
		int r, d;
		//This loop is for each number of rect on each floor ...
		for (r = n, d = lNumberOfDivision; d >= 2; d--){
			long lHorDiv = r / d;
			std::vector<POINT> floorRectInfo;
			findFloorRect(lHorDiv, lNumberOfDivision, sW, h, lTotalProcessedRect, mainRectInfo, subRectInfo, a, floorRectInfo);
			for (long j = lTotalProcessedRect, k = 0; j < lTotalProcessedRect + lHorDiv; j++, k++){
				//floorRectInfo[k].y = minHeight;
				//floorRectInfo[k].x = a[j] * floorRectInfo[k].y;
				totalArea += floorRectInfo[k].y * floorRectInfo[k].x;
				rectInfo.push_back(floorRectInfo[k]);
			}
			floorWiseRectSize.push_back(floorRectInfo);
			lTotalProcessedRect += lHorDiv;
			r -= lHorDiv;
		}

		//Rectangle for last row...
		long lHorDiv = r;
		std::vector<POINT> floorRectInfo;
		findFloorRect(lHorDiv, lNumberOfDivision, sW, h, lTotalProcessedRect, mainRectInfo, subRectInfo, a, floorRectInfo);
		for (long j = lTotalProcessedRect, k = 0; j < lTotalProcessedRect + lHorDiv; j++, k++){
			//floorRectInfo[k].y = minHeight;
			//floorRectInfo[k].x = a[j] * floorRectInfo[k].y;
			totalArea += floorRectInfo[k].y * floorRectInfo[k].x;
			rectInfo.push_back(floorRectInfo[k]);
		}
		floorWiseRectSize.push_back(floorRectInfo);
		long diff = outerArea - totalArea;

		if (minDiff > diff && diff > 0){
			minDiff = diff;
			bestRowDivision = i;
			for (unsigned int x = 0; x < bestfloorWiseRectSize.size(); x++){
				bestfloorWiseRectSize[x].clear();
			}
			bestfloorWiseRectSize.clear();

			for (unsigned int x = 0; x < floorWiseRectSize.size(); x++){
				bestfloorWiseRectSize.push_back(floorWiseRectSize[x]);
			}
			bestRectInfo = rectInfo;
		}
		rectInfo.clear();
		for (unsigned int x = 0; x < floorWiseRectSize.size(); x++){
			floorWiseRectSize[x].clear();
		}
		floorWiseRectSize.clear();
	}

	//Normalize the size of the rectance if they have the same resolution/(height, width)
	normalizeSize(a, bestRectInfo, subRectInfo, bestfloorWiseRectSize);

	//Calculating the Rect Position on each floor...
	findRect(bestfloorWiseRectSize, mainRect, sW, allignment, offset, bestfloorWiseRect);

	
	for (unsigned int x = 0; x < bestfloorWiseRect.size(); x++) {
		long total_image_width = 0;
		for (unsigned int y = 0; y < bestfloorWiseRect[x].size(); y++) {
			RECT rect = bestfloorWiseRect[x][y];
			long width = rect.right - rect.left;
			/*long height = rect.bottom - rect.top;
			float aspect_ratio = (float)width / height;
			long height2 = height - offset;
			long width2 = long(height2 * aspect_ratio);
			rect.left += (width - width2) / 2;
			rect.top += (height - height2) / 2;
			rect.right -= (width - width2) / 2;
			rect.bottom -= (height - height2) / 2;
			bestfloorWiseRect[x][y] = rect;*/
			total_image_width += width;
		}
		long total_width = long(total_image_width + (sW * (bestfloorWiseRect[x].size() - 1)));
		long x_offset = ((mainRect.right - mainRect.left) - total_width) / 2;
		if (bestfloorWiseRect[x].size() > 1) {
			long width = (bestfloorWiseRect[x][0].right - bestfloorWiseRect[x][0].left);
			bestfloorWiseRect[x][0].left = x_offset;
			bestfloorWiseRect[x][0].right = bestfloorWiseRect[x][0].left + width;
			for (unsigned int y = 1; y < bestfloorWiseRect[x].size(); y++) {
				long width = (bestfloorWiseRect[x][y].right - bestfloorWiseRect[x][y].left);
				bestfloorWiseRect[x][y].left = bestfloorWiseRect[x][y - 1].right + sW;
				bestfloorWiseRect[x][y].right = bestfloorWiseRect[x][y].left + width;
			}
		}
	}

	for (unsigned int x = 0; x < bestfloorWiseRectSize.size(); x++){
		bestfloorWiseRectSize[x].clear();
	}
	bestfloorWiseRectSize.clear();
	bestRectInfo.clear();
	a.clear();
}

void InnerRect::GetStrokeImagePosition(long window_width, long window_height, long spacing_between_images, long image_width, long image_height, long total_images, std::vector<RECT> &image_rect, BOOL top_to_bottom) {

	if (total_images > 12 || total_images <= 0) {
		return;
	}

	if (image_height <= 0 || image_width <= 0 || window_width <= 0 || window_height <= 0) {
		return;
	}

	if (spacing_between_images < 0) {
		spacing_between_images = 0;
	}

	image_rect.clear();

	long rows = 0, cols = 0;
	if (total_images == 1) {
		rows = 1;	cols = 1;
	}
	else if (total_images == 2) {
		rows = 1;	cols = 2;
	}
	else if (total_images == 3) {
		rows = 2;	cols = 2;
	}
	else if (total_images == 4) {
		rows = 2;	cols = 2;
	}
	else if (total_images == 5) {
		rows = 2;	cols = 3;
	}
	else if (total_images == 6) {
		rows = 3;	cols = 2;
	}
	else if (total_images == 7) {
		rows = 3;	cols = 3;
	}
	else if (total_images == 8) {
		rows = 3;	cols = 3;
	}
	else if (total_images == 9) {
		rows = 3;	cols = 3;
	}
	else if (total_images == 10) {
		rows = 3;	cols = 4;
	}
	else if (total_images == 11) {
		rows = 3;	cols = 4;
	}
	else if (total_images == 12) {
		rows = 3;	cols = 4;
	}

	if (top_to_bottom) {
		rows = total_images;	cols = 1;
	}

	long W = window_width, w = image_width;
	long H = window_height, h = image_height;

	float image_aspect_ratio = (float)w / h;
	float window_aspect_ratio = (float)W / H;

	long thumb_image_width = (W - ((cols + 1) * spacing_between_images)) / cols;
	long thumb_image_height = long((float)thumb_image_width / image_aspect_ratio);
	if (thumb_image_height * rows > H) {
		thumb_image_height = (H - ((rows + 1) * spacing_between_images)) / rows;
		thumb_image_width = long((float)thumb_image_height * image_aspect_ratio);
	}

	long total_width_occupied = thumb_image_width * cols;
	long total_height_occupied = thumb_image_height * rows;

	long x_offset = ((W - total_width_occupied) / 2) - 2;
	if (x_offset < 0) x_offset = 0;
	long y_offset = ((H - total_height_occupied) / 2) - 2;
	if (y_offset < 0) y_offset = 0;

	long previous_end_x = 0;
	long previous_end_y = 0;
	RECT rect;
	for (int r = 0; r < rows; r++) {

		if (r == 0) {
			rect.top = y_offset;
		}
		else {
			rect.top = previous_end_y;
		}
		rect.bottom = rect.top + thumb_image_height;

		for (int c = 0; c < cols; c++) {

			if (c == 0) {
				rect.left = x_offset;
			}
			else {
				rect.left = previous_end_x;
			}
			rect.right = rect.left + thumb_image_width;

			previous_end_x = rect.right + spacing_between_images;

			image_rect.push_back(rect);
			//printf("\n %d, %d, %d, %d", rect.left, rect.top, rect.right, rect.bottom);
		}
		previous_end_y = rect.bottom + spacing_between_images;
	}
}

CRect InnerRect::GetRectInCenter(CRect ParentRect, CSize ImgSize) {

	CRect WndRect = ParentRect;
	int wnd_width = ImgSize.cx;
	int wnd_height = ImgSize.cy;

	if (wnd_height > ParentRect.Height()) {
		wnd_height = ParentRect.Height();
		wnd_width = (ParentRect.Height() * ImgSize.cx) / ImgSize.cy;
	}
	if (wnd_width > ParentRect.Width()) {
		wnd_width = ParentRect.Width();
		wnd_height = (ParentRect.Width() * ImgSize.cy) / ImgSize.cx;
	}
	wnd_width -= (wnd_width % 4);
	wnd_height -= (wnd_height % 4);

	int offset_x = ParentRect.left + (ParentRect.Width() - wnd_width) / 2;
	int offset_y = ParentRect.top + (ParentRect.Height() - wnd_height) / 2;

	WndRect.SetRect(offset_x, offset_y, offset_x + wnd_width, offset_y + wnd_height);

	return WndRect;
}

CRect InnerRect::GetRectInCenterAndLeftAlligned(CRect ParentRect, CSize ImgSize) {

	CRect WndRect = ParentRect;
	int wnd_width = ImgSize.cx;
	int wnd_height = ImgSize.cy;

	if (wnd_height > ParentRect.Height()) {
		wnd_height = ParentRect.Height();
		wnd_width = (ParentRect.Height() * ImgSize.cx) / ImgSize.cy;
	}
	if (wnd_width > ParentRect.Width()) {
		wnd_width = ParentRect.Width();
		wnd_height = (ParentRect.Width() * ImgSize.cy) / ImgSize.cx;
	}
	wnd_width -= (wnd_width % 4);
	wnd_height -= (wnd_height % 4);

	int offset_x = ParentRect.left + (ParentRect.Width() - wnd_width);
	int offset_y = ParentRect.top + (ParentRect.Height() - wnd_height) / 2;

	WndRect.SetRect(offset_x, offset_y, offset_x + wnd_width, offset_y + wnd_height);

	return WndRect;
}

CRect InnerRect::GetRectInCenterAndBottomAlligned(CRect ParentRect, CSize ImgSize) {

	CRect WndRect = ParentRect;
	int wnd_width = ImgSize.cx;
	int wnd_height = ImgSize.cy;

	if (wnd_height > ParentRect.Height()) {
		wnd_height = ParentRect.Height();
		wnd_width = (ParentRect.Height() * ImgSize.cx) / ImgSize.cy;
	}
	if (wnd_width > ParentRect.Width()) {
		wnd_width = ParentRect.Width();
		wnd_height = (ParentRect.Width() * ImgSize.cy) / ImgSize.cx;
	}
	wnd_width -= (wnd_width % 4);
	wnd_height -= (wnd_height % 4);

	int offset_x = ParentRect.left + (ParentRect.Width() - wnd_width) / 2;
	int offset_y = ParentRect.top + (ParentRect.Height() - wnd_height);

	WndRect.SetRect(offset_x, offset_y, offset_x + wnd_width, offset_y + wnd_height);

	return WndRect;
}