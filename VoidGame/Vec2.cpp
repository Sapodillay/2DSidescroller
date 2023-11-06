#include "Vec2.h"
#include <cmath>

#include <iostream>

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

//tested
bool Vec2::operator==(const Vec2& rhs) const
{
	if (x == rhs.x && y == rhs.y) return true;
	return false;
}

//tested
bool Vec2::operator!=(const Vec2& rhs) const
{
	if (x == rhs.x && y == rhs.y) return false;
	return true;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator*(const float val) const
{
	return Vec2(x * val, y * val);
}

void Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator*=(const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator/=(const float val)
{
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	float diffY = y - rhs.y;
	float diffX = x - rhs.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

float Vec2::magnitude() const
{
	return sqrt(x * x + y * y);
}

Vec2 Vec2::GetUnitVector()
{
	if (x == 0 && y == 0) { return Vec2(0, 0); }

	Vec2 UnitVector;
	float magnitude = this->magnitude();
	UnitVector.x = x / magnitude;
	UnitVector.y = y / magnitude;
	return UnitVector;
}

Intersect LineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d)
{
	Vec2 r = (b - a);
	Vec2 s = (d - c);

	float rxs = r.x * s.y - r.y * s.x;
	Vec2 cma = c - a;

	float t = (cma.x * s.y - cma.y * s.x) / rxs;
	float u = (cma.x * r.y - cma.y * r.x) / rxs;

	if (rxs == 0) {
		// Lines are parallel, no intersection
		return { false, Vec2(0, 0) };
	}

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		// Intersection within line segments
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	else {
		// Intersection outside of line segments
		return { false, Vec2(0, 0) };
	}
}
