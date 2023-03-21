#ifndef ECS_H
#define ECS_H

#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
	static int nextId;
};

template <typename T>
class Component: public BaseComponent {
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
	int GetId() const;

	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
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
	virtual ~PoolBase() {}
};

template <typename T>
class Pool: public PoolBase {
private:
	std::vector<T> data;

public:
	Pool(int size = 100) {
		data.resize(size);
	}
	virtual ~Pool() = default;

	bool IsEmpty() const {
		return data.empty();
	}

	int GetSize() const {
		return data.size();
	}

	void Resize(int n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(int idx, T object) {
		data[idx] = object;
	}

	T& Get(int idx) {
		return static_cast<T>(data[idx]);
	}

	T& operator [](unsigned int idx) {
		return data[idx];
	}
};

class EntityManager {
private:
	int numEntities = 0;
	std::vector<PoolBase*> componentPools;
	std::vector<Signature> entityComponentSignatures;
	std::unordered_map<std::type_index, System*> systems;
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

public:
	EntityManager() = default;

	void Update();

	Entity CreatEntity();

	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	template <typename T, typename ...TArgs> void AddEntityToSystem(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveSystem();
	template <typename T> bool HasSystem() const;
	template <typename T> T& GetSystem() const;

	void AddEntityToSystems(Entity entity);
};

template <typename T>
void System::RequireComponent() {
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componentId);
}

template <typename T, typename ...TArgs>
void EntityManager::AddEntityToSystem(Entity entity, TArgs&& ...args) {
	T* newSystem(new T(std::forward<TArgs>(args)...));
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
		Pool<T>* newComponentPool = new Pool<T>();
		componentPools[componentId] = newComponentPool;
	}

	Pool<T>* componentPool = componentPools[componentId];

	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	T newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);
}

template <typename T>
void EntityManager::RemoveComponent(Entity entity) {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	entityComponentSignatures[entityId].set(componentId, false);
}

template <typename T>
bool EntityManager::HasComponent(Entity entity) const {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

#endif // ECS_H

