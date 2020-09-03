#include "behaviorgraphview.h"
#include "src/ui/mainwindow.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/ui/behaviorgraphview.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/treegraphicsscene.h"
#include "src/ui/hkxclassesui/hkdataui.h"

#include "src/hkxclasses/behavior/generators/bsistatetagginggenerator.h"
#include "src/hkxclasses/behavior/generators/bscyclicblendtransitiongenerator.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgenerator.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/generators/bssynchronizedclipgenerator.h"
#include "src/hkxclasses/behavior/generators/bsoffsetanimationgenerator.h"
#include "src/hkxclasses/behavior/generators/bgsgamebryosequencegenerator.h"

#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/hkxclasses/behavior/generators/hkbmodifiergenerator.h"
#include "src/hkxclasses/behavior/generators/hkbmanualselectorgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbblendergenerator.h"
#include "src/hkxclasses/behavior/generators/hkbposematchinggenerator.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h"

#include "src/hkxclasses/behavior/modifiers/bsisactivemodifier.h"
#include "src/hkxclasses/behavior/modifiers/bslimbikmodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsspeedsamplermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bslookatmodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsmodifyoncemodifier.h"
#include "src/hkxclasses/behavior/modifiers/bseventonfalsetotruemodifier.h"
#include "src/hkxclasses/behavior/modifiers/bseventondeactivatemodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsragdollcontactlistenermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsdirectatmodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsdecomposevectormodifier.h"
#include "src/hkxclasses/behavior/modifiers/bscomputeaddboneanimmodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsdisttriggermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bseventeveryneventsmodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsinterpvaluemodifier.h"
#include "src/hkxclasses/behavior/modifiers/bspassbytargettriggermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bstimermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bstweenermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsistatemanagermodifier.h"
#include "src/hkxclasses/behavior/modifiers/bsgettimestepmodifier.h"

#include "src/hkxclasses/behavior/modifiers/hkbmodifierlist.h"
#include "src/hkxclasses/behavior/modifiers/hkbtwistmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbeventdrivenmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbfootikcontrolsmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbevaluateexpressionmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbrotatecharactermodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbdampingmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbkeyframebonesmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbpoweredragdollcontrolsmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbtimermodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbrigidbodyragdollcontrolsmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbgetupmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbcomputedirectionmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbevaluatehandlemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbgethandleonbonemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbsensehandlemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbdelayedmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbdetectclosetogroundmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbattachmentmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbattributemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbcombinetransformsmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbcomputerotationfromaxisanglemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbcomputerotationtotargetmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbeventsfromrangemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbextractragdollposemodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbgetworldfrommodelmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkblookatmodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbmirrormodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbmovecharactermodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbtransformvectormodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbproxymodifier.h"
#include "src/hkxclasses/behavior/modifiers/hkbhandikcontrolsmodifier.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbgeneratortransitioneffect.h"
#include "src/hkxclasses/behavior/hkbeventrangedataarray.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/hkxclasses/behavior/hkbexpressiondataarray.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/hkbstatemachineeventpropertyarray.h"
#include "src/hkxclasses/behavior/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/behavior/hkbexpressioncondition.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorgraph.h"
#include "src/hkxclasses/behavior/hkbcliptriggerarray.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphstringdata.h"
#include "src/hkxclasses/behavior/hkbvariablevalueset.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"

#include <QBoxLayout>
#include <QMenuBar>
#include <QFile>
#include <QSettings>
#include <QGroupBox>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QSize>

using namespace UI;

