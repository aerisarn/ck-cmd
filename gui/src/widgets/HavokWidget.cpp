#include <src/widgets/HavokWidget.h>

constexpr int FPS_LIMIT = 60.0f;
constexpr int MS_PER_FRAME = (int)((1.0f / FPS_LIMIT) * 1000.0f);

#include <Graphics/Common/hkGraphics.h>
#include <Graphics/Common/Camera/hkgCamera.h>
#include <Graphics/Common/Texture/SkyBox/hkgSkyBox.h>
#include <Graphics/Common/Window/hkgWindow.h>
#include <Graphics/Common/DisplayWorld/hkgDisplayWorld.h>

#include <Graphics/Bridge/System/hkgSystem.h>
#include <Graphics/Bridge/DisplayHandler/hkgDisplayHandler.h>
#include <Graphics/Bridge/SceneData/hkgSceneDataConverter.h>
#include <Graphics/Common/Light/hkgLightManager.h>
#include <Graphics/Common/Material/hkgMaterial.h>
#include <Common\Visualize\hkDebugDisplay.h>

#include <Common/Base/Memory/System/hkMemorySystem.h>

#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>

// We need to create bodies
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Common\GeometryUtilities\Inertia\hkInertiaTensorComputer.h>
#include <Physics\Utilities\Dynamics\Inertia\hkpInertiaTensorComputer.h>

#include <QApplication>

#include <filesystem>
namespace fs = std::filesystem;

HavokWidget::HavokWidget(QWidget* parent) : ads::CDockWidget("Preview", parent)
{
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_NativeWindow);

	// Setting these attributes to our widget and returning null on paintEngine event
	// tells Qt that we'll handle all drawing and updating the widget ourselves.
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);

	initialize();
	connect(&m_qTimer, &QTimer::timeout, this, &HavokWidget::paint);
	m_qTimer.start(MS_PER_FRAME);
}



void HavokWidget::setupScene()
{
	//m_loader = new hkLoader();
	//// Get and convert the scene - this is just a ground grid plane and a camera
	//fs::path scene_path = fs::path(qApp->applicationDirPath().toUtf8().constData()) / "hkScene.hkx";
	//hkRootLevelContainer* container = m_loader->load(scene_path.string().c_str());
	//HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");
	//hkxScene* scene = reinterpret_cast<hkxScene*>(container->findObjectByType(hkxSceneClass.getName()));

	//HK_ASSERT2(0x27343635, scene, "No scene loaded");
	////removeLights(m_env); // assume we have some in the file
	//m_sceneConverter->convert(scene);



}

void HavokWidget::setupFixedShadowFrustum(const hkgLight& light, const hkgAabb& areaOfInterest, float extraNear, float extraFar, int numSplits, int preferedUpAxis)
{
	hkgCamera* lightCam = hkgCamera::createFixedShadowFrustumCamera(light, areaOfInterest, true, extraNear, extraFar, preferedUpAxis);

	HKG_SHADOWMAP_SUPPORT shadowSupport = m_window->getShadowMapSupport();
	if ((numSplits > 0) && (shadowSupport == HKG_SHADOWMAP_VSM))
	{
		m_window->setShadowMapSplits(numSplits); // > 0 and you are requesting PSVSM (if the platforms supports VSM that is)
		m_window->setShadowMapMode(HKG_SHADOWMAP_MODE_PSVSM, lightCam);
	}
	else
	{
		if ((numSplits > 0) && (shadowSupport != HKG_SHADOWMAP_NOSUPPORT))
		{
			static int once = 0;
			if (once == 0)
			{
				HK_WARN_ALWAYS(0x0, "The demo is requesting PSVSM shadows, but VSM is not supported, so just reverting to normal single map, fixed projection.");
				once = 1;
			}
		}

		m_window->setShadowMapMode(HKG_SHADOWMAP_MODE_FIXED, lightCam);
	}

	lightCam->removeReference();
}

