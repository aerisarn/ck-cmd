#include "HkxSimulation.h"

#include <QCoreApplication>
#include <QThread>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <cstdio>

// Physics
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics/Dynamics/World/hkpWorld.h>

#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Dynamics/Phantom/hkpSimpleShapePhantom.h>

// Animation
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>

class CallBackTimer
{
    hkMemoryRouter* _router;
public:
    CallBackTimer(hkMemoryRouter* router)
        :_execute(false),
        _router(router)
    {}

    ~CallBackTimer() {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        };
    }

    void stop()
    {
        _execute.store(false, std::memory_order_release);
        if (_thd.joinable())
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func)
    {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        };
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval, func]()
            {
                hkBaseSystem::initThread(_router);
                while (_execute.load(std::memory_order_acquire)) {
                    func();
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(interval));
                }
                hkBaseSystem::quitThread();
            });
    }

    bool is_running() const noexcept {
        return (_execute.load(std::memory_order_acquire) &&
            _thd.joinable());
    }

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};


HkxSimulation::HkxSimulation(hkMemoryRouter* router, ckcmd::HKX::ResourceManager& manager) :
	_timeStep(1.0f / 60.f), // 60 fps
	_manager(manager)
{
	_world = new hkpWorld(hkpWorldCinfo());

	// Register all collision agents
	hkpAgentRegisterUtil::registerAllAgents(_world->getCollisionDispatcher());

	// Register all the physics viewers
	hkpPhysicsContext::registerAllPhysicsProcesses();

	// Set up a physics context containing the world for the use in the visual debugger
	_context = new hkpPhysicsContext;
	_context->addWorld(_world);

	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(_context);

	_visualDebugger = new hkVisualDebugger(contexts);
	_visualDebugger->serve();

	_worldTimer = new CallBackTimer(router);
    //_worldTimer->start((int)round(_timeStep * 1000.), std::bind(&HkxSimulation::worldStep, this));
}

void HkxSimulation::worldStep()
{
	//// Do a simulation step
	_world->stepDeltaTime(_timeStep);
	//// Step the debugger
	_visualDebugger->step();

	QCoreApplication::processEvents();
}

HkxSimulation::~HkxSimulation()
{
	// Release the reference on the world
	//_world->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	//_context->removeReference();

	_worldTimer->stop();
	delete _worldTimer;
}


//
	// Create Controller
	//
void HkxSimulation::CreateController(CharacterContext& character)
{

	// Construct a shape

	const hkReal totalHeight = 1.7f;
	const hkReal radius = .4f;
	const hkReal capsulePoint = totalHeight * 0.5f - radius;

	hkVector4 vertexA(0, 0, capsulePoint);
	hkVector4 vertexB(0, 0, -capsulePoint);

	// Create a capsule to represent the character standing
	character._standShape = new hkpCapsuleShape(vertexA, vertexB, radius);

		// Construct a Shape Phantom
	character._phantom = new hkpSimpleShapePhantom(character._standShape, hkTransform::getIdentity());

		// Add the phantom to the world
	_world->addPhantom(character._phantom);

		// Fade out capsule so we can see character
#ifdef HK_DEBUG
	HK_SET_OBJECT_COLOR((hkUlong)character._phantom->getCollidable(), hkColor::rgbFromChars(240, 200, 0, 20));
#else
	HK_REMOVE_GEOMETRY((hkUlong)character._phantom->getCollidable());
#endif

	character._phantom->removeReference();

		// Construct a character proxy
	hkpCharacterProxyCinfo cpci;
	cpci.m_position.set(-6, 25, 1);
	//cpci.m_position.set(-5, 20, 1);
	cpci.m_staticFriction = 0.0f;
	cpci.m_dynamicFriction = 1.0f;
	cpci.m_up.setNeg4(_world->getGravity());
	cpci.m_up.normalize3();
	cpci.m_userPlanes = 4;
	cpci.m_maxSlope = HK_REAL_PI / 3.f;

	cpci.m_shapePhantom = character._phantom;
	character._characterProxy = new hkpCharacterProxy(cpci);
}

void HkxSimulation::addSkeleton(const std::string& skeleton)
{
	CharacterContext character;
	CreateController(character);

	auto& resources = _manager.get(skeleton);
	hkRootLevelContainer* container = (hkRootLevelContainer*)resources.first.m_object;
	hkaAnimationContainer* ac = 
		reinterpret_cast<hkaAnimationContainer*>(container->findObjectByType(hkaAnimationContainerClass.getName()));

	hkpPhysicsData* physics = 
		reinterpret_cast<hkpPhysicsData*>(container->findObjectByType(hkpPhysicsDataClass.getName()));
	const hkArray<hkpPhysicsSystem*>& psys = physics->getPhysicsSystems();
	for (int i = 0; i < psys.getSize(); i++)
	{
		hkpPhysicsSystem* system = psys[i];
		// add the lot to the world
		_world->addPhysicsSystem(system);
	}
	character._animatedSkeleton = new hkaAnimatedSkeleton(ac->m_skeletons[0]);

	_visualDebugger->step();

}

//void addAnimation();
//{
//	
//	// Create the animated skeleton for the girl
//	m_characterContext.m_animatedSkeleton = new hkaAnimatedSkeleton(girlSkeleton);
//
//	// Run Animation
//	{
//		hkStringBuf assetFile("Resources/Animation/HavokGirl/hkRunLoop.hkx"); hkAssetManagementUtil::getFilePath(assetFile);
//		hkRootLevelContainer* container = m_loader->load(assetFile.cString());
//		HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
//		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>(container->findObjectByType(hkaAnimationContainerClass.getName()));
//
//		HK_ASSERT2(0x27343435, ac && (ac->m_animations.getSize() > 0), "No animation loaded");
//		HK_ASSERT2(0x27343435, ac && (ac->m_bindings.getSize() > 0), "No binding loaded");
//		hkaAnimationBinding* runBinding = ac->m_bindings[0];
//
//		m_characterContext.m_walkControl = new hkaDefaultAnimationControl(runBinding);
//		m_characterContext.m_walkControl->setMasterWeight(1.0f);
//		m_characterContext.m_walkControl->setPlaybackSpeed(1.0f);
//		m_characterContext.m_walkControl->easeOut(0.0f);
//		m_characterContext.m_walkControl->setEaseInCurve(0, 0, 1, 1);	// Smooth
//		m_characterContext.m_walkControl->setEaseOutCurve(1, 1, 0, 0);	// Smooth
//		m_characterContext.m_animatedSkeleton->addAnimationControl(m_characterContext.m_walkControl);
//		m_characterContext.m_walkControl->removeReference();
//	}
//}