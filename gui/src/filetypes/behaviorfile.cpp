#include "behaviorfile.h"
#include "characterfile.h"
#include "skeletonfile.h"
#include "projectfile.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/ui/mainwindow.h"
#include "src/ui/treegraphicsitem.h"

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
#include "src/hkxclasses/behavior/modifiers/hkbfootikmodifier.h"
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
#include "src/hkxclasses/behavior/hkbcharacterstringdata.h"
#include "src/hkxclasses/behavior/hkbgeneratortransitioneffect.h"
#include "src/hkxclasses/behavior/hkbeventrangedataarray.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/hkxclasses/behavior/hkbexpressiondataarray.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/hkbstatemachineeventpropertyarray.h"
#include "src/hkxclasses/behavior/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/behavior/hkbexpressioncondition.h"
#include "src/hkxclasses/behavior/hkbstringcondition.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorgraph.h"
#include "src/hkxclasses/behavior/hkbcliptriggerarray.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphstringdata.h"
#include "src/hkxclasses/behavior/hkbvariablevalueset.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"

#include <QStringList>

/**
 * Class: BehaviorFile
 *
 */

using namespace UI;

BehaviorFile::BehaviorFile(MainWindow *window, ProjectFile *projectfile, CharacterFile *characterData, const QString & name)
    : HkxFile(window, name),
      project(projectfile),
      character(characterData),
      largestRef(0)
{
    getReader().setFile(this);
    if (!project){
        //CRITICAL_ERROR_MESSAGE("BehaviorFile::BehaviorFile(): The project pointer is nullptr!");
    }
    if (!character){
        //CRITICAL_ERROR_MESSAGE("BehaviorFile::BehaviorFile(): The character pointer is nullptr!");
    }
}

void BehaviorFile::generateNewBehavior(){
    auto root = new hkRootLevelContainer(this);
    auto graph = new hkbBehaviorGraph(this);
    auto strings = new hkbBehaviorGraphStringData(this);
    auto values = new hkbVariableValueSet(this);
    graph->setName(getFileName().replace(".hkx", ".hkb"));
    behaviorGraph = HkxSharedPtr(graph);
    stringData = HkxSharedPtr(strings);
    variableValues = HkxSharedPtr(values);
    auto data = new hkbBehaviorGraphData(this, -1, strings, values);
    graphData = HkxSharedPtr(data);
    graph->setData(data);
    root->addVariant("hkbBehaviorGraph", behaviorGraph.data());
    setRootObject(HkxSharedPtr(root));
    //setIsChanged(true);
}

void BehaviorFile::generateDefaultCharacterData(){
    generateNewBehavior();
    auto data = static_cast<hkbBehaviorGraphData *>(graphData.data());
    if (data){
        //Add common variables...
        data->addVariable(VARIABLE_TYPE_BOOL, "bEquipOk");   //Needs to be initialized to one or custom creatures don't work...
        data->addVariable(VARIABLE_TYPE_REAL, "Speed"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_REAL, "Direction"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_INT32, "iState");
        data->addVariable(VARIABLE_TYPE_REAL, "TurnDelta"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_REAL, "SpeedSampled");
        data->addVariable(VARIABLE_TYPE_REAL, "SpeedDamped");
        data->addVariable(VARIABLE_TYPE_REAL, "TurnDeltaDamped");
        data->addVariable(VARIABLE_TYPE_REAL, "weaponSpeedMult"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_BOOL, "bAnimationDriven");
        data->addVariable(VARIABLE_TYPE_BOOL, "bAllowRotation");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsAttacking");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsStaggering");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsRecoiling");
        data->addVariable(VARIABLE_TYPE_BOOL, "bFootIKEnable");   //Creatures use this, humans don't...
        data->addVariable(VARIABLE_TYPE_BOOL, "bHumanoidFootIKEnable");   //Humans use this, creatures don't...
        data->addVariable(VARIABLE_TYPE_REAL, "staggerMagnitude");
        data->addVariable(VARIABLE_TYPE_REAL, "staggerDirection");
        data->addVariable(VARIABLE_TYPE_BOOL, "bHeadTrackingOn");   //Creatures use this, humans don't...
        data->addVariable(VARIABLE_TYPE_BOOL, "bHeadTracking"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_VECTOR4, "TargetLocation");
        data->addVariable(VARIABLE_TYPE_REAL, "turnSpeedMult");   //Creatures use this, humans???
        //Add common events...
        data->addEvent("moveStart");
        data->addEvent("moveStop");
        data->addEvent("bleedOutStart");
        data->addEvent("bleedOutStop");
        data->addEvent("blockAnticipateStart");
        data->addEvent("blockHitStart");
        //Listed in playercharacterresponse.txt
        data->addEvent("BackupResponse");	//ActorResponse
        data->addEvent("idleChairSitting");	//PlayerChairEnterHandler
        data->addEvent("idleBedSleeping");	//PlayerBedEnterHandler
        data->addEvent("idleChairGetUp");	//PlayerFurnitureExitHandler
        data->addEvent("idleBedGetUp");	//PlayerFurnitureExitHandler
        data->addEvent("BowZoomStart");	//BowZoomStartHandler
        data->addEvent("BowZoomStop");	//BowZoomStopHandler
        data->addEvent("StartAnimatedCamera");	//AnimatedCameraStartHandler
        data->addEvent("StartAnimatedCameraDelta");	//AnimatedCameraDeltaStartHandler
        data->addEvent("EndAnimatedCamera");	//AnimatedCameraEndHandler
        data->addEvent("GraphDeleting");	//AnimatedCameraEndHandler
        data->addEvent("PitchOverrideStart");	//PitchOverrideStartHandler
        data->addEvent("PitchOverrideEnd");	//PitchOverrideEndHandler
        data->addEvent("ZeroOutCameraPitch");	//ZeroPitchHandler
        //Listed in actorresponse.txt
        data->addEvent("weaponSwing");	//WeaponRightSwingHandler
        data->addEvent("weaponLeftSwing");	//WeaponLeftSwingHandler
        data->addEvent("AttackWinStart");	//AttackWinStartHandler
        data->addEvent("AttackWinStartLeft");	//AttackWinStartHandler
        data->addEvent("AttackWinEnd");	//AttackWinEndHandler
        data->addEvent("AttackWinEndLeft");	//AttackWinEndHandler
        data->addEvent("attackStop");	//AttackStopHandler
        data->addEvent("bashStop");	//AttackStopHandler
        data->addEvent("recoilStop");	//RecoilStopHandler
        data->addEvent("MLh_SpellFire_Event");	//LeftHandSpellFireHandler
        data->addEvent("MRh_SpellFire_Event");	//RightHandSpellFireHandler
        data->addEvent("Voice_SpellFire_Event");	//VoiceSpellFireHandler
        data->addEvent("BeginCastLeft");	//LeftHandSpellCastHandler
        data->addEvent("BeginCastRight");	//RightHandSpellCastHandler
        data->addEvent("BeginCastVoice");	//VoiceSpellCastHandler
        data->addEvent("BeginWeaponDraw");	//WeaponBeginDrawRightHandler
        data->addEvent("BeginWeaponSheathe");	//WeaponBeginSheatheRightHandler
        data->addEvent("CameraOverrideStart");	//CameraOverrideStartHandler
        data->addEvent("CameraOverrideStop");	//CameraOverrideStopHandler
        data->addEvent("weaponDraw");	//RightHandWeaponDrawHandler
        data->addEvent("weaponSheathe");	//RightHandWeaponSheatheHandler
        data->addEvent("HitFrame");	//HitFrameHandler
        data->addEvent("preHitFrame");	//AnticipateAttackHandler
        data->addEvent("staggerStop");	//StaggeredStopHandler
        data->addEvent("idleChairSitting");	//ChairEnterHandler
        data->addEvent("idleBedSleeping");	//BedEnterHandler
        data->addEvent("idleChairGetUp");	//ChairFurnitureExitHandler
        data->addEvent("idleBedGetUp");	//BedFurnitureExitHandler
        data->addEvent("idleSleepGetUp");	//BedFurnitureExitHandler
        data->addEvent("KillActor");	//KillActorHandler
        data->addEvent("Decapitate");	//DecapitateHandler
        data->addEvent("FlightTakeOff");	//FlightTakeOffHandler
        data->addEvent("FlightCruising");	//FlightCruisingHandler
        data->addEvent("FlightHovering");	//FlightHoveringHandler
        data->addEvent("FlightLanding");	//FlightLandingHandler
        data->addEvent("FlightPerching");	//FlightPerchingHandler
        data->addEvent("FlightLanded");	//FlightLandHandler
        data->addEvent("FlightLandEnd");	//FlightLandEndHandler
        data->addEvent("FlightAction");	//FlightActionHandler
        data->addEvent("FlightActionEntryEnd");	//FlightActionEntryEndHandler
        data->addEvent("FlightActionEnd");	//FlightActionEndHandler
        data->addEvent("FlightActionGrab");	//FlightActionGrabHandler
        data->addEvent("FlightActionRelease");	//FlightActionReleaseHandler
        data->addEvent("FlightCrashLandStart");	//FlightCrashLandStartHandler
        data->addEvent("HeadTrackingOn");	//HeadTrackingOnHandler
        data->addEvent("HeadTrackingOff");	//HeadTrackingOffHandler
        data->addEvent("BowDrawn");	//BowDrawnHandler
        data->addEvent("BowRelease");	//BowReleaseHandler
        data->addEvent("arrowAttach");	//ArrowAttachHandler
        data->addEvent("arrowDetach");	//ArrowDetachHandler
        data->addEvent("bowReset");	//ArrowDetachHandler
        data->addEvent("arrowRelease");	//ArrowReleaseHandler
        data->addEvent("InterruptCast");	//InterruptCastHandler
        data->addEvent("summonStop");	//EndSummonAnimationHandler
        data->addEvent("PickNewIdle");	//PickNewIdleHandler
        data->addEvent("DeathStop");	//DeathStopHandler
        data->addEvent("ActivationDone");	//ActionActivateDoneHandler
        data->addEvent("StopHorseCamera");	//StopHorseCameraHandler
        data->addEvent("KillMoveStart");	//KillMoveStartHandler
        data->addEvent("KillMoveEnd");	//KillMoveEndHandler
        data->addEvent("pairedStop");	//PairedStopHandler
        data->addEvent("CameraShake");	//CameraShakeHandler
        data->addEvent("DeathEmote");	//DeathEmoteHandler
        data->addEvent("StartMotionDriven");	//MotionDrivenHandler
        data->addEvent("StartAnimationDriven");	//AnimationDrivenHandler
        data->addEvent("StartAllowRotation");	//AllowRotationHandler
        data->addEvent("AddRagdollToWorld");	//AddRagdollHandler
        data->addEvent("RemoveRagdollFromWorld");	//RemoveRagdollHandler
        data->addEvent("RemoveCharacterControllerFromWorld");	//RemoveCharacterControllerHandler
        data->addEvent("GetUpStart");	//GetUpStartHandler
        data->addEvent("GetUpEnd");	//GetUpEndHandler
        data->addEvent("MountEnd");	//MountDismountEndHandler
        data->addEvent("DismountEnd");	//MountDismountEndHandler
        data->addEvent("ExitCartBegin");	//ExitCartBeginHandler
        data->addEvent("ExitCartEnd");	//ExitCartEndHandler
        data->addEvent("EnableBumper");	//EnableCharacterBumperHandler
        data->addEvent("DisableBumper");	//DisableCharacterBumperHandler
        data->addEvent("AnimObjLoad");	//AnimationObjectLoadHandler
        data->addEvent("AnimObjDraw");	//AnimationObjectDrawHandler
        data->addEvent("EnableCharacterPitch");	//EnableCharacterPitchHandler
        data->addEvent("DisableCharacterPitch");	//DisableCharacterPitchHandler
        data->addEvent("JumpBegin");	//JumpAnimEventHandler
        data->addEvent("IdleDialogueLock");	//IdleDialogueEnterHandler
        data->addEvent("IdleDialogueUnlock");	//IdleDialogueExitHandler
        data->addEvent("NPCAttach");	//NPCAttachHandler
        data->addEvent("NPCDetach");	//NPCDetachHandler
        data->addEvent("MTState");	//MTStateHandler
        data->addEvent("VampireFeedEnd");	//VampireFeedEndHandler
    }else{
        CRITICAL_ERROR_MESSAGE("BehaviorFile::generateDefaultCharacterData(): The behavior graph data failed to construct!");
    }
}

