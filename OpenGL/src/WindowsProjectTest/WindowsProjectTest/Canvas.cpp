#include "Canvas.h"
#include <cmath>

#include <Windows.h>
#include <string>

namespace GT {
	void Canvas::drawLine(intV2 pt1, intV2 pt2, RGBA _Color) {
		int disY = abs(pt2.y - pt1.y);
		int disX = abs(pt2.x - pt1.x);

		int xNow = pt1.x;
		int yNow = pt1.y;

		int stepX = 0;
		int stepY = 0;

		// 判断步径方向 
		if (pt1.x < pt2.x) {
			stepX = 1;
		}
		else {
			stepX = -1;
		}

		if (pt1.y < pt2.y) {
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
			std::wstring str = L"index = " + std::to_wstring(index) + L" xNow = " + std::to_wstring(xNow) + L", yNow = " + std::to_wstring(yNow) + L"\n";
			OutputDebugString(str.c_str());
			drawPoint(xNow, yNow, _Color);
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
}