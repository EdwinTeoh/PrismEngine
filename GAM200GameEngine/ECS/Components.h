#pragma once

#include "Transform.h"
#include "Sprite.h"
#include "Rigidbody.h"
#include "RectCollider.h"
#include "CircleCollider.h"
#include "LineCollider.h"
#include "Patrol.h"
#include "VacuumC.h"
#include "Platform.h"
#include "ObstacleComponent.h"
#include "Attack.h"
#include "Vacuumable.h"
#include "Audio.h"
#include "ParticleSysC.h"

#define TransformComponent      Engine::ECS::Components::Transform
#define SpriteComponent         Engine::ECS::Components::Sprite
#define RigidBodyComponent      Engine::ECS::Components::RigidBody2D
#define RectColliderComponent   Engine::ECS::Components::RectCollider
#define CircleColliderComponent Engine::ECS::Components::CircleCollider
#define LineColliderComponent   Engine::ECS::Components::LineCollider
#define PatrolComponent         Engine::ECS::Components::Patrol
#define VacuumComponent         Engine::ECS::Components::VacuumC
#define PlatformComponent       Engine::ECS::Components::Platform
#define ObstacleComponent       Engine::ECS::Components::ObstacleComponent
#define AttackComponent			Engine::ECS::Components::Attack
#define VacuumableComponent		Engine::ECS::Components::Vacuumable
#define AudioComponent			Engine::ECS::Components::AudioComp
#define ParticleComponent		Engine::ECS::Components::ParticleSystem