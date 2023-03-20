#ifndef ECS_H
#define ECS_H

#include <vector>
#include <bitset>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
	static int nextId;
};

template <typename T>
class Component: public BaseComponent {
private:

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
	int GetId() const;
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

class Registry {

};

template <typename T>
void System::RequireComponent() {
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componentId);
}

#endif // ECS_H
