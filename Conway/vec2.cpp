#include <cmath>
#include "vec2.hpp"

Vector2::Vector2() : x(0), y(0) { }

Vector2::Vector2(float x, float y) : x(x), y(y) { }

Vector2 Vector2::operator+(const Vector2& rhs) const
{
	return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator*(float rhs) const
{
	return Vector2(x * rhs, y * rhs);
}

Vector2 Vector2::operator/(float rhs) const
{
	return Vector2(x / rhs, y / rhs);
}

void Vector2::operator+=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vector2::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vector2::operator/=(float rhs)
{
	x /= rhs;
	y /= rhs;
}

void Vector2::operator*=(float rhs)
{
	x *= rhs;
	y *= rhs;
}

Vector2 Vector2::operator-()
{
	return Vector2(-x, -y);
}

float Vector2::Distance(Vector2 left, Vector2 right)
{
	float x = left.x - right.x;
	float y = left.y - right.y;
	x = x * x;
	y = y * y;

	return sqrt(x + y);
}

float Vector2::SqrDistance(Vector2 left, Vector2 right)
{
	float x = left.x - right.x;
	float y = left.y - right.y;
	return (x * x) + (y * y);
}

float Vector2::Magnitude() const
{
	float sx = x * x;
	float sy = y * y;
	return sqrt(sx + sy);
}

float Vector2::SqrMagnitude() const
{
	return (x * x) + (y * y);
}

Vector2 Vector2::Normalized() const
{
	float magnitude = Magnitude();
	if (magnitude == 0) return Vector2();
	return (*this) / Magnitude();
}

Vector2 Vector2::rotate(float angle)
{
	Vector2 rv;
	rv.x = cos(angle) * x - sin(angle) * y;
	rv.y = sin(angle) * x + cos(angle) * y;
	return rv;
}

std::ostream& operator<<(std::ostream& os, Vector2 const& v2)
{
	os << "(" << v2.x << ", " << v2.y << ")";
	return os;
}