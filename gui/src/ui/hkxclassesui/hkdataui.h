#ifndef HKDATAUI_H
#define HKDATAUI_H

#include <QGroupBox>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QStackedLayout;
class QLabel;
class MainWindow;
class BehaviorGraphView;
class TreeGraphicsItem;
class GenericTableWidget;
class MainWindow;

namespace UI {

class AnimationsUI;
class HkxObject;
class BSiStateTaggingGeneratorUI;
class ModifierGeneratorUI;
class ManualSelectorGeneratorUI;
class StateMachineUI;
class StateUI;
class BehaviorVariablesUI;
class EventsUI;
class TransitionsUI;
class HkTransition;
class hkbStateMachineStateInfo;
class hkbStateMachine;
class BlenderGeneratorUI;
class BehaviorGraphUI;
class BSLimbIKModifierUI;
class BSBoneSwitchGeneratorUI;
class BSOffsetAnimationGeneratorUI;
class BSCyclicBlendTransitionGeneratorUI;
class PoseMatchingGeneratorUI;
class ClipGeneratorUI;
class BSSynchronizedClipGeneratorUI;
class BehaviorReferenceGeneratorUI;
class BSDirectAtModifierUI;
class MoveCharacterModifierUI;
class RotateCharacterModifierUI;
class EvaluateExpressionModifierUI;
class ModifierListUI;
class EventDrivenModifierUI;
class GetHandleOnBoneModifierUI;
class EvaluateHandleModifierUI;
class SenseHandleModifierUI;
class BSDecomposeVectorModifierUI;
class BSIsActiveModifierUI;
class ComputeDirectionModifierUI;
class BSComputeAddBoneAnimModifierUI;
class BSDistTriggerModifierUI;
class BSInterpValueModifierUI;
class GetUpModifierUI;
class GetWorldFromModelModifierUI;
class TwistModifierUI;
class TimerModifierUI;
class DampingModifierUI;
class RigidBodyRagdollControlsModifierUI;
class PoweredRagdollControlsModifierUI;
class CombineTransformsModifierUI;
class ComputeRotationFromAxisAngleModifierUI;
class ComputeRotationToTargetModifierUI;
class TransformVectorModifierUI;
class LookAtModifierUI;
class KeyframeBonesModifierUI;
class FootIkControlsModifierUI;
class MirrorModifierUI;
class ExtractRagdollPoseModifierUI;
class BSTimerModifierUI;
class BSGetTimeStepModifierUI;
class DelayedModifierUI;
class BSRagdollContactListenerModifierUI;
class BSEventOnDeactivateModifierUI;
class BSSpeedSamplerModifierUI;
class BSPassByTargetTriggerModifierUI;
class BSLookAtModifierUI;
class DetectCloseToGroundModifierUI;
class BSEventEveryNEventsModifierUI;
class BSEventOnFalseToTrueModifierUI;
class BSModifyOnceModifierUI;
class HandIkControlsModifierUI;
class BSTweenerModifierUI;
class BGSGamebryoSequenceGeneratorUI;
class EventsFromRangeModifierUI;
class BlenderGeneratorChildUI;
class BSBoneSwitchGeneratorBoneDataUI;

/**
 * To add support for a new class we need to add it to the "DATA_TYPE_LOADED" enum, add it to the stacked layout in the correct order,
 * connect it's name change signal to this, deal with any variable event name changes, add it to the list of widgets to be loaded and
 * add it to the set behavior view function if necessary.
 */

class HkDataUI final: public QGroupBox
{
    Q_OBJECT
public:
    HkDataUI(const QString & title);
    HkDataUI& operator=(const HkDataUI&) = delete;
    HkDataUI(const HkDataUI &) = delete;
    ~HkDataUI() = default;
public:
    BehaviorGraphView * loadBehaviorView(BehaviorGraphView *view);
    void setEventsVariablesAnimationsUI(EventsUI *events, BehaviorVariablesUI *variables, AnimationsUI *animations);
    void unloadDataWidget();
public slots:
    void connectToGeneratorTable();
    void connectToModifierTable();
    void disconnectTables();
    void changeCurrentDataWidget(TreeGraphicsItem *icon);
    void modifierAdded(const QString & name, const QString & type);
    void modifierNameChanged(const QString & newName, int index);
    void modifierRemoved(int index);
    void generatorAdded(const QString & name, const QString & type);
    void generatorNameChanged(const QString & newName, int index);
    void generatorRemoved(int index);
    void eventNameChanged(const QString & newName, int index);
    void eventAdded(const QString & name);
    void eventRemoved(int index);
    //void animationNameChanged(const QString & newName, int index);
    void animationAdded(const QString & name);
    //void animationRemoved(int index);
    void variableNameChanged(const QString & newName, int index);
    void variableAdded(const QString & name, const QString & type);
    void variableRemoved(int index);
private:
    enum DATA_TYPE_LOADED{
        NO_DATA_SELECTED,
        BS_I_STATE_TAG_GEN,
        MODIFIER_GENERATOR,
        MANUAL_SELECTOR_GENERATOR,
        STATE_MACHINE,
        STATE,
        BLENDER_GENERATOR,
        BLENDER_GENERATOR_CHILD,
        BEHAVIOR_GRAPH,
        BS_LIMB_IK_MOD,
        BS_BONE_SWITCH_GENERATOR,
        BS_BONE_SWITCH_GENERATOR_CHILD,
        BS_OFFSET_ANIMATION_GENERATOR,
        BS_CYCLIC_BLEND_TRANSITION_GENERATOR,
        POSE_MATCHING_GENERATOR,
        CLIP_GENERATOR,
        SYNCHRONIZED_CLIP_GENERATOR,
        BEHAVIOR_REFERENCE_GENERATOR,
        DIRECT_AT_MODIFIER,
        MOVE_CHARACTER_MODIFIER,
        ROTATE_CHARACTER_MODIFIER,
        EVALUATE_EXPRESSION_MODIFIER,
        MODIFIER_LIST,
        EVENT_DRIVEN_MODIFIER,
        GET_HANDLE_ON_BONE_MODIFIER,
        EVALUATE_HANDLE_MODIFIER,
        SENSE_HANDLE_MODIFIER,
        BS_DECOMPOSE_VECTOR_MODIFIER,
        BS_IS_ACTIVE_MODIFIER,
        COMPUTE_DIRECTION_MODIFIER,
        BS_COMPUTE_ADD_BONE_ANIM_MODIFIER,
        BS_DIST_TRIGGER_MODIFER,
        BS_INTERP_VALUE_MODIFIER,
        GET_UP_MODIFIER,
        GET_WORLD_FROM_MODEL_MODIFIER,
        TWIST_MODIFIER,
        TIMER_MODIFIER,
        DAMPING_MODIFIER,
        RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER,
        POWERED_RAGDOLL_CONTROLS_MODIFIER,
        COMBINE_TRANSFORMS_MODIFIER,
        COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER,
        COMPUTE_ROTATION_TO_TARGET_MODIFIER,
        TRANSFORM_VECTOR_MODIFIER,
        LOOK_AT_MODIFIER,
        KEY_FRAME_BONES_MODIFIER,
        FOOT_IK_CONTROLS_MODIFIER,
        MIRROR_MODIFIER,
        EXTRACT_RAGDOLL_POSE_MODIFIER,
        BS_TIMER_MODIFIER,
        BS_GET_TIME_STEP_MODIFIER,
        DELAYED_MODIFIER,
        BS_RAGDOLL_CONTACT_LISTENER_MODIFIER,
        BS_EVENT_ON_DEACTIVATE_MODIFIER,
        BS_SPEED_SAMPLER_MODIFIER,
        BS_PASS_BY_TARGET_TRIGGER_MODIFIER,
        BS_LOOK_AT_MODIFIER,
        DETECT_CLOSE_TO_GROUND_MODIFIER,
        BS_EVENT_EVERY_N_EVENTS_MODIFIER,
        BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER,
        BS_MODIFY_ONCE_MODIFIER,
        HAND_IK_CONTROLS_MODIFIER,
        BS_TWEENER_MODIFIER,
        BGS_GAMEBYRO_SEQUENCE_GENERATOR,
        EVENTS_FROM_RANGE_MODIFIER
    };
private:
    template<typename UIWidget>
    void changeWidget(HkDataUI::DATA_TYPE_LOADED type, HkxObject *olddata, UIWidget *uiwidget, GenericTableWidget *table1, GenericTableWidget *table2);
    template<typename UIWidget>
    void changeWidget(HkDataUI::DATA_TYPE_LOADED type, HkxObject *olddata, UIWidget *uiwidget, GenericTableWidget *table1, GenericTableWidget *table2, GenericTableWidget *table3);
    template<typename UIWidget>
    void changeWidget(HkDataUI::DATA_TYPE_LOADED type, HkxObject *olddata, UIWidget *uiwidget, GenericTableWidget *table1, GenericTableWidget *table2, GenericTableWidget *table3, GenericTableWidget *table4);
private:
    static const QStringList generatorTypes;
    static const QStringList modifierTypes;
    static const QStringList variableTypes;
    EventsUI *eventsUI;
    BehaviorVariablesUI *variablesUI;
    AnimationsUI *animationsUI;
    BehaviorGraphView *behaviorView;
    QVBoxLayout *verLyt;
    QStackedLayout *stack;
    HkxObject *loadedData;
    GenericTableWidget *generatorsTable;
    GenericTableWidget *modifiersTable;
    GenericTableWidget *variablesTable;
    GenericTableWidget *eventsTable;
    GenericTableWidget *characterPropertiesTable;
    GenericTableWidget *animationsTable;
    GenericTableWidget *ragdollBonesTable;
    QLabel *noDataL;
    BSiStateTaggingGeneratorUI *iStateTagGenUI;
    ModifierGeneratorUI *modGenUI;
    ManualSelectorGeneratorUI *manSelGenUI;
    StateMachineUI *stateMachineUI;
    StateUI *stateUI;
    BlenderGeneratorUI *blenderGeneratorUI;
    BlenderGeneratorChildUI *blenderGeneratorChildUI;
    BehaviorGraphUI *behaviorGraphUI;
    BSLimbIKModifierUI *limbIKModUI;
    BSBoneSwitchGeneratorUI *boneSwitchUI;
    BSBoneSwitchGeneratorBoneDataUI *boneSwitchChildUI;
    BSOffsetAnimationGeneratorUI *offsetAnimGenUI;
    BSCyclicBlendTransitionGeneratorUI *cyclicBlendTransGenUI;
    PoseMatchingGeneratorUI *poseMatchGenUI;
    ClipGeneratorUI *clipGenUI;
    BSSynchronizedClipGeneratorUI *syncClipGenUI;
    BehaviorReferenceGeneratorUI *behaviorRefGenUI;
    BSDirectAtModifierUI *directAtModUI;
    MoveCharacterModifierUI *moveCharModUI;
    RotateCharacterModifierUI *rotateCharModUI;
    EvaluateExpressionModifierUI *evaluateExpModUI;
    ModifierListUI *modListUI;
    EventDrivenModifierUI *eventDrivenModUI;
    GetHandleOnBoneModifierUI *getHandleOnBoneUI;
    EvaluateHandleModifierUI *evaluateHandleModUI;
    SenseHandleModifierUI *senseHandleModUI;
    BSDecomposeVectorModifierUI *decomposeVectorModUI;
    BSIsActiveModifierUI *isActiveModUI;
    ComputeDirectionModifierUI *computeDirMod;
    BSComputeAddBoneAnimModifierUI *computeAddAnimUI;
    BSDistTriggerModifierUI *distTriggerModUI;
    BSInterpValueModifierUI *interpValueModUI;
    GetUpModifierUI *getUpModUI;
    GetWorldFromModelModifierUI *getWorldFromModelModUI;
    TwistModifierUI *twistModUI;
    TimerModifierUI *timerModUI;
    DampingModifierUI *dampingModUI;
    RigidBodyRagdollControlsModifierUI *rigidRagdollControlsModUI;
    PoweredRagdollControlsModifierUI *poweredRagdollControlsModUI;
    CombineTransformsModifierUI *combineTransModUI;
    ComputeRotationFromAxisAngleModifierUI *computeRotationAxisAngleModUI;
    ComputeRotationToTargetModifierUI *computeRotationToTargetModUI;
    TransformVectorModifierUI *transformVectorModUI;
    LookAtModifierUI *lookAtModUI;
    KeyframeBonesModifierUI *keyframeBonesModUI;
    FootIkControlsModifierUI *footIKControlsModUI;
    MirrorModifierUI *mirrorModUI;
    ExtractRagdollPoseModifierUI *extractRagdollPoseModUI;
    BSTimerModifierUI *bsTimerModUI;
    BSGetTimeStepModifierUI *getTimeStepModUI;
    DelayedModifierUI *delayedModUI;
    BSRagdollContactListenerModifierUI *ragdollContactListenerModUI;
    BSEventOnDeactivateModifierUI *eventOnDeactivateModUI;
    BSSpeedSamplerModifierUI *speedSamplerModUI;
    BSPassByTargetTriggerModifierUI *passByTargetTriggerModUI;
    BSLookAtModifierUI *bsLookAtModUI;
    DetectCloseToGroundModifierUI *detectCloseToGroundModUI;
    BSEventEveryNEventsModifierUI *eventEveryNEventsModUI;
    BSEventOnFalseToTrueModifierUI *eventOnFalseToTrueModUI;
    BSModifyOnceModifierUI *modifyOnceModUI;
    HandIkControlsModifierUI *handIKControlsModUI;
    BSTweenerModifierUI *tweenerModUI;
    BGSGamebryoSequenceGeneratorUI *gamebryoSequenceGenUI;
    EventsFromRangeModifierUI *eventsFromRangeModUI;
};
}
#endif // HKDATAUI_H
