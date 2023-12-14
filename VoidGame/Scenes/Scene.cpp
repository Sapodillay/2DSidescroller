#include "Scene.h"

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

Scene::Scene(GameEngine* gameEngine) : m_game(gameEngine), m_currentFrame(0) {}

void Scene::init()
{
}


void Scene::sDoAction(const Action& action)
{
}

void Scene::simulate(const size_t frames)
{
	//todo
}

void Scene::registerAction(sf::Keyboard::Key inputKey, const std::string& actionName)
{
	m_actionMap[static_cast<int>(inputKey)] = actionName;
}

void Scene::registerMouseAction(sf::Mouse::Button inputButton, const std::string& actionName)
{
	m_actionMap[static_cast<int>(inputButton)] = actionName;
}

void Scene::SetMousePosition(Vec2 position)
{
	m_mousePosition = position;
}

Vec2 Scene::GetMousePosition()
{
	return m_mousePosition;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

