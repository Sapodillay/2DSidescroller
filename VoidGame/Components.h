#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include "Animation.h"

#include <iostream>

class Component
{
public:
	bool has = false;
};


class CAnimation : public Component
{
public:
	CAnimation() {}
	CAnimation(const Animation& animation, bool r) : m_animation(animation), m_repeat(r) {};

	Animation m_animation;
	bool m_repeat = false;

};


class CTransform : public Component
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prev_pos = { 0.0, 0.0 };
	Vec2 velocity = { 0.0, 0.0 };
	float angle = 0;
	CTransform() {}
	CTransform(const Vec2& p) : CTransform(p, Vec2(0,0), 0) {}
	CTransform(const Vec2& p, const Vec2& v, float a) : pos(p), velocity(v), angle(a) {}
};

class CShape : public Component
{
public:
	sf::CircleShape circle;
	CShape() {}
	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
	{
		circle.setRadius(radius);
		circle.setPointCount(points);
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};


class CCollision : public Component
{
public:
	float radius = 0;
	float height = 0;
	float width = 0;
	CCollision() {}
	CCollision(float r) : radius(r) {}
	CCollision(float h, float w) : height(h), width(w) {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	sf::Color debugColor = sf::Color::Red;

	CBoundingBox() {}
	CBoundingBox(const Vec2& s) : size(s), halfSize(s.x / 2, s.y / 2) {}
};



class CScore : public Component
{
public:
	int score = 0;
	CScore() {}
	CScore(int s) : score(s)
	{

	}
};

class CLifespan : public Component
{
public:
	int remaining = 0; // amount of lfiespan remainingg on the entity
	int total = 0; //the total inital amount of lifespan
	CLifespan() {}
	CLifespan(int total) : remaining(total), total(total)
	{

	}
};

class CInput : public Component
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;

	CInput() {}
};

class CPlayerState : public Component
{
public:
	bool isJumping = false;
	float jumpTimer = 0.0f;
	std::string state = "down";

	CPlayerState() {}
};

class CGravity : public Component
{
public:
	float gravity = 0;

	CGravity() {}
	CGravity(float grav) : gravity(grav) {}
};