BehaviorGraphView::BehaviorGraphView(HkDataUI *mainUI, BehaviorFile * file)
    : TreeGraphicsView(0),
      ui(mainUI),
      behavior(file),
      contextMenu(new QMenu(this)),
      appendGeneratorMenu(new QMenu("Append Generator:", contextMenu)),
      appendStateMachineAct(new QAction("State Machine", appendGeneratorMenu)),
      appendStateAct(new QAction("State", appendGeneratorMenu)),
      appendManualSelectorGeneratorAct(new QAction("Manual Selector Generator", appendGeneratorMenu)),
      appendModifierGeneratorAct(new QAction("Modifier Generator", appendGeneratorMenu)),
      appendBSIStateTaggingGeneratorAct(new QAction("BS iState Tagging Generator", appendGeneratorMenu)),
      appendBSSynchronizedClipGeneratorAct(new QAction("BS Synchronized Clip Generator", appendGeneratorMenu)),
      appendBSOffsetAnimationGeneratorAct(new QAction("BS Offset Animation Generator", appendGeneratorMenu)),
      appendBSCyclicBlendTransitionGeneratorAct(new QAction("BS Cyclic Blend Transition Generator", appendGeneratorMenu)),
      appendPoseMatchingGeneratorAct(new QAction("Pose Matching Generator", appendGeneratorMenu)),
      appendClipGeneratorAct(new QAction("Clip Generator", appendGeneratorMenu)),
      appendBehaviorReferenceGeneratorAct(new QAction("Behavior Reference Generator", appendGeneratorMenu)),
      appendBGSGamebryoSequenceGeneratorAct(new QAction("BGS Gamebryo Sequence Generator", appendGeneratorMenu)),
      appendBlenderMenu(new QMenu("Append Blend:", contextMenu)),
      appendBlenderGeneratorAct(new QAction("Blender Generator", appendBlenderMenu)),
      appendBlenderGeneratorChildAct(new QAction("Blender Generator Child", appendBlenderMenu)),
      appendBSBoneSwitchGeneratorAct(new QAction("BS Bone Switch Generator", appendBlenderMenu)),
      appendBSBoneSwitchGeneratorChildAct(new QAction("BS Bone Switch Generator Child", appendBlenderMenu)),
      wrapGeneratorMenu(new QMenu("Wrap inside Generator:", contextMenu)),
      wrapStateMachineAct(new QAction("State Machine", wrapGeneratorMenu)),
      wrapManualSelectorGeneratorAct(new QAction("Manual Selector Generator", wrapGeneratorMenu)),
      wrapModifierGeneratorAct(new QAction("Modifier Generator", wrapGeneratorMenu)),
      wrapBSIStateTaggingGeneratorAct(new QAction("BS iState Tagging Generator", wrapGeneratorMenu)),
      wrapBSSynchronizedClipGeneratorAct(new QAction("BS Synchronized Clip Generator", wrapGeneratorMenu)),
      wrapBSOffsetAnimationGeneratorAct(new QAction("BS Offset Animation Generator", wrapGeneratorMenu)),
      wrapBSCyclicBlendTransitionGeneratorAct(new QAction("BS Cyclic Blend Transition Generator", wrapGeneratorMenu)),
      wrapPoseMatchingGeneratorAct(new QAction("Pose Matching Generator", wrapGeneratorMenu)),
      wrapBlenderMenu(new QMenu("Wrap inside Blend:", contextMenu)),
      wrapBlenderGeneratorAct(new QAction("Blender Generator", wrapBlenderMenu)),
      wrapBSBoneSwitchGeneratorAct(new QAction("BS Bone Switch Generator", wrapBlenderMenu)),
      appendModifierMenu(new QMenu("Append Modifier:", contextMenu)),
      appendModifierListAct(new QAction("Modifier List", appendModifierMenu)),
      appendTwistModifierAct(new QAction("Twist Modifier", appendModifierMenu)),
      appendEventDrivenModifierAct(new QAction("Event Driven Modifier", appendModifierMenu)),
      appendIsActiveModifierAct(new QAction("BS Is Active Modifier", appendModifierMenu)),
      appendLimbIKModifierAct(new QAction("BS Limb IK Modifier", appendModifierMenu)),
      appendInterpValueModifierAct(new QAction("BS Interp Value Modifier", appendModifierMenu)),
      appendGetTimeStepModifierAct(new QAction("BS Get Time Step Modifier", appendModifierMenu)),
      appendFootIkControlsModifierAct(new QAction("Foot IK Controls Modifier", appendModifierMenu)),
      appendGetHandleOnBoneModifierAct(new QAction("Get Handle On Bone Modifier", appendModifierMenu)),
      appendTransformVectorModifierAct(new QAction("Transform Vector Modifier", appendModifierMenu)),
      appendProxyModifierAct(new QAction("Proxy Modifier", appendModifierMenu)),
      appendLookAtModifierAct(new QAction("Look At Modifier", appendModifierMenu)),
      appendMirrorModifierAct(new QAction("Mirror Modifier", appendModifierMenu)),
      appendGetWorldFromModelModifierAct(new QAction("Get World From Model Modifier", appendModifierMenu)),
      appendSenseHandleModifierAct(new QAction("Sense Handle Modifier", appendModifierMenu)),
      appendEvaluateExpressionModifierAct(new QAction("Evaluate Expression Modifier", appendModifierMenu)),
      appendEvaluateHandleModifierAct(new QAction("Evaluate Handle Modifier", appendModifierMenu)),
      appendAttachmentModifierAct(new QAction("Attachment Modifier", appendModifierMenu)),
      appendAttributeModifierAct(new QAction("AttributeModifier", appendModifierMenu)),
      appendCombineTransformsModifierAct(new QAction("Combine Transforms Modifier", appendModifierMenu)),
      appendComputeRotationFromAxisAngleModifierAct(new QAction("Compute Rotation From Axis Angle Modifier", appendModifierMenu)),
      appendComputeRotationToTargetModifierAct(new QAction("Compute Rotation To Target Modifier", appendModifierMenu)),
      appendEventsFromRangeModifierAct(new QAction("Events From Range Modifier", appendModifierMenu)),
      appendMoveCharacterModifierAct(new QAction("Move Character Modifier", appendModifierMenu)),
      appendExtractRagdollPoseModifierAct(new QAction("Extract Ragdoll Pose Modifier", appendModifierMenu)),
      appendModifyOnceModifierAct(new QAction("BS Modify Once Modifier", appendModifierMenu)),
      appendEventOnDeactivateModifierAct(new QAction("BS Event On Deactivate Modifier", appendModifierMenu)),
      appendEventEveryNEventsModifierAct(new QAction("BS Event Every N Events Modifier", appendModifierMenu)),
      appendRagdollContactListenerModifierAct(new QAction("Ragdoll Contact Listener Modifier", appendModifierMenu)),
      appendPoweredRagdollControlsModifierAct(new QAction("Powered Ragdoll Controls Modifier", appendModifierMenu)),
      appendEventOnFalseToTrueModifierAct(new QAction("BS Event On False To True Modifier", appendModifierMenu)),
      appendDirectAtModifierAct(new QAction("BS Direct At Modifier", appendModifierMenu)),
      appendDistTriggerModifierAct(new QAction("BS Dist Trigger Modifier", appendModifierMenu)),
      appendDecomposeVectorModifierAct(new QAction("BS Decompose Vector Modifier", appendModifierMenu)),
      appendComputeAddBoneAnimModifierAct(new QAction("BS Compute Add Bone Anim Modifier", appendModifierMenu)),
      appendTweenerModifierAct(new QAction("BS Tweener Modifier", appendModifierMenu)),
      appendBSIStateManagerModifierAct(new QAction("BS I State Manager Modifier", appendModifierMenu)),
      appendTimerModifierAct(new QAction("Timer Modifier", appendModifierMenu)),
      appendRotateCharacterModifierAct(new QAction("Rotate Character Modifier", appendModifierMenu)),
      appendDampingModifierAct(new QAction("Damping Modifier", appendModifierMenu)),
      appendDelayedModifierAct(new QAction("Delayed Modifier", appendModifierMenu)),
      appendGetUpModifierAct(new QAction("Get Up Modifier", appendModifierMenu)),
      appendKeyframeBonesModifierAct(new QAction("Keyframe Bones Modifier", appendModifierMenu)),
      appendComputeDirectionModifierAct(new QAction("Compute Direction Modifier", appendModifierMenu)),
      appendRigidBodyRagdollControlsModifierAct(new QAction("Rigid Body Ragdoll Controls Modifier", appendModifierMenu)),
      appendBSSpeedSamplerModifierAct(new QAction("BS Speed Sampler Modifier", appendModifierMenu)),
      appendDetectCloseToGroundModifierAct(new QAction("Detect Close To Ground Modifier", appendModifierMenu)),
      appendBSTimerModifierAct(new QAction("BS Timer Modifier", appendModifierMenu)),
      appendBSLookAtModifierAct(new QAction("BS Look At Modifier", appendModifierMenu)),
      appendBSPassByTargetTriggerModifierAct(new QAction("BS Pass By Target Trigger Modifier", appendModifierMenu)),
      appendHandIKControlsModifierAct(new QAction("Hand IK Controls Modifier", appendModifierMenu)),
      removeObjBranchAct(new QAction("Remove Selected Object and Children", contextMenu))
{
    contextMenu->addMenu(appendGeneratorMenu);
    appendGeneratorMenu->addAction(appendStateMachineAct);
    appendGeneratorMenu->addAction(appendStateAct);
    appendGeneratorMenu->addAction(appendManualSelectorGeneratorAct);
    appendGeneratorMenu->addAction(appendModifierGeneratorAct);
    appendGeneratorMenu->addAction(appendBSIStateTaggingGeneratorAct);
    appendGeneratorMenu->addAction(appendBSSynchronizedClipGeneratorAct);
    appendGeneratorMenu->addAction(appendBSOffsetAnimationGeneratorAct);
    appendGeneratorMenu->addAction(appendBSCyclicBlendTransitionGeneratorAct);
    appendGeneratorMenu->addAction(appendPoseMatchingGeneratorAct);
    appendGeneratorMenu->addAction(appendClipGeneratorAct);
    appendGeneratorMenu->addAction(appendBehaviorReferenceGeneratorAct);
    appendGeneratorMenu->addAction(appendBGSGamebryoSequenceGeneratorAct);
    contextMenu->addMenu(appendBlenderMenu);
    appendBlenderMenu->addAction(appendBlenderGeneratorAct);
    appendBlenderMenu->addAction(appendBlenderGeneratorChildAct);
    appendBlenderMenu->addAction(appendBSBoneSwitchGeneratorAct);
    appendBlenderMenu->addAction(appendBSBoneSwitchGeneratorChildAct);
    contextMenu->addMenu(wrapGeneratorMenu);
    wrapGeneratorMenu->addAction(wrapStateMachineAct);
    wrapGeneratorMenu->addAction(wrapManualSelectorGeneratorAct);
    wrapGeneratorMenu->addAction(wrapModifierGeneratorAct);
    wrapGeneratorMenu->addAction(wrapBSIStateTaggingGeneratorAct);
    wrapGeneratorMenu->addAction(wrapBSSynchronizedClipGeneratorAct);
    wrapGeneratorMenu->addAction(wrapBSOffsetAnimationGeneratorAct);
    wrapGeneratorMenu->addAction(wrapBSCyclicBlendTransitionGeneratorAct);
    wrapGeneratorMenu->addAction(wrapPoseMatchingGeneratorAct);
    contextMenu->addMenu(wrapBlenderMenu);
    wrapBlenderMenu->addAction(wrapBlenderGeneratorAct);
    wrapBlenderMenu->addAction(wrapBSBoneSwitchGeneratorAct);
    contextMenu->addMenu(appendModifierMenu);
    appendModifierMenu->addAction(appendModifierListAct);
    appendModifierMenu->addAction(appendTwistModifierAct);
    appendModifierMenu->addAction(appendEventDrivenModifierAct);
    appendModifierMenu->addAction(appendIsActiveModifierAct);
    appendModifierMenu->addAction(appendLimbIKModifierAct);
    appendModifierMenu->addAction(appendInterpValueModifierAct);
    appendModifierMenu->addAction(appendGetTimeStepModifierAct);
    appendModifierMenu->addAction(appendFootIkControlsModifierAct);
    appendModifierMenu->addAction(appendGetHandleOnBoneModifierAct);
    appendModifierMenu->addAction(appendTransformVectorModifierAct);
    appendModifierMenu->addAction(appendProxyModifierAct);
    appendModifierMenu->addAction(appendLookAtModifierAct);
    appendModifierMenu->addAction(appendMirrorModifierAct);
    appendModifierMenu->addAction(appendGetWorldFromModelModifierAct);
    appendModifierMenu->addAction(appendSenseHandleModifierAct);
    appendModifierMenu->addAction(appendEvaluateExpressionModifierAct);
    appendModifierMenu->addAction(appendEvaluateHandleModifierAct);
    appendModifierMenu->addAction(appendAttachmentModifierAct);
    appendModifierMenu->addAction(appendAttributeModifierAct);
    appendModifierMenu->addAction(appendCombineTransformsModifierAct);
    appendModifierMenu->addAction(appendComputeRotationFromAxisAngleModifierAct);
    appendModifierMenu->addAction(appendComputeRotationToTargetModifierAct);
    appendModifierMenu->addAction(appendEventsFromRangeModifierAct);
    appendModifierMenu->addAction(appendMoveCharacterModifierAct);
    appendModifierMenu->addAction(appendExtractRagdollPoseModifierAct);
    appendModifierMenu->addAction(appendModifyOnceModifierAct);
    appendModifierMenu->addAction(appendEventOnDeactivateModifierAct);
    appendModifierMenu->addAction(appendEventEveryNEventsModifierAct);
    appendModifierMenu->addAction(appendRagdollContactListenerModifierAct);
    appendModifierMenu->addAction(appendPoweredRagdollControlsModifierAct);
    appendModifierMenu->addAction(appendEventOnFalseToTrueModifierAct);
    appendModifierMenu->addAction(appendDirectAtModifierAct);
    appendModifierMenu->addAction(appendDistTriggerModifierAct);
    appendModifierMenu->addAction(appendDecomposeVectorModifierAct);
    appendModifierMenu->addAction(appendComputeAddBoneAnimModifierAct);
    appendModifierMenu->addAction(appendTweenerModifierAct);
    appendModifierMenu->addAction(appendBSIStateManagerModifierAct);
    appendModifierMenu->addAction(appendTimerModifierAct);
    appendModifierMenu->addAction(appendRotateCharacterModifierAct);
    appendModifierMenu->addAction(appendDampingModifierAct);
    appendModifierMenu->addAction(appendDelayedModifierAct);
    appendModifierMenu->addAction(appendGetUpModifierAct);
    appendModifierMenu->addAction(appendKeyframeBonesModifierAct);
    appendModifierMenu->addAction(appendComputeDirectionModifierAct);
    appendModifierMenu->addAction(appendRigidBodyRagdollControlsModifierAct);
    appendModifierMenu->addAction(appendBSSpeedSamplerModifierAct);
    appendModifierMenu->addAction(appendDetectCloseToGroundModifierAct);
    appendModifierMenu->addAction(appendBSTimerModifierAct);
    appendModifierMenu->addAction(appendBSLookAtModifierAct);
    appendModifierMenu->addAction(appendBSPassByTargetTriggerModifierAct);
    appendModifierMenu->addAction(appendHandIKControlsModifierAct);
    contextMenu->addAction(removeObjBranchAct);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(appendStateMachineAct, SIGNAL(triggered()), this, SLOT(appendStateMachine()), Qt::UniqueConnection);
    connect(appendStateAct, SIGNAL(triggered()), this, SLOT(appendState()), Qt::UniqueConnection);
    connect(appendManualSelectorGeneratorAct, SIGNAL(triggered()), this, SLOT(appendManualSelectorGenerator()), Qt::UniqueConnection);
    connect(appendModifierGeneratorAct, SIGNAL(triggered()), this, SLOT(appendModifierGenerator()), Qt::UniqueConnection);
    connect(appendBSIStateTaggingGeneratorAct, SIGNAL(triggered()), this, SLOT(appendIStateTaggingGenerator()), Qt::UniqueConnection);
    connect(appendBSSynchronizedClipGeneratorAct, SIGNAL(triggered()), this, SLOT(appendSynchronizedClipGenerator()), Qt::UniqueConnection);
    connect(appendBSOffsetAnimationGeneratorAct, SIGNAL(triggered()), this, SLOT(appendOffsetAnimationGenerator()), Qt::UniqueConnection);
    connect(appendBSCyclicBlendTransitionGeneratorAct, SIGNAL(triggered()), this, SLOT(appendCyclicBlendTransitionGenerator()), Qt::UniqueConnection);
    connect(appendPoseMatchingGeneratorAct, SIGNAL(triggered()), this, SLOT(appendPoseMatchingGenerator()), Qt::UniqueConnection);
    connect(appendBlenderGeneratorAct, SIGNAL(triggered()), this, SLOT(appendBlenderGenerator()), Qt::UniqueConnection);
    connect(appendBlenderGeneratorChildAct, SIGNAL(triggered()), this, SLOT(appendBlenderGeneratorChild()), Qt::UniqueConnection);
    connect(appendBSBoneSwitchGeneratorAct, SIGNAL(triggered()), this, SLOT(appendBoneSwitchGenerator()), Qt::UniqueConnection);
    connect(appendBSBoneSwitchGeneratorChildAct, SIGNAL(triggered()), this, SLOT(appendBoneSwitchGeneratorChild()), Qt::UniqueConnection);
    connect(appendClipGeneratorAct, SIGNAL(triggered()), this, SLOT(appendClipGenerator()), Qt::UniqueConnection);
    connect(appendBehaviorReferenceGeneratorAct, SIGNAL(triggered()), this, SLOT(appendBehaviorReferenceGenerator()), Qt::UniqueConnection);
    connect(appendBGSGamebryoSequenceGeneratorAct, SIGNAL(triggered()), this, SLOT(appendBGSGamebryoSequenceGenerator()), Qt::UniqueConnection);
    connect(wrapManualSelectorGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapManualSelectorGenerator()), Qt::UniqueConnection);
    connect(wrapModifierGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapModifierGenerator()), Qt::UniqueConnection);
    connect(wrapBSIStateTaggingGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapIStateTaggingGenerator()), Qt::UniqueConnection);
    connect(wrapBSSynchronizedClipGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapSynchronizedClipGenerator()), Qt::UniqueConnection);
    connect(wrapBSOffsetAnimationGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapOffsetAnimationGenerator()), Qt::UniqueConnection);
    connect(wrapBSCyclicBlendTransitionGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapCyclicBlendTransitionGenerator()), Qt::UniqueConnection);
    connect(wrapBSBoneSwitchGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapBoneSwitchGenerator()), Qt::UniqueConnection);
    connect(wrapStateMachineAct, SIGNAL(triggered()), this, SLOT(wrapStateMachine()), Qt::UniqueConnection);
    connect(wrapBlenderGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapBlenderGenerator()), Qt::UniqueConnection);
    connect(wrapPoseMatchingGeneratorAct, SIGNAL(triggered()), this, SLOT(wrapPoseMatchingGenerator()), Qt::UniqueConnection);
    connect(appendModifierListAct, SIGNAL(triggered()), this, SLOT(appendModifierList()), Qt::UniqueConnection);
    connect(appendTwistModifierAct, SIGNAL(triggered()), this, SLOT(appendTwistModifier()), Qt::UniqueConnection);
    connect(appendEventDrivenModifierAct, SIGNAL(triggered()), this, SLOT(appendEventDrivenModifier()), Qt::UniqueConnection);
    connect(appendIsActiveModifierAct, SIGNAL(triggered()), this, SLOT(appendIsActiveModifier()), Qt::UniqueConnection);
    connect(appendLimbIKModifierAct, SIGNAL(triggered()), this, SLOT(appendLimbIKModifier()), Qt::UniqueConnection);
    connect(appendInterpValueModifierAct, SIGNAL(triggered()), this, SLOT(appendInterpValueModifier()), Qt::UniqueConnection);
    connect(appendGetTimeStepModifierAct, SIGNAL(triggered()), this, SLOT(appendGetTimeStepModifier()), Qt::UniqueConnection);
    connect(appendFootIkControlsModifierAct, SIGNAL(triggered()), this, SLOT(appendFootIkControlsModifier()), Qt::UniqueConnection);
    connect(appendGetHandleOnBoneModifierAct, SIGNAL(triggered()), this, SLOT(appendGetHandleOnBoneModifier()), Qt::UniqueConnection);
    connect(appendTransformVectorModifierAct, SIGNAL(triggered()), this, SLOT(appendTransformVectorModifier()), Qt::UniqueConnection);
    connect(appendProxyModifierAct, SIGNAL(triggered()), this, SLOT(appendProxyModifier()), Qt::UniqueConnection);
    connect(appendLookAtModifierAct, SIGNAL(triggered()), this, SLOT(appendLookAtModifier()), Qt::UniqueConnection);
    connect(appendMirrorModifierAct, SIGNAL(triggered()), this, SLOT(appendMirrorModifier()), Qt::UniqueConnection);
    connect(appendGetWorldFromModelModifierAct, SIGNAL(triggered()), this, SLOT(appendGetWorldFromModelModifier()), Qt::UniqueConnection);
    connect(appendSenseHandleModifierAct, SIGNAL(triggered()), this, SLOT(appendSenseHandleModifier()), Qt::UniqueConnection);
    connect(appendEvaluateExpressionModifierAct, SIGNAL(triggered()), this, SLOT(appendEvaluateExpressionModifier()), Qt::UniqueConnection);
    connect(appendEvaluateHandleModifierAct, SIGNAL(triggered()), this, SLOT(appendEvaluateHandleModifier()), Qt::UniqueConnection);
    connect(appendAttachmentModifierAct, SIGNAL(triggered()), this, SLOT(appendAttachmentModifier()), Qt::UniqueConnection);
    connect(appendAttributeModifierAct, SIGNAL(triggered()), this, SLOT(appendAttributeModifier()), Qt::UniqueConnection);
    connect(appendCombineTransformsModifierAct, SIGNAL(triggered()), this, SLOT(appendCombineTransformsModifier()), Qt::UniqueConnection);
    connect(appendComputeRotationFromAxisAngleModifierAct, SIGNAL(triggered()), this, SLOT(appendComputeRotationFromAxisAngleModifier()), Qt::UniqueConnection);
    connect(appendComputeRotationToTargetModifierAct, SIGNAL(triggered()), this, SLOT(appendComputeRotationToTargetModifier()), Qt::UniqueConnection);
    connect(appendEventsFromRangeModifierAct, SIGNAL(triggered()), this, SLOT(appendEventsFromRangeModifier()), Qt::UniqueConnection);
    connect(appendMoveCharacterModifierAct, SIGNAL(triggered()), this, SLOT(appendMoveCharacterModifier()), Qt::UniqueConnection);
    connect(appendExtractRagdollPoseModifierAct, SIGNAL(triggered()), this, SLOT(appendExtractRagdollPoseModifier()), Qt::UniqueConnection);
    connect(appendModifyOnceModifierAct, SIGNAL(triggered()), this, SLOT(appendModifyOnceModifier()), Qt::UniqueConnection);
    connect(appendEventOnDeactivateModifierAct, SIGNAL(triggered()), this, SLOT(appendEventOnDeactivateModifier()), Qt::UniqueConnection);
    connect(appendEventEveryNEventsModifierAct, SIGNAL(triggered()), this, SLOT(appendEventEveryNEventsModifier()), Qt::UniqueConnection);
    connect(appendRagdollContactListenerModifierAct, SIGNAL(triggered()), this, SLOT(appendRagdollContactListenerModifier()), Qt::UniqueConnection);
    connect(appendPoweredRagdollControlsModifierAct, SIGNAL(triggered()), this, SLOT(appendPoweredRagdollControlsModifier()), Qt::UniqueConnection);
    connect(appendEventOnFalseToTrueModifierAct, SIGNAL(triggered()), this, SLOT(appendEventOnFalseToTrueModifier()), Qt::UniqueConnection);
    connect(appendDirectAtModifierAct, SIGNAL(triggered()), this, SLOT(appendDirectAtModifier()), Qt::UniqueConnection);
    connect(appendDistTriggerModifierAct, SIGNAL(triggered()), this, SLOT(appendDistTriggerModifier()), Qt::UniqueConnection);
    connect(appendDecomposeVectorModifierAct, SIGNAL(triggered()), this, SLOT(appendDecomposeVectorModifier()), Qt::UniqueConnection);
    connect(appendComputeAddBoneAnimModifierAct, SIGNAL(triggered()), this, SLOT(appendComputeAddBoneAnimModifier()), Qt::UniqueConnection);
    connect(appendTweenerModifierAct, SIGNAL(triggered()), this, SLOT(appendTweenerModifier()), Qt::UniqueConnection);
    connect(appendBSIStateManagerModifierAct, SIGNAL(triggered()), this, SLOT(appendBSIStateManagerModifier()), Qt::UniqueConnection);
    connect(appendTimerModifierAct, SIGNAL(triggered()), this, SLOT(appendTimerModifier()), Qt::UniqueConnection);
    connect(appendRotateCharacterModifierAct, SIGNAL(triggered()), this, SLOT(appendRotateCharacterModifier()), Qt::UniqueConnection);
    connect(appendDampingModifierAct, SIGNAL(triggered()), this, SLOT(appendDampingModifier()), Qt::UniqueConnection);
    connect(appendDelayedModifierAct, SIGNAL(triggered()), this, SLOT(appendDelayedModifier()), Qt::UniqueConnection);
    connect(appendGetUpModifierAct, SIGNAL(triggered()), this, SLOT(appendGetUpModifier()), Qt::UniqueConnection);
    connect(appendKeyframeBonesModifierAct, SIGNAL(triggered()), this, SLOT(appendKeyframeBonesModifier()), Qt::UniqueConnection);
    connect(appendComputeDirectionModifierAct, SIGNAL(triggered()), this, SLOT(appendComputeDirectionModifier()), Qt::UniqueConnection);
    connect(appendRigidBodyRagdollControlsModifierAct, SIGNAL(triggered()), this, SLOT(appendRigidBodyRagdollControlsModifier()), Qt::UniqueConnection);
    connect(appendBSSpeedSamplerModifierAct, SIGNAL(triggered()), this, SLOT(appendBSSpeedSamplerModifier()), Qt::UniqueConnection);
    connect(appendDetectCloseToGroundModifierAct, SIGNAL(triggered()), this, SLOT(appendDetectCloseToGroundModifier()), Qt::UniqueConnection);
    connect(appendBSTimerModifierAct, SIGNAL(triggered()), this, SLOT(appendBSTimerModifier()), Qt::UniqueConnection);
    connect(appendBSLookAtModifierAct, SIGNAL(triggered()), this, SLOT(appendBSLookAtModifier()), Qt::UniqueConnection);
    connect(appendHandIKControlsModifierAct, SIGNAL(triggered()), this, SLOT(appendHandIKControlsModifier()), Qt::UniqueConnection);
    connect(appendBSPassByTargetTriggerModifierAct, SIGNAL(triggered()), this, SLOT(appendBSPassByTargetTriggerModifier()), Qt::UniqueConnection);
    connect(removeObjBranchAct, SIGNAL(triggered()), this, SLOT(deleteSelectedObjectBranchSlot()), Qt::UniqueConnection);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popUpMenuRequested(QPoint)), Qt::UniqueConnection);
}

