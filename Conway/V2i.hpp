#ifndef _V2I_HPP_
#define _V2I_HPP_

#include "vec2.hpp"

struct
	V2i
{
public:
	int x, y;

public:
	V2i();
	V2i(int x, int y);

public:
	V2i operator*(int scalar) const noexcept;
	V2i operator/(int scalar) const noexcept;
	V2i operator+(const V2i& add) const noexcept;
	V2i operator-(const V2i& sub) const noexcept;
	V2i operator-() const noexcept;
	bool operator==(const V2i& rhs) const noexcept;

	Vector2 to_v2f() const noexcept;

	friend std::ostream& operator<<(std::ostream& stream, const V2i& v2i);
};

Vector2 operator+(const V2i& v2i, const Vector2& vec);
Vector2 operator+(const Vector2& vec, const V2i& v2i);

#endif