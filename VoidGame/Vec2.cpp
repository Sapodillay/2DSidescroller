#include "Vec2.h"
#include <cmath>

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

Vec2 Vec2::Lerp(const Vec2& rhs, float t) const
{
	return Vec2(x, y) * t + rhs * (1.f - t);
}
