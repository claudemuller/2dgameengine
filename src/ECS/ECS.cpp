#include "ECS.h"
#include "../Logger/Logger.h"

int BaseComponent::nextId = 0;

void Entity::Kill() {
	entityManager->KillEntity(*this);
}

int Entity::GetId() const {
	return id;
};

void Entity::Tag(const std::string &tag) {
	entityManager->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string &tag) const {
	return entityManager->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string &group) {
	entityManager->GroupEntity(*this, group);
}

bool Entity::InGroup(const std::string &group) const {
	return entityManager->EntityInGroup(*this, group);
}

void System::AddEntitySystem(Entity entity) {
	entities.push_back(entity);
}

void System::RemoveEntitySystem(Entity entity) {
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}

Entity EntityManager::CreateEntity() {
	int entityId;

	if (freeIds.empty()) {
		entityId = numEntities++;
		if (entityId >= entityComponentSignatures.size()) {
			entityComponentSignatures.resize(entityId + 1);
		}
	} else {
		entityId = freeIds.front();
		freeIds.pop_front();
	}

	Entity entity(entityId);
	entity.entityManager = this;
	entitiesToBeAdded.insert(entity);

	Logger::Info("entity created with id = " + std::to_string(entityId));

	return entity;
}

void EntityManager::KillEntity(Entity entity) {
	entitiesToBeKilled.insert(entity);
}

size_t EntityManager::NumEntites() const {
	return numEntities;
}

void EntityManager::TagEntity(Entity entity, const std::string &tag) {
	entityByTag.emplace(tag, entity);
	tagByEntity.emplace(entity.GetId(), tag);
}

bool EntityManager::EntityHasTag(Entity entity, const std::string &tag) const {
	if (tagByEntity.find(entity.GetId()) == tagByEntity.end()) {
		return false;
	}
	return entityByTag.find(tag)->second == entity;
}

Entity EntityManager::GetEntityByTag(const std::string &tag) const {
	return entityByTag.at(tag);
}

void EntityManager::RemoveEntityTag(Entity entity) {
	auto taggedEntity = tagByEntity.find(entity.GetId());
	if (taggedEntity != tagByEntity.end()) {
		auto tag = taggedEntity->second;
		entityByTag.erase(tag);
		tagByEntity.erase(taggedEntity);
	}
}

void EntityManager::GroupEntity(Entity entity, const std::string &group) {
	entitiesByGroup.emplace(group, std::set<Entity>());
	entitiesByGroup[group].emplace(entity);
	groupByEntity.emplace(entity.GetId(), group);
}

bool EntityManager::EntityInGroup(Entity entity, const std::string &group) const {
	if (entitiesByGroup.find(group) == entitiesByGroup.end()) {
		return false;
	}
	auto groupEntities = entitiesByGroup.at(group);
	return groupEntities.find(entity.GetId()) != groupEntities.end();
}

std::vector<Entity> EntityManager::GetEntitiesByGroup(const std::string &group) const {
	if (entitiesByGroup.find(group) == entitiesByGroup.end()) {
		return std::vector<Entity>();
	}
	auto &setOfEntities = entitiesByGroup.at(group);
	return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void EntityManager::RemoveEntityroup(Entity entity) {
	auto groupedEntity = groupByEntity.find(entity.GetId());
	if (groupedEntity != groupByEntity.end()) {
		auto group = entitiesByGroup.find(groupedEntity->second);
		if (group != entitiesByGroup.end()) {
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end()) {
				group->second.erase(entityInGroup);
			}
		}
		groupByEntity.erase(groupedEntity);
	}
}

void EntityManager::AddEntityToSystems(Entity entity) {
	const auto entityId = entity.GetId();
	const auto &entityComponentSignature = entityComponentSignatures[entityId];

	for (auto &system: systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
		if (isInterested) {
			system.second->AddEntitySystem(entity);
		}
	}
}

void EntityManager::RemoveEntityFromSystems(Entity entity) {
	for (auto system: systems) {
		system.second->RemoveEntitySystem(entity);
	}
}

void EntityManager::Update() {
	for (auto entity: entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	for (auto entity: entitiesToBeKilled) {
		RemoveEntityFromSystems(entity);
		entityComponentSignatures[entity.GetId()].reset();

		for (auto pool: componentPools) {
			if (pool) pool->RemoveEntityFromPool(entity.GetId());
		}

		freeIds.push_back(entity.GetId());

		RemoveEntityTag(entity);
		RemoveEntityroup(entity);
	}
	entitiesToBeKilled.clear();
}
