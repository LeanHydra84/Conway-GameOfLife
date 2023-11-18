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
	V2i operator*(int scalar) const noexcept;
	V2i operator+(const V2i& add) const noexcept;
	V2i operator-(const V2i& sub) const noexcept;
	V2i operator-() const noexcept;
	bool operator==(const V2i& rhs) const noexcept;
};