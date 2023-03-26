#ifndef ECS_H
#define ECS_H

#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <algorithm>
#include <deque>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
	static int nextId;
};

template <typename T>
class Component: public BaseComponent {
public:
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	int id;

public:
	Entity(int id): id(id) {};
	Entity(const Entity &entity) = default;

	void Kill();
	int GetId() const;

	void Tag(const std::string &tag);
	bool HasTag(const std::string &tag) const;
	void Group(const std::string &group);
	bool InGroup(const std::string &group) const;

	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	template <typename T, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename T> void RemoveComponent();
	template <typename T> bool HasComponent() const;
	template <typename T> T& GetComponent() const;

	class EntityManager *entityManager;
};

class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;

	void AddEntitySystem(Entity entity);
	void RemoveEntitySystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	template <typename T> void RequireComponent();
};

class PoolBase {
public:
	virtual ~PoolBase() = default;
	virtual void RemoveEntityFromPool(size_t entityId) = 0;
};

template <typename T>
class Pool: public PoolBase {
private:
	std::vector<T> data;
	size_t size;

	std::unordered_map<size_t, size_t> entityIdToIdx;
	std::unordered_map<size_t, size_t> idxToEntityId;

public:
	Pool(size_t capacity = 100) {
		size = 0;
		data.resize(capacity);
	}
	virtual ~Pool() = default;

	bool IsEmpty() const {
		return size == 0;
	}

	void Resize(size_t n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
		size = 0;
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(size_t entityId, T object) {
		if (entityIdToIdx.find(entityId) != entityIdToIdx.end()) {
			size_t idx = entityIdToIdx[entityId];
			data[idx] = object;
			return;
		}

		size_t idx = size;
		entityIdToIdx.emplace(entityId, idx);
		idxToEntityId.emplace(idx, entityId);

		if (idx >= data.capacity()) data.resize(size*2);

		data[idx] = object;
		size++;
	}

	void Remove(size_t entityId) {
		size_t idxOfRemoved = entityIdToIdx[entityId];
		size_t idxOfLast = size-1;
		data[idxOfRemoved] = data[idxOfLast];

		size_t entityIdOfLastElement = idxToEntityId[idxOfLast];
		entityIdToIdx[entityIdOfLastElement] = idxOfRemoved;
		idxToEntityId[idxOfRemoved] = entityIdOfLastElement;

		entityIdToIdx.erase(entityId);
		idxToEntityId.erase(idxOfLast);

		size--;
	}

	void RemoveEntityFromPool(size_t entityId) override {
		if (entityIdToIdx.find(entityId) != entityIdToIdx.end()) {
			Remove(entityId);
		}
	}

	T& Get(size_t entityId) {
		size_t idx = entityIdToIdx[entityId];
		return static_cast<T&>(data[idx]);
	}

	T& operator [](size_t idx) {
		return data[idx];
	}
};

class EntityManager {
private:
	int numEntities = 0;
	std::vector<std::shared_ptr<PoolBase>> componentPools;
	std::vector<Signature> entityComponentSignatures;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

	std::deque<int> freeIds;

	std::unordered_map<std::string, Entity> entityByTag;
	std::unordered_map<int, std::string> tagByEntity;

	std::unordered_map<std::string, std::set<Entity>> entitiesByGroup;
	std::unordered_map<int, std::string> groupByEntity;

public:
	EntityManager() = default;

	void Update();

	Entity CreateEntity();
	void KillEntity(Entity entity);
	size_t NumEntites() const;

	void TagEntity(Entity entity, const std::string &tag);
	bool EntityHasTag(Entity entity, const std::string &tag) const;
	Entity GetEntityByTag(const std::string &tag) const;
	void RemoveEntityTag(Entity entity);

	void GroupEntity(Entity entity, const std::string &group);
	bool EntityInGroup(Entity entity, const std::string &group) const;
	std::vector<Entity> GetEntitiesByGroup(const std::string &group) const;
	void RemoveEntityroup(Entity entity);

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	template <typename T, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename T> void RemoveSystem();
	template <typename T> bool HasSystem() const;
	template <typename T> T& GetSystem() const;

	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);
};

template <typename T>
void System::RequireComponent() {
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componentId);
}

template <typename T, typename ...TArgs>
void EntityManager::AddSystem(TArgs&& ...args) {
	std::shared_ptr<T> newSystem = std::make_shared<T>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(T)), newSystem));
}

template <typename T>
void EntityManager::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(T)));
	systems.erase(system);
}

template <typename T>
bool EntityManager::HasSystem() const {
	return systems.find(std::type_index(typeid(T))) != systems.end();
}

template <typename T>
T& EntityManager::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(T)));
	return *(std::static_pointer_cast<T>(system->second));
}

template <typename T, typename ...TArgs>
void EntityManager::AddComponent(Entity entity, TArgs&& ...args) {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>();
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

	T newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);

	Logger::Info("component id = " + std::to_string(componentId) + " was added to entity id = " + std::to_string(entityId));
}

template <typename T>
void EntityManager::RemoveComponent(Entity entity) {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
	componentPool->Remove(entityId);

	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Info("component id = " + std::to_string(componentId) + " was removed from entity id = " + std::to_string(entityId));
}

template <typename T>
bool EntityManager::HasComponent(Entity entity) const {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename T>
T& EntityManager::GetComponent(Entity entity) const {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template <typename T, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
	entityManager->AddComponent<T>(*this, std::forward<TArgs>(args)...);
}

template <typename T>
void Entity::RemoveComponent() {
	entityManager->RemoveComponent<T>(*this);
}

template <typename T>
bool Entity::HasComponent() const {
	return entityManager->HasComponent<T>(*this);
}

template <typename T>
T& Entity::GetComponent() const {
	return entityManager->GetComponent<T>(*this);
}

#endif // ECS_H

