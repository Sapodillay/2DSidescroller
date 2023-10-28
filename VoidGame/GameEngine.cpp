#include "GameEngine.h"
#include "Scene_Play.h"

#include <iostream>


GameEngine::GameEngine(const std::string& filename)
{
	init(filename);
}


void GameEngine::init(const std::string& path)
{
	//load assets from file.
	m_assets.loadFromFile("config/AssetFiles.txt");

	m_window.create(sf::VideoMode(1280, 768), "Void Game");
	m_window.setFramerateLimit(60);

	changeScene("GAME", std::make_shared<Scene_Play>(this, path));

}

void GameEngine::run()
{
	while (m_running)
	{
		update();
	}

}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

Assets& GameEngine::getAssets()
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running;
}

void GameEngine::update()
{	
	sUserInput();
	currentScene()->update();

}


std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}


void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene != nullptr)
	{
		m_sceneMap[sceneName] = scene;
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cout << "Scene" << sceneName << " was not found" << std::endl;
			return;
		}
		if (endCurrentScene)
		{
			m_sceneMap.erase(m_sceneMap.find(m_currentScene));
		}
	}
	m_currentScene = sceneName;
}



void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{

			ActionMap actionMap = currentScene()->getActionMap();

			if (actionMap.find(event.key.code) == actionMap.end()) { continue; }
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			currentScene()->sDoAction(Action(actionMap.at(event.key.code), actionType));
		}



	}
}


void GameEngine::quit()
{
	m_running = false;
}