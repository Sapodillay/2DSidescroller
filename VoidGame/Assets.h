#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include "Animation.h"


class Assets
{

public:
	void loadFromFile(const std::string& path);

	void addTexture(const std::string& name, const std::string path);
	void addAnimation(const std::string& name, const std::string path);
	void addAnimation(const std::string& name, const std::string path, size_t frameCount, size_t speed);
	void addFont(const std::string& name, const std::string& path);

	sf::Texture getTexture(const std::string& name);
	Animation getAnimation(const std::string& name);
	sf::Font getFont(const std::string& name);

	std::vector<Animation> getAnimations();


private:
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, sf::Font> m_fonts;
	std::map<std::string, Animation> m_animations;
};

