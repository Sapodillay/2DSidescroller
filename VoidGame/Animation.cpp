#include "Animation.h"
#include <iostream>

Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& tex) : Animation(name, tex, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& tex, size_t frameCount, size_t speed) 
: m_name(name), m_sprite(tex), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed)
{
	//seperate frame size from file size.
	m_size = Vec2((float)tex.getSize().x / frameCount, (float)tex.getSize().y);
	//set origin to the middle of the sprite.
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);

	//crop texture to the current frame.
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

const std::string& Animation::getName() const
{
	return m_name;
}

void Animation::setSize(Vec2 size)
{
	sf::FloatRect spriteBounds = m_sprite.getLocalBounds();

	float scaleX = size.x / spriteBounds.width;
	float scaleY = size.y / spriteBounds.height;


	m_sprite.setScale(scaleX, scaleY);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));

	m_size.x = m_size.x * scaleX;
	m_size.y = m_size.y * scaleY;

}

const Vec2& Animation::getSize()
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
