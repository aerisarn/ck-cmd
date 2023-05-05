#pragma once

#include "DockWidget.h"

#include <QTimer>
#include <src/models/ModelEdge.h>

class hkgWindow;
class hkgDisplayHandler;
class hkgSceneDataConverter;
class hkgDisplayContext;
class hkgDisplayWorld;
class hkTextDisplay;
class hkgLight;
class hkgAabb;

template<class>
class hkRefPtr;

class hkLoader;
class hkgGeometry;
class hkgFaceSet;
class hkaAnimationContainer;
class hkMatrix4;
class hkaSkeleton;
class hkgShaderEffectCollection;

class HavokWidget : public ::ads::CDockWidget
{
    hkgWindow* m_window = nullptr;
    ckcmd::HKX::ProjectModel* _model;

    // Default viewport, or for any viewport in which m_viewport has null data
    hkgDisplayHandler* m_displayHandler = nullptr;
    hkgSceneDataConverter* m_sceneConverter = nullptr;
    hkgDisplayWorld* m_displayWorld = nullptr;
    hkgDisplayWorld* m_skeletalWorld = nullptr;
    hkTextDisplay* m_textDisplay = nullptr;
    hkLoader* m_loader = nullptr;
    hkgShaderEffectCollection* skeletalShader = nullptr;

    // This should be an animcontainer instead
    hkaSkeleton* skeleton = nullptr;
    bool m_skelNeedsUpdate = false;
    std::vector<hkMatrix4> boneAbsTransforms;
    std::vector<hkMatrix4> boneModelSpaces;

    bool m_showWorldAxis = true;
    bool m_showGrid = true;

    QTimer m_qTimer;

    void setupLights();
    void setupFixedShadowFrustum(const hkgLight& light, const hkgAabb& areaOfInterest, float extraNear = 0, float extraFar = 0, int numSplits = 0, int preferedUpAxis = -1);
    void showAxis();
    void tickFrame(bool justCamera);
    void renderFrame();
    void clearFrameData();
    void setupScene();
    void drawSkeletalNormals();
    void relinkSkeleton();
    void drawSkeletalTriangleThingy(hkaSkeleton* skeletal, const std::vector<hkMatrix4>& boneAbsTransform);
    hkRefPtr<hkgGeometry> createBoneVertices(const float vTarget[3], const float vOrigin[3]);
    void createDoublePyramid(const float vA[3], const float vB[3], hkgGeometry* geom, hkgDisplayContext* context);
    void drawGrid();
    void setupCamera();

    void createGridMesh();

public:
    explicit HavokWidget(ckcmd::HKX::ProjectModel*, QWidget* parent);
    void treeSelectionChanged(const QModelIndex& current, const QModelIndex& previous);


protected:
    void initialize();
    void updateSkeleton();

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event);

    void paint();

};