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
	hkgSystem::init("ogl");
	m_window = hkgWindow::create();
	m_window->setShadowMapSize(0); // 0 == use default platform size
	HKG_WINDOW_CREATE_FLAG windowFlags = HKG_WINDOW_WINDOWED;

	m_window->initialize(windowFlags,
		HKG_WINDOW_BUF_COLOR | HKG_WINDOW_BUF_DEPTH32, size().width(), size().height(),
		"Preview");
	m_window->getContext()->lock();

	// don't allow viewport resizing
	m_window->setWantViewportBorders(false);
	m_window->setWantViewportResizeByMouse(false);

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

	m_window->getContext()->unlock();
}

void HavokWidget::resize(int w, int h)
{
	m_window->getContext()->lock();
	m_window->updateSize(w, h);
	m_window->getContext()->unlock();
}


void HavokWidget::showAxis()
{
	// world axis
	if (m_showWorldAxis)
	{
		hkgWindow* window = m_window;
		hkgCamera* camera = window->getViewport(0)->getCamera();
		float			res[] = { 0,0,0 };	camera->unProjectExact(32, 32, 0.75f, window->getWidth(), window->getHeight(), res);
		float			res2[] = { 0,0,0 };	camera->unProjectExact(32, 32 + 24, 0.75f, window->getWidth(), window->getHeight(), res2);
		hkVector4		org; org.set(res[0], res[1], res[2], 0);
		hkVector4		del; del.set(res2[0], res2[1], res2[2], 0);
		hkReal			s(org.distanceTo3(del));

		HK_DISPLAY_ARROW(org, hkVector4(s, 0, 0), hkColor::RED);
		HK_DISPLAY_ARROW(org, hkVector4(0, s, 0), hkColor::GREEN);
		HK_DISPLAY_ARROW(org, hkVector4(0, 0, s), hkColor::BLUE);
	}
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
	//if (env.m_shareCameraBetweenViewports)
	//{
	//	const hkgCamera* fromC = masterView->getCamera();
	//	for (int viewportIndex = 0; viewportIndex < window->getNumViewports(); ++viewportIndex)
	//	{
	//		// only reason there won't be a displayWorld is if the demo is doing the rendering itself.
	//		hkgViewport* v = window->getViewport(viewportIndex);
	//		if (v != masterView)
	//		{
	//			hkgCamera* toC = v->getCamera();
	//			// Don't copy to different modes as doesn't make sense
	//			if ((toC->getProjectionMode() == HKG_CAMERA_PERSPECTIVE) && (fromC->getProjectionMode() == HKG_CAMERA_PERSPECTIVE))
	//			{
	//				float aspect = toC->getAspect();
	//				toC->copy(*fromC);
	//				toC->setAspect(aspect);
	//				toC->computeProjection();
	//				toC->computeFrustumPlanes();
	//			}
	//		}
	//	}
	//}

	//for (int viewportIndex = 0; viewportIndex < window->getNumViewports(); ++viewportIndex)
	//{
		// only reason there won't be a displayWorld is if the demo is doing the rendering itself.
		hkgViewport* v = window->getViewport(0);
		//hkDemoEnvironment::ViewportData* viewportData = env.getViewportData(viewportIndex);

		HKG_TIMER_SPLIT_LIST("SetViewport");

		v->setAsCurrent(ctx);

		//_sendVdbCamera(v, viewportIndex);

		if (v->getSkyBox())
		{
			HKG_TIMER_SPLIT_LIST("SkyBox");
			v->getSkyBox()->render(ctx, v->getCamera());
		}

		hkgDisplayWorld* dw = m_displayWorld;
		//if (dw)
		//{
		//	dw->setFrameTime(env.m_frameTimer.getLastFrameTime());
		//}

		HKG_TIMER_SPLIT_LIST("DisplayWorld");
		if (dw)
		{
			// can't alter the world in the middle of a render pass, so it will lock itself
			dw->render(ctx, true, true); // culled with shadows (if any setup)

			//if (env.m_options->m_edgedFaces)
			//{
			//	HKG_COLOR_MODE origColorMode = ctx->getColorMode();
			//	HKG_BLEND_MODE origBlendMode = ctx->getBlendMode();
			//	HKG_ENABLED_STATE origEnabledState = ctx->getEnabledState();

			//	ctx->setWireframeState(true);
			//	ctx->setDepthReadState(true);
			//	ctx->setDepthWriteState(false);
			//	ctx->setLightingState(false);
			//	ctx->setTexture2DState(false);
			//	ctx->setBlendState(true);

			//	// Most assets will be loaded using the ShaderLib
			//	// Currently that lib only supports lit senarios
			//	// wheras here we want the wireframe to be unlit etc.
			//	// So we will enforce no per material shader (not required as default shaders will be ok for most things, except particles etc)

			//	// Render white wireframe with low alpha
			//	hkgMaterial* globalWireMaterial = hkgMaterial::create();
			//	globalWireMaterial->setDiffuseColor(1, 1, 1, 0.1f);
			//	ctx->setCurrentMaterial(globalWireMaterial, HKG_MATERIAL_VERTEX_HINT_NONE); // Global mat
			//	globalWireMaterial->removeReference();

			//	ctx->setColorMode(HKG_COLOR_GLOBAL | HKG_COLOR_GLOBAL_SHADER_COLLECTION);

			//	hkgCamera* curCamera = v->getCamera();
			//	float origFrom[3];
			//	curCamera->getFrom(origFrom);
			//	float* currentTo = curCamera->getToPtr();

			//	float newFrom[3];
			//	hkgVec3Sub(newFrom, currentTo, origFrom);
			//	hkgVec3Scale(newFrom, 1.0e-3f);
			//	hkgVec3Add(newFrom, origFrom);
			//	curCamera->setFrom(newFrom);
			//	curCamera->computeModelView(false);
			//	curCamera->setAsCurrent(ctx);

			//	// Render from this shifted POV
			//	dw->render(ctx, true, false);

			//	// Reset context mode
			//	ctx->setColorMode(origColorMode);
			//	ctx->matchState(origEnabledState, ctx->getCullfaceMode(), origBlendMode, ctx->getAlphaSampleMode());
			//	curCamera->setFrom(origFrom);
			//	curCamera->computeModelView(false);
			//	curCamera->setAsCurrent(ctx);
			//}
		}


		HKG_TIMER_SPLIT_LIST("DrawImmediate");
		hkgDisplayHandler* dh = m_displayHandler;
		if (dh)
		{
			dh->drawImmediate();
		}
	//}

	HKG_TIMER_SPLIT_LIST("PostEffects");

	m_window->applyPostEffects();

	HKG_TIMER_SPLIT_LIST("Final Pass Objects");
	{
		//for (int viewportIndex = 0; viewportIndex < window->getNumViewports(); ++viewportIndex)
		//{
			// only reason there won't be a displayWorld is if the demo is doing the rendering itself.
			//hkDemoEnvironment::ViewportData* viewportData = env.getViewportData(viewportIndex);
			hkgDisplayWorld* dw = m_displayWorld;
			if (dw)
			{
				window->getViewport(0)->setAsCurrent(ctx);
				dw->finalRender(ctx, true /* frustum cull */);
			}
		//}
	}

	masterView->setAsCurrent(ctx);

	//HKG_TIMER_SPLIT_LIST("DemoPostRenderWindow");
	//if (demo)
	//{
	//	demo->postRenderWindow(window);
	//}

	// Draw text after all post effects etc
	//HKG_TIMER_SPLIT_LIST("Display3DText");
	//{
	//	for (int viewportIndex = 0; viewportIndex < window->getNumViewports(); ++viewportIndex)
	//	{
	//		// only reason there won't be a displayWorld is if the demo is doing the rendering itself.
	//		hkDemoEnvironment::ViewportData* viewportData = env.getViewportData(viewportIndex);
	//		hkTextDisplay* td = viewportData && viewportData->m_textDisplay ? viewportData->m_textDisplay : env.m_textDisplay;
	//		if (td)
	//		{
	//			hkgViewport* v = window->getViewport(viewportIndex);
	//			td->displayJust3DText(window, v);
	//		}
	//	}
	//}

	// Text after post render window (as it displays the stats bar etc)
	//env.m_textDisplay->displayJust2DText(window);


	// We commonly use 'swap discard' (better for SLi/CrossFire etc) as the swap effect, so 
	// that means you neeed tyo save the RT before Swap or its contents may be erased
	// A side effect of this is that the image will not have the Havok logo on the lower corner

	//if (env.m_virtualFrameBufferServer)
	//{
	//	sendCurrentFrameOverNetwork(env);
	//}


	//if (env.m_options->m_saveFrames)
	//{
	//	HKG_TIMER_SPLIT_LIST("SaveBMP");

	//	char filename[128];
	//	hkString::sprintf(filename, "frame%05i.bmp", env.m_options->m_numSaveFrames);
	//	window->saveCurrentRenderTargetToBmp(filename);
	//	env.m_options->m_numSaveFrames++;
	//}

	//if (env.m_options->m_recordMovie)
	//{
	//	env.m_movieRecorder->addFrame();
	//}

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




	showAxis();

	renderFrame();

	//tick
	tickFrame(false);


}