/*
void BehaviorFile::generateDefaultCharacterData(){
    generateNewBehavior();
    auto data = static_cast<hkbBehaviorGraphData *>(graphData.data());
    if (data){
        //Add common variables...
        data->addVariable(VARIABLE_TYPE_BOOL, "bEquipOk");   //Needs to be initialized to one or custom creatures don't work...
        data->addVariable(VARIABLE_TYPE_REAL, "Speed"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_REAL, "Direction"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_INT32, "iState");
        data->addVariable(VARIABLE_TYPE_REAL, "TurnDelta"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_REAL, "SpeedSampled");
        data->addVariable(VARIABLE_TYPE_REAL, "SpeedDamped");
        data->addVariable(VARIABLE_TYPE_REAL, "TurnDeltaDamped");
        data->addVariable(VARIABLE_TYPE_REAL, "weaponSpeedMult"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_BOOL, "bAnimationDriven");
        data->addVariable(VARIABLE_TYPE_BOOL, "bAllowRotation");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsAttacking");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsStaggering");
        data->addVariable(VARIABLE_TYPE_BOOL, "IsRecoiling");
        data->addVariable(VARIABLE_TYPE_BOOL, "bFootIKEnable");   //Creatures use this, humans don't...
        data->addVariable(VARIABLE_TYPE_BOOL, "bHumanoidFootIKEnable");   //Humans use this, creatures don't...
        data->addVariable(VARIABLE_TYPE_REAL, "staggerMagnitude");
        data->addVariable(VARIABLE_TYPE_REAL, "staggerDirection");
        data->addVariable(VARIABLE_TYPE_BOOL, "bHeadTrackingOn");   //Creatures use this, humans don't...
        data->addVariable(VARIABLE_TYPE_BOOL, "bHeadTracking"); //Protected variable...
        data->addVariable(VARIABLE_TYPE_VECTOR4, "TargetLocation");
        data->addVariable(VARIABLE_TYPE_REAL, "turnSpeedMult");   //Creatures use this, humans???
        //Add common events...
        data->addEvent("moveStart");
        data->addEvent("moveStop");
        data->addEvent("bleedOutStart");
        data->addEvent("bleedOutStop");
        data->addEvent("blockAnticipateStart");
        data->addEvent("blockHitStart");
        data->addEvent("NPC_BumpedFromRight");
        data->addEvent("NPC_BumpedFromLeft");
        data->addEvent("NPC_BumpedFromBack");
        data->addEvent("NPC_BumpedFromFront");
        data->addEvent("Ragdoll");
        data->addEvent("DeathAnim");
        data->addEvent("WeapEquip");
        data->addEvent("WeapSoloEquip");
        data->addEvent("Magic_Equip");
        data->addEvent("attackStartDualWield");
        data->addEvent("attackPowerStartH2HCombo");
        data->addEvent("attackPowerStartDualWield");
        data->addEvent("StaffBashRelease");
        data->addEvent("JumpFallDirectional");
        data->addEvent("JumpFall");
        data->addEvent("MC_WeapOutRightReplaceForceEquip");
        data->addEvent("WeapSoloEquip");
        data->addEvent("torchEquip");
        data->addEvent("ShdEquip");
        data->addEvent("swimForceEquip");
        data->addEvent("WeapOutLeftReplaceForceEquip");
        data->addEvent("torchForceEquip");
        data->addEvent("MagicWeap_ForceEquip");
        data->addEvent("WeapOutRightReplaceForceEquip");
        data->addEvent("Magic_Solo_Equip");
        data->addEvent("MagicForceEquipLeft");
        data->addEvent("MagicForceEquipRight");
        data->addEvent("MagicForceEquip");
        data->addEvent("GetUpBegin");
        data->addEvent("IdlePlayer");
        data->addEvent("IdleStop");
        data->addEvent("IdleStopInstant");
        data->addEvent("JumpDirectionalStart");
        data->addEvent("JumpStandingStart");
        data->addEvent("JumpLandDirectional");
        data->addEvent("JumpLandEnd");
        data->addEvent("JumpLand");
        data->addEvent("blockStart");
        data->addEvent("attackStart_MC_1HMLeft");
        data->addEvent("attackStartSprintLeftHand");
        data->addEvent("attackStartH2HLeft");
        data->addEvent("MLh_SpellSelfStart");
        data->addEvent("MLh_SpellAimedStart");
        data->addEvent("MLh_WardStart");
        data->addEvent("MLh_SpellSelfConcentrationStart");
        data->addEvent("MLh_SpellAimedConcentrationStart");
        data->addEvent("RitualSpellStart");
        data->addEvent("RitualSpellAimConcentrationStart");
        data->addEvent("DualMagic_SpellAimedStart");
        data->addEvent("DualMagic_SpellSelfStart");
        data->addEvent("DualMagic_SpellAimedStart");
        data->addEvent("DualMagic_SpellAimedConcentrationStart");
        data->addEvent("DualMagic_WardStart");
        data->addEvent("DualMagic_SpellSelfConcentrationStart");
        data->addEvent("MLh_SpellTelekinesisStart");
        data->addEvent("bashStart");
        data->addEvent("attackStartLeftHand");
        data->addEvent("RitualSpellOut");
        data->addEvent("MLh_Equipped_Event");
        data->addEvent("attackPowerStart_MC_1HMLeft");
        data->addEvent("attackPowerStart_SprintLeftHand");
        data->addEvent("attackPowerStartForwardLeftHand");
        data->addEvent("attackPowerStartRightLeftHand");
        data->addEvent("attackPowerStartLeftLeftHand");
        data->addEvent("attackPowerStartBackLeftHand");
        data->addEvent("attackPowerStartInPlaceLeftHand");
        data->addEvent("attackPowerStartForwardH2HLeftHand");
        data->addEvent("MLh_SpellReady_event");
        data->addEvent("blockStop");
        data->addEvent("bashRelease");
        data->addEvent("attackReleaseL");
        data->addEvent("MLH_SpellRelease_event");
        data->addEvent("NPC_TurnRight90");
        data->addEvent("NPC_TurnRight180");
        data->addEvent("NPC_TurnLeft90");
        data->addEvent("NPC_TurnLeft180");
        data->addEvent("NPC_TurnToWalkRight90");
        data->addEvent("NPC_TurnToWalkRight180");
        data->addEvent("NPC_TurnToWalkLeft90");
        data->addEvent("NPC_TurnToWalkLeft180");
        data->addEvent("recoilStart");
        data->addEvent("recoilLargeStart");
        data->addEvent("IdleForceDefaultState");
        data->addEvent("attackStart_MC_1HMRight");
        data->addEvent("bowAttackStart");
        data->addEvent("attackStartSprint");
        data->addEvent("attackStartH2HRight");
        data->addEvent("MRh_SpellSelfStart");
        data->addEvent("MRh_SpellAimedStart");
        data->addEvent("MRh_WardStart");
        data->addEvent("MRh_SpellSelfConcentrationStart");
        data->addEvent("MRh_SpellAimedConcentrationStart");
        data->addEvent("MRh_SpellTelekinesisStart");
        data->addEvent("BashFail");
        data->addEvent("attackStart");
        data->addEvent("attackStop");
        data->addEvent("MRh_Equipped_Event");
        data->addEvent("attackPowerStart_MC_1HMRight");
        data->addEvent("attackPowerStart_2HMSprint");
        data->addEvent("attackPowerStart_2HWSprint");
        data->addEvent("attackPowerStart_Sprint");
        data->addEvent("attackPowerStartForwardH2HRightHand");
        data->addEvent("attackPowerStartInPlace");
        data->addEvent("attackPowerStartForward");
        data->addEvent("attackPowerStartRight");
        data->addEvent("attackPowerStartLeft");
        data->addEvent("attackPowerStartBackward");
        data->addEvent("bashPowerStart");
        data->addEvent("MRh_SpellReady_event");
        data->addEvent("MRH_SpellRelease_event");
        data->addEvent("attackRelease");
        data->addEvent("UnequipNoAnim");
        data->addEvent("Unequip");
        data->addEvent("SneakStart");
        data->addEvent("SneakStop");
        data->addEvent("BlockBashSprint");
        data->addEvent("SneakSprintStartRoll");
        data->addEvent("SprintStart");
        data->addEvent("sprintStop");
        data->addEvent("00NextClip");
        data->addEvent("StaggerPlayer");
        data->addEvent("staggerStart");
        data->addEvent("MountedStaggerStart");
        data->addEvent("SwimStart");
        data->addEvent("swimStop");
        data->addEvent("MountedSwimStart");
        data->addEvent("MountedSwimStop");
        data->addEvent("TurnLeft");
        data->addEvent("TurnRight");
        data->addEvent("turnStop");
        data->addEvent("NPCshoutStart");
        data->addEvent("shoutStart");
        data->addEvent("shoutStop");
        data->addEvent("shoutSprintMediumStart");
        data->addEvent("shoutSprintLongStart");
        data->addEvent("shoutSprintLongestStart");
        data->addEvent("shoutReleaseSlowTime");
        data->addEvent("CombatReady_BreathExhaleShort");
        data->addEvent("MT_BreathExhaleShort");
        data->addEvent("wardAbsorb");
        data->addEvent("WardBreak");
        //Events raised in the character behaviors that the game engine probably responds to in some way...
        data->addEvent("AddRagdollToWorld");
        data->addEvent("RemoveCharacterControllerFromWorld");
        data->addEvent("AddCharacterControllerToWorld");
        data->addEvent("InterruptCast");
        data->addEvent("Reanimated");
        data->addEvent("HeadTrackingOff");
        data->addEvent("HeadTrackingOn");
        data->addEvent("tailUnequip");
        data->addEvent("arrowDetach");
        data->addEvent("EnableBumper");
        data->addEvent("DisableBumper");
        data->addEvent("weaponSheathe");
        data->addEvent("JumpUp");
        data->addEvent("MTState");
        data->addEvent("AnimObjectUnequip");
        data->addEvent("tailMTState");
        data->addEvent("bowReset");
        data->addEvent("BeginWeaponSheathe");
        data->addEvent("tailCombatLocomotion");
        data->addEvent("tailSneakLocomotion");
        data->addEvent("tailCombatIdle");
        data->addEvent("tailSneakIdle");
        data->addEvent("weaponDraw");
        data->addEvent("PairedKillTarget");
        data->addEvent("PairEnd");
        data->addEvent("MountEnd");
        data->addEvent("StartAnimatedCamera");
        data->addEvent("AnimObjLoad");
        data->addEvent("AnimObjDraw");
        data->addEvent("EndAnimatedCamera");
        data->addEvent("StartAnimatedCameraDelta");
        data->addEvent("2_PairEnd");
        data->addEvent("ExitCartBegin");
        data->addEvent("ExitCartEnd");
        data->addEvent("SoundPlay.NPCWerewolfTransformation");
        data->addEvent("ZeroOutCameraPitch");
        data->addEvent("PowerAttack_Start_end");
        data->addEvent("CastOKStop");
        data->addEvent("slowdownStart");
        data->addEvent("FootRight");
        data->addEvent("FootLeft");
        data->addEvent("preHitFrame");
        data->addEvent("weaponSwing");
        data->addEvent("HitFrame");
        data->addEvent("AttackWinStart");
        data->addEvent("AttackWinEnd");
        /*data->addEvent("SoundPlay.WPNSwingUnarmed");
        data->addEvent("SoundPlay.NPCHumanCombatIdleA");
        data->addEvent("SoundPlay.NPCHumanCombatIdleB");
        data->addEvent("SoundPlay.NPCHumanCombatIdleC");
        data->addEvent("2_SoundPlay.NPCKill1HMDualWieldA");
        data->addEvent("NPCSoundPlay.NPCKillStruggle");
        data->addEvent("NPCSoundPlay.NPCKillShove");
        data->addEvent("NPCSoundPlay.NPCKillChop");
        data->addEvent("NPCSoundPlay.NPCKillSmash");
        data->addEvent("NPCSoundPlay.NPCKillMeleeB");
        data->addEvent("NPCSoundPlay.NPCKillGore");
        data->addEvent("NPCSoundPlay.NPCKillBodyfall");
        data->addEvent("NPCSoundPlay.NPCKillStabIn");
        data->addEvent("NPCSoundPlay.WPNBlockBlade2HandVsOtherSD");
        data->addEvent("SoundPlay.WPNBowZoomIn");
        data->addEvent("2_KillMoveStart");
        data->addEvent("2_DeathEmote");
        data->addEvent("2_KillMoveEnd");
        data->addEvent("2_KillActor");
        data->addEvent("MRh_SpellFire_Event");
    }else{
        CRITICAL_ERROR_MESSAGE("BehaviorFile::generateDefaultCharacterData(): The behavior graph data failed to construct!");
    }
}*/

