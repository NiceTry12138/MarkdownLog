#include <cmath>
#include <string>
#include <algorithm>

#include <Windows.h>

#include "Canvas.h"
#include "Tool.h"
#include "Image.h"

namespace GT {
	void Canvas::drawLine(const Point& pt1, const Point& pt2) {
		int disY = abs(pt2.m_y - pt1.m_y);
		int disX = abs(pt2.m_x - pt1.m_x);

		int xNow = pt1.m_x;
		int yNow = pt1.m_y;

		int stepX = 0;
		int stepY = 0;

		// 判断步径方向 
		if (pt1.m_x < pt2.m_x) {
			stepX = 1;
		}
		else {
			stepX = -1;
		}

		if (pt1.m_y < pt2.m_y) {
			stepY = 1;
		}
		else {
			stepY = -1;
		}

		// 对比 x、y 偏移量，决定步径的方向选取 x 或者 y
		int sumStep = disX;
		bool useXStep = true;

		if (disX < disY) {
			// 表示斜率 > 1
			sumStep = disY;
			useXStep = false;
			SWAP_INT(disX, disY);
		}

		// 初始化 p 值 也就是公式推导时最开始的值 (2dy - dx)
		int p = 2 * disY - disX;

		for (int index = 0; index <= sumStep; ++ index) {
			//std::wstring str = L"index = " + std::to_wstring(index) + L" xNow = " + std::to_wstring(xNow) + L", yNow = " + std::to_wstring(yNow) + L"\n";
			//OutputDebugString(str.c_str());

			RGBA pointColor;

			if (m_State.m_enableTexture && m_State.m_texture != nullptr) {
				// 开启贴图 并且贴图有效 使用贴图颜色
				floatV2 uv = uvLerp(pt1.m_uv, pt2.m_uv, (float)index / sumStep);
				pointColor = m_State.m_texture->GetColorByUV(uv, m_State.m_textureType);
			}
			else {
				pointColor = colorLerp(pt1.m_color, pt2.m_color, (float)index / sumStep);
			}
			drawPoint(Point(xNow, yNow, pointColor));
			if (p >= 0) {
				if (useXStep) {
					yNow += stepY;
				}
				else {
					xNow += stepX;
				}

				// (2dy - dx) + 2(dy - dx)
				p += 2 * (disY - disX);
			} else {
				// (2dy - dx) + 2dy
				p += 2 * disY;
			}

			// 更新步径坐标
			if (useXStep) {
				xNow += stepX;
			}
			else {
				yNow += stepY;
			}
		}
	}

	void Canvas::drawPoint(const Point& _pt) {
		if (_pt.m_x < 0 || _pt.m_x >= m_Width || _pt.m_y < 0 || _pt.m_y >= m_Height) {
			return;
		}

		int _index = _pt.m_y * m_Width + _pt.m_x;
		m_Buffer[_index] = _pt.m_color;
	}

	void Canvas::drawTriangle(const Point& pt1, const Point& pt2, const Point& pt3)
	{
		std::vector<Point> pVec;
		pVec.push_back(pt1);
		pVec.push_back(pt2);
		pVec.push_back(pt3);

		GT_RECT _rect(0, m_Width, m_Height, 0);
		//是否双方相交
		while (true)
		{
			// TODO 这里判断是有问题的
			// 如果三角形三个顶点不在矩形范围内，矩形的四个顶点也不在三角形范围内，依旧有可能三角形和矩形相交
			// 三角形三个顶点坐标 (50, 110) (-10, 50) (110, 50), 矩形四个顶点坐标 (0, 0) (0, 100) (100, 0) (100, 100) 
			if (judgeInRect(pt1, _rect) || judgeInRect(pt2, _rect) || judgeInRect(pt3, _rect))
			{
				break;
			}
			Point rpt1(0, 0, RGBA());
			Point rpt2(0, m_Width, RGBA());
			Point rpt3(0, m_Height, RGBA());
			Point rpt4(m_Width, m_Height,RGBA());

			if (judgeInTriangle(rpt1, pVec) ||
				judgeInTriangle(rpt2, pVec) ||
				judgeInTriangle(rpt3, pVec) ||
				judgeInTriangle(rpt4, pVec))
			{
				break;
			}

			return;
		}

		// 将顶点按照 y 坐标进行排序，使得 ptMax 是 y 坐标最大的点，ptMin 是 y 坐标最小的点
		std::sort(pVec.begin(), pVec.end(), [](const Point& pt1, const Point& pt2) { return pt1.m_y > pt2.m_y; });

		Point ptMax = pVec[0];  // y 最大的点
		Point ptMid = pVec[1];  // y 中间的点
		Point ptMin = pVec[2];  // y 最小的点

		if (ptMax.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMax, ptMid, ptMin);
			return;
		}

