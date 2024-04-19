#include "Canvas.h"
#include <cmath>

#include <Windows.h>
#include <string>

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