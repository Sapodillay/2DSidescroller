#pragma once

#include "Scenes/Scene.h"
#include "Assets.h"

#include <SFML/Graphics.hpp>


typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;


class GameEngine
{
	sf::RenderWindow m_window;
	Assets m_assets;
	std::string m_currentScene;
	SceneMap m_sceneMap;
	size_t m_simulationSpeed = 1;
	
	bool m_running = true;

	void init(const std::string& path);
	void update();

	void sUserInput();

	std::shared_ptr<Scene> currentScene();

public:
	sf::Clock deltaClock;
	GameEngine(const std::string& path);

	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
	//change to an already existing scene.
	void changeScene(const std::string& sceneName);

	void quit();
	void run();

	sf::RenderWindow& window();
	Assets& getAssets();
	bool isRunning();

};