void HavokWidget::setupLights()
{
	// make some default lights
	hkgLightManager* lm = m_displayWorld->getLightManager();

	if (!lm)
	{
		lm = hkgLightManager::create();
		m_displayWorld->setLightManager(lm);
		lm->release();
		lm->lock();
	}
	else
	{
		lm->lock();
		// clear out the lights currently in the world.
		while (lm->getNumLights() > 0)
		{
			hkgLight* l = lm->removeLight(0); // gives back reference
			l->release();
		}
	}

	// Background color
	float bg[4] = { 0.53f, 0.55f, 0.61f, 1 };
	m_window->setClearColor(bg);

	float v[4]; v[3] = 255;
	hkgLight* light;


	// the sun (direction downwards)
	{
		light = hkgLight::create();
		light->setType(HKG_LIGHT_DIRECTIONAL);
		v[0] = 256;
		v[1] = 256;
		v[2] = 256;
		v[0] /= 255; v[1] /= 255; v[2] /= 255;
		light->setDiffuse(v);
		light->setSpecular(v);
		v[0] = 0;
		v[1] = -1;
		v[2] = -0.5f;
		light->setDirection(v);
		v[0] = 0;
		v[1] = 1000;
		v[2] = 0;
		light->setPosition(v);
		light->setDesiredEnabledState(true);
		lm->addLight(light);

		// float shadowPlane[] = { 0,1,0,-0.01f };
		// light->addShadowPlane( shadowPlane );
		light->release();
	}

	hkgAabb areaOfInterest;
	areaOfInterest.m_max[0] = 10;
	areaOfInterest.m_max[1] = 10;
	areaOfInterest.m_max[2] = 10;
	areaOfInterest.m_min[0] = -10;
	areaOfInterest.m_min[1] = -10;
	areaOfInterest.m_min[2] = -10;
	setupFixedShadowFrustum(*light, areaOfInterest);

	// if se have shadow maps we only support on light by default (or else it looks dodge)
	//if (!m_enableShadows || (env->m_window->getShadowMapSupport() == HKG_SHADOWMAP_NOSUPPORT))
	//{
	//	// fill 1 - blue
	//	{
	//		light = hkgLight::create();
	//		light->setType(HKG_LIGHT_DIRECTIONAL);
	//		v[0] = 200;
	//		v[1] = 200;
	//		v[2] = 240;
	//		v[0] /= 255; v[1] /= 255; v[2] /= 255;
	//		light->setDiffuse(v);
	//		v[0] = 1;
	//		v[1] = 1;
	//		v[2] = 1;
	//		light->setDirection(v);
	//		v[0] = -1000;
	//		v[1] = -1000;
	//		v[2] = -1000;
	//		light->setPosition(v);
	//		light->setDesiredEnabledState(true);
	//		lm->addLight(light);
	//		light->release();
	//	}

	//	// fill 2 - yellow
	//	{
	//		light = hkgLight::create();
	//		light->setType(HKG_LIGHT_DIRECTIONAL);
	//		v[0] = 240;
	//		v[1] = 240;
	//		v[2] = 200;
	//		v[0] /= 255; v[1] /= 255; v[2] /= 255;
	//		light->setDiffuse(v);
	//		v[0] = -1;
	//		v[1] = 1;
	//		v[2] = -1;
	//		light->setDirection(v);
	//		v[0] = 1000;
	//		v[1] = -1000;
	//		v[2] = 1000;
	//		light->setPosition(v);
	//		light->setDesiredEnabledState(true);
	//		lm->addLight(light);
	//		light->release();
	//	}
	//}

	lm->computeActiveSet(HKG_VEC3_ZERO);
	lm->unlock();
}

