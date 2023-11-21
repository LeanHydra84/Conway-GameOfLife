#include "V2i.hpp"

V2i::V2i() : x(0), y(0) { }

V2i::V2i(int x, int y) : x(x), y(y) { }

V2i V2i::operator*(int scalar) const noexcept
{
	return V2i(x * scalar, y * scalar);
}

V2i V2i::operator/(int scalar) const noexcept
{
	return V2i(x / scalar, y / scalar);
}

V2i V2i::operator+(const V2i& add) const noexcept
{
	return V2i(x + add.x, y + add.y);
}

V2i V2i::operator-(const V2i& sub) const noexcept
{
	return V2i(x - sub.x, y - sub.y);
}

V2i V2i::operator-() const noexcept
{
	return V2i(-x, -y);
}

bool V2i::operator==(const V2i& rhs) const noexcept
{
	return (x == rhs.x && y == rhs.y);
}

// EXTERIOR OPERATIONS

Vector2 operator+(const V2i& v2i, const Vector2& vec)
{
	return Vector2(vec.x + v2i.x, vec.y + v2i.y);
}

Vector2 operator+(const Vector2& vec, const V2i& v2i)
{
	return v2i + vec;
}

Vector2 V2i::to_v2f() const noexcept
{
	return Vector2(x, y);
}

std::ostream& operator<<(std::ostream& stream, const V2i& v2i)
{
	stream << "(" << v2i.x << ", " << v2i.y << ")";
	return stream;
}