bool BehaviorGraphView::confirmationDialogue(const QString & message, QWidget *parent){
    auto ret = QMessageBox::warning(parent, "Skyrim Behavior Tool", message, QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes){
        return true;
    }
    return false;
}

TreeGraphicsItem * BehaviorGraphView::getSelectedIconsChildIcon(HkxObject *child){
    auto selecteditem = getSelectedItem();
    if (selecteditem){
        return selecteditem->getChildWithData(static_cast<DataIconManager*>(child));
    }
    return nullptr;
}

QStringList BehaviorGraphView::getEventNames() const{
    if (behavior){
        return behavior->getEventNames();
    }else{
        LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
    }
    return QStringList();
}

void BehaviorGraphView::removeGeneratorData(){
    auto removedIndices = behavior->removeGeneratorData();
    for (auto i = 0; i < removedIndices.size(); i++){
        emit removedGenerator(removedIndices.at(i) + 1);
    }
}

void BehaviorGraphView::removeModifierData(){
    auto removedIndices = behavior->removeModifierData();
    for (auto i = 0; i < removedIndices.size(); i++){
        emit removedModifier(removedIndices.at(i) + 1);
    }
}

void BehaviorGraphView::removeOtherData(){
    (behavior) ? behavior->removeOtherData() : LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
}

