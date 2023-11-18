#pragma once

struct
	V2i
{
public:
	int x, y;

public:
	V2i();
	V2i(int x, int y);

public:
	constexpr V2i operator*(int scalar) const noexcept;
	constexpr V2i operator+(const V2i& add) const noexcept;
	constexpr V2i operator-(const V2i& sub) const noexcept;
	constexpr V2i operator-() const noexcept;
};