#include <cmath>
#include <string>
#include <algorithm>

#include <Windows.h>

#include "Canvas.h"
#include "Tool.h"

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

		for (int index = 0; index < sumStep; ++ index) {
			//std::wstring str = L"index = " + std::to_wstring(index) + L" xNow = " + std::to_wstring(xNow) + L", yNow = " + std::to_wstring(yNow) + L"\n";
			//OutputDebugString(str.c_str());

			auto pointColor = colorLerp(pt1.m_color, pt2.m_color, (float)index / sumStep);
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

		drawTriangleFlat(ptMid, npt, ptMax);
		drawTriangleFlat(ptMid, npt, ptMin);

		return;
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
		int totalStemp = abs(pt.m_y - pt1.m_y) + 1;

		int step = 0;
		int posY = pt.m_y;

		// TODO 限制绘制区域 计算 posY 和 step 的值

		for (; step < totalStemp; posY += stepValue, ++step) {
			// 超出边界 不做处理
			if (!GT::UTool::inRange(posY, 0, m_Height)) {
				break;
			}
			int l1x = startX + 1 / k1 * stepValue * step;
			RGBA color1 = colorLerp(pt.m_color, pt1.m_color, step * 1.0 / totalStemp);
			int l2x = startX + 1 / k2 * stepValue * step;
			RGBA color2 = colorLerp(pt.m_color, pt2.m_color, step * 1.0 / totalStemp);
			Point p1 = Point(l1x, posY, color1);
			Point p2 = Point(l2x, posY, color2);
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
}