bool BehaviorGraphView::refocus(){
    auto selecteditem = getSelectedItem();
    if (selecteditem){
        centerOn(selecteditem);
        return true;
    }
    return false;
}

void BehaviorGraphView::setSelectedItem(TreeGraphicsItem *item){
    (scene()) ? static_cast<TreeGraphicsScene *>(scene())->selectIcon(item, TreeGraphicsScene::EXPAND_CONTRACT_ONE) : NULL;
}

QString BehaviorGraphView::getBehaviorFilename() const{
    if (behavior){
        return behavior->fileName();
    }else{
        LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
    }
    return "";
}

void BehaviorGraphView::focusOnGeneratorIcon(int index, const QString &){
    (behavior) ? behavior->setFocusGeneratorIcon(index) : LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
    emit disconnectTablesFromHkDataUI();
}

void BehaviorGraphView::focusOnModifierIcon(int index, const QString &){
    (behavior) ? behavior->setFocusModifierIcon(index) : LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
    emit disconnectTablesFromHkDataUI();
}

void BehaviorGraphView::deleteSelectedObjectBranchSlot(){
    auto selecteditem = getSelectedItem();
    if (selecteditem){
        selecteditem->unselect();
        removeItemFromGraph(selecteditem, 0, true, true);
        ui->changeCurrentDataWidget(nullptr);
        removeObjects();
    }
}

