#pragma once
#include "GTMATH.hpp"
#include <string.h>

namespace GT {
	// ������������
	class Canvas
	{
	private:
		int m_Width{ -1 };
		int m_Height{ -1 };
		RGBA* m_Buffer{ nullptr };


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

		// ����
		void drawLine(intV2 pt1, intV2 pt2, RGBA _Color);

		// �������
		void drawPoint(int x, int y, RGBA _collor) {
			if (x < 0 || x >= m_Width || y < 0 || y >= m_Height) {
				return;
			}

			m_Buffer[y * m_Width + x] = _collor;
		}

		// �������
		void clear() {
			if (m_Buffer != nullptr) {
				memset(m_Buffer, 0, sizeof(RGBA) * m_Width * m_Height);
			}
		}
	};
}



