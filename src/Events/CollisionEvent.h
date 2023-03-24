#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class CollisionEvent: public Event {
public:
	Entity a;
	Entity b;

	CollisionEvent(Entity a, Entity b): a(a), b(b) {}
};

#endif // COLLISION_EVENT_H
