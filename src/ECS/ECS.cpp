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

	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Info("Entity created with id = " + std::to_string(entityId));

	return entity;
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

void EntityManager::Update() {
	for (auto entity: entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// for (auto entity: entitiesToBeKilled) {
	// 	Rem(entity);
	// }
	// entitiesToBeKilled.clear();
}

