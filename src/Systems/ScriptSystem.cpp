#include "ScriptSystem.h"

std::tuple<double, double> GetEntityPosition(Entity entity) {
    if (!entity.HasComponent<TransformComponent>()) {
        Logger::Err("Trying to get the position of an entity that has no transform component");
        return std::make_tuple(0.0, 0.0);
    }
    const auto transform = entity.GetComponent<TransformComponent>();
    return std::make_tuple(transform.position.x, transform.position.y);
}

std::tuple<double, double> GetEntityVelocity(Entity entity) {
    if (!entity.HasComponent<RigidBodyComponent>()) {
        Logger::Err("Trying to get the velocity of an entity that has no rigidbody component");
        return std::make_tuple(0.0, 0.0);
    }
    const auto rigidbody = entity.GetComponent<RigidBodyComponent>();
    return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
}

void SetEntityPosition(Entity entity, double x, double y) {
    if (!entity.HasComponent<TransformComponent>()) {
        Logger::Err("Trying to set the position of an entity that has no transform component");
	return;
    }
    auto& transform = entity.GetComponent<TransformComponent>();
    transform.position.x = x;
    transform.position.y = y;
}

void SetEntityVelocity(Entity entity, double x, double y) {
    if (!entity.HasComponent<RigidBodyComponent>()) {
        Logger::Err("Trying to set the velocity of an entity that has no rigidbody component");
	return;
    }
    auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
    rigidbody.velocity.x = x;
    rigidbody.velocity.y = y;
}

void SetEntityRotation(Entity entity, double angle) {
    if (!entity.HasComponent<TransformComponent>()) {
        Logger::Err("Trying to set the rotation of an entity that has no transform component");
	return;
    }
    auto& transform = entity.GetComponent<TransformComponent>();
    transform.rotation = angle;
}

void SetEntityAnimationFrame(Entity entity, int frame) {
    if (!entity.HasComponent<AnimationComponent>()) {
        Logger::Err("Trying to set the animation frame of an entity that has no animation component");
	return;
    }
    auto& animation = entity.GetComponent<AnimationComponent>();
    animation.currentFrame = frame;
}

void SetProjectileVelocity(Entity entity, double x, double y) {
    if (!entity.HasComponent<ProjectileEmitterComponent>()) {
        Logger::Err("Trying to set the projectile velocity of an entity that has no projectile emitter component");
	return;
    }
    auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
    projectileEmitter.projectileVelocity.x = x;
    projectileEmitter.projectileVelocity.y = y;
}
