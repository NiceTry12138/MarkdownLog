#pragma once
#include <string.h>
#include "GTMATH.hpp"

namespace GT {
	class Image
	{
	private:
		int m_Width = 0;	// 图片宽度
		int m_Height = 0;	// 图片高度
		RGBA* m_Data = nullptr;	// 图片颜色数据

		float m_Alpha = 1.0;

	public:
		enum TEXTURE_TYPE
		{
			TX_CLAMP_TO_EDGE = 0,
			TX_REPEAT
		};

	public:
		inline int GetWidth() const {
			return m_Width;
		}

		inline int GetHeight() const {
			return m_Height;
		}

		inline void setAlpha(float inAlpha) { 
			m_Alpha = inAlpha;
		}

		inline float getAlpha() const {
			return m_Alpha;
		}

		RGBA GetColor(int x, int y) const;

		RGBA GetColorByUV(floatV2 inUV, TEXTURE_TYPE inType) const;

		Image(int _width, int _height, byte* _data) {
			m_Width = _width;
			m_Height = _height;
			if (_data) {
				m_Data = new RGBA[m_Width * m_Height];
				memcpy(m_Data, _data, sizeof(RGBA) * m_Width * m_Height);
			}
		}

		~Image() {
			if (m_Data)
			{
				delete[] m_Data;
				m_Data = nullptr;
			}
		}

	public:
		static Image* readFromFile(const char* _fileName);
		static Image* zoomImageSimple(const Image* inImage, float inZoomX, float inZoomY);	// 简单最近邻插值
		static Image* zoomImageBilinearInerp(const Image* inImage, float inZoomX, float inZoomY);	// 双线性插值
	};
}

