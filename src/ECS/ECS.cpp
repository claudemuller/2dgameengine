#include "ECS.h"
#include "../Logger/Logger.h"

int BaseComponent::nextId = 0;

int Entity::GetId() const {
	return id;
};

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


Entity EntityManager::CreatEntity() {
	int entityId = numEntities++;
	Entity entity(entityId);
	entitiesToBeAdded.insert(entity);

	Logger::Info("Entity created with id = " + std::to_string(entityId));

	return entity;
}

void EntityManager::Update() {

}

void EntityManager::AddEntityToSystem(Entity entity) {

}