QStringList BehaviorFile::getAllBehaviorFileNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    QDirIterator it(QFileInfo(*this).absolutePath()+"/");
    while (it.hasNext()){
        if (QFileInfo(it.next()).fileName().contains(".hkx")){
            list.append(it.fileInfo().filePath().section("/", -1, -1)/*.replace("/", "\\")*/);
        }
    }
    return list;
}

void BehaviorFile::setLocalTimeForClipGenAnimData(const QString &clipname, int triggerindex, qreal time){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setLocalTimeForClipGenAnimData(clipname, triggerindex, time) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::setEventNameForClipGenAnimData(const QString &clipname, int triggerindex, int eventid){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setEventNameForClipGenAnimData(clipname, triggerindex, getEventNameAt(eventid)) : LogFile::writeToLog(fileName()+" has no associated project!");
}

bool BehaviorFile::isClipGenNameTaken(const QString & name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < generators.size() - 1; i++){
        if (generators.at(i)->getSignature() == HKB_CLIP_GENERATOR && name == static_cast<hkbClipGenerator *>(generators.at(i).data())->getName()){
            return true;
        }
    }
    return false;
}

bool BehaviorFile::isClipGenNameAvailable(const QString &name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return !project->isClipGenNameTaken(name);
    }
    return false;
}

bool BehaviorFile::addClipGenToAnimationData(const QString &name){
    //std::lock_guard <std::mutex> guard(mutex);
    if (!project->isClipGenNameTaken(name) || !project){
        return false;
    }
    return project->appendClipGeneratorAnimData(name);
}

bool BehaviorFile::removeClipGenFromAnimData(const QString & animationname, const QString & clipname, const QString & variablename){
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->removeClipGenFromAnimData(animationname, clipname, variablename);
    }
    return false;
}

void BehaviorFile::setClipNameAnimData(const QString &oldclipname, const QString &newclipname){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setClipNameAnimData(oldclipname, newclipname) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::setAnimationIndexAnimData(int index, const QString &clipGenName){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setAnimationIndexForClipGen(index, clipGenName) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::setPlaybackSpeedAnimData(const QString &clipGenName, qreal speed){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setPlaybackSpeedAnimData(clipGenName, speed) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::setCropStartAmountLocalTimeAnimData(const QString &clipGenName, qreal time){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setCropStartAmountLocalTimeAnimData(clipGenName, time) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::setCropEndAmountLocalTimeAnimData(const QString &clipGenName, qreal time){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->setCropEndAmountLocalTimeAnimData(clipGenName, time) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::appendClipTriggerToAnimData(const QString &clipGenName){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->appendClipTriggerToAnimData(clipGenName, getEventNameAt(0)) : LogFile::writeToLog(fileName()+" has no associated project!");
}

void BehaviorFile::removeClipTriggerToAnimDataAt(const QString &clipGenName, int index){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->removeClipTriggerToAnimDataAt(clipGenName, index) : LogFile::writeToLog(fileName()+" has no associated project!");
}

QString BehaviorFile::isEventReferenced(int eventindex) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto stringdata = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    QString objnames;
    int size;
    auto getreferencedevents = [&](const QVector <HkxSharedPtr> & list){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i).constData()->isEventReferenced(eventindex)){
                objnames.append(static_cast<const DataIconManager *>(list.at(i).constData())->getName()+"\n");
            }
            if (objnames.size() > MAX_ERROR_STRING_SIZE){
                objnames.append("...");
                break;
            }
        }
    };
    if (stringdata && stringdata->eventNames.size() > eventindex && eventindex > -1){
        objnames = "The event \""+stringdata->getEventNameAt(eventindex)+" \" is referenced by the following objects: \n";
        size = objnames.size();
        getreferencedevents(generators);
        getreferencedevents(modifiers);
    }else{
        LogFile::writeToLog(fileName()+": stringdata null or invalid event index!!!");
    }
    if (objnames.size() > size){
        return objnames;
    }
    return "";
}

void BehaviorFile::updateEventIndices(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    auto stringdata = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (stringdata && stringdata->getNumberOfEvents() >= index && index > -1){
        for (auto i = 0; i < generators.size(); i++){
            generators.at(i)->updateEventIndices(index);
        }
        for (auto i = 0; i < modifiers.size(); i++){
            modifiers.at(i)->updateEventIndices(index);
        }
    }else{
        LogFile::writeToLog(fileName()+": stringdata null or invalid event index!!!");
    }
}

QString BehaviorFile::isVariableReferenced(int variableindex) const{
    //std::lock_guard <std::mutex> guard(mutex);
    int size;
    QString objnames;
    auto stringdata = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    auto getreferencedvars = [&](const QVector <HkxSharedPtr> & list){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i).constData()->isVariableReferenced(variableindex)){
                objnames.append(static_cast<const DataIconManager *>(list.at(i).constData())->getName()+"\n");
            }
            if (objnames.size() > MAX_ERROR_STRING_SIZE){
                objnames.append("...");
                break;
            }
        }
    };
    if (stringdata && stringdata->getNumberOfVariables() > variableindex && variableindex > -1){
        objnames = "The variable \""+stringdata->getVariableNameAt(variableindex)+" \" is referenced by the following objects: \n";
        size = objnames.size();
        getreferencedvars(generators);
        getreferencedvars(modifiers);
        for (auto i = 0; i < otherTypes.size(); i++){
            if (otherTypes.at(i).constData()->getSignature() == HKB_BLENDING_TRANSITION_EFFECT && otherTypes.at(i).constData()->isVariableReferenced(variableindex)){
                objnames.append(static_cast<const hkbBlendingTransitionEffect *>(otherTypes.at(i).constData())->getName()+"\n");
            }
            if (objnames.size() > MAX_ERROR_STRING_SIZE){
                objnames.append("...");
                break;
            }
        }
    }else{
        LogFile::writeToLog(fileName()+": stringdata null or invalid variable index!!!");
    }
    if (objnames.size() > size){
        return objnames;
    }
    return "";
}

