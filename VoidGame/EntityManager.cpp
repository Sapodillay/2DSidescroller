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


	for (auto& e : m_entitesToRemove)
	{
		auto i = std::find(m_entities.begin(), m_entities.end(), e);
		if (i != m_entities.end()) {
			m_entities.erase(i);
			std::cout << "Deleted" << std::endl;
		}

		auto mapIter = m_entityMap.find(e->tag());
		if (mapIter != m_entityMap.end()) {
			auto& entityList = mapIter->second;
			auto listerIter = std::find(entityList.begin(), entityList.end(), e);
			if (listerIter != entityList.end()) {
				entityList.erase(listerIter);
			}
		}
	}
	m_entitesToRemove.clear();
}

void EntityManager::deleteEntity(std::shared_ptr<Entity> e)
{
	m_entitesToRemove.push_back(e);
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