void HavokWidget::initialize()
{
	hkgSystem::init("d3d9s");
	m_window = hkgWindow::create();
	m_window->setWantDrawHavokLogo(false);
	m_window->setWantDrawMousePointer(false);
	m_window->setShadowMapSize(0); // 0 == use default platform size
	HKG_WINDOW_CREATE_FLAG windowFlags = HKG_WINDOW_WINDOWED | HKG_WINDOW_MSAA;

	m_window->initialize(windowFlags,
		HKG_WINDOW_BUF_COLOR | HKG_WINDOW_BUF_DEPTH32, size().width(), size().height(),
		"Preview", (void*)QWidget::winId());
	m_window->getContext()->lock();

	// don't allow viewport resizing
	m_window->setWantViewportBorders(false);
	m_window->setWantViewportResizeByMouse(false);

	m_window->getViewport(0)->setNavigationMode(HKG_CAMERA_NAV_TRACKBALL);
	m_window->getViewport(0)->setMouseConvention(HKG_MC_MAYA);

	for (int i = 0; i < 2; ++i)
	{
		m_window->clearBuffers();
		m_window->swapBuffers();
	}

	m_displayWorld = hkgDisplayWorld::create();
	m_sceneConverter = new hkgSceneDataConverter(m_displayWorld, m_window->getContext());
	//env.m_gamePad = pad0;
	//env.m_gamePadB = pad1;
	m_displayHandler = new hkgDisplayHandler(m_displayWorld, m_window->getContext(), m_window);
	m_displayHandler->setShaderLib(m_sceneConverter->m_shaderLibrary);

	setupLights();

	hkgCamera* camera = m_window->getCurrentViewport()->getCamera();

	float v[3] = { 0,0,0 };
	//camera->setFrom(v);
	//hkgVec3Zero(v);
	//camera->setTo(v);
	v[2] = 1;
	//camera->setUp(v);
	m_window->getCurrentViewport()->setFlyModeUp(v);
	cameraSetFront();

	m_window->getContext()->unlock();
}

void HavokWidget::cameraSetFront()
{
	float front[3] = { 0,1,0 };
	hkgCamera* camera = m_window->getCurrentViewport()->getCamera();
	camera->setProjectionMode(HKG_CAMERA_COMPUTE_ORTHOGRAPHIC);
	float t[3];
	camera->getTo(t);
	float f[3];
	camera->getFrom(f);
	hkgVec3Sub(t, f);
	float num = hkgVec3Length(t);
	hkgVec3Scale(front, num);

	camera->getTo(t);
	hkgVec3Add(front, t);

	camera->setFrom(front);
	camera->setUp(m_window->getCurrentViewport()->getFlyModeUpPtr());
	camera->computeModelView(false);
	camera->computeProjection();
}

void HavokWidget::resizeEvent(QResizeEvent* event)
{
	m_window->getContext()->lock();
	m_window->updateSize(event->size().width(), event->size().height());
	m_window->getContext()->unlock();
}

void HavokWidget::mouseMoveEvent(QMouseEvent* event)
{
	m_window->processMouseMove(event->pos().x(), m_window->getHeight() - event->pos().y() - 1, false);
}

void HavokWidget::mousePressEvent(QMouseEvent* event)
{
	HKG_MOUSE_BUTTON btn;
	switch (event->button())
	{
	case Qt::MouseButton::LeftButton:
		btn = HKG_MOUSE_LEFT_BUTTON;
		break;
	case Qt::MouseButton::RightButton:
		btn = HKG_MOUSE_RIGHT_BUTTON;
		break;
	case Qt::MouseButton::MiddleButton:
	case Qt::MouseButton::XButton1:
	case Qt::MouseButton::XButton2:
		btn = HKG_MOUSE_MIDDLE_BUTTON;
		break;
	default:
		return;
	}
	m_window->processMouseButton(btn, true, event->pos().x(), m_window->getHeight() - event->pos().y() - 1, false);
}

void HavokWidget::mouseReleaseEvent(QMouseEvent* event)
{
	HKG_MOUSE_BUTTON btn;
	switch (event->button())
	{
	case Qt::MouseButton::LeftButton:
		btn = HKG_MOUSE_LEFT_BUTTON;
		break;
	case Qt::MouseButton::RightButton:
		btn = HKG_MOUSE_RIGHT_BUTTON;
		break;
	case Qt::MouseButton::MiddleButton:
	case Qt::MouseButton::XButton1:
	case Qt::MouseButton::XButton2:
		btn = HKG_MOUSE_MIDDLE_BUTTON;
		break;
	default:
		return;
	}
	m_window->processMouseButton(btn, false, event->pos().x(), m_window->getHeight() - event->pos().y() - 1, false);
}

void HavokWidget::wheelEvent(QWheelEvent* event)
{
	m_window->processMouseWheel(event->delta(), event->pos().x(), m_window->getHeight() - event->pos().y() - 1, false);
}

