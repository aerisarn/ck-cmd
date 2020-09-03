#ifndef UTILITY_H
#define UTILITY_H

#include <QtGlobal>
#include <QString>
//#include <QStringBuilder>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <mutex>
#include <fstream>

namespace UI {

#define MAX_HKXXML_LINE_LENGTH 512
//#define WRITE_TO_LOG(message){QFile file(QDir::currentPath()+"/DebugLog.txt");if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){QTextStream out(&file);out << message << "\n";}}
//#define CRITICAL_ERROR_MESSAGE(message){QFile log(QDir::currentPath()+"/DebugLog.txt");if (log.open(QIODevice::WriteOnly | QIODevice::Text)){QTextStream stream(&log);stream << message << "\n\n";abort();}}
//#define FATAL_ERROR_MESSAGE(message){QString str(message);QMessageBox msg;msg.setModal(true);msg.setText("WARNING: "+str);msg.exec();abort();}
#define CRITICAL_ERROR_MESSAGE(message){QString str(message);QMessageBox msg;msg.setModal(true);msg.setText("CRITICAL ERROR: "+str+"\n\nWe advise that you save and then close the application and contact the author!!!");msg.exec();}
#define WARNING_MESSAGE(message){QString str(message);QMessageBox msg;msg.setModal(true);msg.setText("WARNING: "+str);msg.exec();}
#define USER_MESSAGE(message){QString str(message);QMessageBox msg;msg.setModal(true);msg.setText(str);msg.exec();}

#define VIEW_GENERATORS_TABLE_TIP "Double click to view the table of generators"
#define VIEW_MODIFIERS_TABLE_TIP "Double click to view the table of modifiers"
#define VIEW_EVENTS_TABLE_TIP "Double click to view the table of events"
#define VIEW_ANIMATIONS_TABLE_TIP "Double click to view the table of animations"
#define VIEW_VARIABLES_TABLE_TIP "Double click to view the table of variables, check the box to view the table of character properties instead"
#define VIEW_BONES_TABLE_TIP "Double click to view the table of bones"
#define DRAG_DROP_N_SWAP "Hold click to start drag, release click to swap dragged item with item under the mouse cursor"

#define MAX_ERROR_STRING_SIZE 200

enum HkxSignature: unsigned long long {
    NULL_SIGNATURE = 0x0,
    //Project
    HK_ROOT_LEVEL_CONTAINER = 0x2772c11e,
    HKB_PROJECT_DATA = 0x13a39ba7,
    HKB_PROJECT_STRING_DATA = 0x76ad60a,
    //Character
    HKB_CHARACTER_STRING_DATA = 0x655b42bc,
    HKB_MIRRORED_SKELETON_INFO = 0xc6c2da4f,
    HKB_FOOT_IK_DRIVER_INFO = 0xc6a09dbf,
    HKB_BONE_WEIGHT_ARRAY = 0xcd902b77,
    HKB_VARIABLE_VALUE_SET = 0x27812d8d,
    HKB_CHARACTER_DATA = 0x300d6808,
    HKB_HAND_IK_DRIVER_INFO = 0xc299090a,
    //Skeleton
    HKA_ANIMATION_CONTAINER = 0x8dc20333,
    HKA_SKELETON = 0x366e8220,
    HK_SIMPLE_LOCAL_FRAME = 0xe758f63c,
    //Behavior::Generators
    HKB_BEHAVIOR_GRAPH = 0xb1218f86,
    HKB_STATE_MACHINE = 0x816c1dcb,
    HKB_STATE_MACHINE_STATE_INFO = 0xed7f9d0,
    HKB_BEHAVIOR_REFERENCE_GENERATOR = 0xfcb5423,
    BS_CYCLIC_BLEND_TRANSITION_GENERATOR = 0x5119eb06,
    HKB_BLENDER_GENERATOR = 0x22df7147,
    HKB_BLENDER_GENERATOR_CHILD = 0xe2b384b0,
    HKB_MANUAL_SELECTOR_GENERATOR = 0xd932fab8,
    HKB_MODIFIER_GENERATOR = 0x1f81fae6,
    BS_BONE_SWITCH_GENERATOR = 0xf33d3eea,
    BS_BONE_SWITCH_GENERATOR_BONE_DATA = 0xc1215be6,
    BS_I_STATE_TAGGING_GENERATOR = 0xf0826fc1,
    BS_OFFSET_ANIMATION_GENERATOR = 0xb8571122,
    HKB_CLIP_GENERATOR = 0x333b85b9,
    HKB_POSE_MATCHING_GENERATOR = 0x29e271b4,
    HKB_REFERENCE_POSE_GENERATOR = 0x26a5675a,
    BS_SYNCHRONIZED_CLIP_GENERATOR = 0xd83bea64,
    BGS_GAMEBYRO_SEQUENCE_GENERATOR = 0xc8df2d77,
    //Behavior::Modifiers
    HKB_HAND_IK_CONTROLS_MODIFIER = 0x9f0488bb,
    HKB_GET_WORLD_FROM_MODEL_MODIFIER = 0x873fc6f7,
    BS_DIST_TRIGGER_MODIFER = 0xb34d2bbd,
    HKB_FOOT_IK_CONTROLS_MODIFIER = 0xe5b6f544,
    HKB_EVALUATE_EXPRESSION_MODIFIER = 0xf900f6be,
    HKB_MODIFIER_LIST = 0xa4180ca1,
    HKB_EVENT_DRIVEN_MODIFIER = 0x7ed3f44e,
    HKB_ROTATE_CHARACTER_MODIFIER = 0x877ebc0b,
    HKB_TWIST_MODIFIER = 0xb6b76b32,
    HKB_TIMER_MODIFIER = 0x338b4879,
    BS_DIRECT_AT_MODIFIER = 0x19a005c0,
    BS_IS_ACTIVE_MODIFIER = 0xb0fde45a,
    HKB_DAMPING_MODIFIER = 0x9a040f03,
    BS_LIMB_IK_MODIFIER = 0x8ea971e5,
    HKB_COMPUTE_DIRECTION_MODIFIER = 0xdf358bd3,
    BS_DECOMPOSE_VECTOR_MODIFIER = 0x31f6b8b6,
    HKB_GET_HANDLE_ON_BONE_MODIFIER = 0x50c34a17,
    HKB_EVALUATE_HANDLE_MODIFIER = 0x79757102,
    HKB_DETECT_CLOSE_TO_GROUND_MODIFIER = 0x981687b2,
    HKB_DELAYED_MODIFIER = 0x8e101a7a,
    HKB_SENSE_HANDLE_MODIFIER = 0x2a064d99,
    HKB_MOVE_CHARACTER_MODIFIER = 0x8f7492a0,
    HKB_ATTACHMENT_MODIFIER = 0xcc0aab32,
    HKB_ATTRIBUTE_MODIFIER = 0x1245d97d,
    HKB_COMBINE_TRANSFORMS_MODIFIER = 0xfd1f0b79,
    HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER = 0x9b3f6936,
    HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER = 0x47665f1c,
    HKB_EVENTS_FROM_RANGE_MODIFIER = 0xbc561b6e,
    HKB_EXTRACT_RAGDOLL_POSE_MODIFIER = 0x804dcbab,
    HKB_GENERATOR_TRANSITION_EFFECT = 0x5f771b12,
    HKB_GET_UP_MODIFIER = 0x61cb7ac0,
    HKB_KEY_FRAME_BONES_MODIFIER = 0x95f66629,
    HKB_LOOK_AT_MODIFIER = 0x3d28e066,
    HKB_MIRROR_MODIFIER = 0xa9a271ea,
    HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER = 0xaa87d1eb,
    HKB_TRANSFORM_VECTOR_MODIFIER = 0xf93e0e24,
    BS_COMPUTE_ADD_BONE_ANIM_MODIFIER = 0xa67f8c46,
    BS_EVENT_EVERY_N_EVENTS_MODIFIER = 0x6030970c,
    BS_EVENT_ON_DEACTIVATE_MODIFIER = 0x1062d993,
    BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER = 0x81d0777a,
    BS_GET_TIME_STEP_MODIFIER = 0xbda33bfe,
    BS_INTERP_VALUE_MODIFIER = 0x29adc802,
    BS_I_STATE_MANAGER_MODIFIER = 0x6cb24f2e,
    BS_LOOK_AT_MODIFIER = 0xd756fc25,
    BS_MODIFY_ONCE_MODIFIER = 0x1e20a97a,
    BS_PASS_BY_TARGET_TRIGGER_MODIFIER = 0x703d7b66,
    BS_RAGDOLL_CONTACT_LISTENER_MODIFIER = 0x8003d8ce,
    BS_SPEED_SAMPLER_MODIFIER = 0xd297fda9,
    BS_TIMER_MODIFIER = 0x531f3292,
    BS_TWEENER_MODIFIER = 0x0d2d9a04,
    HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER = 0x7cb54065,
    HKB_PROXY_MODIFIER = 0x8a41554f,
    HKB_FOOT_IK_MODIFIER = 0xed8966c0,
    //Behavior::Other
    HKB_STRING_CONDITION = 0x5ab50487,
    HKB_EVENT_RANGE_DATA_ARRAY = 0x330a56ee,
    HKB_BONE_INDEX_ARRAY = 0xaa8619,
    HKB_VARIABLE_BINDING_SET = 0x338ad4ff,
    HKB_BEHAVIOR_GRAPH_STRING_DATA = 0xc713064e,
    HKB_BEHAVIOR_GRAPH_DATA = 0x95aca5d,
    HKB_STRING_EVENT_PAYLOAD = 0xed04256a,
    HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY = 0xe397b11e,
    HKB_EXPRESSION_CONDITION = 0x1c3c1045,
    HKB_BLENDING_TRANSITION_EFFECT = 0xfd8584fe,
    HKB_EXPRESSION_DATA_ARRAY = 0x4b9ee1a2,
    HKB_CLIP_TRIGGER_ARRAY = 0x59c23a0f,
    /*HKB_POWERED_RAGDOLL_CONTROL_DATA = 0x0f5ba21b,*/
    /*HKB_RIGID_BODY_RAGDOLL_CONTROL_DATA = 0x1e0bc068,*/
    HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY = 0xb07b4388
};

namespace{
class LogFile final{
private:
    static std::ofstream logFile;
    static std::mutex mutex;
public:
    LogFile(){
//        logFile.open(QDir::currentPath().toStdString()+"/DebugLog.txt", std::ios::out | std::ios::trunc);
    }
    ~LogFile(){
//        logFile << "\n\nApplication closing... Bye..\n";
 //       logFile.close();
    }