void BehaviorGraphView::removeObjects(){
    if (behavior){
        removeGeneratorData();
        removeModifierData();
        behavior->removeOtherData();
        behavior->setIsChanged(true);
    }else{
        LogFile::writeToLog("BehaviorGraphView: behavior file is nullptr!");
    }
}

void BehaviorGraphView::deleteAllObjectBranches(){
    if (behavior && ui){
        auto graph = static_cast<hkbBehaviorGraph *>(behavior->getBehaviorGraph());
        if (graph->getNumberOfIcons() > 0){
            auto firsticon = graph->getFirstIcon();
            if (firsticon){
                auto list = firsticon->childItems();
                if (!list.isEmpty()){
                    auto root = static_cast<TreeGraphicsItem *>(list.first());
                    root->unselect();
                    removeItemFromGraph(root, 0, true, true);
                    ui->changeCurrentDataWidget(nullptr);
                    removeObjects();
                }
            }else{
                LogFile::writeToLog("BehaviorGraphView: behavior graph has no icon!");
            }
        }
    }else{
        LogFile::writeToLog("BehaviorGraphView: behavior file or ui are nullptr!");
    }
}

BehaviorFile *BehaviorGraphView::getBehavior() const{
    return behavior;
}

void BehaviorGraphView::appendStateMachine(){
    append(new hkbStateMachine(behavior));
}

void BehaviorGraphView::appendState(){
    append(new hkbStateMachineStateInfo(behavior, (hkbStateMachine *)getSelectedData())) ;
}

void BehaviorGraphView::appendManualSelectorGenerator(){
    append(new hkbManualSelectorGenerator(behavior));
}

void BehaviorGraphView::appendModifierGenerator(){
    append(new hkbModifierGenerator(behavior));
}

