#pragma once
class Vec2
{
public:
	float x = 0;
	float y = 0;


	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const float val) const;
	Vec2 operator * (const float val) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float dist(const Vec2& rhs) const;
	float magnitude() const;
	Vec2 GetUnitVector();

};

struct Line { Vec2 a; Vec2 b; };

struct Intersect { bool result; Vec2 position; };

Intersect LineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d);
