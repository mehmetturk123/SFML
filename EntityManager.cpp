#include "EntityManager.h"
#include<algorithm>

void EntityManager::update()
{
	//TODO: add entities from a m_entitiesToAdd the proper location(s)
	// - add them to the vector of all entities
	// - add them to the vector inside the map, with the tag as a key
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	
	//remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	//remove dead entities from each vector in the entity map
	//C++17 way of iterating through [key,value] pairs in a map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}

	m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	//TODO: remove all dead entities from the input vector
	//this is called by the update() function
	auto it = std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) -> bool
		{
			return!(e->isActive());
		});
	vec.erase(it, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++,tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	// TODO: return the correct vector from the map
	return m_entityMap[tag];
}