void BehaviorGraphView::appendIStateTaggingGenerator(){
    append(new BSiStateTaggingGenerator(behavior));
}

void BehaviorGraphView::appendSynchronizedClipGenerator(){
    append(new BSSynchronizedClipGenerator(behavior));
}

void BehaviorGraphView::appendOffsetAnimationGenerator(){
    append(new BSOffsetAnimationGenerator(behavior));
}

void BehaviorGraphView::appendCyclicBlendTransitionGenerator(){
    append(new BSCyclicBlendTransitionGenerator(behavior));
}

void BehaviorGraphView::appendPoseMatchingGenerator(){
    append(new hkbPoseMatchingGenerator(behavior));
}

void BehaviorGraphView::appendBlenderGenerator(){
    append(new hkbBlenderGenerator(behavior));
}

void BehaviorGraphView::appendBlenderGeneratorChild(){
    append(new hkbBlenderGeneratorChild(behavior, (hkbBlenderGenerator *)getSelectedData())) ;
}

void BehaviorGraphView::appendBoneSwitchGenerator(){
    append(new BSBoneSwitchGenerator(behavior));
}

void BehaviorGraphView::appendBoneSwitchGeneratorChild(){
    append(new BSBoneSwitchGeneratorBoneData(behavior, (BSBoneSwitchGenerator *)getSelectedData())) ;
}

void BehaviorGraphView::appendClipGenerator(){
    append(new hkbClipGenerator(behavior, 0, true));
}

void BehaviorGraphView::appendBehaviorReferenceGenerator(){
    append(new hkbBehaviorReferenceGenerator(behavior));
}

void BehaviorGraphView::appendBGSGamebryoSequenceGenerator(){
    append(new BGSGamebryoSequenceGenerator(behavior));
}

void BehaviorGraphView::appendModifierList(){
    append(new hkbModifierList(behavior));
}

void BehaviorGraphView::appendTwistModifier(){
    append(new hkbTwistModifier(behavior));
}

void BehaviorGraphView::appendEventDrivenModifier(){
    append(new hkbEventDrivenModifier(behavior));
}

void BehaviorGraphView::appendIsActiveModifier(){
    append(new BSIsActiveModifier(behavior));
}

void BehaviorGraphView::appendLimbIKModifier(){
    append(new BSLimbIKModifier(behavior));
}

void BehaviorGraphView::appendInterpValueModifier(){
    append(new BSInterpValueModifier(behavior));
}

void BehaviorGraphView::appendGetTimeStepModifier(){
    append(new BSGetTimeStepModifier(behavior));
}

void BehaviorGraphView::appendFootIkControlsModifier(){
    append(new hkbFootIkControlsModifier(behavior));
}

void BehaviorGraphView::appendGetHandleOnBoneModifier(){
    append(new hkbGetHandleOnBoneModifier(behavior));
}

void BehaviorGraphView::appendTransformVectorModifier(){
    append(new hkbTransformVectorModifier(behavior));
}

void BehaviorGraphView::appendProxyModifier(){
    append(new hkbProxyModifier(behavior));
}

void BehaviorGraphView::appendLookAtModifier(){
    append(new hkbLookAtModifier(behavior));
}

void BehaviorGraphView::appendMirrorModifier(){
    append(new hkbMirrorModifier(behavior));
}

void BehaviorGraphView::appendGetWorldFromModelModifier(){
    append(new hkbGetWorldFromModelModifier(behavior));
}

void BehaviorGraphView::appendSenseHandleModifier(){
    append(new hkbSenseHandleModifier(behavior));
}

void BehaviorGraphView::appendEvaluateExpressionModifier(){
    append(new hkbEvaluateExpressionModifier(behavior));
}

void BehaviorGraphView::appendEvaluateHandleModifier(){
    append(new hkbEvaluateHandleModifier(behavior));
}

void BehaviorGraphView::appendAttachmentModifier(){
    append(new hkbAttachmentModifier(behavior));
}

void BehaviorGraphView::appendAttributeModifier(){
    append(new hkbAttributeModifier(behavior));
}

void BehaviorGraphView::appendCombineTransformsModifier(){
    append(new hkbCombineTransformsModifier(behavior));
}

void BehaviorGraphView::appendComputeRotationFromAxisAngleModifier(){
    append(new hkbComputeRotationFromAxisAngleModifier(behavior));
}

void BehaviorGraphView::appendComputeRotationToTargetModifier(){
    append(new hkbComputeRotationToTargetModifier(behavior));
}

void BehaviorGraphView::appendEventsFromRangeModifier(){
    append(new hkbEventsFromRangeModifier(behavior));
}

void BehaviorGraphView::appendMoveCharacterModifier(){
    append(new hkbMoveCharacterModifier(behavior));
}

void BehaviorGraphView::appendExtractRagdollPoseModifier(){
    append(new hkbExtractRagdollPoseModifier(behavior));
}

void BehaviorGraphView::appendModifyOnceModifier(){
    append(new BSModifyOnceModifier(behavior));
}

void BehaviorGraphView::appendEventOnDeactivateModifier(){
    append(new BSEventOnDeactivateModifier(behavior));
}

void BehaviorGraphView::appendEventEveryNEventsModifier(){
    append(new BSEventEveryNEventsModifier(behavior));
}

void BehaviorGraphView::appendRagdollContactListenerModifier(){
    append(new BSRagdollContactListenerModifier(behavior));
}

void BehaviorGraphView::appendPoweredRagdollControlsModifier(){
    append(new hkbPoweredRagdollControlsModifier(behavior));
}

void BehaviorGraphView::appendEventOnFalseToTrueModifier(){
    append(new BSEventOnFalseToTrueModifier(behavior));
}

void BehaviorGraphView::appendDirectAtModifier(){
    append(new BSDirectAtModifier(behavior));
}

void BehaviorGraphView::appendDistTriggerModifier(){
    append(new BSDistTriggerModifier(behavior));
}

void BehaviorGraphView::appendDecomposeVectorModifier(){
    append(new BSDecomposeVectorModifier(behavior));
}

void BehaviorGraphView::appendComputeAddBoneAnimModifier(){
    append(new BSComputeAddBoneAnimModifier(behavior));
}

void BehaviorGraphView::appendTweenerModifier(){
    append(new BSTweenerModifier(behavior));
}

void BehaviorGraphView::appendBSIStateManagerModifier(){
    append(new BSIStateManagerModifier(behavior));
}

void BehaviorGraphView::appendTimerModifier(){
    append(new hkbTimerModifier(behavior));
}

void BehaviorGraphView::appendRotateCharacterModifier(){
    append(new hkbRotateCharacterModifier(behavior));
}

void BehaviorGraphView::appendDampingModifier(){
    append(new hkbDampingModifier(behavior));
}

void BehaviorGraphView::appendDelayedModifier(){
    append(new hkbDelayedModifier(behavior));
}

void BehaviorGraphView::appendGetUpModifier(){
    append(new hkbGetUpModifier(behavior));
}

void BehaviorGraphView::appendKeyframeBonesModifier(){
    append(new hkbKeyframeBonesModifier(behavior));
}

void BehaviorGraphView::appendComputeDirectionModifier(){
    append(new hkbComputeDirectionModifier(behavior));
}

