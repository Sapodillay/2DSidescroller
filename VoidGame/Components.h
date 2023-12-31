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
	CAnimation(const Animation& animation, bool r, bool f) : m_animation(animation), m_repeat(r), m_flipped(f) {};


	Animation m_animation;
	bool m_repeat = false;
	bool m_flipped = false;

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

class CHealth : public Component
{
public:
	int m_health = 0;
	int m_maxHealth = 0;
	//the amount of time between attacks
	int m_damageCooldownTime = 30.0f;
	//int tracking current cooldown amount
	int m_damageCooldown = 0.0f;
	std::string m_healthString;

	CHealth() {}
	CHealth(int maxHealth) : m_health(maxHealth), m_maxHealth(maxHealth)
	{
		UpdateString();
	}
	CHealth(int maxHealth, int currentHealth) : m_health(currentHealth), m_maxHealth(maxHealth)
	{
		UpdateString();
	}

	//updates health string drawn on UI
	void UpdateString()
	{
		m_healthString = "Health: " + std::to_string(m_health) + "/" + std::to_string(m_maxHealth);
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
	std::string state = "Down";
	bool moveDirection = true;

	CPlayerState() {}
};


//Simple path movement, enemy lerps between both points.
class CPathMovement : public Component
{
public:
	Vec2 p1;
	Vec2 p2;
	float progress = 0.0f;
	float speed = 1.0f;
	bool reverse = false;
	CPathMovement() {};
	CPathMovement(Vec2 point1, Vec2 point2, float moveSpeed) : p1(point1), p2(point2), speed(moveSpeed)
	{
	}
};

class CGravity : public Component
{
public:
	float gravity = 0;

	CGravity() {}
	CGravity(float grav) : gravity(grav) {}
};