		if (ptMin.m_y == ptMid.m_y)
		{
			drawTriangleFlat(ptMin, ptMid, ptMax);
			return;
		}

		float k = 0.0;

		if (ptMax.m_x != ptMin.m_x)
		{
			k = (float)(ptMax.m_y - ptMin.m_y) / (float)(ptMax.m_x - ptMin.m_x);
		}
		float b = (float)ptMax.m_y - (float)ptMax.m_x * k;

		Point npt(0, 0, RGBA(255, 0, 0));
		npt.m_y = ptMid.m_y;
		if (k == 0)
		{
			npt.m_x = ptMax.m_x;
		}
		else
		{
			npt.m_x = ((float)npt.m_y - b) / k;
		}
		float s = (float)(npt.m_y - ptMin.m_y) / (float)(ptMax.m_y - ptMin.m_y);
		npt.m_color = colorLerp(ptMin.m_color, ptMax.m_color, s);
		npt.m_uv = uvLerp(ptMin.m_uv, ptMax.m_uv, s);

		drawTriangleFlat(ptMid, npt, ptMax);
		drawTriangleFlat(ptMid, npt, ptMin);

		return;
	}

	RGBA Canvas::getColor(int inX, int inY)
	{
		if (inX < 0 || inX >= m_Width || inY < 0 || inY >= m_Height)
		{
			return RGBA(0, 0, 0, 0);
		}

		return m_Buffer[inY * m_Width + inX];
	}

	void Canvas::setBlend(bool inUseBlend)
	{
		m_State.m_UseBlend = inUseBlend;
	}

	void Canvas::enableTexture(bool inEnable)
	{
		m_State.m_enableTexture = inEnable;
	}

	void Canvas::bindTexture(Image* inImage)
	{
		m_State.m_texture = inImage;
	}

	void Canvas::setTextureType(Image::TEXTURE_TYPE inType)
	{
		m_State.m_textureType = inType;
	}

	void Canvas::gtVertexPointer(int inSize, DATA_TYPE inType, int inStride, byte* inData)
	{
		m_State.m_vertextData.m_size = inSize;
		m_State.m_vertextData.m_type = inType;
		m_State.m_vertextData.m_stride = inStride;
		m_State.m_vertextData.m_data = inData;
	}

	void Canvas::gtColorPointer(int inSize, DATA_TYPE inType, int inStride, byte* inData)
	{
		m_State.m_colorData.m_size = inSize;
		m_State.m_colorData.m_type = inType;
		m_State.m_colorData.m_stride = inStride;
		m_State.m_colorData.m_data = inData;
	}

	void Canvas::gtTexCoordPointer(int inSize, DATA_TYPE inType, int inStride, byte* inData)
	{
		m_State.m_texCoordData.m_size = inSize;
		m_State.m_texCoordData.m_type = inType;
		m_State.m_texCoordData.m_stride = inStride;
		m_State.m_texCoordData.m_data = inData;
	}

	void Canvas::gtDrawArray(DRAW_MODE inMode, int inFirstIndex, int inCount)
	{
		Point pt0, pt1, pt2;
		byte* vertexData = m_State.m_vertextData.m_data + inFirstIndex * m_State.m_vertextData.m_stride;
		byte* colorData = m_State.m_colorData.m_data + inFirstIndex * m_State.m_colorData.m_stride;
		byte* texCoordData = m_State.m_texCoordData.m_data + inFirstIndex * m_State.m_texCoordData.m_stride;
		switch (inMode)
		{
		case GT::GT_LINE: {
			inCount = inCount / 2;
			for (int i = 0; i < inCount; i++)
			{
				float* vertexDataFloat = (float*)vertexData;
				pt0.m_x = vertexDataFloat[0];
				pt0.m_y = vertexDataFloat[1];
				vertexData += m_State.m_vertextData.m_stride;

				vertexDataFloat = (float*)vertexData;
				pt1.m_x = vertexDataFloat[0];
				pt1.m_y = vertexDataFloat[1];
				vertexData += m_State.m_vertextData.m_stride;

				//取颜色坐标

				RGBA* colorDataRGBA = (RGBA*)colorData;
				pt0.m_color = colorDataRGBA[0];

				colorData += m_State.m_colorData.m_stride;

				colorDataRGBA = (RGBA*)colorData;
				pt1.m_color = colorDataRGBA[0];
				colorData += m_State.m_colorData.m_stride;

				drawLine(pt0, pt1);
			}
			break;
		}
		case GT::GT_TRIANGLE: {

			inCount = inCount / 3;
			for (int i = 0; i < inCount; i++)
			{
				float* _vertexDataFloat = (float*)vertexData;
				pt0.m_x = _vertexDataFloat[0];
				pt0.m_y = _vertexDataFloat[1];
				vertexData += m_State.m_vertextData.m_stride;

				_vertexDataFloat = (float*)vertexData;
				pt1.m_x = _vertexDataFloat[0];
				pt1.m_y = _vertexDataFloat[1];
				vertexData += m_State.m_vertextData.m_stride;

				_vertexDataFloat = (float*)vertexData;
				pt2.m_x = _vertexDataFloat[0];
				pt2.m_y = _vertexDataFloat[1];
				vertexData += m_State.m_vertextData.m_stride;


				//取颜色坐标
				RGBA* colorDataRGBA = (RGBA*)colorData;
				pt0.m_color = colorDataRGBA[0];
				colorData += m_State.m_colorData.m_stride;

				colorDataRGBA = (RGBA*)colorData;
				pt1.m_color = colorDataRGBA[0];
				colorData += m_State.m_colorData.m_stride;

				colorDataRGBA = (RGBA*)colorData;
				pt2.m_color = colorDataRGBA[0];
				colorData += m_State.m_colorData.m_stride;

				//取uv坐标
				floatV2* _uvData = (floatV2*)texCoordData;
				pt0.m_uv = _uvData[0];
				texCoordData += m_State.m_texCoordData.m_stride;

				_uvData = (floatV2*)texCoordData;
				pt1.m_uv = _uvData[0];
				texCoordData += m_State.m_texCoordData.m_stride;

				_uvData = (floatV2*)texCoordData;
				pt2.m_uv = _uvData[0];
				texCoordData += m_State.m_texCoordData.m_stride;

				drawTriangle(pt0, pt1, pt2);
			}

			break;
		}
		default:
			break;
		}
	}

	void Canvas::drawTriangleFlat(const Point& pt1, const Point& pt2, const Point& pt)
	{
		float k1 = 0.0f;
		float k2 = 0.0f;

		if (pt.m_x != pt1.m_x) {
			k1 = (pt1.m_y - pt.m_y) * 1.0f / (pt1.m_x - pt.m_x);
		}
		if (pt.m_x != pt2.m_x) {
			k2 = (pt2.m_y - pt.m_y) * 1.0f / (pt2.m_x - pt.m_x);
		}

		bool upToDown = pt.m_y > pt1.m_y;
		int stepValue = upToDown ? -1 : 1;
		int startX = pt.m_x;
		int totalStemp = abs(pt.m_y - pt1.m_y);

		int step = 0;
		int posY = pt.m_y;

		// 限制绘制区域 计算 posY 和 step 的值 防止出现 y = 1000000 或者 y = -1000000
		if (!GT::UTool::inRange(pt.m_y, 0.0f, m_Height * 1.0f) || !GT::UTool::inRange(pt1.m_y, 0.0f, m_Height * 1.0f)) {
			int pos1Y = GT::UTool::clamp(pt1.m_y, 0.0f, m_Height * 1.0f);
			posY = GT::UTool::clamp(pt.m_y, 0.0f, m_Height * 1.0f);
			step = abs(posY - pt.m_y);
			totalStemp = abs(pt.m_y - pt1.m_y) - abs(pt1.m_y - pos1Y);
		}

		for (; step <= totalStemp; posY += stepValue, ++step) {
			// 如果 k1 或者 k2 为0 则 X 无变化，这里添加判断防止出现除0错误
			int l1x = GT::UTool::dcmp(k1) == 0 ? startX : startX + 1 / k1 * stepValue * step;
			float scale = step * 1.0f / totalStemp;
			RGBA color1 = colorLerp(pt.m_color, pt1.m_color, scale);
			floatV2 uv1 = uvLerp(pt.m_uv, pt1.m_uv, scale);
			int l2x = GT::UTool::dcmp(k2) == 0 ? startX : startX + 1 / k2 * stepValue * step;
			RGBA color2 = colorLerp(pt.m_color, pt2.m_color, scale);
			floatV2 uv2 = uvLerp(pt.m_uv, pt2.m_uv, scale);

			int edgeX1 = UTool::clamp(l1x, 0, m_Width);	// 将边界限制在画布左右两边 避免出现 x = -100000 或者 x = 100000 的情况
			int edgeX2 = UTool::clamp(l2x, 0, m_Width);

			// 下面使用 dcmp 是为了防止 l2x == l1x 导致除零错误
			float pointScale1 = UTool::dcmp(l2x - l1x) == 0 ? 0 : abs(edgeX1 - l1x) * 1.0f / abs(l2x - l1x);	// 插值计算 X1 端点的颜色
			float pointScale2 = UTool::dcmp(l2x - l1x) == 0 ? 0 : abs(edgeX2 - l1x) * 1.0f / abs(l2x - l1x);	// 插值计算 X2 端点的颜色

			RGBA edgeColor1 = colorLerp(color1, color2, pointScale1);	// 根据端点颜色 重新计算直线两端颜色
			floatV2 edgeUV1 = uvLerp(uv1, uv2, pointScale1);

			RGBA edgeColor2 = colorLerp(color1, color2, pointScale2);
			floatV2 edgeUV2 = uvLerp(uv1, uv2, pointScale2);

			Point p1 = Point(edgeX1, posY, edgeColor1, edgeUV1);
			Point p2 = Point(edgeX2, posY, edgeColor2, edgeUV2);

			drawLine(p1, p2);
		}
	}

	// 扫描线算法绘制三角形
	void Canvas::drawTriangle_scan(const Point& pt1, const Point& pt2, const Point& pt3)
	{
		// 获取包围盒
		int left = MIN(pt3.m_x, MIN(pt2.m_x, pt1.m_x));
		int bottom = MIN(pt3.m_y, MIN(pt2.m_y, pt1.m_y));
		int right = MAX(pt3.m_x, MAX(pt2.m_x, pt1.m_x));
		int top = MAX(pt3.m_y, MAX(pt2.m_y, pt1.m_y));
		
		// 剪裁屏幕
		left = MAX(left, 0);
		bottom = MAX(bottom, 0);
		right = MIN(right, m_Width);
		top = MIN(top, m_Height);

		std::vector<Point> points = { pt1, pt2, pt3 };

		for (int x = left; x <= right; ++x) {
			for (int y = bottom; y <= top; ++y) {
				if (judgeInTriangle(GT::Point(x, y), points)) {
					drawPoint(Point(x, y, RGBA(255, 0, 0)));
				}
			}
		}
	}

	bool Canvas::judgeInTriangle(const Point& pt, const std::vector<Point>& _ptArray)
	{
		bool flag = false; //相当于计数
		Point P1, P2; //多边形一条边的两个顶点
		int num = _ptArray.size() - 1;
		for (int i = 0, j = num; i <= num; j = i++)
		{
			//polygon[]是给出多边形的顶点
			P1 = _ptArray[i];
			P2 = _ptArray[j];
			
			// 点在多边形一条边上
			if (GT::UTool::onSegment(P1, P2, pt)) {
				return true; 
			}
			// (dcmp(P1.y-P.y)>0 != dcmp(P2.y-P.y)>0)，它判断点 P 的纵坐标是否在线段 P1P2 的纵坐标范围内
			// (P.y-P1.y)*(P1.x-P2.x)/(P1.y-P2.y) 得到交点的横坐标，再与点 P 的横坐标 P.x 进行比较，判断交点是否在点 P 的左侧
			if ((GT::UTool::dcmp(P1.m_y - pt.m_y) > 0 != GT::UTool::dcmp(P2.m_y - pt.m_y) > 0) && GT::UTool::dcmp(pt.m_x - (pt.m_y - P1.m_y) * (P1.m_x - P2.m_x) / (P1.m_y - P2.m_y) - P1.m_x) < 0)
				flag = !flag;
		}
		return flag;
	}

	bool Canvas::judgeInRect(const Point& pt, const GT_RECT& rect)
	{
		bool inX = pt.m_x >= rect.m_left && pt.m_x <= rect.m_right;
		bool inY = pt.m_y >= rect.m_bottom && pt.m_y <= rect.m_top;
		return inX && inY;
	}

	inline RGBA Canvas::colorLerp(const RGBA& _color1, const RGBA& _color2, float _scale)
	{
		RGBA result;

		result.m_r = _color1.m_r + (float)(_color2.m_r - _color1.m_r) * _scale;
		result.m_g = _color1.m_g + (float)(_color2.m_g - _color1.m_g) * _scale;
		result.m_b = _color1.m_b + (float)(_color2.m_b - _color1.m_b) * _scale;
		result.m_a = _color1.m_a + (float)(_color2.m_a - _color1.m_a) * _scale;

		return result;
	}
	
	inline floatV2 Canvas::uvLerp(const floatV2 inUV1, floatV2 inUV2, float inScale)
	{
		floatV2 result;
		result.x = inUV1.x + (inUV2.x - inUV1.x) * inScale;
		result.y = inUV1.y + (inUV2.y - inUV1.y) * inScale;
		return result;
	}

	void Canvas::drawImage(int inX, int inY, GT::Image* inImage)
	{
		for (int u = 0; u < inImage->GetWidth(); ++u) {
			for (int v = 0; v < inImage->GetHeight(); ++v) {
				const RGBA srcColor = inImage->GetColor(u, v);
				if (!m_State.m_UseBlend) {
					drawPoint(Point(inX + u, inY + v, srcColor));
				}
				else {
					RGBA dstColor = getColor(inX + u, inY + v);
					float alpha = (float)srcColor.m_a / 255.0f * inImage->getAlpha();
					RGBA finalColor = colorLerp(dstColor, srcColor, alpha);
					drawPoint(Point(inX + u, inY + v, finalColor));
				}
			}
		}
	}
}