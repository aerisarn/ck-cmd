#pragma once

#include "DockWidget.h"

#include <QTimer>

class hkgWindow;
class hkgDisplayHandler;
class hkgSceneDataConverter;
class hkgDisplayWorld;
class hkTextDisplay;
class hkgLight;
class hkgAabb;
class hkLoader;

class HavokWidget : public ::ads::CDockWidget
{
    hkgWindow* m_window = nullptr;

    // Default viewport, or for any viewport in which m_viewport has null data
    hkgDisplayHandler* m_displayHandler = nullptr;
    hkgSceneDataConverter* m_sceneConverter = nullptr;
    hkgDisplayWorld* m_displayWorld = nullptr;
    hkTextDisplay* m_textDisplay = nullptr;
    hkLoader* m_loader = nullptr;

    bool m_showWorldAxis = true;

    QTimer m_qTimer;

    void setupLights();
    void setupFixedShadowFrustum(const hkgLight& light, const hkgAabb& areaOfInterest, float extraNear = 0, float extraFar = 0, int numSplits = 0, int preferedUpAxis = -1);
    void showAxis();
    void tickFrame(bool justCamera);
    void renderFrame();
    void clearFrameData();
    void setupScene();

public:
    explicit HavokWidget(QWidget* parent);



protected:
    void initialize();

    virtual void resizeEvent(QResizeEvent* event) override;

    void paint();

};