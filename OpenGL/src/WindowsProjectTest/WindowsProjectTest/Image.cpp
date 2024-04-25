#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include "Tool.h"

namespace GT {
	RGBA Image::GetColor(int x, int y) const
	{
		if (!UTool::inRange(x, 0, m_Width) || !UTool::inRange(y, 0, m_Height)) {
			return RGBA(0, 0, 0, 0);
		}
		if (m_Data == nullptr) {
			return RGBA(0, 0, 0, 0);
		}
		return m_Data[y * m_Width + x];
	}

	Image* Image::readFromFile(const char* _fileName)
	{
		int			_picType = 0;
		int			_width = 0;
		int			_height = 0;

		//stbimage读入的图片是反过来的
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bits = stbi_load(_fileName, &_width, &_height, &_picType, STBI_rgb_alpha);

		for (int i = 0; i < _width * _height * 4; i += 4)
		{
			byte tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;
		}

		Image* _image = new Image(_width, _height, bits);

		stbi_image_free(bits);
		return _image;
	}
	Image* Image::zoomImageSimple(const Image* inImage, float inZoomX, float inZoomY)
	{
		int width = inImage->GetWidth() * inZoomX;
		int height = inImage->GetHeight() * inZoomY;
		byte* data = new byte[width * height * sizeof(RGBA)];
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				int imageX = (float)x / inZoomX;
				int imageY = (float)y / inZoomY;
				imageX = imageX < inImage->GetWidth() ? imageX : inImage->GetWidth() - 1;
				imageY = imageY < inImage->GetHeight() ? imageY : inImage->GetHeight() - 1;
				RGBA color = inImage->GetColor(imageX, imageY);
				memcpy(data + (x + y * width) * sizeof(RGBA), &color, sizeof(RGBA));
			}
		}

		Image* newImage = new Image(width, height, data);
		delete[] data;
		data = nullptr;
		newImage->setAlpha(inImage->getAlpha());
		return newImage;
	}

	Image* Image::zoomImageBilinearInerp(const Image* inImage, float inZoomX, float inZoomY)
	{
		int width = inImage->GetWidth() * inZoomX;
		int height = inImage->GetHeight() * inZoomY;
		byte* data = new byte[width * height * sizeof(RGBA)];

		float coordX = 0, coordY = 0;

		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		float disX1 = 0, disY1 = 0, disX2 = 0, disY2 = 0;

		for (int i = 0; i < width; i++)
		{
			coordX = (float)i / inZoomX;
			x1 = (int)coordX;
			if (x1 >= inImage->GetWidth() - 1)
			{
				x1 = inImage->GetWidth() - 1;
				x2 = x1;
			}
			else
			{
				x2 = x1 + 1;
			}
			disX1 = coordX - x1;
			disX2 = 1.0 - disX1;
			for (int j = 0; j < height; j++)
			{
				coordY = (float)j / inZoomY;
				y1 = (int)coordY;
				if (y1 >= inImage->GetHeight() - 1)
				{
					y1 = inImage->GetHeight() - 1;
					y2 = y1;
				}
				else
				{
					y2 = y1 + 1;
				}
				disY1 = coordY - y1;
				disY2 = 1.0 - disY1;

				RGBA _color11 = inImage->GetColor(x1, y1);
				RGBA _color12 = inImage->GetColor(x1, y2);
				RGBA _color21 = inImage->GetColor(x2, y1);
				RGBA _color22 = inImage->GetColor(x2, y2);

				RGBA _targetColor;
				_targetColor.m_r = (float)_color11.m_r * disX2 * disY2 +
					(float)_color12.m_r * disX2 * disY1 +
					(float)_color21.m_r * disX1 * disY2 +
					(float)_color22.m_r * disX1 * disY1;

				_targetColor.m_g = (float)_color11.m_g * disX2 * disY2 +
					(float)_color12.m_g * disX2 * disY1 +
					(float)_color21.m_g * disX1 * disY2 +
					(float)_color22.m_g * disX1 * disY1;

				_targetColor.m_b = (float)_color11.m_b * disX2 * disY2 +
					(float)_color12.m_b * disX2 * disY1 +
					(float)_color21.m_b * disX1 * disY2 +
					(float)_color22.m_b * disX1 * disY1;

				_targetColor.m_a = (float)_color11.m_a * disX2 * disY2 +
					(float)_color12.m_a * disX2 * disY1 +
					(float)_color21.m_a * disX1 * disY2 +
					(float)_color22.m_a * disX1 * disY1;

				memcpy(data + (j * width + i) * sizeof(RGBA), &_targetColor, sizeof(RGBA));
			}
		}

		Image* newImage = new Image(width, height, data);
		delete[] data;
		data = nullptr;
		newImage->setAlpha(inImage->getAlpha());
		return newImage;
	}
}

