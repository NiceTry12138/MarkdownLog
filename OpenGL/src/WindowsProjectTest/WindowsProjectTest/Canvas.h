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
		void drawLine(const Point &pt1, const Point &pt2);

		// �������
		void drawPoint(const Point &_pt) {
			if (_pt.m_x < 0 || _pt.m_x >= m_Width || _pt.m_y < 0 || _pt.m_y >= m_Height) {
				return;
			}

			int _index = _pt.m_y * m_Width + _pt.m_x;
			m_Buffer[_index] = _pt.m_color;
		}

		// ��ɫ���Բ�ֵ
		inline RGBA colorLerp(const RGBA& _color1, const RGBA& _color2, float _scale);

		// �������
		void clear() {
			if (m_Buffer != nullptr) {
				memset(m_Buffer, 0, sizeof(RGBA) * m_Width * m_Height);
			}
		}
	};
}