void HavokWidget::showAxis()
{
	uint color = 0xFF0000;
	constexpr uint alphaMask = (0xFF << 24);
	static const float array[102] =
	{
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.85f,
		0.1f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.85f,
		-0.1f,
		0.0f,
		1.1f,
		0.1f,
		0.0f,
		1.2f,
		-0.1f,
		0.0f,
		1.2f,
		0.1f,
		0.0f,
		1.1f,
		-0.1f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.1f,
		0.85f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		-0.1f,
		0.85f,
		0.0f,
		0.1f,
		1.1f,
		0.0f,
		0.0f,
		1.15f,
		0.0f,
		0.1f,
		1.2f,
		0.0f,
		0.0f,
		1.15f,
		0.0f,
		0.0f,
		1.15f,
		0.0f,
		-0.1f,
		1.15f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.1f,
		0.0f,
		0.85f,
		0.0f,
		0.0f,
		1.0f,
		-0.1f,
		0.0f,
		0.85f,
		0.1f,
		0.0f,
		1.1f,
		0.1f,
		0.0f,
		1.2f,
		0.1f,
		0.0f,
		1.2f,
		-0.1f,
		0.0f,
		1.1f,
		-0.1f,
		0.0f,
		1.1f,
		-0.1f,
		0.0f,
		1.2f
	};

	if (!m_showWorldAxis)
		return;

	auto* ctx = m_window->getContext();
	HKG_ENABLED_STATE enabledState = ctx->getEnabledState();

	ctx->matchState(HKG_ENABLED_NONE, ctx->getCullfaceMode(), ctx->getBlendMode(), ctx->getAlphaSampleMode());
	float m[16];
	float result[3];
	float temp[3];

	hkgMat4Identity(m);
	hkgVec3Zero(result);
	hkgVec3Zero(temp);

	hkgViewport* viewport = m_window->getViewport(0);
	hkgCamera* camera = viewport->getCamera();
	camera->unProject(40, 40, 0.2f, viewport->getWidth(), viewport->getHeight(), result);
	camera->getFrom(temp);
	hkgVec3Sub(temp, result);

	float distanceFromCam = hkgVec3Length(temp);
	float scale = camera->computeIconVerticalDrawSize(distanceFromCam, 32, viewport->getHeight());
	ctx->pushMatrix();
	m[12] = result[0];
	m[13] = result[1];
	m[14] = result[2];

	ctx->multMatrix(m);

	ctx->beginGroup(HKG_IMM_LINES);
	int index = 0;
	float currentPosition[3];
	hkgVec3Zero(currentPosition);


	float pos[3];
	for (int i = 0; i < 5; i++) 
	{
		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]); 
		index += 3;
		hkgVec3Scale(pos, -scale);
		ctx->setCurrentPosition(pos);

		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]);
		index += 3;
		hkgVec3Scale(pos, -scale);
		ctx->setCurrentPosition(pos);
	}

	color >>= 8;
	for (int i = 0; i < 6; i++)
	{
		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]);
		index += 3;
		hkgVec3Scale(pos, -scale);
		ctx->setCurrentPosition(pos);

		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]);
		index += 3;
		hkgVec3Scale(pos, -scale);
		ctx->setCurrentPosition(pos);
	}

	color >>= 8;
	for (int i = 0; i < 6; i++)
	{
		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]);
		index += 3;
		hkgVec3Scale(pos, scale);
		ctx->setCurrentPosition(pos);

		ctx->setCurrentColorPacked(color | alphaMask);
		hkgVec3Copy(pos, &array[index]);
		index += 3;
		hkgVec3Scale(pos, scale);
		ctx->setCurrentPosition(pos);
	}
	ctx->endGroup();

	ctx->popMatrix();
	ctx->matchState(enabledState, ctx->getCullfaceMode(), ctx->getBlendMode(),ctx->getAlphaSampleMode());
}