void BehaviorFile::updateVariableIndices(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    auto stringdata = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (stringdata && stringdata->getNumberOfVariables() >= index && index > -1){
        for (auto i = 0; i < otherTypes.size(); i++){
            if (otherTypes.at(i)->getSignature() == HKB_VARIABLE_BINDING_SET){
                static_cast<hkbVariableBindingSet *>(otherTypes.at(i).data())->updateVariableIndices(index);
            }
        }
    }else{
        LogFile::writeToLog(fileName()+":  stringdata null or invalid variable index!!!");
    }
}

void BehaviorFile::removeUnreferencedFiles(const hkbBehaviorReferenceGenerator *gentoignore){
    //std::lock_guard <std::mutex> guard(mutex);
    (project) ? project->removeUnreferencedFiles(gentoignore) : LogFile::writeToLog(fileName()+" has no associated project!");
}

QStringList BehaviorFile::getReferencedBehaviors(const hkbBehaviorReferenceGenerator *gentoignore) const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i)->getSignature() == HKB_BEHAVIOR_REFERENCE_GENERATOR && gentoignore != generators.at(i).constData()){
            auto name = static_cast<hkbBehaviorReferenceGenerator *>(generators.at(i).data())->getBehaviorName();
            (!list.contains(name)) ? list.append(name) : NULL;
        }
    }
    return list;
}

void BehaviorFile::removeAllData(){
    //std::lock_guard <std::mutex> guard(mutex);
    removeDataNoLock(generators);
    removeDataNoLock(modifiers);
    removeDataNoLock(otherTypes);
}

void BehaviorFile::getCharacterPropertyBoneWeightArray(const QString &name, hkbBoneWeightArray *ptrtosetdata) const{
    //std::lock_guard <std::mutex> guard(mutex);
    (character) ? static_cast<CharacterFile *>(character)->getCharacterPropertyBoneWeightArray(name, ptrtosetdata) : LogFile::writeToLog(fileName()+" has no associated character file!");
}

hkbStateMachine *BehaviorFile::findRootStateMachineFromBehavior(const QString behaviorname) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->findRootStateMachineFromBehavior(behaviorname);
    }
    return nullptr;
}

qreal BehaviorFile::getAnimationDurationFromAnimData(const QString &animationname) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->getAnimationDurationFromAnimData(animationname);
    }
    return 0;
}

void BehaviorFile::mergeEventIndices(int oldindex, int newindex){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < generators.size(); i++){
        generators.at(i)->mergeEventIndex(oldindex, newindex);
    }
    for (auto i = 0; i < modifiers.size(); i++){
        modifiers.at(i)->mergeEventIndex(oldindex, newindex);
    }
}

void BehaviorFile::mergeVariableIndices(int oldindex, int newindex){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < otherTypes.size(); i++){
        if (otherTypes.at(i)->getSignature() == HKB_VARIABLE_BINDING_SET){
            static_cast<hkbVariableBindingSet *>(otherTypes.at(i).data())->mergeVariableIndex(oldindex, newindex);
        }
    }
}

bool BehaviorFile::isNameUniqueInProject(HkxObject *object) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto value = false;
    (project) ? value = project->isNameUniqueInProject(object, fileName()) : LogFile::writeToLog(fileName()+" has no associated project!");
    return value;
}

bool BehaviorFile::existsInBehavior(HkDynamicObject *object, int startindex) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto found = false;
    HkxSignature objsig;
    HkxSignature listobjsig;
    QString objanimationname;
    QString listobjanimationname;
    QString objname;
    DataIconManager *obj;
    auto search = [&](const QVector <HkxSharedPtr> & list){
        for (auto i = startindex; i < list.size(); i++){
            listobjsig = list.at(i)->getSignature();
            auto listobjname = static_cast<DataIconManager *>(list.at(i).data())->getName();
            if (objsig == HKB_CLIP_GENERATOR){
                objanimationname = static_cast<hkbClipGenerator *>(obj)->getAnimationName();
            }
            if (listobjsig == HKB_CLIP_GENERATOR){
                listobjanimationname = static_cast<hkbClipGenerator *>(list.at(i).data())->getAnimationName();
            }
            if (!objname.contains("TKDodge") && objsig == listobjsig && ((objsig == HKB_CLIP_GENERATOR && !QString::compare(objanimationname, listobjanimationname, Qt::CaseInsensitive) && (objname == listobjname)) || (objname == listobjname))){
                found = true;
                break;
            }
        }
        if (!found){
            for (auto i = 0; i < startindex; i++){
                listobjsig = list.at(i)->getSignature();
                auto listobjname = static_cast<DataIconManager *>(list.at(i).data())->getName();
                if (objsig == HKB_CLIP_GENERATOR){
                    objanimationname = static_cast<hkbClipGenerator *>(obj)->getAnimationName();
                    listobjanimationname = static_cast<hkbClipGenerator *>(list.at(i).data())->getAnimationName();
                }
                if (!objname.contains("TKDodge") && objsig == listobjsig && ((objsig == HKB_CLIP_GENERATOR && !QString::compare(objanimationname, listobjanimationname, Qt::CaseInsensitive) && (objname == listobjname)) || (objname == listobjname))){
                    found = true;
                    break;
                }
            }
        }
    };
    if (object && startindex > -1){
        obj = static_cast<DataIconManager *>(object);
        auto type = object->getType();
        objsig = obj->getSignature();
        objname = obj->getName();
        if (type == HkxObject::TYPE_GENERATOR){
            search(generators);
        }else if (type == HkxObject::TYPE_MODIFIER){
            search(modifiers);
        }
    }
    return found;
}

hkbStateMachine * BehaviorFile::getRootStateMachine() const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = nullptr;
    behaviorGraph.data() ? ptr = static_cast<hkbBehaviorGraph *>(behaviorGraph.data())->getRootGenerator() : LogFile::writeToLog(fileName()+" behaviorGraph is nullptr!");
    return ptr;
}

hkbBehaviorGraph * BehaviorFile::getBehaviorGraph() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (behaviorGraph.data()){
        return static_cast<hkbBehaviorGraph *>(behaviorGraph.data());
    }
    return nullptr;
}

QStringList BehaviorFile::getRigBoneNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getRigBoneNames();
    }
    return QStringList();
}

int BehaviorFile::getNumberOfBones(bool ragdoll) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getNumberOfBones(ragdoll);
    }
    return -1;
}

QStringList BehaviorFile::getRagdollBoneNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getRagdollBoneNames();
    }
    return QStringList();
}

bool BehaviorFile::addObjectToFile(HkxObject *obj, long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (ref < 1){
            largestRef++;
            ref = largestRef;
        }else if (ref > largestRef){
            largestRef = ref;
        }else{
            largestRef++;
        }
        obj->setReference(largestRef);
        if (obj->getType() == HkxObject::TYPE_GENERATOR){
            generators.append(HkxSharedPtr(obj, ref));
        }else if (obj->getType() == HkxObject::TYPE_MODIFIER){
            modifiers.append(HkxSharedPtr(obj, ref));
        }else if (obj->getType() == HkxObject::TYPE_OTHER){
            otherTypes.append(HkxSharedPtr(obj, ref));
        }else{
            LogFile::writeToLog(fileName()+": addObjectToFile() failed!\nInvalid type enum for this object!\nObject signature is: "+QString::number(obj->getSignature(), 16));
            return false;
        }
        obj->setParentFile(this);
        return true;
    }
    return false;
}

