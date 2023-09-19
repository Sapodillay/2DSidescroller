#include "Assets.h"

#include <fstream>
#include <sstream>

void Assets::loadFromFile(const std::string& path)
{
	//load assets from file.



	std::ifstream configFile(path);
	std::string line;
	while (std::getline(configFile, line))
	{
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "Animation")
		{
			std::string name;
			std::string path;
			int frameCount;
			int speed;

			iss >> name >> path >> frameCount >> speed;
			std::cout << "Adding animation: " + name << " " << path << " " << frameCount << " " << speed << std::endl;
			addAnimation(name, path, frameCount, speed);
		}


	}




}

void Assets::addTexture(const std::string& name, const std::string path)
{
	sf::Texture tex;

	if (!tex.loadFromFile(path))
		std::cout << "Failed to load texture: " << name << std::endl;
	m_textures[name] = tex;
}

void Assets::addAnimation(const std::string& name, const std::string path)
{
	addAnimation(name, path, 1, 0);
}

void Assets::addAnimation(const std::string& name, const std::string path, size_t frameCount, size_t speed)
{
	sf::Texture tex;

	if (!tex.loadFromFile(path))
		std::cout << "Failed to load texture in animation : " << name << std::endl;

	auto result = m_textures.insert(std::make_pair(name, tex));

	if (result.second)
	{
		const sf::Texture& textureRef = result.first->second;
		Animation anim(name, textureRef, frameCount, speed);
		m_animations.insert(std::make_pair(name, anim));
	}
}

sf::Texture Assets::getTexture(const std::string& name)
{
	return m_textures[name];
}

Animation Assets::getAnimation(const std::string& name)
{
	return m_animations[name];
}
