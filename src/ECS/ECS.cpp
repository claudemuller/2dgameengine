#include "ECS.h"

int Entity::GetId() const {
	return id;
};


void System::AddEntitySystem(Entity entity) {
	entities.push_back(entity);
}

void System::RemoveEntitySystem(Entity entity) {
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}
