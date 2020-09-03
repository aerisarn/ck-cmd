#include "rigidbodyragdollcontrolsmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbRigidBodyRagdollControlsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/boneindexarrayui.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 16

#define NAME_ROW 0
#define ENABLE_ROW 1
#define HIERARCHY_ROW 2
#define VELOCITY_DAMPING_ROW 3
#define ACCELERATION_GAIN_ROW 4
#define VELOCITY_GAIN_ROW 5
#define POSITION_GAIN_ROW 6
#define POSITION_MAX_LINEAR_VELOCITY_ROW 7
#define POSITION_MAX_ANGULAR_VELOCITY_ROW 8
#define SNAP_GAIN_ROW 9
#define SNAP_MAX_LINEAR_VELOCITY_ROW 10
#define SNAP_MAX_ANGULAR_VELOCITY_ROW 11
#define SNAP_MAX_LINEAR_DISTANCE_ROW 12
#define SNAP_MAX_ANGULAR_DISTANCE_ROW 13
#define DURATION_TO_BLEND_ROW 14
#define BONES_ROW 15

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList RigidBodyRagdollControlsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

RigidBodyRagdollControlsModifierUI::RigidBodyRagdollControlsModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      groupBox(new QGroupBox("hkbRigidBodyRagdollControlsModifier")),
      table(new TableWidget(QColor(Qt::white))),
      boneIndexUI(new BoneIndexArrayUI),
      name(new LineEdit),
      enable(new CheckBox),
      hierarchyGain(new DoubleSpinBox),
      velocityDamping(new DoubleSpinBox),
      accelerationGain(new DoubleSpinBox),
      velocityGain(new DoubleSpinBox),
      positionGain(new DoubleSpinBox),
      positionMaxLinearVelocity(new DoubleSpinBox),
      positionMaxAngularVelocity(new DoubleSpinBox),
      snapGain(new DoubleSpinBox),
      snapMaxLinearVelocity(new DoubleSpinBox),
      snapMaxAngularVelocity(new DoubleSpinBox),
      snapMaxLinearDistance(new DoubleSpinBox),
      snapMaxAngularDistance(new DoubleSpinBox),
      durationToBlend(new DoubleSpinBox),
      bones(new CheckButtonCombo("Edit"))
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(HIERARCHY_ROW, NAME_COLUMN, new TableWidgetItem("hierarchyGain"));
    table->setItem(HIERARCHY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(HIERARCHY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HIERARCHY_ROW, VALUE_COLUMN, hierarchyGain);
    table->setItem(VELOCITY_DAMPING_ROW, NAME_COLUMN, new TableWidgetItem("velocityDamping"));
    table->setItem(VELOCITY_DAMPING_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(VELOCITY_DAMPING_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VELOCITY_DAMPING_ROW, VALUE_COLUMN, velocityDamping);
    table->setItem(ACCELERATION_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("accelerationGain"));
    table->setItem(ACCELERATION_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ACCELERATION_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ACCELERATION_GAIN_ROW, VALUE_COLUMN, accelerationGain);
    table->setItem(VELOCITY_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("velocityGain"));
    table->setItem(VELOCITY_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(VELOCITY_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VELOCITY_GAIN_ROW, VALUE_COLUMN, velocityGain);
    table->setItem(POSITION_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("positionGain"));
    table->setItem(POSITION_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSITION_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSITION_GAIN_ROW, VALUE_COLUMN, positionGain);
    table->setItem(POSITION_MAX_LINEAR_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("positionMaxLinearVelocity"));
    table->setItem(POSITION_MAX_LINEAR_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSITION_MAX_LINEAR_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSITION_MAX_LINEAR_VELOCITY_ROW, VALUE_COLUMN, positionMaxLinearVelocity);
    table->setItem(POSITION_MAX_ANGULAR_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("positionMaxAngularVelocity"));
    table->setItem(POSITION_MAX_ANGULAR_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(POSITION_MAX_ANGULAR_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POSITION_MAX_ANGULAR_VELOCITY_ROW, VALUE_COLUMN, positionMaxAngularVelocity);
    table->setItem(SNAP_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("snapGain"));
    table->setItem(SNAP_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SNAP_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SNAP_GAIN_ROW, VALUE_COLUMN, snapGain);
    table->setItem(SNAP_MAX_LINEAR_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("snapMaxLinearVelocity"));
    table->setItem(SNAP_MAX_LINEAR_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SNAP_MAX_LINEAR_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SNAP_MAX_LINEAR_VELOCITY_ROW, VALUE_COLUMN, snapMaxLinearVelocity);
    table->setItem(SNAP_MAX_ANGULAR_VELOCITY_ROW, NAME_COLUMN, new TableWidgetItem("snapMaxAngularVelocity"));
    table->setItem(SNAP_MAX_ANGULAR_VELOCITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SNAP_MAX_ANGULAR_VELOCITY_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SNAP_MAX_ANGULAR_VELOCITY_ROW, VALUE_COLUMN, snapMaxAngularVelocity);
    table->setItem(SNAP_MAX_LINEAR_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("snapMaxLinearDistance"));
    table->setItem(SNAP_MAX_LINEAR_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SNAP_MAX_LINEAR_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SNAP_MAX_LINEAR_DISTANCE_ROW, VALUE_COLUMN, snapMaxLinearDistance);
    table->setItem(SNAP_MAX_ANGULAR_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("snapMaxAngularDistance"));
    table->setItem(SNAP_MAX_ANGULAR_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SNAP_MAX_ANGULAR_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SNAP_MAX_ANGULAR_DISTANCE_ROW, VALUE_COLUMN, snapMaxAngularDistance);
    table->setItem(DURATION_TO_BLEND_ROW, NAME_COLUMN, new TableWidgetItem("durationToBlend"));
    table->setItem(DURATION_TO_BLEND_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DURATION_TO_BLEND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DURATION_TO_BLEND_ROW, VALUE_COLUMN, durationToBlend);
    table->setItem(BONES_ROW, NAME_COLUMN, new TableWidgetItem("bones"));
    table->setItem(BONES_ROW, TYPE_COLUMN, new TableWidgetItem("hkbBoneIndexArray", Qt::AlignCenter));
    table->setItem(BONES_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(BONES_ROW, VALUE_COLUMN, bones);
    topLyt->addWidget(table, 0, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(boneIndexUI);
    toggleSignals(true);
}

void RigidBodyRagdollControlsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(hierarchyGain, SIGNAL(editingFinished()), this, SLOT(setHierarchyGain()), Qt::UniqueConnection);
        connect(velocityDamping, SIGNAL(editingFinished()), this, SLOT(setVelocityDamping()), Qt::UniqueConnection);
        connect(accelerationGain, SIGNAL(editingFinished()), this, SLOT(setAccelerationGain()), Qt::UniqueConnection);
        connect(velocityGain, SIGNAL(editingFinished()), this, SLOT(setVelocityGain()), Qt::UniqueConnection);
        connect(positionGain, SIGNAL(editingFinished()), this, SLOT(setPositionGain()), Qt::UniqueConnection);
        connect(positionMaxLinearVelocity, SIGNAL(editingFinished()), this, SLOT(setPositionMaxLinearVelocity()), Qt::UniqueConnection);
        connect(positionMaxAngularVelocity, SIGNAL(editingFinished()), this, SLOT(setPositionMaxAngularVelocity()), Qt::UniqueConnection);
        connect(snapGain, SIGNAL(editingFinished()), this, SLOT(setSnapGain()), Qt::UniqueConnection);
        connect(snapMaxLinearVelocity, SIGNAL(editingFinished()), this, SLOT(setSnapMaxLinearVelocity()), Qt::UniqueConnection);
        connect(snapMaxAngularVelocity, SIGNAL(editingFinished()), this, SLOT(setSnapMaxAngularVelocity()), Qt::UniqueConnection);
        connect(snapMaxLinearDistance, SIGNAL(editingFinished()), this, SLOT(setSnapMaxLinearDistance()), Qt::UniqueConnection);
        connect(snapMaxAngularDistance, SIGNAL(editingFinished()), this, SLOT(setSnapMaxAngularDistance()), Qt::UniqueConnection);
        connect(durationToBlend, SIGNAL(editingFinished()), this, SLOT(setDurationToBlend()), Qt::UniqueConnection);
        connect(bones, SIGNAL(pressed()), this, SLOT(viewBones()), Qt::UniqueConnection);
        connect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(hierarchyGain, SIGNAL(editingFinished()), this, SLOT(setHierarchyGain()));
        disconnect(velocityDamping, SIGNAL(editingFinished()), this, SLOT(setVelocityDamping()));
        disconnect(accelerationGain, SIGNAL(editingFinished()), this, SLOT(setAccelerationGain()));
        disconnect(velocityGain, SIGNAL(editingFinished()), this, SLOT(setVelocityGain()));
        disconnect(positionGain, SIGNAL(editingFinished()), this, SLOT(setPositionGain()));
        disconnect(positionMaxLinearVelocity, SIGNAL(editingFinished()), this, SLOT(setPositionMaxLinearVelocity()));
        disconnect(positionMaxAngularVelocity, SIGNAL(editingFinished()), this, SLOT(setPositionMaxAngularVelocity()));
        disconnect(snapGain, SIGNAL(editingFinished()), this, SLOT(setSnapGain()));
        disconnect(snapMaxLinearVelocity, SIGNAL(editingFinished()), this, SLOT(setSnapMaxLinearVelocity()));
        disconnect(snapMaxAngularVelocity, SIGNAL(editingFinished()), this, SLOT(setSnapMaxAngularVelocity()));
        disconnect(snapMaxLinearDistance, SIGNAL(editingFinished()), this, SLOT(setSnapMaxLinearDistance()));
        disconnect(snapMaxAngularDistance, SIGNAL(editingFinished()), this, SLOT(setSnapMaxAngularDistance()));
        disconnect(durationToBlend, SIGNAL(editingFinished()), this, SLOT(setDurationToBlend()));
        disconnect(bones, SIGNAL(pressed()), this, SLOT(viewBones()));
        disconnect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)));
        disconnect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void RigidBodyRagdollControlsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *ragdollBones){
    if (variables && properties && ragdollBones){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(ragdollBones, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(ragdollBones, SIGNAL(elementSelected(int,QString)), boneIndexUI, SLOT(setRagdollBone(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(viewRagdollBones(int)), ragdollBones, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER){
            bsData = static_cast<hkbRigidBodyRagdollControlsModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            hierarchyGain->setValue(bsData->getHierarchyGain());
            velocityDamping->setValue(bsData->getVelocityDamping());
            accelerationGain->setValue(bsData->getAccelerationGain());
            velocityGain->setValue(bsData->getVelocityGain());
            positionGain->setValue(bsData->getPositionGain());
            positionMaxLinearVelocity->setValue(bsData->getPositionMaxLinearVelocity());
            positionMaxAngularVelocity->setValue(bsData->getPositionMaxAngularVelocity());
            snapGain->setValue(bsData->getSnapGain());
            snapMaxLinearVelocity->setValue(bsData->getSnapMaxLinearVelocity());
            snapMaxAngularVelocity->setValue(bsData->getSnapMaxAngularVelocity());
            snapMaxLinearDistance->setValue(bsData->getSnapMaxLinearDistance());
            snapMaxAngularDistance->setValue(bsData->getSnapMaxAngularDistance());
            durationToBlend->setValue(bsData->getDurationToBlend());
            if (bsData->bones.data()){
                bones->setChecked(true);
                bones->setText("Edit");
            }else{
                bones->setChecked(false);
                bones->setText("nullptr");
            }
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(HIERARCHY_ROW, BINDING_COLUMN, varBind, "hierarchyGain", table, bsData);
            UIHelper::loadBinding(VELOCITY_DAMPING_ROW, BINDING_COLUMN, varBind, "velocityDamping", table, bsData);
            UIHelper::loadBinding(ACCELERATION_GAIN_ROW, BINDING_COLUMN, varBind, "accelerationGain", table, bsData);
            UIHelper::loadBinding(VELOCITY_GAIN_ROW, BINDING_COLUMN, varBind, "velocityGain", table, bsData);
            UIHelper::loadBinding(POSITION_GAIN_ROW, BINDING_COLUMN, varBind, "positionGain", table, bsData);
            UIHelper::loadBinding(POSITION_MAX_LINEAR_VELOCITY_ROW, BINDING_COLUMN, varBind, "positionMaxLinearVelocity", table, bsData);
            UIHelper::loadBinding(POSITION_MAX_ANGULAR_VELOCITY_ROW, BINDING_COLUMN, varBind, "positionMaxAngularVelocity", table, bsData);
            UIHelper::loadBinding(SNAP_GAIN_ROW, BINDING_COLUMN, varBind, "snapGain", table, bsData);
            UIHelper::loadBinding(SNAP_MAX_LINEAR_VELOCITY_ROW, BINDING_COLUMN, varBind, "snapMaxLinearVelocity", table, bsData);
            UIHelper::loadBinding(SNAP_MAX_ANGULAR_VELOCITY_ROW, BINDING_COLUMN, varBind, "snapMaxAngularVelocity", table, bsData);
            UIHelper::loadBinding(SNAP_MAX_LINEAR_DISTANCE_ROW, BINDING_COLUMN, varBind, "snapMaxLinearDistance", table, bsData);
            UIHelper::loadBinding(SNAP_MAX_ANGULAR_DISTANCE_ROW, BINDING_COLUMN, varBind, "snapMaxAngularDistance", table, bsData);
            UIHelper::loadBinding(DURATION_TO_BLEND_ROW, BINDING_COLUMN, varBind, "durationToBlend", table, bsData);
        }else{
            LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void RigidBodyRagdollControlsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setName(): The data is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setHierarchyGain(){
    (bsData) ? bsData->setHierarchyGain(hierarchyGain->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setHierarchyGain(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setVelocityDamping(){
    (bsData) ? bsData->setVelocityDamping(velocityDamping->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setVelocityDamping(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setAccelerationGain(){
    (bsData) ? bsData->setAccelerationGain(accelerationGain->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setAccelerationGain(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setVelocityGain(){
    (bsData) ? bsData->setVelocityGain(velocityGain->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setVelocityGain(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setPositionGain(){
    (bsData) ? bsData->setPositionGain(positionGain->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setPositionGain(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setPositionMaxLinearVelocity(){
    (bsData) ? bsData->setPositionMaxLinearVelocity(positionMaxLinearVelocity->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setPositionMaxLinearVelocity(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setPositionMaxAngularVelocity(){
    (bsData) ? bsData->setPositionMaxAngularVelocity(positionMaxAngularVelocity->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setPositionMaxAngularVelocity(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setSnapGain(){
    (bsData) ? bsData->setSnapGain(snapGain->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setSnapGain(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setSnapMaxLinearVelocity(){
    (bsData) ? bsData->setSnapMaxLinearVelocity(snapMaxLinearVelocity->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setSnapMaxLinearVelocity(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setSnapMaxAngularVelocity(){
    (bsData) ? bsData->setSnapMaxAngularVelocity(snapMaxAngularVelocity->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setSnapMaxAngularVelocity(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setSnapMaxLinearDistance(){
    (bsData) ? bsData->setSnapMaxLinearDistance(snapMaxLinearDistance->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setSnapMaxLinearDistance(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setSnapMaxAngularDistance(){
    (bsData) ? bsData->setSnapMaxAngularDistance(snapMaxAngularDistance->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setSnapMaxAngularDistance(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::setDurationToBlend(){
    (bsData) ? bsData->setDurationToBlend(durationToBlend->value()) : LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setDurationToBlend(): The 'bsData' pointer is nullptr!!");
}

void RigidBodyRagdollControlsModifierUI::toggleBones(bool enable){
    if (bsData){
        if (!enable){
            bsData->setBones(nullptr);
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }else if (!bsData->getBones()){
            bsData->setBones(new hkbBoneIndexArray(bsData->getParentFile()));
            bones->setText("Edit");
        }
        bsData->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::toggleBones(): The data is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::viewBones(){
    if (bsData){
        boneIndexUI->loadData(bsData->getBones());
        setCurrentIndex(CHILD_WIDGET);
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::viewBones(): The data is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case ENABLE_ROW:
                checkisproperty(ENABLE_ROW, "enable"); break;
            case HIERARCHY_ROW:
                checkisproperty(HIERARCHY_ROW, "hierarchyGain"); break;
            case VELOCITY_DAMPING_ROW:
                checkisproperty(VELOCITY_DAMPING_ROW, "velocityDamping"); break;
            case ACCELERATION_GAIN_ROW:
                checkisproperty(ACCELERATION_GAIN_ROW, "accelerationGain"); break;
            case VELOCITY_GAIN_ROW:
                checkisproperty(VELOCITY_GAIN_ROW, "velocityGain"); break;
            case POSITION_GAIN_ROW:
                checkisproperty(POSITION_GAIN_ROW, "positionGain"); break;
            case POSITION_MAX_LINEAR_VELOCITY_ROW:
                checkisproperty(POSITION_MAX_LINEAR_VELOCITY_ROW, "positionMaxLinearVelocity"); break;
            case POSITION_MAX_ANGULAR_VELOCITY_ROW:
                checkisproperty(POSITION_MAX_ANGULAR_VELOCITY_ROW, "positionMaxAngularVelocity"); break;
            case SNAP_GAIN_ROW:
                checkisproperty(SNAP_GAIN_ROW, "snapGain"); break;
            case SNAP_MAX_LINEAR_VELOCITY_ROW:
                checkisproperty(SNAP_MAX_LINEAR_VELOCITY_ROW, "snapMaxLinearVelocity"); break;
            case SNAP_MAX_ANGULAR_VELOCITY_ROW:
                checkisproperty(SNAP_MAX_ANGULAR_VELOCITY_ROW, "snapMaxAngularVelocity"); break;
            case SNAP_MAX_LINEAR_DISTANCE_ROW:
                checkisproperty(SNAP_MAX_LINEAR_DISTANCE_ROW, "snapMaxLinearDistance"); break;
            case SNAP_MAX_ANGULAR_DISTANCE_ROW:
                checkisproperty(SNAP_MAX_ANGULAR_DISTANCE_ROW, "snapMaxAngularDistance"); break;
            case DURATION_TO_BLEND_ROW:
                checkisproperty(DURATION_TO_BLEND_ROW, "durationToBlend"); break;
            }
        }
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::selectTableToView(bool viewisProperty, const QString & path){
    if (bsData){
        if (viewisProperty){
            if (bsData->getVariableBindingSetData()){
                emit viewProperties(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (bsData->getVariableBindingSetData()){
                emit viewVariables(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("hierarchyGain", HIERARCHY_ROW);
            setname("velocityDamping", VELOCITY_DAMPING_ROW);
            setname("accelerationGain", ACCELERATION_GAIN_ROW);
            setname("velocityGain", VELOCITY_GAIN_ROW);
            setname("positionGain", POSITION_GAIN_ROW);
            setname("positionMaxLinearVelocity", POSITION_MAX_LINEAR_VELOCITY_ROW);
            setname("positionMaxAngularVelocity", POSITION_MAX_ANGULAR_VELOCITY_ROW);
            setname("snapGain", SNAP_GAIN_ROW);
            setname("snapMaxLinearVelocity", SNAP_MAX_LINEAR_VELOCITY_ROW);
            setname("snapMaxAngularVelocity", SNAP_MAX_ANGULAR_VELOCITY_ROW);
            setname("snapMaxLinearDistance", SNAP_MAX_LINEAR_DISTANCE_ROW);
            setname("snapMaxAngularDistance", SNAP_MAX_ANGULAR_DISTANCE_ROW);
            setname("durationToBlend", DURATION_TO_BLEND_ROW);
        }
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case ENABLE_ROW:
            checkisproperty(ENABLE_ROW, "enable", VARIABLE_TYPE_BOOL); break;
        case HIERARCHY_ROW:
            checkisproperty(HIERARCHY_ROW, "hierarchyGain", VARIABLE_TYPE_REAL); break;
        case VELOCITY_DAMPING_ROW:
            checkisproperty(VELOCITY_DAMPING_ROW, "velocityDamping", VARIABLE_TYPE_REAL); break;
        case ACCELERATION_GAIN_ROW:
            checkisproperty(ACCELERATION_GAIN_ROW, "accelerationGain", VARIABLE_TYPE_REAL); break;
        case VELOCITY_GAIN_ROW:
            checkisproperty(VELOCITY_GAIN_ROW, "velocityGain", VARIABLE_TYPE_REAL); break;
        case POSITION_GAIN_ROW:
            checkisproperty(POSITION_GAIN_ROW, "positionGain", VARIABLE_TYPE_REAL); break;
        case POSITION_MAX_LINEAR_VELOCITY_ROW:
            checkisproperty(POSITION_MAX_LINEAR_VELOCITY_ROW, "positionMaxLinearVelocity", VARIABLE_TYPE_REAL); break;
        case POSITION_MAX_ANGULAR_VELOCITY_ROW:
            checkisproperty(POSITION_MAX_ANGULAR_VELOCITY_ROW, "positionMaxAngularVelocity", VARIABLE_TYPE_REAL); break;
        case SNAP_GAIN_ROW:
            checkisproperty(SNAP_GAIN_ROW, "snapGain", VARIABLE_TYPE_REAL); break;
        case SNAP_MAX_LINEAR_VELOCITY_ROW:
            checkisproperty(SNAP_MAX_LINEAR_VELOCITY_ROW, "snapMaxLinearVelocity", VARIABLE_TYPE_REAL); break;
        case SNAP_MAX_ANGULAR_VELOCITY_ROW:
            checkisproperty(SNAP_MAX_ANGULAR_VELOCITY_ROW, "snapMaxAngularVelocity", VARIABLE_TYPE_REAL); break;
        case SNAP_MAX_LINEAR_DISTANCE_ROW:
            checkisproperty(SNAP_MAX_LINEAR_DISTANCE_ROW, "snapMaxLinearDistance", VARIABLE_TYPE_REAL); break;
        case SNAP_MAX_ANGULAR_DISTANCE_ROW:
            checkisproperty(SNAP_MAX_ANGULAR_DISTANCE_ROW, "snapMaxAngularDistance", VARIABLE_TYPE_REAL); break;
        case DURATION_TO_BLEND_ROW:
            checkisproperty(DURATION_TO_BLEND_ROW, "durationToBlend", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("RigidBodyRagdollControlsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void RigidBodyRagdollControlsModifierUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}