void HavokWidget::drawGrid()
{
	if (!m_showGrid)
		return;

	hkgDisplayContext* ctx = m_window->getContext();
	float num = 100.0f;

	auto enabledState = ctx->getEnabledState();
	auto cullfaceMode = ctx->getCullfaceMode();
	auto blendMode = ctx->getBlendMode();

	ctx->matchState(HKG_ENABLED_ALPHABLEND|HKG_ENABLED_ZREAD, HKG_CULLFACE_CCW, HKG_BLEND_ADD, ctx->getAlphaSampleMode());
	ctx->beginGroup(HKG_IMM_LINES);
	
	float p[3];
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	for (int i = 0; i <= 100; ++i)
	{
		float num2 = -0.5f * num + i;
		color[3] = !(i % 10) ? 1.0f : 0.15f;

		hkgVec3Set(p, num2, 0, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);

		hkgVec3Set(p, num2, 0.5f * num, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);

		hkgVec3Set(p, num2, 0, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);

		hkgVec3Set(p, num2, -0.5f * num, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p); 
	}
	
	for (int j = 0; j <= 100; ++j)
	{
		float num3 = -0.5f * num + (float)j;
		color[3] = (j % 10 == 0) ? 1.0f : 0.15f;

		hkgVec3Set(p, 0, num3, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);

		hkgVec3Set(p, 0.5f * num, num3, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);


		hkgVec3Set(p, 0, num3, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);

		hkgVec3Set(p, -0.5f * num, num3, 0);
		ctx->setCurrentColor4(color);
		ctx->setCurrentPosition(p);
	}
	ctx->endGroup();
	ctx->matchState(enabledState, cullfaceMode, blendMode, ctx->getAlphaSampleMode());
}

void HavokWidget::tickFrame(bool justCamera)
{
	hkgDisplayContext* ctx = m_window->getContext();
	ctx->lock();
	int frameNum = ctx->advanceFrame();
	// Primary world:
	if (m_displayWorld)
	{
		m_displayWorld->advanceToFrame(frameNum, !justCamera, m_window);
		m_window->getViewport(0)->getCamera()->advanceToFrame(frameNum);
	}
	ctx->unlock();
}

void HavokWidget::clearFrameData()
{

}

void HavokWidget::renderFrame()
{
	HKG_TIMER_BEGIN_LIST("_Render", "render");

	hkgWindow* window = m_window;

	if (!window || (window->getWidth() == 0) || (window->getHeight() == 0))
	{
		HKG_TIMER_BEGIN("SwapBuffers", HK_NULL);
		if (window)
		{
			window->getContext()->lock();
			window->swapBuffers();
			window->getContext()->unlock();
		}
		HKG_TIMER_END();

		clearFrameData();
		HKG_TIMER_END_LIST(); // render
		return; // nothing to render too..
	}

	hkgDisplayContext* ctx = window->getContext();
	ctx->lock();

	hkgViewport* masterView = window->getCurrentViewport();

	window->inViewportResize();

	HKG_TIMER_SPLIT_LIST("SetViewport");

	masterView->setAsCurrent(ctx);


	if (masterView->getSkyBox())
	{
		HKG_TIMER_SPLIT_LIST("SkyBox");
		masterView->getSkyBox()->render(ctx, masterView->getCamera());
	}

	hkgDisplayWorld* dw = m_displayWorld;

	HKG_TIMER_SPLIT_LIST("DisplayWorld");
	if (dw)
	{
		// can't alter the world in the middle of a render pass, so it will lock itself
		dw->render(ctx, true, true); // culled with shadows (if any setup)
	}


	HKG_TIMER_SPLIT_LIST("DrawImmediate");
	hkgDisplayHandler* dh = m_displayHandler;
	if (dh)
	{
		dh->drawImmediate();
	}
	
	// Draw Gizmos
	drawGrid();
	showAxis();

	HKG_TIMER_SPLIT_LIST("PostEffects");

	m_window->applyPostEffects();

	HKG_TIMER_SPLIT_LIST("Final Pass Objects");
	{
		dw->finalRender(ctx, true /* frustum cull */); 
	}

	window->stepInput();

	HKG_TIMER_SPLIT_LIST("SwapBuffers");

	window->swapBuffers();

	//env.m_inRenderLoop = false;

	ctx->unlock();

	HKG_TIMER_END_LIST(); // render
}

void HavokWidget::paint()
{

	m_window->clearBuffers();

	hkMemorySystem::getInstance().advanceFrame();

	renderFrame();

	////tick
	tickFrame(false);


}