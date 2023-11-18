#include "V2i.hpp"

V2i::V2i() : x(0), y(0) { }

V2i::V2i(int x, int y) : x(x), y(y) { }

V2i V2i::operator*(int scalar) const noexcept
{
	return V2i(x * scalar, y * scalar);
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