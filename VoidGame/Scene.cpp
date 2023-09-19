#include "Scene.h"

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

Scene::Scene(GameEngine* gameEngine) : m_game(gameEngine), m_currentFrame(0) {}


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

