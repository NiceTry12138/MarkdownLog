#include <Windows.h>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "GTMATH.hpp"

namespace GT {
	class UTool {
	public:

	public:
		static void DebugPrint(const wchar_t* format, ...) {
			wchar_t buffer[256];

			va_list args;
			va_start(args, format);
			vswprintf(buffer, sizeof(buffer), format, args);
			va_end(args);

			OutputDebugString(buffer);
		}

		static int dcmp(float x) {
			if (fabs(x) < eps) {
				return 0;
			}
			return x < 0 ? -1 : 1;
		}

		static Point reducePoint(const Point& p1, const Point& p2, bool useP1Color = true) {
			RGBA color = useP1Color ? p1.m_color : p2.m_color;
			return Point(p1.m_x - p2.m_x, p1.m_y - p2.m_y, color);
		}

		//判断点Q是否在P1和P2的线段上
		static bool onSegment(Point P1, Point P2, Point Q) {
			Vec2 vecp1q = Vec2(P1.m_x - Q.m_x, P1.m_y - Q.m_y);			
			Vec2 vecp2q = Vec2(P2.m_x - Q.m_x, P2.m_y - Q.m_y);
			// 这个函数用于判断点 Q 是否在线段 P1P2 上
			// 它先计算向量 (P1-Q)^(P2-Q) 的叉积是否为 0（判断点 Q 是否在直线 P1P2 上）
			// 然后再判断点 Q 是否在线段 P1P2 的范围内（计算 (P1-Q)*(P2-Q) 的点积是否小于等于 0）
			// 如果满足这两个条件，则点 Q 在线段 P1P2 上
			float cross = vecp1q.cross(vecp2q);
			int dcmpCross = dcmp(vecp1q.cross(vecp2q));
			float po = vecp1q * vecp2q;
			int dcmpPo = dcmp(vecp1q * vecp2q) <= 0;
			bool res1 = dcmp(vecp1q.cross(vecp2q)) == 0;
			bool res2 = dcmp(vecp1q * vecp2q) <= 0;
			bool result = dcmp(vecp1q.cross(vecp2q)) == 0 && dcmp(vecp1q * vecp2q) <= 0;
			return result;
		}

		template<typename T>
		static T clamp(const T& val, const T& inMinVal, const T& inMaxVal) {
			T minVal = min(inMinVal, inMaxVal);
			T maxVal = max(inMinVal, inMaxVal);
			return min(maxVal, max(val, minVal));
		}

		template<typename T>
		static bool inRange(const T& val, const T& inMinVal, const T& inMaxVal, bool shouldEqual = true) {
			if (shouldEqual) {
				return val >= inMinVal && val <= inMaxVal;
			}
			return val > inMinVal && val < inMaxVal;
		}
	};
}

