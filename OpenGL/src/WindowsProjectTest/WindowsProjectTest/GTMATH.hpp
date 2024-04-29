#pragma once
#include <math.h>

namespace GT
{
#define	PI									3.14159265358979323f
#define	DEG2RAD(theta)						(0.01745329251994329 * (theta))
#define MIN(a,b)							((a)<(b)?(a):(b))
#define MAX(a,b)							((a)>(b)?(a):(b))

	template<typename T>
	void	swapT(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

#define		SWAP_INT(a , b)   swapT<int>(a , b);
	template<typename T>
	struct tVec2
	{
		T	x;
		T	y;

		tVec2(T	_x, T	_y)
		{
			x = _x;
			y = _y;
		}
		tVec2()
		{
			x = -1;
			y = -1;
		}
	};

	typedef tVec2<int>		intV2;
	typedef tVec2<float>	floatV2;

	typedef unsigned int	uint;
	typedef unsigned char	byte;

	struct GT_RECT
	{
		int m_left;
		int m_right;
		int m_top;
		int m_bottom;
		GT_RECT(int _left = 0,
			int _right = 0,
			int _top = 0,
			int _bottom = 0)
		{
			m_left = _left;
			m_right = _right;
			m_top = _top;
			m_bottom = _bottom;
		}
	};

	struct RGBA
	{
		byte m_b;
		byte m_g;
		byte m_r;
		byte m_a;

		RGBA(byte _r = 255,
			byte _g = 255,
			byte _b = 255,
			byte _a = 255)
		{
			m_r = _r;
			m_g = _g;
			m_b = _b;
			m_a = _a;
		}
	};

	struct Point
	{
	public:
		float m_x;
		float m_y;
		RGBA m_color;
		floatV2 m_uv;
		Point(float _x = 0.0f, float _y = 0.0f, RGBA _color = RGBA(0, 0, 0, 0), floatV2 _uv = floatV2(0.0f, 0.0f))
		{
			m_x = _x;
			m_y = _y;
			m_color = _color;
			m_uv = _uv;
		}
		~Point()
		{

		}
	};

	struct Vec2 {
	public:
		float x = 0;
		float y = 0;

		Vec2(float inX = 0, float inY = 0) {
			x = inX;
			y = inY;
		}

		Vec2 operator - (const Vec2& _Other) {
			return Vec2(x - _Other.x, y - _Other.y);
		}

		Vec2 operator + (const Vec2& _Other) {
			return Vec2(x + _Other.x, y + _Other.y);
		}

		float operator * (const Vec2& _Other) {
			return x * _Other.x + y * _Other.y;
		}

		float cross(const Vec2& other) const {
			return x * other.y - y * other.x;
		}
	};

	struct Vec3
	{
	public:
		float x = 0;
		float y = 0;
		float z = 0;
	};

	const float eps = 1e-6f;
}