#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ProjectileEmitterComponent.h"

std::tuple<double, double> GetEntityPosition(Entity entity);
std::tuple<double, double> GetEntityVelocity(Entity entity);
void SetEntityPosition(Entity entity, double x, double y);
void SetEntityVelocity(Entity entity, double x, double y);
void SetEntityRotation(Entity entity, double angle);
void SetEntityAnimationFrame(Entity entity, int frame);
void SetProjectileVelocity(Entity entity, double x, double y);

class ScriptSystem: public System {
public:
	ScriptSystem() {
		RequireComponent<ScriptComponent>();
	}

	void CreateScriptBindings(sol::state &lua) {
		lua.new_usertype<Entity>(
				"entity",
				"get_id", &Entity::GetId,
				"destroy", &Entity::Kill,
				"has_tag", &Entity::HasTag,
				"in_group", &Entity::InGroup
		);

            lua.set_function("get_position", GetEntityPosition);
            lua.set_function("get_velocity", GetEntityVelocity);
            lua.set_function("set_position", SetEntityPosition);
            lua.set_function("set_velocity", SetEntityVelocity);
            lua.set_function("set_rotation", SetEntityRotation);
            lua.set_function("set_projectile_velocity", SetProjectileVelocity);
            lua.set_function("set_animation_frame", SetEntityAnimationFrame);
	}

	void Update(double dt, int ellapsedTime) {
		for (auto entity: GetSystemEntities()) {
			const auto script = entity.GetComponent<ScriptComponent>();
			script.func(entity, dt, ellapsedTime);
		}
	}
};

#endif // SCRIPT_SYSTEM_H
