#pragma once

#include <vector>

#include <src/hkx/ResourceManager.h>

class hkpWorld;
class hkpPhysicsContext;

class hkVisualDebugger;
class CallBackTimer;
class hkMemoryRouter;

class hkpShape;
class hkpShapePhantom;
class hkpCharacterProxy;
class hkaAnimatedSkeleton;
class hkaRagdollInstance;
class hkaSkeletonMapper;


class HkxSimulation {

	struct CharacterContext
	{
		hkpShape* _standShape;
		hkpShapePhantom* _phantom;
		hkpCharacterProxy* _characterProxy;
		//Animation
		hkaAnimatedSkeleton* _animatedSkeleton;
		//Physics
		hkaRagdollInstance* _ragdollInstance;
		hkaSkeletonMapper* _characterToRagdollMapper;
		hkaSkeletonMapper* _ragdollToCharacterMapper;
	};

	hkpWorld* _world;
	hkpPhysicsContext* _context;
	hkVisualDebugger* _visualDebugger;
	CallBackTimer* _worldTimer;
	ckcmd::HKX::ResourceManager& _manager;
	std::vector<CharacterContext> characters;

	const float _timeStep;

	void worldStep();

	void CreateController(CharacterContext& character);

public:
	HkxSimulation(hkMemoryRouter* router, ckcmd::HKX::ResourceManager& manager);
	~HkxSimulation();

	void addSkeleton(const std::string& skeleton);
};