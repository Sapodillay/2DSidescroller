#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();

	removeDeadEntities(m_entities);

	//C++17
	//for (auto& [tag, entityVec] : m_entityMap)
	//{
	//	removeDeadEntities(entityVec);
	//}



}

void EntityManager::removeDeadEntities(EntityVec& vec)
{

	for (auto e : vec)
	{
		if (!e->isActive())
		{
			//todo

		}
	}



}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
	m_entitiesToAdd.push_back(e);
	return e;
}

EntityVec& EntityManager::getEntities()
{
	// TODO: insert return statement here
	return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}