void BehaviorGraphView::appendRigidBodyRagdollControlsModifier(){
    append(new hkbRigidBodyRagdollControlsModifier(behavior));
}

void BehaviorGraphView::appendBSSpeedSamplerModifier(){
    append(new BSSpeedSamplerModifier(behavior));
}

void BehaviorGraphView::appendDetectCloseToGroundModifier(){
    append(new hkbDetectCloseToGroundModifier(behavior));
}

void BehaviorGraphView::appendBSTimerModifier(){
    append(new BSTimerModifier(behavior));
}

void BehaviorGraphView::appendBSLookAtModifier(){
    append(new BSLookAtModifier(behavior));
}

void BehaviorGraphView::appendBSPassByTargetTriggerModifier(){
    append(new BSPassByTargetTriggerModifier(behavior));
}

void BehaviorGraphView::appendHandIKControlsModifier(){
    append(new hkbHandIkControlsModifier(behavior));
}

template <typename T>
void BehaviorGraphView::append(T *obj){
    auto selecteditem = getSelectedItem();
    if (selecteditem && behavior){
        TreeGraphicsItem *newIcon = nullptr;
        auto selectedItemData = selecteditem->getItemData();
        if (selectedItemData){
            auto sig = selectedItemData->getSignature();
            if (selectedItemData->hasChildren() && sig != HKB_STATE_MACHINE && sig != HKB_MANUAL_SELECTOR_GENERATOR && sig != HKB_BLENDER_GENERATOR && sig != BS_BONE_SWITCH_GENERATOR && sig != HKB_POSE_MATCHING_GENERATOR && sig != HKB_MODIFIER_LIST){
                /*if (sig == HKB_MODIFIER_GENERATOR){
                    if (!selectedItemData->getIndexOfObj(nullptr) && obj->getType() == HkxObject::TYPE_MODIFIER && !confirmationDialogue("WARNING! THIS WILL REPLACE THE CURRENT MODIFIER!!!\n\nARE YOU SURE YOU WANT TO DO THIS?", this)){
                        behavior->removeModifierData();
                    }else if (selectedItemData->getIndexOfObj(nullptr) == 1 && obj->getType() == HkxObject::TYPE_GENERATOR && !confirmationDialogue("WARNING! THIS WILL REPLACE THE CURRENT MODIFIER!!!\n\nARE YOU SURE YOU WANT TO DO THIS?", this)){
                        behavior->removeGeneratorData();
                    }
                    return;
                }else{*/
                    if (!confirmationDialogue("WARNING! THIS WILL REPLACE THE CURRENT GENERATOR/MODIFIER!!!\n\nARE YOU SURE YOU WANT TO DO THIS?", this)){
                        if (obj->getType() == HkxObject::TYPE_GENERATOR){
                            behavior->removeGeneratorData();
                        }else if (obj->getType() == HkxObject::TYPE_MODIFIER){
                            behavior->removeModifierData();
                        }/*else if (obj->getType() == HkxObject::TYPE_OTHER){
                            behavior->removeOtherData();
                        }*/
                        return;
                    //}
                }
                if (!selecteditem->childItems().isEmpty()){
                    if (sig == HKB_MODIFIER_GENERATOR){
                        if (selecteditem->childItems().size() == 2 && obj->getType() == HkxObject::TYPE_GENERATOR){
                            removeItemFromGraph(((TreeGraphicsItem *)selecteditem->childItems()[1]), 1);
                        }else if (obj->getType() == HkxObject::TYPE_MODIFIER){
                            if (((TreeGraphicsItem *)selecteditem->childItems().first())->getItemData()->getType() == HkxObject::TYPE_MODIFIER){
                                removeItemFromGraph(((TreeGraphicsItem *)selecteditem->childItems().first()), 0);
                                newIcon = addItemToGraph(selecteditem, obj, 0);
                            }else{
                                newIcon = addItemToGraph(selecteditem, obj, 0);
                            }
                        }
                    }else{
                        removeItemFromGraph(((TreeGraphicsItem *)selecteditem->childItems().first()), 0);
                    }
                }
            }
            if (!newIcon){
                newIcon = addItemToGraph(selecteditem, obj, -1);
            }
            if (((TreeGraphicsItem *)selecteditem->parentItem())){
                ((HkxObject *)((TreeGraphicsItem *)selecteditem->parentItem())->itemData)->evaluateDataValidity();
            }
            selectedItemData->evaluateDataValidity();
            behavior->setIsChanged(true);
            getSelectedItem()->reposition();
            treeScene->selectIcon(newIcon, TreeGraphicsScene::EXPAND_CONTRACT_ZERO);    //Removing this breaks adding child with generator!!!
            auto rect = sceneRect();
            setSceneRect(rect.marginsAdded(QMarginsF(0, 0, newIcon->boundingRect().width()*2, newIcon->boundingRect().height()*2)));
            if (obj->getType() == HkxObject::TYPE_GENERATOR){
                emit addedGenerator(obj->getName(), obj->getClassname());
            }else if (obj->getType() == HkxObject::TYPE_MODIFIER){
                emit addedModifier(obj->getName(), obj->getClassname());
            }
        }else{
            delete obj;
            LogFile::writeToLog("BehaviorGraphView::append(): \n The selected icon has no data!!");
        }
    }
}

template <typename T>
void BehaviorGraphView::wrap(T *obj){
    if (getSelectedItem() && ((TreeGraphicsItem *)getSelectedItem()->parentItem()) && ((TreeGraphicsItem *)getSelectedItem()->parentItem())->itemData){
        behavior->setIsChanged(true);
        auto newIcon = addItemToGraph(getSelectedItem(), static_cast<DataIconManager*>((obj)), -1, true);
        behavior->setIsChanged(true);
        getSelectedItem()->reposition();
        treeScene->selectIcon(newIcon, TreeGraphicsScene::EXPAND_CONTRACT_ZERO);
        emit addedGenerator(obj->getName(), obj->getClassname());
    }
}

void BehaviorGraphView::wrapManualSelectorGenerator(){
    wrap(new hkbManualSelectorGenerator(behavior));
}

void BehaviorGraphView::wrapModifierGenerator(){
    wrap(new hkbModifierGenerator(behavior));
}

void BehaviorGraphView::wrapIStateTaggingGenerator(){
    wrap(new BSiStateTaggingGenerator(behavior));
}

void BehaviorGraphView::wrapSynchronizedClipGenerator(){
    wrap(new BSSynchronizedClipGenerator(behavior));
}

void BehaviorGraphView::wrapOffsetAnimationGenerator(){
    wrap(new BSOffsetAnimationGenerator(behavior));
}

void BehaviorGraphView::wrapCyclicBlendTransitionGenerator(){
    wrap(new BSCyclicBlendTransitionGenerator(behavior));
}

void BehaviorGraphView::wrapBoneSwitchGenerator(){
    auto bonegen = new BSBoneSwitchGenerator(behavior);
    wrap(new BSBoneSwitchGeneratorBoneData(behavior, bonegen));
    wrap(bonegen);
}

void BehaviorGraphView::wrapStateMachine(){
    auto statemachine = new hkbStateMachine(behavior);
    wrap(new hkbStateMachineStateInfo(behavior, statemachine));
    wrap(statemachine);
}

