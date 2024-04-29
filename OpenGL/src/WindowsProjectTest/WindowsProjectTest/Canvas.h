#pragma once
#include "GTMATH.hpp"
#include <string.h>
#include <vector>

#include "Image.h"

namespace GT {

	// 操作画布的类
	class Canvas
	{
	private:
		int m_Width{ -1 };
		int m_Height{ -1 };
		RGBA* m_Buffer{ nullptr };

		byte m_alphaLimit{ 0 }; // 大于此值的像素才可以进行绘制
		bool m_UseBlend = true; // 是否进行颜色混合

		bool m_enableTexture = true;	// 是否启用纹理贴图
		Image* m_texture{ nullptr };	// 绘制剩下顶点的时候 使用那种图片
		Image::TEXTURE_TYPE m_textureType = Image::TEXTURE_TYPE::TX_REPEAT;	// 纹理绘制
	public: 
		Canvas(int _width, int _height, void* _buffer) {
			if (_width <= 0 || _height <= 0) {
				m_Width = -1;
				m_Height = -1;
			}
			m_Width = _width;
			m_Height = _height;
			m_Buffer = (RGBA*)_buffer;
		}

		~Canvas() {

		}

		// 画线
		void drawLine(const Point &pt1, const Point &pt2);

		// 画点操作
		void drawPoint(const Point& _pt);

		// 绘制多边形
		void drawTriangle(const Point& pt1, const Point& pt2, const Point& pt);

		// 颜色线性插值
		inline RGBA colorLerp(const RGBA& _color1, const RGBA& _color2, float _scale);

		// uv 插值
		inline floatV2 uvLerp(const floatV2 inUV1, floatV2 inUV2, float inScale);

		// 
		void drawImage(int inX, int inY, GT::Image* inImage);

		// 清理操作
		inline void clear() {
			if (m_Buffer != nullptr) {
				memset(m_Buffer, 0, sizeof(RGBA) * m_Width * m_Height);
			}
		}

		void setAlphaLimit(byte inLimit) {
			m_alphaLimit = inLimit;
		}

		RGBA getColor(int inX, int inY);

		void setBlend(bool inUseBlend);

		void enableTexture(bool inEnable);

		void bindTexture(Image* inImage);

		void setTextureType(Image::TEXTURE_TYPE inType);

	protected:
		// 平底平顶三角形绘制 pt1 和 pt2 是平底或平顶的两点，pt 是单独的一个顶点
		void drawTriangleFlat(const Point& pt1, const Point& pt2, const Point& pt);

		// 扫描线算法实现三角形绘制
		void drawTriangle_scan(const Point& pt1, const Point& pt2, const Point& pt3);

		bool judgeInTriangle(const Point& pt, const std::vector<Point>& _ptArray);

		bool judgeInRect(const Point& pt, const GT_RECT& rect);
	};
}