bool BehaviorFile::parse(){
    ////std::lock_guard <std::mutex> guard(mutex);
    long index = 2;
    auto ok = false;
    HkxSignature signature;
    QByteArray value;
    auto ref = 0;
    auto appendnread = [&](HkxObject *obj, const QString & nameoftype){
        (!appendAndReadData(index, obj)) ? LogFile::writeToLog("BehaviorFile: parse(): Failed to read a "+nameoftype+" object! Ref: "+QString::number(ref)) : NULL;
    };
    if (getReader().parse()){
        for (; index < getReader().getNumElements(); index++){
            value = getReader().getNthAttributeNameAt(index, 1);
            if (value == "class"){
                value = getReader().getNthAttributeValueAt(index, 2);
                if (value != ""){
                    ref = getReader().getNthAttributeValueAt(index, 0).remove(0, 1).toLong(&ok);
                    (!ok) ? LogFile::writeToLog("BehaviorFile: parse() failed! The object reference string contained invalid characters and failed to convert to an integer!") : NULL;
                    signature = (HkxSignature)value.toULongLong(&ok, 16);
                    (!ok) ? LogFile::writeToLog("BehaviorFile: parse() failed! The object signature string contained invalid characters and failed to convert to an integer!") : NULL;
                    switch (signature){
                    case HKB_STATE_MACHINE_STATE_INFO:
                        appendnread(new hkbStateMachineStateInfo(this, nullptr, ref), "HKB_STATE_MACHINE_STATE_INFO"); break;
                    case HKB_STATE_MACHINE:
                        appendnread(new hkbStateMachine(this, ref), "HKB_STATE_MACHINE"); break;
                    case HKB_VARIABLE_BINDING_SET:
                        appendnread(new hkbVariableBindingSet(this, ref), "HKB_VARIABLE_BINDING_SET"); break;
                    case HKB_CLIP_GENERATOR:
                        appendnread(new hkbClipGenerator(this, ref), "HKB_CLIP_GENERATOR"); break;
                    case HKB_CLIP_TRIGGER_ARRAY:
                        appendnread(new hkbClipTriggerArray(this, ref), "HKB_CLIP_TRIGGER_ARRAY"); break;
                    case HKB_BLENDER_GENERATOR_CHILD:
                        appendnread(new hkbBlenderGeneratorChild(this, nullptr, ref), "HKB_BLENDER_GENERATOR_CHILD"); break;
                    case HKB_BLENDER_GENERATOR:
                        appendnread(new hkbBlenderGenerator(this, ref), "HKB_BLENDER_GENERATOR"); break;
                    case HKB_BONE_WEIGHT_ARRAY:
                        appendnread(new hkbBoneWeightArray(this, ref), "HKB_BONE_WEIGHT_ARRAY"); break;
                    case HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY:
                        appendnread(new hkbStateMachineTransitionInfoArray(this, nullptr, ref), "HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY"); break;
                    case HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY:
                        appendnread(new hkbStateMachineEventPropertyArray(this, ref), "HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY"); break;
                    case HKB_MODIFIER_GENERATOR:
                        appendnread(new hkbModifierGenerator(this, ref), "HKB_MODIFIER_GENERATOR"); break;
                    case BS_I_STATE_TAGGING_GENERATOR:
                        appendnread(new BSiStateTaggingGenerator(this, ref), "BS_I_STATE_TAGGING_GENERATOR"); break;
                    case BS_CYCLIC_BLEND_TRANSITION_GENERATOR:
                        appendnread(new BSCyclicBlendTransitionGenerator(this, ref), "BS_CYCLIC_BLEND_TRANSITION_GENERATOR"); break;
                    case BS_BONE_SWITCH_GENERATOR:
                        appendnread(new BSBoneSwitchGenerator(this, ref), "BS_BONE_SWITCH_GENERATOR"); break;
                    case BS_BONE_SWITCH_GENERATOR_BONE_DATA:
                        appendnread(new BSBoneSwitchGeneratorBoneData(this, nullptr, ref), "BS_BONE_SWITCH_GENERATOR_BONE_DATA"); break;
                    case BS_SYNCHRONIZED_CLIP_GENERATOR:
                        appendnread(new BSSynchronizedClipGenerator(this, ref), "BS_SYNCHRONIZED_CLIP_GENERATOR"); break;
                    case HKB_MANUAL_SELECTOR_GENERATOR:
                        appendnread(new hkbManualSelectorGenerator(this, ref), "HKB_MANUAL_SELECTOR_GENERATOR"); break;
                    case HKB_MODIFIER_LIST:
                        appendnread(new hkbModifierList(this, ref), "HKB_MODIFIER_LIST"); break;
                    case HKB_BLENDING_TRANSITION_EFFECT:
                        appendnread(new hkbBlendingTransitionEffect(this, ref), "HKB_BLENDING_TRANSITION_EFFECT"); break;
                    case HKB_EXPRESSION_CONDITION:
                        appendnread(new hkbExpressionCondition(this, "", ref), "HKB_EXPRESSION_CONDITION"); break;
                    case HKB_STRING_CONDITION:
                        appendnread(new hkbStringCondition(this, "", ref), "HKB_STRING_CONDITION"); break;
                    case HKB_BEHAVIOR_REFERENCE_GENERATOR:
                        appendnread(new hkbBehaviorReferenceGenerator(this, ref), "HKB_BEHAVIOR_REFERENCE_GENERATOR"); break;
                    case BS_OFFSET_ANIMATION_GENERATOR:
                        appendnread(new BSOffsetAnimationGenerator(this, ref), "BS_OFFSET_ANIMATION_GENERATOR"); break;
                    case HKB_EVALUATE_EXPRESSION_MODIFIER:
                        appendnread(new hkbEvaluateExpressionModifier(this, ref), "HKB_EVALUATE_EXPRESSION_MODIFIER"); break;
                    case HKB_EXPRESSION_DATA_ARRAY:
                        appendnread(new hkbExpressionDataArray(this, ref), "HKB_EXPRESSION_DATA_ARRAY"); break;
                    case HKB_TWIST_MODIFIER:
                        appendnread(new hkbTwistModifier(this, ref), "HKB_TWIST_MODIFIER"); break;
                    case HKB_EVENT_DRIVEN_MODIFIER:
                        appendnread(new hkbEventDrivenModifier(this, ref), "HKB_EVENT_DRIVEN_MODIFIER"); break;
                    case BS_IS_ACTIVE_MODIFIER:
                        appendnread(new BSIsActiveModifier(this, ref), "BS_IS_ACTIVE_MODIFIER"); break;
                    case BS_DIRECT_AT_MODIFIER:
                        appendnread(new BSDirectAtModifier(this, ref), "BS_DIRECT_AT_MODIFIER"); break;
                    case BS_TIMER_MODIFIER:
                        appendnread(new BSTimerModifier(this, ref), "BS_TIMER_MODIFIER"); break;
                    case BS_EVENT_ON_DEACTIVATE_MODIFIER:
                        appendnread(new BSEventOnDeactivateModifier(this, ref), "BS_EVENT_ON_DEACTIVATE_MODIFIER"); break;
                    case BS_EVENT_EVERY_N_EVENTS_MODIFIER:
                        appendnread(new BSEventEveryNEventsModifier(this, ref), "BS_EVENT_EVERY_N_EVENTS_MODIFIER"); break;
                    case BS_LOOK_AT_MODIFIER:
                        appendnread(new BSLookAtModifier(this, ref), "BS_LOOK_AT_MODIFIER"); break;
                    case BS_LIMB_IK_MODIFIER:
                        appendnread(new BSLimbIKModifier(this, ref), "BS_LIMB_IK_MODIFIER"); break;
                    case BS_INTERP_VALUE_MODIFIER:
                        appendnread(new BSInterpValueModifier(this, ref), "BS_INTERP_VALUE_MODIFIER"); break;
                    case BS_GET_TIME_STEP_MODIFIER:
                        appendnread(new BSGetTimeStepModifier(this, ref), "BS_GET_TIME_STEP_MODIFIER"); break;
                    case HKB_GET_HANDLE_ON_BONE_MODIFIER:
                        appendnread(new hkbGetHandleOnBoneModifier(this, ref), "HKB_GET_HANDLE_ON_BONE_MODIFIER"); break;
                    case HKB_TRANSFORM_VECTOR_MODIFIER:
                        appendnread(new hkbTransformVectorModifier(this, ref), "HKB_TRANSFORM_VECTOR_MODIFIER"); break;
                    case HKB_EVENT_RANGE_DATA_ARRAY:
                        appendnread(new hkbEventRangeDataArray(this, ref), "HKB_EVENT_RANGE_DATA_ARRAY"); break;
                    case HKB_EVALUATE_HANDLE_MODIFIER:
                        appendnread(new hkbEvaluateHandleModifier(this, ref), "HKB_EVALUATE_HANDLE_MODIFIER"); break;
                    case HKB_COMBINE_TRANSFORMS_MODIFIER:
                        appendnread(new hkbCombineTransformsModifier(this, ref), "HKB_COMBINE_TRANSFORMS_MODIFIER"); break;
                    case HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER:
                        appendnread(new hkbComputeRotationFromAxisAngleModifier(this, ref), "HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER"); break;
                    case HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER:
                        appendnread(new hkbComputeRotationToTargetModifier(this, ref), "HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER"); break;
                    case HKB_EVENTS_FROM_RANGE_MODIFIER:
                        appendnread(new hkbEventsFromRangeModifier(this, ref), "HKB_EVENTS_FROM_RANGE_MODIFIER"); break;
                    case HKB_MOVE_CHARACTER_MODIFIER:
                        appendnread(new hkbMoveCharacterModifier(this, ref), "HKB_MOVE_CHARACTER_MODIFIER"); break;
                    case HKB_EXTRACT_RAGDOLL_POSE_MODIFIER:
                        appendnread(new hkbExtractRagdollPoseModifier(this, ref), "HKB_EXTRACT_RAGDOLL_POSE_MODIFIER"); break;
                    case BS_MODIFY_ONCE_MODIFIER:
                        appendnread(new BSModifyOnceModifier(this, ref), "BS_MODIFY_ONCE_MODIFIER"); break;
                    case HKB_STRING_EVENT_PAYLOAD:
                        appendnread(new hkbStringEventPayload(this, "", ref), "HKB_STRING_EVENT_PAYLOAD"); break;
                    case BS_RAGDOLL_CONTACT_LISTENER_MODIFIER:
                        appendnread(new BSRagdollContactListenerModifier(this, ref), "BS_RAGDOLL_CONTACT_LISTENER_MODIFIER"); break;
                    case HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER:
                        appendnread(new hkbPoweredRagdollControlsModifier(this, ref), "HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER"); break;
                    case BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER:
                        appendnread(new BSEventOnFalseToTrueModifier(this, ref), "BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER"); break;
                    case BS_DIST_TRIGGER_MODIFER:
                        appendnread(new BSDistTriggerModifier(this, ref), "BS_DIST_TRIGGER_MODIFER"); break;
                    case BS_DECOMPOSE_VECTOR_MODIFIER:
                        appendnread(new BSDecomposeVectorModifier(this, ref), "BS_DECOMPOSE_VECTOR_MODIFIER"); break;
                    case BS_COMPUTE_ADD_BONE_ANIM_MODIFIER:
                        appendnread(new BSComputeAddBoneAnimModifier(this, ref), "BS_COMPUTE_ADD_BONE_ANIM_MODIFIER"); break;
                    case BS_TWEENER_MODIFIER:
                        appendnread(new BSTweenerModifier(this, ref), "BS_TWEENER_MODIFIER"); break;
                    case BS_I_STATE_MANAGER_MODIFIER:
                        appendnread(new BSIStateManagerModifier(this, ref), "BS_I_STATE_MANAGER_MODIFIER"); break;
                    case HKB_TIMER_MODIFIER:
                        appendnread(new hkbTimerModifier(this, ref), "HKB_TIMER_MODIFIER"); break;
                    case HKB_ROTATE_CHARACTER_MODIFIER:
                        appendnread(new hkbRotateCharacterModifier(this, ref), "HKB_ROTATE_CHARACTER_MODIFIER"); break;
                    case HKB_DAMPING_MODIFIER:
                        appendnread(new hkbDampingModifier(this, ref), "HKB_DAMPING_MODIFIER"); break;
                    case HKB_DELAYED_MODIFIER:
                        appendnread(new hkbDelayedModifier(this, ref), "HKB_DELAYED_MODIFIER"); break;
                    case HKB_GET_UP_MODIFIER:
                        appendnread(new hkbGetUpModifier(this, ref), "HKB_GET_UP_MODIFIER"); break;
                    case HKB_BONE_INDEX_ARRAY:
                        appendnread(new hkbBoneIndexArray(this, ref), "HKB_BONE_INDEX_ARRAY"); break;
                    case HKB_KEY_FRAME_BONES_MODIFIER:
                        appendnread(new hkbKeyframeBonesModifier(this, ref), "HKB_KEY_FRAME_BONES_MODIFIER"); break;
                    case HKB_COMPUTE_DIRECTION_MODIFIER:
                        appendnread(new hkbComputeDirectionModifier(this, ref), "HKB_COMPUTE_DIRECTION_MODIFIER"); break;
                    case HKB_MIRROR_MODIFIER:
                        appendnread(new hkbMirrorModifier(this, ref), "HKB_MIRROR_MODIFIER"); break;
                    case HKB_GET_WORLD_FROM_MODEL_MODIFIER:
                        appendnread(new hkbGetWorldFromModelModifier(this, ref), "HKB_GET_WORLD_FROM_MODEL_MODIFIER"); break;
                    case HKB_SENSE_HANDLE_MODIFIER:
                        appendnread(new hkbSenseHandleModifier(this, ref), "HKB_SENSE_HANDLE_MODIFIER"); break;
                    case HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER:
                        appendnread(new hkbRigidBodyRagdollControlsModifier(this, ref), "HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER"); break;
                    case HKB_POSE_MATCHING_GENERATOR:
                        appendnread(new hkbPoseMatchingGenerator(this, ref), "HKB_POSE_MATCHING_GENERATOR"); break;
                    case HKB_GENERATOR_TRANSITION_EFFECT:
                        appendnread(new hkbGeneratorTransitionEffect(this, ref), "HKB_GENERATOR_TRANSITION_EFFECT"); break;
                    case BS_SPEED_SAMPLER_MODIFIER:
                        appendnread(new BSSpeedSamplerModifier(this, ref), "BS_SPEED_SAMPLER_MODIFIER"); break;
                    case HKB_FOOT_IK_CONTROLS_MODIFIER:
                        appendnread(new hkbFootIkControlsModifier(this, ref), "HKB_FOOT_IK_CONTROLS_MODIFIER"); break;
                    case HKB_FOOT_IK_MODIFIER:
                        appendnread(new hkbFootIkModifier(this, ref), "HKB_FOOT_IK_MODIFIER"); break;
                    case HKB_DETECT_CLOSE_TO_GROUND_MODIFIER:
                        appendnread(new hkbDetectCloseToGroundModifier(this, ref), "HKB_DETECT_CLOSE_TO_GROUND_MODIFIER"); break;
                    case BS_PASS_BY_TARGET_TRIGGER_MODIFIER:
                        appendnread(new BSLookAtModifier(this, ref), "BS_PASS_BY_TARGET_TRIGGER_MODIFIER"); break;
                    case HKB_HAND_IK_CONTROLS_MODIFIER:
                        appendnread(new hkbHandIkControlsModifier(this, ref), "HKB_HAND_IK_CONTROLS_MODIFIER"); break;
                    case HKB_ATTACHMENT_MODIFIER:
                        appendnread(new hkbAttachmentModifier(this, ref), "HKB_ATTACHMENT_MODIFIER"); break;
                    case HKB_ATTRIBUTE_MODIFIER:
                        appendnread(new hkbAttributeModifier(this, ref), "HKB_ATTRIBUTE_MODIFIER"); break;
                    case HKB_PROXY_MODIFIER:
                        appendnread(new hkbProxyModifier(this, ref), "HKB_PROXY_MODIFIER"); break;
                    case HKB_LOOK_AT_MODIFIER:
                        appendnread(new hkbLookAtModifier(this, ref), "HKB_LOOK_AT_MODIFIER"); break;
                    case BGS_GAMEBYRO_SEQUENCE_GENERATOR:
                        appendnread(new BGSGamebryoSequenceGenerator(this, ref), "BGS_GAMEBYRO_SEQUENCE_GENERATOR"); break;
                    case HKB_BEHAVIOR_GRAPH:
                        appendnread(new hkbBehaviorGraph(this, ref), "HKB_BEHAVIOR_GRAPH");
                        behaviorGraph = generators.last();
                        generators.removeLast();
                        break;
                    case HKB_BEHAVIOR_GRAPH_DATA:
                        appendnread(new hkbBehaviorGraphData(this, ref), "HKB_BEHAVIOR_GRAPH_DATA");
                        graphData = otherTypes.last();
                        otherTypes.removeLast();
                        break;
                    case HKB_BEHAVIOR_GRAPH_STRING_DATA:
                        appendnread(new hkbBehaviorGraphStringData(this, ref), "HKB_BEHAVIOR_GRAPH_STRING_DATA");
                        stringData = otherTypes.last();
                        otherTypes.removeLast();
                        break;
                    case HKB_VARIABLE_VALUE_SET:
                        appendnread(new hkbVariableValueSet(this, ref), "HKB_VARIABLE_VALUE_SET");
                        variableValues = otherTypes.last();
                        otherTypes.removeLast();
                        break;
                    case HK_ROOT_LEVEL_CONTAINER:
                        appendnread(new hkRootLevelContainer(this, ref), "HK_ROOT_LEVEL_CONTAINER");
                        setRootObject(otherTypes.last());
                        otherTypes.removeLast();
                        break;
                    default:
                        LogFile::writeToLog(fileName()+": Unknown signature detected! Unknown object class name is: "+getReader().getNthAttributeValueAt(index, 1)+" Unknown object signature is: "+QString::number(signature, 16));
                    }
                }
            }
        }
        closeFile();
        getReader().clear();
        //if (checkForDuplicateReferencesNumbersNoLock()){
            (link()) ? ok = true : LogFile::writeToLog(fileName()+": failed to link!!!");
        /*}else{
            LogFile::writeToLog(fileName()+": Duplicate References Found!!!");
        }*/
    }else{
        LogFile::writeToLog(fileName()+": failed to parse!!!");
    }
    return ok;
}

