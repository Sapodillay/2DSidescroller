#pragma once

#include "Entity.h"
#include <string>
#include <Memory>
typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;
class EntityManager
{
	EntityVec m_entities;
	EntityVec m_entitiesToAdd;
	EntityMap m_entityMap;

	size_t m_totalEntities = 0;

public:
	EntityManager();
	void update();
	void removeDeadEntities(EntityVec& vec);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	//gets all entities.
	EntityVec& getEntities();
	//gets all entities given a tag,
	EntityVec& getEntities(const std::string& tag);


};

