#include "hkbmodifier.h"

using namespace UI;

hkbModifier::hkbModifier(HkxFile *parent, long ref)
    : DataIconManager(parent, ref)
{
    //
}

bool hkbModifier::link(){
    return false;
}

QString hkbModifier::getName() const{
    return "";
}

QString hkbModifier::getClassname() const{
    switch (getSignature()){
    case HKB_MODIFIER_LIST:
        return "hkbModifierList";
    case HKB_TWIST_MODIFIER:
        return "hkbTwistModifier";
    case HKB_EVENT_DRIVEN_MODIFIER:
        return "hkbEventDrivenModifier";
    case BS_IS_ACTIVE_MODIFIER:
        return "BSIsActiveModifier";
    case BS_LIMB_IK_MODIFIER:
        return "BSLimbIKModifier";
    case BS_INTERP_VALUE_MODIFIER:
        return "BSInterpValueModifier";
    case BS_GET_TIME_STEP_MODIFIER:
        return "BSGetTimeStepModifier";
    case HKB_FOOT_IK_CONTROLS_MODIFIER:
        return "hkbFootIkControlsModifier";
    case HKB_GET_HANDLE_ON_BONE_MODIFIER:
        return "hkbGetHandleOnBoneModifier";
    case HKB_TRANSFORM_VECTOR_MODIFIER:
        return "hkbTransformVectorModifier";
    case HKB_PROXY_MODIFIER:
        return "hkbProxyModifier";
    case HKB_LOOK_AT_MODIFIER:
        return "hkbLookAtModifier";
    case HKB_MIRROR_MODIFIER:
        return "hkbMirrorModifier";
    case HKB_GET_WORLD_FROM_MODEL_MODIFIER:
        return "hkbGetWorldFromModelModifier";
    case HKB_SENSE_HANDLE_MODIFIER:
        return "hkbSenseHandleModifier";
    case HKB_EVALUATE_EXPRESSION_MODIFIER:
        return "hkbEvaluateExpressionModifier";
    case HKB_EVALUATE_HANDLE_MODIFIER:
        return "hkbEvaluateHandleModifier";
    case HKB_ATTACHMENT_MODIFIER:
        return "hkbAttachmentModifier";
    case HKB_ATTRIBUTE_MODIFIER:
        return "hkbAttributeModifier";
    case HKB_COMBINE_TRANSFORMS_MODIFIER:
        return "hkbCombineTransformsModifier";
    case HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER:
        return "hkbComputeRotationFromAxisAngleModifier";
    case HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER:
        return "hkbComputeRotationToTargetModifier";
    case HKB_EVENTS_FROM_RANGE_MODIFIER:
        return "hkbEventsFromRangeModifier";
    case HKB_MOVE_CHARACTER_MODIFIER:
        return "hkbMoveCharacterModifier";
    case HKB_EXTRACT_RAGDOLL_POSE_MODIFIER:
        return "hkbExtractRagdollPoseModifier";
    case BS_MODIFY_ONCE_MODIFIER:
        return "BSModifyOnceModifier";
    case BS_EVENT_ON_DEACTIVATE_MODIFIER:
        return "BSEventOnDeactivateModifier";
    case BS_EVENT_EVERY_N_EVENTS_MODIFIER:
        return "BSEventEveryNEventsModifier";
    case BS_RAGDOLL_CONTACT_LISTENER_MODIFIER:
        return "BSRagdollContactListenerModifier";
    case HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER:
        return "hkbPoweredRagdollControlsModifier";
    case BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER:
        return "BSEventOnFalseToTrueModifier";
    case BS_DIRECT_AT_MODIFIER:
        return "BSDirectAtModifier";
    case BS_DIST_TRIGGER_MODIFER:
        return "BSDistTriggerModifier";
    case BS_DECOMPOSE_VECTOR_MODIFIER:
        return "BSDecomposeVectorModifier";
    case BS_COMPUTE_ADD_BONE_ANIM_MODIFIER:
        return "BSComputeAddBoneAnimModifier";
    case BS_TWEENER_MODIFIER:
        return "BSTweenerModifier";
    case BS_I_STATE_MANAGER_MODIFIER:
        return "BSIStateManagerModifier";
    case HKB_TIMER_MODIFIER:
        return "hkbTimerModifier";
    case HKB_ROTATE_CHARACTER_MODIFIER:
        return "hkbRotateCharacterModifier";
    case HKB_DAMPING_MODIFIER:
        return "hkbDampingModifier";
    case HKB_DELAYED_MODIFIER:
        return "hkbDelayedModifier";
    case HKB_GET_UP_MODIFIER:
        return "hkbGetUpModifier";
    case HKB_KEY_FRAME_BONES_MODIFIER:
        return "hkbKeyframeBonesModifier";
    case HKB_COMPUTE_DIRECTION_MODIFIER:
        return "hkbComputeDirectionModifier";
    case HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER:
        return "hkbRigidBodyRagdollControlsModifier";
    case BS_SPEED_SAMPLER_MODIFIER:
        return "BSSpeedSamplerModifier";
    case HKB_DETECT_CLOSE_TO_GROUND_MODIFIER:
        return "hkbDetectCloseToGroundModifier";
    case BS_LOOK_AT_MODIFIER:
        return "BSLookAtModifier";
    case BS_TIMER_MODIFIER:
        return "BSTimerModifier";
    case BS_PASS_BY_TARGET_TRIGGER_MODIFIER:
        return "BSPassByTargetTriggerModifier";
    default:
        return "";
    }
}