bool BehaviorFile::link(){
    auto result = true;
    auto linkobjs = [&](QVector <HkxSharedPtr> & list){
        for (auto i = 0; i < list.size(); i++){
            (!list.at(i)->link()) ? result = false : NULL;
        }
    };
    if (getRootObject().constData()){
        getRootObject()->link();
        linkobjs(generators);
        linkobjs(modifiers);
        linkobjs(otherTypes);
        (!behaviorGraph->link()) ? result = false : NULL;
        (!variableValues->link()) ? result = false : NULL;
        (!graphData->link()) ? result = false : NULL;
        for (auto i = 0; i < generators.size(); i++){
            auto sig = generators.at(i)->getSignature();
            (sig == HKB_STATE_MACHINE || sig == HKB_BLENDER_GENERATOR || sig == BS_BONE_SWITCH_GENERATOR) ? ((generators.at(i)->link()) ? result = false : NULL) : NULL;
        }
        return result;
    }else{
        LogFile::writeToLog("BehaviorFile: link() failed!\nThe root object of this behavior file is nullptr!");
    }
    return false;
}

QVector <int> BehaviorFile::removeDataNoLock(QVector <HkxSharedPtr> & objects){
    QVector <int> removedIndices;
    for (auto i = objects.size() - 1; i >= 0; i--){
        if (objects.at(i).data()->ref < 2){
            objects.removeAt(i);
            removedIndices.append(i);
        }
    }
    return removedIndices;
}

QStringList BehaviorFile::getRefedAnimations() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList refedanimations;
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i).constData() && generators.at(i).constData()->getSignature() == HKB_CLIP_GENERATOR){
            auto temp = static_cast<const hkbClipGenerator *>(generators.at(i).constData())->getAnimationName();
            if (!refedanimations.contains(temp, Qt::CaseInsensitive)){
                refedanimations.append(temp);
            }
        }
    }
    return refedanimations;
}

QString BehaviorFile::detectErrorsMT(int & taskcount, std::mutex & mutex, std::condition_variable & conditionVar){
    errorList.clear();
    auto errors = false;
    auto checkError = [&](QVector <HkxSharedPtr> & objects, int index){
        auto obj = static_cast<HkxObject *>(objects.at(index).data());
        if (obj){
            auto error = obj->evaluateDataValidity();
            if (error != ""){
                errorList.append(error);
                if (obj->getType() != HkxObject::TYPE_OTHER){
                    mutex.lock();
                    static_cast<DataIconManager *>(obj)->setDataValidity(false);
                    mutex.unlock();
                }
                errors = true;
            }
        }
    };
    /*if ((behaviorGraph.data() && !behaviorGraph->evaluateDataValidity()) || (stringData.data() && !stringData->evaluateDataValidity()) ||
            (variableValues.data() && !variableValues->evaluateDataValidity()) || (graphData.data() && !graphData->evaluateDataValidity()))
    {
        errors = true;
        LogFile::writeToLog(getFileName()+": Root data is corrupted!"+"\n");
    }*/
    for (auto i = 0; i < generators.size(); i++){
        checkError(generators, i);
    }
    for (auto i = 0; i < modifiers.size(); i++){
        checkError(modifiers, i);
    }
    /*for (auto i = 0; i < otherTypes.size(); i++){
        checkError(otherTypes, i);
    }*/
    mutex.lock();
    taskcount--;
    conditionVar.notify_one();
    mutex.unlock();
    if (errors){
        return "WARNING: Errors found in \""+getFileName()+"\"!\n";
    }else{
        return "";
    }
}

