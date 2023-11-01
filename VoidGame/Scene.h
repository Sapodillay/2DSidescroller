#pragma once

#include "EntityManager.h"
#include "Action.h"
#include <memory>
#include <SFML/Graphics.hpp>

class GameEngine;

typedef std::map<int, std::string> ActionMap;


class Scene
{

protected:
	GameEngine* m_game = nullptr;
	EntityManager m_entityManager;
	ActionMap m_actionMap;
	bool m_paused = false;
	bool m_hasEnded = false;
	size_t m_currentFrame = 0;
	Vec2 m_mousePosition;

	virtual void onEnd() = 0;
	void setPaused(bool paused);

public:
	Scene();
	Scene(GameEngine* gameEngine);

	virtual void update() = 0;
	virtual void sDoAction(const Action& action);
	void simulate(const size_t frames);
	void registerAction(sf::Keyboard::Key inputKey, const std::string& actionName);
	void registerMouseAction(sf::Mouse::Button inputButton, const std::string& actionName);

	void SetMousePosition(Vec2 position);
	Vec2 GetMousePosition();

	//size_t width() const;
	//size_t height() const;
	size_t currentFrame() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	void drawLine(const Vec2& p1, const Vec2& p2);



};

