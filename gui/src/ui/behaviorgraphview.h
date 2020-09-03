#ifndef BEHAVIORGRAPHVIEW_H
#define BEHAVIORGRAPHVIEW_H

#include "src/hkxclasses/hkxobject.h"
#include "src/ui/treegraphicsView.h"

class FileSelectWindow;
class QVBoxLayout;
class QMenuBar;
class QMenu;
class QFile;
class QGraphicsScene;
class QGroupBox;
class QHBoxLayout;
class BehaviorFile;
class TreeGraphicsItem;

namespace UI{
	class hkbManualSelectorGenerator;
	class hkbBlenderGenerator;
	class hkbBlenderGeneratorChild;
	class hkbStateMachine;
	class hkbStateMachineStateInfo;
	class BSBoneSwitchGenerator;
	class BSBoneSwitchGeneratorBoneData;
	class hkbGenerator;
	class HkDataUI;
	class hkbPoseMatchingGenerator;
	class ModifierListUI;
}

class BehaviorGraphView final: public TreeGraphicsView
{
    Q_OBJECT
    friend class MainWindow;
    friend class UI::ModifierListUI;
public:
    BehaviorGraphView(UI::HkDataUI *mainUI, BehaviorFile * file);
    BehaviorGraphView& operator=(const BehaviorGraphView&) = delete;
    BehaviorGraphView(const BehaviorGraphView &) = delete;
    ~BehaviorGraphView();
public:
    QStringList getEventNames() const;
    void removeGeneratorData();
    void removeModifierData();
    void removeOtherData();
    bool refocus();
    void setSelectedItem(TreeGraphicsItem *item);
    QString getBehaviorFilename() const;
    TreeGraphicsItem * getSelectedIconsChildIcon(UI::HkxObject *child);
    void removeObjects();
    BehaviorFile *getBehavior() const;
signals:
    void addedGenerator(const QString & name, const QString & type);
    void addedModifier(const QString & name, const QString & type);
    void removedGenerator(int index);
    void removedModifier(int index);
    void disconnectTablesFromHkDataUI();
public slots:
    void appendStateMachine();
    void appendState();
    void appendManualSelectorGenerator();
    void appendModifierGenerator();
    void appendIStateTaggingGenerator();
    void appendSynchronizedClipGenerator();
    void appendOffsetAnimationGenerator();
    void appendCyclicBlendTransitionGenerator();
    void appendPoseMatchingGenerator();
    void appendBlenderGenerator();
    void appendBlenderGeneratorChild();
    void appendBoneSwitchGenerator();
    void appendBoneSwitchGeneratorChild();
    void appendClipGenerator();
    void appendBehaviorReferenceGenerator();
    void appendBGSGamebryoSequenceGenerator();
private slots:
    void focusOnGeneratorIcon(int index, const QString &);
    void focusOnModifierIcon(int index, const QString &);
    void appendModifierList();
    void appendTwistModifier();
    void appendEventDrivenModifier();
    void appendIsActiveModifier();
    void appendLimbIKModifier();
    void appendInterpValueModifier();
    void appendGetTimeStepModifier();
    void appendFootIkControlsModifier();
    void appendGetHandleOnBoneModifier();
    void appendTransformVectorModifier();
    void appendProxyModifier();
    void appendLookAtModifier();
    void appendMirrorModifier();
    void appendGetWorldFromModelModifier();
    void appendSenseHandleModifier();
    void appendEvaluateExpressionModifier();
    void appendEvaluateHandleModifier();
    void appendAttachmentModifier();
    void appendAttributeModifier();
    void appendCombineTransformsModifier();
    void appendComputeRotationFromAxisAngleModifier();
    void appendComputeRotationToTargetModifier();
    void appendEventsFromRangeModifier();
    void appendMoveCharacterModifier();
    void appendExtractRagdollPoseModifier();
    void appendModifyOnceModifier();
    void appendEventOnDeactivateModifier();
    void appendEventEveryNEventsModifier();
    void appendRagdollContactListenerModifier();
    void appendPoweredRagdollControlsModifier();
    void appendEventOnFalseToTrueModifier();
    void appendDirectAtModifier();
    void appendDistTriggerModifier();
    void appendDecomposeVectorModifier();
    void appendComputeAddBoneAnimModifier();
    void appendTweenerModifier();
    void appendBSIStateManagerModifier();
    void appendTimerModifier();
    void appendRotateCharacterModifier();
    void appendDampingModifier();
    void appendDelayedModifier();
    void appendGetUpModifier();
    void appendKeyframeBonesModifier();
    void appendComputeDirectionModifier();
    void appendRigidBodyRagdollControlsModifier();
    void appendBSSpeedSamplerModifier();
    void appendDetectCloseToGroundModifier();
    void appendBSTimerModifier();
    void appendBSLookAtModifier();
    void appendBSPassByTargetTriggerModifier();
    void appendHandIKControlsModifier();
    void wrapStateMachine();
    void wrapBlenderGenerator();
    void wrapPoseMatchingGenerator();
    void wrapManualSelectorGenerator();
    void wrapModifierGenerator();
    void wrapIStateTaggingGenerator();
    void wrapSynchronizedClipGenerator();
    void wrapOffsetAnimationGenerator();
    void wrapCyclicBlendTransitionGenerator();
    void wrapBoneSwitchGenerator();
    void deleteSelectedObjectBranchSlot();
    void popUpMenuRequested(const QPoint &pos);
private:
    template <typename T>
    void append(T *obj);
    template <typename T>
    void wrap(T *obj);
    void enableAllMenuActions(QMenu *menu);
    void disableAllMenuActions(QMenu *menu);
    bool confirmationDialogue(const QString & message, QWidget *parent);
    void deleteAllObjectBranches();
private:
	UI::HkDataUI *ui;
    BehaviorFile *behavior;
    QMenu *contextMenu;
    QMenu *appendGeneratorMenu;
    QAction *appendStateMachineAct;
    QAction *appendStateAct;
    QAction *appendManualSelectorGeneratorAct;
    QAction *appendModifierGeneratorAct;
    QAction *appendBSIStateTaggingGeneratorAct;
    QAction *appendBSSynchronizedClipGeneratorAct;
    QAction *appendBSOffsetAnimationGeneratorAct;
    QAction *appendBSCyclicBlendTransitionGeneratorAct;
    QAction *appendPoseMatchingGeneratorAct;
    QAction *appendClipGeneratorAct;
    QAction *appendBehaviorReferenceGeneratorAct;
    QAction *appendBGSGamebryoSequenceGeneratorAct;
    QMenu *appendBlenderMenu;
    QAction *appendBlenderGeneratorAct;
    QAction *appendBlenderGeneratorChildAct;
    QAction *appendBSBoneSwitchGeneratorAct;
    QAction *appendBSBoneSwitchGeneratorChildAct;
    QMenu *wrapGeneratorMenu;
    QAction *wrapStateMachineAct;
    QAction *wrapManualSelectorGeneratorAct;
    QAction *wrapModifierGeneratorAct;
    QAction *wrapBSIStateTaggingGeneratorAct;
    QAction *wrapBSSynchronizedClipGeneratorAct;
    QAction *wrapBSOffsetAnimationGeneratorAct;
    QAction *wrapBSCyclicBlendTransitionGeneratorAct;
    QAction *wrapPoseMatchingGeneratorAct;
    QMenu *wrapBlenderMenu;
    QAction *wrapBlenderGeneratorAct;
    QAction *wrapBSBoneSwitchGeneratorAct;
    QMenu *appendModifierMenu;
    QAction *appendModifierListAct;
    QAction *appendTwistModifierAct;
    QAction *appendEventDrivenModifierAct;
    QAction *appendIsActiveModifierAct;
    QAction *appendLimbIKModifierAct;
    QAction *appendInterpValueModifierAct;
    QAction *appendGetTimeStepModifierAct;
    QAction *appendFootIkControlsModifierAct;
    QAction *appendGetHandleOnBoneModifierAct;
    QAction *appendTransformVectorModifierAct;
    QAction *appendProxyModifierAct;
    QAction *appendLookAtModifierAct;
    QAction *appendMirrorModifierAct;
    QAction *appendGetWorldFromModelModifierAct;
    QAction *appendSenseHandleModifierAct;
    QAction *appendEvaluateExpressionModifierAct;
    QAction *appendEvaluateHandleModifierAct;
    QAction *appendAttachmentModifierAct;
    QAction *appendAttributeModifierAct;
    QAction *appendCombineTransformsModifierAct;
    QAction *appendComputeRotationFromAxisAngleModifierAct;
    QAction *appendComputeRotationToTargetModifierAct;
    QAction *appendEventsFromRangeModifierAct;
    QAction *appendMoveCharacterModifierAct;
    QAction *appendExtractRagdollPoseModifierAct;
    QAction *appendModifyOnceModifierAct;
    QAction *appendEventOnDeactivateModifierAct;
    QAction *appendEventEveryNEventsModifierAct;
    QAction *appendRagdollContactListenerModifierAct;
    QAction *appendPoweredRagdollControlsModifierAct;
    QAction *appendEventOnFalseToTrueModifierAct;
    QAction *appendDirectAtModifierAct;
    QAction *appendDistTriggerModifierAct;
    QAction *appendDecomposeVectorModifierAct;
    QAction *appendComputeAddBoneAnimModifierAct;
    QAction *appendTweenerModifierAct;
    QAction *appendBSIStateManagerModifierAct;
    QAction *appendTimerModifierAct;
    QAction *appendRotateCharacterModifierAct;
    QAction *appendDampingModifierAct;
    QAction *appendDelayedModifierAct;
    QAction *appendGetUpModifierAct;
    QAction *appendKeyframeBonesModifierAct;
    QAction *appendComputeDirectionModifierAct;
    QAction *appendRigidBodyRagdollControlsModifierAct;
    QAction *appendBSSpeedSamplerModifierAct;
    QAction *appendDetectCloseToGroundModifierAct;
    QAction *appendBSTimerModifierAct;
    QAction *appendBSLookAtModifierAct;
    QAction *appendBSPassByTargetTriggerModifierAct;
    QAction *appendHandIKControlsModifierAct;
    QAction *removeObjBranchAct;
};


#endif // BEHAVIORGRAPHVIEW_H