QString BehaviorFile::detectErrors(){
    //std::lock_guard <std::mutex> guard(mutex);
    errorList.clear();
    auto errors = false;
    auto checkError = [&](QVector <HkxSharedPtr> & objects, int index){
        auto obj = static_cast<HkxObject *>(objects.at(index).data());
        if (obj){
            auto error = obj->evaluateDataValidity();
            if (error != ""){
                errorList.append(error);
                if (obj->getType() != HkxObject::TYPE_OTHER){
                    static_cast<DataIconManager *>(obj)->setDataValidity(false);
                }
                errors = true;
            }
            if (obj->getType() != HkxObject::TYPE_OTHER && static_cast<DataIconManager *>(obj)->isCircularLoop()){
                errorList.append(static_cast<DataIconManager *>(obj)->getName()+"-->Ref: "+obj->getReferenceString()+" IS A CIRCULAR REFERENCE!!!");
                static_cast<DataIconManager *>(obj)->setDataValidity(false);
                errors = true;
            }
        }
    };
    /*if ((behaviorGraph.data() && !behaviorGraph->evaluateDataValidity()) || (stringData.data() && !stringData->evaluateDataValidity()) ||
            (variableValues.data() && !variableValues->evaluateDataValidity()) || (graphData.data() && !graphData->evaluateDataValidity()))
    {
        errors = true;
        LogFile::writeToLog(getFileName()+": Root data is corrupted!"+"\n");
    }*/
    for (auto i = 0; i < generators.size(); i++){
        checkError(generators, i);
    }
    for (auto i = 0; i < modifiers.size(); i++){
        checkError(modifiers, i);
    }
    for (auto i = 0; i < otherTypes.size(); i++){
        (otherTypes.at(i)->getSignature() == HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY) ? otherTypes.at(i)->evaluateDataValidity() : NULL;
    }
    if (errors){
        return "WARNING: Potential errors found in \""+getFileName()+"\"!\n";
    }else{
        return "";
    }
}

QVector <DataIconManager *> BehaviorFile::merge(BehaviorFile *recessivefile, int & taskCount, std::mutex & mutex, std::condition_variable & conditionVar){
    //std::lock_guard <std::mutex> guard(mutex);
    bool found;
    QVector <DataIconManager *> objectsnotfound;
    auto gensize = generators.size();
    auto modsize = modifiers.size();
    auto searchMerge = [&](const QVector <HkxSharedPtr> & domlist, int domsize, const QVector <HkxSharedPtr> & reclist){
        for (auto i = 0; i < reclist.size(); i++){
            found = false;
            for (auto j = i; j < domsize; j++){
                if (static_cast<const DataIconManager *>(domlist.at(j).data())->hasSameSignatureAndName(static_cast<const DataIconManager *>(reclist.at(i).data()))){
                    domlist.at(j)->merge(reclist.at(i).data());
                    found = true;
                    break;
                }
            }
            if (!found){
                for (auto j = 0; j < i && j < domsize; j++){
                    if (static_cast<const DataIconManager *>(domlist.at(j).data())->hasSameSignatureAndName(static_cast<const DataIconManager *>(reclist.at(i).data()))){
                        domlist.at(j)->merge(reclist.at(i).data());
                        found = true;
                        break;
                    }
                }
            }
            if (!found){
                objectsnotfound.append(static_cast<DataIconManager *>(reclist.at(i).data()));
            }
        }
    };
    auto searchMergeForward = [&](const QVector <HkxSharedPtr> & domlist, int domsize, const QVector <HkxSharedPtr> & reclist){
        for (auto i = reclist.size() - 1; i >= 0; i--){
            found = false;
            for (auto j = domsize - 1; j >= 0; j--){
                if (static_cast<const DataIconManager *>(domlist.at(j).data())->hasSameSignatureAndName(static_cast<const DataIconManager *>(reclist.at(i).data()))){
                    domlist.at(j)->merge(reclist.at(i).data());
                    found = true;
                    break;
                }
            }
            if (!found){
                objectsnotfound.append(static_cast<DataIconManager *>(reclist.at(i).data()));
            }
        }
    };
    if (recessivefile){
        //searchMerge(generators, gensize, recessivefile->generators);
        //searchMerge(modifiers, modsize, recessivefile->modifiers);
        searchMergeForward(generators, gensize, recessivefile->generators);
        searchMergeForward(modifiers, modsize, recessivefile->modifiers);
    }else{
        LogFile::writeToLog("ProjectFile: merge() failed!\nrecessiveproject is nullptr!");
    }
    for (auto i = objectsnotfound.size() - 1; i >= 0; i--){
        found = false;
        for (auto j = gensize; j < generators.size(); j++){
            if (static_cast<const DataIconManager *>(objectsnotfound.at(i))->hasSameSignatureAndName(static_cast<const DataIconManager *>(generators.at(j).data()))){
                objectsnotfound.removeAt(i);
                break;
            }
        }
    }
    mutex.lock();
    taskCount--;
    conditionVar.notify_one();
    mutex.unlock();
    return objectsnotfound;
}

QVector <DataIconManager *> BehaviorFile::merge(BehaviorFile *recessivefile){
    //std::lock_guard <std::mutex> guard(mutex);
    QVector <DataIconManager *> objectsnotfound;
    bool found;
    auto gensize = generators.size();
    auto modsize = modifiers.size();
    auto searchMergeForward = [&](const QVector <HkxSharedPtr> & domlist, int domsize, const QVector <HkxSharedPtr> & reclist){
        for (auto i = reclist.size() - 1; i >= 0; i--){
            found = false;
            for (auto j = domsize - 1; j >= 0; j--){
                if (static_cast<const DataIconManager *>(domlist.at(j).data())->hasSameSignatureAndName(static_cast<const DataIconManager *>(reclist.at(i).data()))){
                    domlist.at(j)->merge(reclist.at(i).data());
                    found = true;
                    break;
                }
            }
            if (!found){
                objectsnotfound.append(static_cast<DataIconManager *>(reclist.at(i).data()));
            }
        }
    };
    auto checkobjects = [&](const QVector <HkxSharedPtr> & domlist){
        for (auto i = objectsnotfound.size() - 1; i >= 0; i--){
            found = false;
            for (auto j = gensize; j < domlist.size(); j++){
                if (static_cast<const DataIconManager *>(objectsnotfound.at(i))->hasSameSignatureAndName(static_cast<const DataIconManager *>(domlist.at(j).data()))){
                    objectsnotfound.removeAt(i);
                    break;
                }
            }
        }
    };
    if (recessivefile){
        searchMergeForward(generators, gensize, recessivefile->generators);
        searchMergeForward(modifiers, modsize, recessivefile->modifiers);
        checkobjects(generators);
        checkobjects(modifiers);
    }else{
        LogFile::writeToLog("ProjectFile: merge() failed!\nrecessiveproject is nullptr!");
    }
    return objectsnotfound;
}

void BehaviorFile::mergeObjects(QVector <DataIconManager *> & recessiveobjects){
    //std::lock_guard <std::mutex> guard(mutex);
    bool found;
    auto gensize = generators.size();
    auto modsize = modifiers.size();
    auto searchMergeForward = [&](const QVector <HkxSharedPtr> & domlist, int domsize){
        for (auto i = recessiveobjects.size() - 1; i >= 0; i--){
            found = false;
            if (recessiveobjects.at(i)->getParentFilename().compare(getFileName(), Qt::CaseInsensitive)){
                for (auto j = domsize - 1; j >= 0; j--){
                    if (static_cast<const DataIconManager *>(domlist.at(j).data())->hasSameSignatureAndName(static_cast<const DataIconManager *>(recessiveobjects.at(i)))){
                        domlist.at(j)->merge(recessiveobjects.at(i));
                        found = true;
                        break;
                    }
                }
                if (found){
                    recessiveobjects.removeAt(i);
                }
            }
        }
    };
    if (!recessiveobjects.isEmpty()){
        searchMergeForward(generators, gensize);
        searchMergeForward(modifiers, modsize);
    }
}

void BehaviorFile::mergedWrite(){
    //std::lock_guard <std::mutex> guard(mutex);
    long ref = 100;
    auto root = getRootObject().data();
    if (root){
        root->setIsWritten(false);
        stringData->setIsWritten(false);
        variableValues->setIsWritten(false);
        graphData->setIsWritten(false);
        behaviorGraph->setIsWritten(false);
        root->setReference(ref);
        ref++;
        stringData->setReference(ref);
        ref++;
        variableValues->setReference(ref);
        ref++;
        graphData->setReference(ref);
        ref++;
        behaviorGraph->setReference(ref);
        ref++;
        auto objects = static_cast<DataIconManager *>(behaviorGraph.data())->getChildren();
        QVector <DataIconManager *> children;
        while (!objects.isEmpty()){
            auto obj = objects.last();
            if (!obj->getRefsUpdated()){
                obj->updateReferences(ref);
                obj->setRefsUpdated(true);
                obj->setIsWritten(false);
                children = obj->getChildren();
            }
            objects.removeLast();
            objects = objects + children;
            children.clear();
            ref++;
        }
        ref++;
        auto prepforwrite = [&](const QVector <HkxSharedPtr> & list){
            for (auto i = 0; i < list.size(); i++, ref++){
                if (!list.at(i)->getRefsUpdated()){
                    list.at(i)->setIsWritten(false);
                    list.at(i)->setReference(ref);
                }
            }
            ref++;
        };
        prepforwrite(generators);
        prepforwrite(modifiers);
        prepforwrite(otherTypes);
        getWriter().setFile(this);
        if (!getWriter().writeToXMLFile()){
            CRITICAL_ERROR_MESSAGE("BehaviorFile::write(): writeToXMLFile() failed!!");
        }
    }else{
        CRITICAL_ERROR_MESSAGE("BehaviorFile::write(): The root object is nullptr!!");
    }
}