    static void writeToLog(const QString & message){
//        std::lock_guard <std::mutex> guard(mutex);
        //logFile.open(QDir::currentPath().toStdString()+"/DebugLog.txt", std::ios::out | std::ios::trunc);
 //       logFile << message.toStdString() << "\n\n";
    }

    static void writeToLog(const QStringList & messages){
 //       std::lock_guard <std::mutex> guard(mutex);
        //logFile.open(QDir::currentPath().toStdString()+"/DebugLog.txt", std::ios::out | std::ios::trunc);
 //       for (auto i = 0; i < messages.size(); i++){
 //           logFile << messages.at(i).toStdString() << "\n";
        //}
    }
};

std::ofstream LogFile::logFile;
std::mutex LogFile::mutex;
}

struct hkVector3
{
    hkVector3(qreal x = 0, qreal y = 0, qreal z = 0): x(x), y(y), z(z){}
    qreal x;
    qreal y;
    qreal z;
};

struct hkQuadVariable
{
    hkQuadVariable(qreal x = 0, qreal y = 0, qreal z = 0, qreal w = 0)
        : x(x),
          y(y),
          z(z),
          w(w)
    {
        //
    }

    bool operator!=(const hkQuadVariable & rhs) const{
        if (x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w){
            return true;
        }
        return false;
    }