void BehaviorGraphView::wrapBlenderGenerator(){
    auto blender = new hkbBlenderGenerator(behavior);
    wrap(new hkbBlenderGeneratorChild(behavior, blender));
    wrap(blender);
}

void BehaviorGraphView::wrapPoseMatchingGenerator(){
    auto pose = new hkbPoseMatchingGenerator(behavior);
    wrap(new hkbBlenderGeneratorChild(behavior, pose));
    wrap(pose);
}

void BehaviorGraphView::enableAllMenuActions(QMenu *menu){
    auto actions = menu->actions();
    for (auto i = 0; i < actions.size(); i++){
        actions.at(i)->setDisabled(false);
    }
}

void BehaviorGraphView::disableAllMenuActions(QMenu *menu){
    auto actions = menu->actions();
    for (auto i = 0; i < actions.size(); i++){
        actions.at(i)->setDisabled(true);
    }
}

void BehaviorGraphView::popUpMenuRequested(const QPoint &pos){
    if (!getSelectedItem() || !getSelectedItem()->itemData){
        return;
    }
    auto sig = ((HkxObject *)(getSelectedItem()->itemData))->getSignature();
    auto parentSig = NULL_SIGNATURE;
    if (getSelectedItem()->parentItem()){
        parentSig = ((HkxObject *)(((TreeGraphicsItem *)(getSelectedItem()->parentItem()))->itemData))->getSignature();
    }
    auto type = ((HkxObject *)(getSelectedItem()->itemData))->getType();
    removeObjBranchAct->setDisabled(false);
    appendStateAct->setDisabled(true);
    if (type == HkxObject::TYPE_MODIFIER){
        appendGeneratorMenu->menuAction()->setDisabled(true);
        appendBlenderMenu->menuAction()->setDisabled(true);
        wrapGeneratorMenu->menuAction()->setDisabled(true);
        wrapBlenderMenu->menuAction()->setDisabled(true);
        if (sig == HKB_MODIFIER_LIST || sig == HKB_EVENT_DRIVEN_MODIFIER){
            appendModifierMenu->menuAction()->setDisabled(false);
        }else{
            appendModifierMenu->menuAction()->setDisabled(true);
        }/*else if (sig == HKB_MODIFIER_LIST){
            //enable action
        }*/
    }else if (sig == HKB_STATE_MACHINE){
        disableAllMenuActions(appendGeneratorMenu);
        appendBlenderMenu->menuAction()->setDisabled(true);
        appendModifierMenu->menuAction()->setDisabled(true);
        wrapGeneratorMenu->menuAction()->setDisabled(false);
        wrapBlenderMenu->menuAction()->setDisabled(false);
        appendStateAct->setDisabled(false);
    }else if (type == HkxObject::TYPE_GENERATOR){
        appendGeneratorMenu->menuAction()->setDisabled(false);
        enableAllMenuActions(appendGeneratorMenu);
        appendBlenderMenu->menuAction()->setDisabled(false);
        enableAllMenuActions(appendBlenderMenu);
        wrapGeneratorMenu->menuAction()->setDisabled(false);
        enableAllMenuActions(wrapGeneratorMenu);
        wrapBlenderMenu->menuAction()->setDisabled(false);
        enableAllMenuActions(wrapBlenderMenu);
        appendModifierMenu->menuAction()->setDisabled(true);
        appendStateAct->setDisabled(true);
        appendBlenderGeneratorChildAct->setDisabled(true);
        appendBSBoneSwitchGeneratorChildAct->setDisabled(true);
        if (sig == HKB_BLENDER_GENERATOR || sig == HKB_POSE_MATCHING_GENERATOR){
            disableAllMenuActions(appendGeneratorMenu);
            disableAllMenuActions(appendBlenderMenu);
            enableAllMenuActions(wrapGeneratorMenu);
            appendBlenderGeneratorChildAct->setDisabled(false);
        }else if (sig == BS_BONE_SWITCH_GENERATOR){
            enableAllMenuActions(wrapGeneratorMenu);
            if (static_cast<BSBoneSwitchGenerator *>(getSelectedItem()->itemData)->hasGenerator()){
                disableAllMenuActions(appendBlenderMenu);
                disableAllMenuActions(appendGeneratorMenu);
                appendBSBoneSwitchGeneratorChildAct->setDisabled(false);
            }else{
                enableAllMenuActions(appendGeneratorMenu);
            }
        }else if (sig == HKB_CLIP_GENERATOR || sig == HKB_BEHAVIOR_REFERENCE_GENERATOR || sig == BGS_GAMEBYRO_SEQUENCE_GENERATOR){
            appendGeneratorMenu->menuAction()->setDisabled(true);
            appendBlenderMenu->menuAction()->setDisabled(true);
            enableAllMenuActions(wrapGeneratorMenu);
        }else if (sig == BS_CYCLIC_BLEND_TRANSITION_GENERATOR){
            disableAllMenuActions(appendGeneratorMenu);
            appendBlenderGeneratorAct->setDisabled(false);
        }else if (sig == BS_SYNCHRONIZED_CLIP_GENERATOR){
            disableAllMenuActions(appendGeneratorMenu);
            appendClipGeneratorAct->setDisabled(false);
        }else if (sig == HKB_MODIFIER_GENERATOR){
            appendModifierMenu->menuAction()->setDisabled(false);
        }else if (sig == HKB_BEHAVIOR_GRAPH){
            disableAllMenuActions(appendGeneratorMenu);
            wrapGeneratorMenu->menuAction()->setDisabled(true);
            appendBlenderMenu->menuAction()->setDisabled(true);
            wrapBlenderMenu->menuAction()->setDisabled(true);
            appendStateMachineAct->setDisabled(false);
            removeObjBranchAct->setDisabled(true);
        }else if (sig == HKB_STATE_MACHINE_STATE_INFO || sig == HKB_BLENDER_GENERATOR_CHILD || sig == BS_BONE_SWITCH_GENERATOR_BONE_DATA){
            wrapGeneratorMenu->menuAction()->setDisabled(true);
            wrapBlenderMenu->menuAction()->setDisabled(true);
        }
        if (parentSig == BS_SYNCHRONIZED_CLIP_GENERATOR || parentSig == BS_OFFSET_ANIMATION_GENERATOR){
            wrapGeneratorMenu->menuAction()->setDisabled(true);
            wrapBlenderMenu->menuAction()->setDisabled(true);
        }else if (parentSig == BS_CYCLIC_BLEND_TRANSITION_GENERATOR){
            wrapGeneratorMenu->menuAction()->setDisabled(true);
            wrapBlenderGeneratorAct->setDisabled(false);
            wrapBSBoneSwitchGeneratorAct->setDisabled(true);
        }else if (parentSig == HKB_BEHAVIOR_GRAPH){
            wrapGeneratorMenu->menuAction()->setDisabled(true);
            wrapBlenderMenu->menuAction()->setDisabled(true);
        }
    }
    contextMenu->popup(viewport()->mapToGlobal(pos));
}

BehaviorGraphView::~BehaviorGraphView(){
    ui->loadBehaviorView(nullptr);
}