void BehaviorFile::write(){
    //std::lock_guard <std::mutex> guard(mutex);
    long ref = 100;
    auto root = getRootObject().data();
    if (root){
        root->setIsWritten(false);
        stringData->setIsWritten(false);
        variableValues->setIsWritten(false);
        graphData->setIsWritten(false);
        behaviorGraph->setIsWritten(false);
        root->setReference(ref++);
        stringData->setReference(ref++);
        variableValues->setReference(ref++);
        graphData->setReference(ref++);
        behaviorGraph->setReference(ref++);
        auto prepforwrite = [&](const QVector <HkxSharedPtr> & list){
            for (auto i = 0; i < list.size(); i++, ref++){
                list.at(i)->setIsWritten(false);
                list.at(i)->setReference(ref);
            }
            ref++;
        };
        prepforwrite(generators);
        prepforwrite(modifiers);
        prepforwrite(otherTypes);
        getWriter().setFile(this);
        if (!getWriter().writeToXMLFile()){
            CRITICAL_ERROR_MESSAGE("BehaviorFile::write(): writeToXMLFile() failed!!");
        }
    }else{
        CRITICAL_ERROR_MESSAGE("BehaviorFile::write(): The root object is nullptr!!");
    }
}

HkxSharedPtr * BehaviorFile::findGenerator(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (ref > 0){
        for (auto i = 0; i < generators.size(); i++){
            if (ref == generators.at(i).getShdPtrReference()){
                return &generators[i];
            }
        }
    }
    return nullptr;
}

HkxSharedPtr * BehaviorFile::findModifier(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (ref > 0){
        for (auto i = 0; i < modifiers.size(); i++){
            if (ref == modifiers.at(i).getShdPtrReference()){
                return &modifiers[i];
            }
        }
    }
    return nullptr;
}

HkxSharedPtr * BehaviorFile::findHkxObject(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (ref > 0){
        for (auto i = 0; i < otherTypes.size(); i++){
            if (ref == otherTypes.at(i).getShdPtrReference()){
                return &otherTypes[i];
            }
        }
    }
    return nullptr;
}

int BehaviorFile::addCharacterProperty(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->addVariable(character->getCharacterPropertyTypeAt(index), character->getCharacterPropertyNameAt(index), true);
    }
    return -1;
}

QString BehaviorFile::getVariableNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data()){
        return static_cast<hkbBehaviorGraphStringData *>(stringData.data())->getVariableNameAt(index);
    }
    return "";
}

QString BehaviorFile::getEventNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data()){
        return static_cast<hkbBehaviorGraphStringData *>(stringData.data())->getEventNameAt(index);
    }
    return "";
}

QString BehaviorFile::getCharacterPropertyNameAt(int index, bool fromBehaviorFile) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (fromBehaviorFile){
        if (graphData.data()){
            return static_cast<hkbBehaviorGraphData *>(graphData.data())->getCharacterPropertyNameAt(index);
        }
    }else{
        if (character){
            return character->getCharacterPropertyNameAt(index);
        }
    }
    return "";
}

QStringList BehaviorFile::getAnimationNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getAnimationNames();
    }
    return QStringList();
}

QString BehaviorFile::getAnimationNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getAnimationNameAt(index);
    }
    return QString();
}

QStringList BehaviorFile::getLocalFrameNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getLocalFrameNames();
    }
    return QStringList();
}

void BehaviorFile::removeBindings(int varIndex){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < otherTypes.size(); i++){
        if (otherTypes.at(i)->getSignature() == HKB_VARIABLE_BINDING_SET){
            static_cast<hkbVariableBindingSet *>(otherTypes.at(i).data())->removeBinding(varIndex);
        }
    }
}

bool BehaviorFile::checkForDuplicateReferencesNumbersNoLock() const{
    auto duplicates = false;
    auto ref1 = 0;
    auto ref2 = 0;
    auto checkrefs = [&](const QVector <HkxSharedPtr> list){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i).data()){
                ref1 = list.at(i)->getReference();
                for (auto j = 0; j < list.size(); j++){
                    if (list.at(j).data()){
                        ref2 = list.at(j)->getReference();
                        if (ref1 == ref2 && j != i){
                            duplicates = true;
                            LogFile::writeToLog(getFileName()+"Reference "+QString::number(ref2)+" is duplicated!");
                        }
                    }
                }
            }
        }
    };
    checkrefs(generators);
    checkrefs(modifiers);
    checkrefs(otherTypes);
    return duplicates;
}

HkxSharedPtr BehaviorFile::getGraphData() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return graphData;
}

HkxSharedPtr BehaviorFile::getStringData() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return stringData;
}

HkxSharedPtr BehaviorFile::getVariableValues() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return variableValues;
}

HkxSharedPtr * BehaviorFile::findBehaviorGraph(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (behaviorGraph.getShdPtrReference() == ref){
        return &behaviorGraph;
    }
    return nullptr;
}

QVector<int> BehaviorFile::removeGeneratorData(){
    //std::lock_guard <std::mutex> guard(mutex);
    return removeDataNoLock(generators);
}

QVector<int> BehaviorFile::removeModifierData(){
    //std::lock_guard <std::mutex> guard(mutex);
    return removeDataNoLock(modifiers);
}

QVector<int> BehaviorFile::removeOtherData(){
    //std::lock_guard <std::mutex> guard(mutex);
    return removeDataNoLock(otherTypes);
}

hkVariableType BehaviorFile::getVariableTypeAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getVariableTypeAt(index);
    }
    return hkVariableType();
}

QStringList BehaviorFile::getGeneratorNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < generators.size(); i++){
        list.append(static_cast<hkbGenerator *>(generators.at(i).data())->getName());
    }
    return list;
}

QStringList BehaviorFile::getGeneratorTypeNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < generators.size(); i++){
        list.append(static_cast<hkbGenerator *>(generators.at(i).data())->getClassname());
    }
    return list;
}

QStringList BehaviorFile::getModifierNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < modifiers.size(); i++){
        list.append(static_cast<hkbGenerator *>(modifiers.at(i).data())->getName());
    }
    return list;
}

QStringList BehaviorFile::getModifierTypeNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < modifiers.size(); i++){
        list.append(static_cast<hkbModifier *>(modifiers.at(i).data())->getClassname());
    }
    return list;
}

int BehaviorFile::getCharacterPropertyIndexFromBehavior(const QString &name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data()){
        auto strings = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
        for (auto i = 0; i < strings->characterPropertyNames.size(); i++){
            if (strings->characterPropertyNames.at(i) == name){
                return i;
            }
        }
    }
    return -1;
}

int BehaviorFile::getCharacterPropertyIndex(const QString &name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getCharacterPropertyIndex(name);
    }
    return -1;
}

int BehaviorFile::findCharacterPropertyIndexFromCharacter(int indexOfBehaviorProperty) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character && stringData.data()){
        return character->getCharacterPropertyIndex(static_cast<hkbBehaviorGraphStringData *>(stringData.data())->getCharacterPropertyNameAt(indexOfBehaviorProperty));
    }
    return -1;
}

QStringList BehaviorFile::getCharacterPropertyNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return character->getCharacterPropertyNames();
    }
    return QStringList();
}

QStringList BehaviorFile::getAllReferencedBehaviorFilePaths() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i).constData() && generators.at(i)->getSignature() == HKB_BEHAVIOR_REFERENCE_GENERATOR){
            auto ptr = static_cast<hkbBehaviorReferenceGenerator *>(generators.at(i).data());
            list.append(ptr->getBehaviorName());
        }
    }
    return list;
}

HkxObject * BehaviorFile::getBehaviorGraphData() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return graphData.data();
}

QStringList BehaviorFile::getErrors() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return errorList;
}

int BehaviorFile::getIndexOfGenerator(const HkxSharedPtr & obj) const{
    //std::lock_guard <std::mutex> guard(mutex);
    return generators.indexOf(obj);
}

bool BehaviorFile::setGeneratorData(HkxSharedPtr & ptrToSet, int index){
    //std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < generators.size()){
        ptrToSet = generators.at(index);
        return true;
    }
    return false;
}

hkbGenerator *BehaviorFile::getGeneratorDataAt(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < generators.size()){
        return static_cast<hkbGenerator *>(generators[index].data());
    }
    return nullptr;
}

int BehaviorFile::getIndexOfModifier(const HkxSharedPtr & obj) const{
    //std::lock_guard <std::mutex> guard(mutex);
    return modifiers.indexOf(obj);
}

bool BehaviorFile::setModifierData(HkxSharedPtr & ptrToSet, int index){
    //std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < modifiers.size()){
        ptrToSet = modifiers.at(index);
        return true;
    }
    return false;
}

hkbModifier* BehaviorFile::getModifierDataAt(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < modifiers.size()){
        return static_cast<hkbModifier *>(modifiers[index].data());
    }
    return nullptr;
}

QStringList BehaviorFile::getVariableTypenames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getVariableTypeNames();
    }
    return QStringList();
}

QStringList BehaviorFile::getCharacterPropertyTypenames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return static_cast<CharacterFile *>(character)->getCharacterPropertyTypenames();
    }
    return QStringList();
}

hkVariableType BehaviorFile::getCharacterPropertyTypeAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (character){
        return static_cast<CharacterFile *>(character)->getCharacterPropertyTypeAt(index);
    }
    return VARIABLE_TYPE_INT8;
}

QStringList BehaviorFile::getVariableNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getVariableNames();
    }
    return QStringList();
}

int BehaviorFile::getNumberOfVariables() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getNumberOfVariables();
    }
    return -1;
}

QStringList BehaviorFile::getEventNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getEventNames();
    }
    return QStringList();
}

int BehaviorFile::getNumberOfEvents() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (graphData.data()){
        return static_cast<hkbBehaviorGraphData *>(graphData.data())->getNumberOfEvents();
    }
    return -1;
}

bool BehaviorFile::doesBehaviorExist(const QString &behaviorname) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->doesBehaviorExist(behaviorname);
    }
    return false;
}

void BehaviorFile::setFocusGeneratorIcon(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    index--;
    if (index < generators.size() && index > -1){
        static_cast<DataIconManager *>(generators.at(index).data())->setFocusOnTopIcon();
    }
}

void BehaviorFile::setFocusModifierIcon(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    index--;
    if (index < modifiers.size() && index > -1){
        static_cast<DataIconManager *>(modifiers.at(index).data())->setFocusOnTopIcon();
    }
}

BehaviorFile::~BehaviorFile(){
    //
}