    QString getValueAsString() const{
        return "("+QString::number(x, char('f'), 6)+" "+QString::number(y, char('f'), 6)+" "+QString::number(z, char('f'), 6)+" "+QString::number(w, char('f'), 6)+")";
    }

    qreal x;
    qreal y;
    qreal z;
    qreal w;
};

struct hkTransform
{
    hkVector3 transformA;
    hkVector3 transformB;
};

struct hkQsTransform
{
    QString getValueAsString() const{
        return "("+QString::number(v1.x, char('f'), 6)+" "+QString::number(v1.y, char('f'), 6)+" "+QString::number(v1.z, char('f'), 6)+")("+
                QString::number(v2.x, char('f'), 6)+" "+QString::number(v2.y, char('f'), 6)+" "+QString::number(v2.z, char('f'), 6)+" "+QString::number(v2.w, char('f'), 6)+")("+
                QString::number(v3.x, char('f'), 6)+" "+QString::number(v3.y, char('f'), 6)+" "+QString::number(v3.z, char('f'), 6)+")";
    }

    hkVector3 v1;
    hkQuadVariable v2;
    hkVector3 v3;
};

enum hkVariableType {
    VARIABLE_TYPE_BOOL,
    VARIABLE_TYPE_INT8,
    VARIABLE_TYPE_INT16,
    VARIABLE_TYPE_INT32,
    VARIABLE_TYPE_REAL,
    VARIABLE_TYPE_POINTER,
    VARIABLE_TYPE_VECTOR4,
    VARIABLE_TYPE_QUATERNION
};

namespace {

QString getVariableTypeAsString(hkVariableType type){
    switch (type){
    case VARIABLE_TYPE_BOOL:
        return "VARIABLE_TYPE_BOOL";
    case VARIABLE_TYPE_INT8:
        return "VARIABLE_TYPE_INT8";
    case VARIABLE_TYPE_INT16:
        return "VARIABLE_TYPE_INT16";
    case VARIABLE_TYPE_INT32:
        return "VARIABLE_TYPE_INT32";
    case VARIABLE_TYPE_REAL:
        return "VARIABLE_TYPE_REAL";
    case VARIABLE_TYPE_POINTER:
        return "VARIABLE_TYPE_POINTER";
    case VARIABLE_TYPE_VECTOR4:
        return "VARIABLE_TYPE_VECTOR4";
    case VARIABLE_TYPE_QUATERNION:
        return "VARIABLE_TYPE_QUATERNION";
    }
    return "";
}

bool chopLine(QFile * file, QByteArray & line, ulong & linecount){
    if (file){
        if (!file->atEnd()){
            line = file->readLine();
            line.chop(1);
            linecount++;
            return true;
        }
    }
    return false;
}

QString trimFloat(QString & string){
    for (auto i = string.size() - 1; i >= 0; i--){
        if (string.at(i) == '0'){
            string.remove(i, 1);
        }else if (string.at(i) == '.'){
            string.remove(i, 1);
            break;
        }else{
            break;
        }
    }
    return string;
}

void generateAppendStringToList(QStringList & list, QString & newname, const QChar & wheretoappend){
    for (auto i = 0, num = 0, index = 0; i < list.size() - 1; i++){
        if (list.at(i) == newname){
            index = newname.lastIndexOf(wheretoappend);
            (index > -1) ? newname.remove(++index, newname.size()) : NULL;
            newname.append(QString::number(num));
            (++num > 1) ? i = -1 : NULL;
        }
    }
    list.append(newname);
}

bool areVariableTypesCompatible(hkVariableType type1, hkVariableType type2){
    if (type1 == type2 || (type1 < VARIABLE_TYPE_REAL && type2 < VARIABLE_TYPE_REAL)){
        return true;
    }
    return false;
}

}

}
#endif // UTILITY_H
