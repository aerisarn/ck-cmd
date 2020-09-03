#include "bslookatmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSLookAtModifier.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/bsboneui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/mainwindow.h"

#include <QHeaderView>

#include "src/ui/genericdatawidgets.h"

using namespace UI;

#define BASE_NUMBER_OF_ROWS 19

#define NAME_ROW 0
#define ENABLE_ROW 1
#define LOOK_AT_TARGET_ROW 2
#define LIMIT_ANGLE_DEGREES_ROW 3
#define LIMIT_ANGLE_THRESHOLD_DEGREES_ROW 4
#define CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW 5
#define ON_GAIN_ROW 6
#define OFF_GAIN_ROW 7
#define USE_BONE_GAINS_ROW 8
#define TARGET_LOCATION_ROW 9
#define TARGET_OUTSIDE_LIMITS_ROW 10
#define TARGET_OUT_OF_LIMIT_EVENT_ID_ROW 11
#define TARGET_OUT_OF_LIMIT_EVENT_PAYLOAD_ROW 12
#define LOOK_AT_CAMERA_ROW 13
#define LOOK_AT_CAMERA_X_ROW 14
#define LOOK_AT_CAMERA_Y_ROW 15
#define LOOK_AT_CAMERA_Z_ROW 16
#define ADD_BONE_ROW 17
#define INITIAL_ADD_EYE_BONE_ROW 18

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSLookAtModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSLookAtModifierUI::BSLookAtModifierUI()
    : eyeBoneButtonRow(INITIAL_ADD_EYE_BONE_ROW),
      bsData(nullptr),
      groupBox(new QGroupBox("BSLookAtModifier")),
      topLyt(new QGridLayout),
      boneUI(new BSBoneUI),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      lookAtTarget(new CheckBox),
      limitAngleDegrees(new DoubleSpinBox),
      limitAngleThresholdDegrees(new DoubleSpinBox),
      continueLookOutsideOfLimit(new CheckBox),
      onGain(new DoubleSpinBox),
      offGain(new DoubleSpinBox),
      useBoneGains(new CheckBox),
      targetLocation(new QuadVariableWidget),
      targetOutsideLimits(new CheckBox),
      targetOutOfLimitEventPayload(new LineEdit),
      lookAtCamera(new CheckBox),
      lookAtCameraX(new DoubleSpinBox),
      lookAtCameraY(new DoubleSpinBox),
      lookAtCameraZ(new DoubleSpinBox)
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
    table->setItem(ENABLE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(LOOK_AT_TARGET_ROW, NAME_COLUMN, new TableWidgetItem("lookAtTarget"));
    table->setItem(LOOK_AT_TARGET_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(LOOK_AT_TARGET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(LOOK_AT_TARGET_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(LOOK_AT_TARGET_ROW, VALUE_COLUMN, lookAtTarget);
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleDegrees"));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_DEGREES_ROW, VALUE_COLUMN, limitAngleDegrees);
    table->setItem(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleThresholdDegrees"));
    table->setItem(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, VALUE_COLUMN, limitAngleThresholdDegrees);
    table->setItem(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, NAME_COLUMN, new TableWidgetItem("continueLookOutsideOfLimit"));
    table->setItem(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, VALUE_COLUMN, continueLookOutsideOfLimit);
    table->setItem(ON_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("onGain"));
    table->setItem(ON_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ON_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ON_GAIN_ROW, VALUE_COLUMN, onGain);
    table->setItem(OFF_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("offGain"));
    table->setItem(OFF_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFF_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFF_GAIN_ROW, VALUE_COLUMN, offGain);
    table->setItem(USE_BONE_GAINS_ROW, NAME_COLUMN, new TableWidgetItem("useBoneGains"));
    table->setItem(USE_BONE_GAINS_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(USE_BONE_GAINS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(USE_BONE_GAINS_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(USE_BONE_GAINS_ROW, VALUE_COLUMN, useBoneGains);
    table->setItem(TARGET_LOCATION_ROW, NAME_COLUMN, new TableWidgetItem("targetLocation"));
    table->setItem(TARGET_LOCATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_LOCATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(TARGET_LOCATION_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(TARGET_LOCATION_ROW, VALUE_COLUMN, targetLocation);
    table->setItem(TARGET_OUTSIDE_LIMITS_ROW, NAME_COLUMN, new TableWidgetItem("targetOutsideLimits"));
    table->setItem(TARGET_OUTSIDE_LIMITS_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(TARGET_OUTSIDE_LIMITS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(TARGET_OUTSIDE_LIMITS_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(TARGET_OUTSIDE_LIMITS_ROW, VALUE_COLUMN, targetOutsideLimits);
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("targetOutOfLimitEventId"));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("targetOutOfLimitEventPayload"));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(TARGET_OUT_OF_LIMIT_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(TARGET_OUT_OF_LIMIT_EVENT_PAYLOAD_ROW, VALUE_COLUMN, targetOutOfLimitEventPayload);
    table->setItem(LOOK_AT_CAMERA_ROW, NAME_COLUMN, new TableWidgetItem("lookAtCamera"));
    table->setItem(LOOK_AT_CAMERA_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(LOOK_AT_CAMERA_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOOK_AT_CAMERA_ROW, VALUE_COLUMN, lookAtCamera);
    table->setItem(LOOK_AT_CAMERA_X_ROW, NAME_COLUMN, new TableWidgetItem("lookAtCameraX"));
    table->setItem(LOOK_AT_CAMERA_X_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LOOK_AT_CAMERA_X_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOOK_AT_CAMERA_X_ROW, VALUE_COLUMN, lookAtCameraX);
    table->setItem(LOOK_AT_CAMERA_Y_ROW, NAME_COLUMN, new TableWidgetItem("lookAtCameraY"));
    table->setItem(LOOK_AT_CAMERA_Y_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LOOK_AT_CAMERA_Y_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOOK_AT_CAMERA_Y_ROW, VALUE_COLUMN, lookAtCameraY);
    table->setItem(LOOK_AT_CAMERA_Z_ROW, NAME_COLUMN, new TableWidgetItem("lookAtCameraZ"));
    table->setItem(LOOK_AT_CAMERA_Z_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LOOK_AT_CAMERA_Z_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOOK_AT_CAMERA_Z_ROW, VALUE_COLUMN, lookAtCameraZ);
    table->setItem(ADD_BONE_ROW, NAME_COLUMN, new TableWidgetItem("Add Bone", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new Bone"));
    table->setItem(ADD_BONE_ROW, TYPE_COLUMN, new TableWidgetItem("BsBone", Qt::AlignCenter));
    table->setItem(ADD_BONE_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected Bone"));
    table->setItem(ADD_BONE_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected Bone"));
    table->setItem(INITIAL_ADD_EYE_BONE_ROW, NAME_COLUMN, new TableWidgetItem("Add Eye Bone", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new Eye Bone"));
    table->setItem(INITIAL_ADD_EYE_BONE_ROW, TYPE_COLUMN, new TableWidgetItem("BsBone", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(INITIAL_ADD_EYE_BONE_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Eye Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected Eye Bone"));
    table->setItem(INITIAL_ADD_EYE_BONE_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Eye Bone", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected Eye Bone"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(boneUI);
    toggleSignals(true);
}

void BSLookAtModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(lookAtTarget, SIGNAL(released()), this, SLOT(setLookAtTarget()), Qt::UniqueConnection);
        connect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()), Qt::UniqueConnection);
        connect(limitAngleThresholdDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleThresholdDegrees()), Qt::UniqueConnection);
        connect(continueLookOutsideOfLimit, SIGNAL(released()), this, SLOT(setContinueLookOutsideOfLimit()), Qt::UniqueConnection);
        connect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()), Qt::UniqueConnection);
        connect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()), Qt::UniqueConnection);
        connect(useBoneGains, SIGNAL(released()), this, SLOT(setUseBoneGains()), Qt::UniqueConnection);
        connect(targetLocation, SIGNAL(editingFinished()), this, SLOT(setTargetLocation()), Qt::UniqueConnection);
        connect(targetOutsideLimits, SIGNAL(released()), this, SLOT(setTargetOutsideLimits()), Qt::UniqueConnection);
        connect(targetOutOfLimitEventPayload, SIGNAL(editingFinished()), this, SLOT(setTargetOutOfLimitEventPayload()), Qt::UniqueConnection);
        connect(lookAtCamera, SIGNAL(released()), this, SLOT(setLookAtCamera()), Qt::UniqueConnection);
        connect(lookAtCameraX, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraX()), Qt::UniqueConnection);
        connect(lookAtCameraY, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraY()), Qt::UniqueConnection);
        connect(lookAtCameraZ, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraZ()), Qt::UniqueConnection);
        connect(boneUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(boneUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(boneUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(lookAtTarget, SIGNAL(released()), this, SLOT(setLookAtTarget()));
        disconnect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()));
        disconnect(limitAngleThresholdDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleThresholdDegrees()));
        disconnect(continueLookOutsideOfLimit, SIGNAL(released()), this, SLOT(setContinueLookOutsideOfLimit()));
        disconnect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()));
        disconnect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()));
        disconnect(useBoneGains, SIGNAL(released()), this, SLOT(setUseBoneGains()));
        disconnect(targetLocation, SIGNAL(editingFinished()), this, SLOT(setTargetLocation()));
        disconnect(targetOutsideLimits, SIGNAL(released()), this, SLOT(setTargetOutsideLimits()));
        disconnect(targetOutOfLimitEventPayload, SIGNAL(editingFinished()), this, SLOT(setTargetOutOfLimitEventPayload()));
        disconnect(lookAtCamera, SIGNAL(released()), this, SLOT(setLookAtCamera()));
        disconnect(lookAtCameraX, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraX()));
        disconnect(lookAtCameraY, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraY()));
        disconnect(lookAtCameraZ, SIGNAL(editingFinished()), this, SLOT(setLookAtCameraZ()));
        disconnect(boneUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(boneUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(boneUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void BSLookAtModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data && data->getSignature() == BS_LOOK_AT_MODIFIER){
        bsData = static_cast<BSLookAtModifier *>(data);
        name->setText(bsData->getName());
        enable->setChecked(bsData->getEnable());
        lookAtTarget->setChecked(bsData->getLookAtTarget());
        limitAngleDegrees->setValue(bsData->getLimitAngleDegrees());
        limitAngleThresholdDegrees->setValue(bsData->getLimitAngleThresholdDegrees());
        continueLookOutsideOfLimit->setChecked(bsData->getContinueLookOutsideOfLimit());
        onGain->setValue(bsData->getOnGain());
        offGain->setValue(bsData->getOffGain());
        useBoneGains->setChecked(bsData->getUseBoneGains());
        targetLocation->setValue(bsData->getTargetLocation());
        targetOutsideLimits->setChecked(bsData->getTargetOutsideLimits());
        auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getId());
        (text != "") ? table->item(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
        auto payload = bsData->getPayload();
        (payload) ? targetOutOfLimitEventPayload->setText(payload->getData()) : targetOutOfLimitEventPayload->setText("");
        lookAtCamera->setChecked(bsData->getLookAtCamera());
        lookAtCameraX->setValue(bsData->getLookAtCameraX());
        lookAtCameraY->setValue(bsData->getLookAtCameraY());
        lookAtCameraZ->setValue(bsData->getLookAtCameraZ());
        auto varBind = bsData->getVariableBindingSetData();
        UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
        UIHelper::loadBinding(LOOK_AT_TARGET_ROW, BINDING_COLUMN, varBind, "lookAtTarget", table, bsData);
        UIHelper::loadBinding(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, varBind, "limitAngleDegrees", table, bsData);
        UIHelper::loadBinding(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, BINDING_COLUMN, varBind, "limitAngleThresholdDegrees", table, bsData);
        UIHelper::loadBinding(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, BINDING_COLUMN, varBind, "continueLookOutsideOfLimit", table, bsData);
        UIHelper::loadBinding(ON_GAIN_ROW, BINDING_COLUMN, varBind, "onGain", table, bsData);
        UIHelper::loadBinding(OFF_GAIN_ROW, BINDING_COLUMN, varBind, "offGain", table, bsData);
        UIHelper::loadBinding(USE_BONE_GAINS_ROW, BINDING_COLUMN, varBind, "useBoneGains", table, bsData);
        UIHelper::loadBinding(TARGET_LOCATION_ROW, BINDING_COLUMN, varBind, "targetLocation", table, bsData);
        UIHelper::loadBinding(TARGET_OUTSIDE_LIMITS_ROW, BINDING_COLUMN, varBind, "targetOutsideLimits", table, bsData);
        UIHelper::loadBinding(LOOK_AT_CAMERA_ROW, BINDING_COLUMN, varBind, "lookAtCamera", table, bsData);
        UIHelper::loadBinding(LOOK_AT_CAMERA_X_ROW, BINDING_COLUMN, varBind, "lookAtCameraX", table, bsData);
        UIHelper::loadBinding(LOOK_AT_CAMERA_Y_ROW, BINDING_COLUMN, varBind, "lookAtCameraY", table, bsData);
        UIHelper::loadBinding(LOOK_AT_CAMERA_Z_ROW, BINDING_COLUMN, varBind, "lookAtCameraZ", table, bsData);
        if (boneList.isEmpty()){
            boneList = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::loadData(): The data is nullptr or an incorrect type!!");
    }
    toggleSignals(true);
}

void BSLookAtModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_BONE_ROW + bsData->getNumberOfBones() + 1 - eyeBoneButtonRow;
        if (temp > 0){
            for (auto i = 0; i < temp; i++){
                table->insertRow(eyeBoneButtonRow);
                eyeBoneButtonRow++;
            }
        }else if (temp < 0){
            for (auto i = temp; i < 0; i++){
                table->removeRow(eyeBoneButtonRow - 1);
                eyeBoneButtonRow--;
            }
        }
        auto setupbonerows = [&](int start, int end, const QVector <BSLookAtModifier::BsBone> & bones){
            for (auto i = start, j = 0; i < end, j < bones.size(); i++, j++){
                if (bones.at(j).index >= 0 && bones.at(j).index < boneList.size()){
                    UIHelper::setRowItems(i, boneList.at(bones.at(j).index + 1), "BsBone", "Remove", "Edit", "Double click to remove this bone", "Double click to edit this bone", table);
                }else{
                    LogFile::writeToLog("BSLookAtModifierUI::loadDynamicTableRows(): Bone index out of range!!");
                }
            }
        };
        eyeBoneButtonRow = ADD_BONE_ROW + bsData->getNumberOfBones() + 1;
        setupbonerows(INITIAL_ADD_EYE_BONE_ROW, eyeBoneButtonRow, bsData->bones);
        table->setRowCount(eyeBoneButtonRow + bsData->getNumberOfEyeBones() + 1);
        setupbonerows(eyeBoneButtonRow + 1, table->rowCount(), bsData->eyeBones);
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::setName(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLookAtTarget(){
    (bsData) ? bsData->setLookAtTarget(lookAtTarget->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setLookAtTarget(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLimitAngleDegrees(){
    (bsData) ? bsData->setLimitAngleDegrees(limitAngleDegrees->value()) : LogFile::writeToLog("BSLookAtModifierUI::setLimitAngleDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLimitAngleThresholdDegrees(){
    (bsData) ? bsData->setLimitAngleThresholdDegrees(limitAngleThresholdDegrees->value()) : LogFile::writeToLog("BSLookAtModifierUI::setLimitAngleThresholdDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setContinueLookOutsideOfLimit(){
    (bsData) ? bsData->setContinueLookOutsideOfLimit(continueLookOutsideOfLimit->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setContinueLookOutsideOfLimit(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setOnGain(){
    (bsData) ? bsData->setOnGain(onGain->value()) : LogFile::writeToLog("BSLookAtModifierUI::setOnGain(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setOffGain(){
    (bsData) ? bsData->setOffGain(offGain->value()) : LogFile::writeToLog("BSLookAtModifierUI::setOffGain(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setUseBoneGains(){
    (bsData) ? bsData->setUseBoneGains(useBoneGains->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setUseBoneGains(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setTargetLocation(){
    (bsData) ? bsData->setTargetLocation(targetLocation->value()) : LogFile::writeToLog("BSLookAtModifierUI::setTargetLocation(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setTargetOutsideLimits(){
    (bsData) ? bsData->setTargetOutsideLimits(targetOutsideLimits->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setTargetOutsideLimits(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setTargetOutOfLimitEventId(int index, const QString &name){
    if (bsData){
        bsData->setId(index - 1);
        table->item(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::setTargetOutOfLimitEventId(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::setTargetOutOfLimitEventPayload(){
    if (bsData){
        auto payloaddata = bsData->getPayload();
        if (targetOutOfLimitEventPayload->text() != ""){
            if (payloaddata){
                payloaddata->setData(targetOutOfLimitEventPayload->text());
            }else{
                payloaddata = new hkbStringEventPayload(bsData->getParentFile(), targetOutOfLimitEventPayload->text());
                bsData->setPayload(payloaddata);
            }
        }else{
            bsData->setPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::setTargetOutOfLimitEventPayload(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::setLookAtCamera(){
    (bsData) ? bsData->setLookAtCamera(lookAtCamera->isChecked()) : LogFile::writeToLog("BSLookAtModifierUI::setLookAtCamera(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLookAtCameraX(){
    (bsData) ? bsData->setLookAtCameraX(lookAtCameraX->value()) : LogFile::writeToLog("BSLookAtModifierUI::setLookAtCameraX(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLookAtCameraY(){
    (bsData) ? bsData->setLookAtCameraY(lookAtCameraY->value()) : LogFile::writeToLog("BSLookAtModifierUI::setLookAtCameraY(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setLookAtCameraZ(){
    (bsData) ? bsData->setLookAtCameraZ(lookAtCameraZ->value()) : LogFile::writeToLog("BSLookAtModifierUI::setLookAtCameraZ(): The 'bsData' pointer is nullptr!!");
}

void BSLookAtModifierUI::setBindingVariable(int index, const QString & name){
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
        case LOOK_AT_TARGET_ROW:
            checkisproperty(LOOK_AT_TARGET_ROW, "lookAtTarget", VARIABLE_TYPE_BOOL); break;
        case LIMIT_ANGLE_DEGREES_ROW:
            checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_THRESHOLD_DEGREES_ROW:
            checkisproperty(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, "limitAngleThresholdDegrees", VARIABLE_TYPE_REAL); break;
        case CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW:
            checkisproperty(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, "continueLookOutsideOfLimit", VARIABLE_TYPE_BOOL); break;
        case ON_GAIN_ROW:
            checkisproperty(ON_GAIN_ROW, "onGain", VARIABLE_TYPE_REAL); break;
        case OFF_GAIN_ROW:
            checkisproperty(OFF_GAIN_ROW, "offGain", VARIABLE_TYPE_REAL); break;
        case USE_BONE_GAINS_ROW:
            checkisproperty(USE_BONE_GAINS_ROW, "useBoneGains", VARIABLE_TYPE_REAL); break;
        case TARGET_LOCATION_ROW:
            checkisproperty(TARGET_LOCATION_ROW, "targetLocation", VARIABLE_TYPE_VECTOR4); break;
        case TARGET_OUTSIDE_LIMITS_ROW:
            checkisproperty(TARGET_OUTSIDE_LIMITS_ROW, "targetOutsideLimits", VARIABLE_TYPE_BOOL); break;
        case LOOK_AT_CAMERA_ROW:
            checkisproperty(LOOK_AT_CAMERA_ROW, "lookAtCamera", VARIABLE_TYPE_BOOL); break;
        case LOOK_AT_CAMERA_X_ROW:
            checkisproperty(LOOK_AT_CAMERA_X_ROW, "lookAtCameraX", VARIABLE_TYPE_REAL); break;
        case LOOK_AT_CAMERA_Y_ROW:
            checkisproperty(LOOK_AT_CAMERA_Y_ROW, "lookAtCameraY", VARIABLE_TYPE_REAL); break;
        case LOOK_AT_CAMERA_Z_ROW:
            checkisproperty(LOOK_AT_CAMERA_Z_ROW, "lookAtCameraZ", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
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
        LogFile::writeToLog("BSLookAtModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto result = row - BASE_NUMBER_OF_ROWS + 1;
        auto loadboneui = [&](QVector <BSLookAtModifier::BsBone> & bones, int numbones){
            if (numbones > result && result >= 0 && result < boneList.size()){
                if (column == VALUE_COLUMN){
                    boneUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &bones[result], bsData, result, false);
                    setCurrentIndex(BONE_UI_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the bone \""+boneList.at(bones.at(result).index + 1)+"\"?") == QMessageBox::Yes){
                        removeBone(result);
                    }
                }
            }else{
                LogFile::writeToLog("BSLookAtModifierUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        };
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (row < ADD_BONE_ROW && row >= 0){
            if (column == VALUE_COLUMN && row == TARGET_OUT_OF_LIMIT_EVENT_ID_ROW){
                emit viewEvents(bsData->getId() + 1, QString(), QStringList());
            }else if (column == BINDING_COLUMN){
                switch (row){
                case ENABLE_ROW:
                    checkisproperty(ENABLE_ROW, "enable"); break;
                case LOOK_AT_TARGET_ROW:
                    checkisproperty(LOOK_AT_TARGET_ROW, "lookAtTarget"); break;
                case LIMIT_ANGLE_DEGREES_ROW:
                    checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees"); break;
                case LIMIT_ANGLE_THRESHOLD_DEGREES_ROW:
                    checkisproperty(LIMIT_ANGLE_THRESHOLD_DEGREES_ROW, "limitAngleThresholdDegrees"); break;
                case CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW:
                    checkisproperty(CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW, "continueLookOutsideOfLimit"); break;
                case ON_GAIN_ROW:
                    checkisproperty(ON_GAIN_ROW, "onGain"); break;
                case OFF_GAIN_ROW:
                    checkisproperty(OFF_GAIN_ROW, "offGain"); break;
                case USE_BONE_GAINS_ROW:
                    checkisproperty(USE_BONE_GAINS_ROW, "useBoneGains"); break;
                case TARGET_LOCATION_ROW:
                    checkisproperty(TARGET_LOCATION_ROW, "targetLocation"); break;
                case TARGET_OUTSIDE_LIMITS_ROW:
                    checkisproperty(TARGET_OUTSIDE_LIMITS_ROW, "targetOutsideLimits"); break;
                case LOOK_AT_CAMERA_ROW:
                    checkisproperty(LOOK_AT_CAMERA_ROW, "lookAtCamera"); break;
                case LOOK_AT_CAMERA_X_ROW:
                    checkisproperty(LOOK_AT_CAMERA_X_ROW, "lookAtCameraX"); break;
                case LOOK_AT_CAMERA_Y_ROW:
                    checkisproperty(LOOK_AT_CAMERA_Y_ROW, "lookAtCameraY"); break;
                case LOOK_AT_CAMERA_Z_ROW:
                    checkisproperty(LOOK_AT_CAMERA_Z_ROW, "lookAtCameraZ"); break;
                }
            }
        }else if (row == ADD_BONE_ROW && column == NAME_COLUMN){
            addBone();
        }else if (row == eyeBoneButtonRow && column == NAME_COLUMN){
            addEyeBone();
        }else if (row > ADD_BONE_ROW && row < eyeBoneButtonRow){
            result = row - BASE_NUMBER_OF_ROWS + 1;
            loadboneui(bsData->getBones(), bsData->getNumberOfBones());
        }else if (row > eyeBoneButtonRow && row < table->rowCount()){
            result = row - BASE_NUMBER_OF_ROWS - bsData->getNumberOfBones();
            loadboneui(bsData->getEyeBones(), bsData->getNumberOfEyeBones());
        }
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case BONE_UI_WIDGET:
        boneUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("BSLookAtModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BSLookAtModifierUI::addBone(){
    (bsData) ? bsData->addBone(), loadDynamicTableRows() : LogFile::writeToLog("BSLookAtModifierUI::addBone(): The data is nullptr!!");
}

void BSLookAtModifierUI::removeBone(int index){
    (bsData) ? bsData->removeBone(index), loadDynamicTableRows() : LogFile::writeToLog("BSLookAtModifierUI::removeBone(): The data is nullptr!!");
}

void BSLookAtModifierUI::removeEyeBone(int index){
    (bsData) ? bsData->removeEyeBone(index), loadDynamicTableRows() : LogFile::writeToLog("BSLookAtModifierUI::removeEyeBone(): The data is nullptr!!");
}

void BSLookAtModifierUI::addEyeBone(){
    (bsData) ? bsData->addEyeBone(), loadDynamicTableRows() : LogFile::writeToLog("BSLookAtModifierUI::addEyeBone(): The data is nullptr!!");
}

void BSLookAtModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void BSLookAtModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && events && properties){
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setTargetOutOfLimitEventId(int,QString)), Qt::UniqueConnection);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSLookAtModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("lookAtTarget", LOOK_AT_TARGET_ROW);
            setname("limitAngleDegrees", LIMIT_ANGLE_DEGREES_ROW);
            setname("limitAngleThresholdDegrees", LIMIT_ANGLE_THRESHOLD_DEGREES_ROW);
            setname("continueLookOutsideOfLimit", CONTINUE_LOOK_OUTSIDE_OF_LIMIT_ROW);
            setname("onGain", ON_GAIN_ROW);
            setname("offGain", OFF_GAIN_ROW);
            setname("useBoneGains", USE_BONE_GAINS_ROW);
            setname("targetLocation", TARGET_LOCATION_ROW);
            setname("targetOutsideLimits", TARGET_OUTSIDE_LIMITS_ROW);
            setname("lookAtCamera", LOOK_AT_CAMERA_ROW);
            setname("lookAtCameraX", LOOK_AT_CAMERA_X_ROW);
            setname("lookAtCameraY", LOOK_AT_CAMERA_Y_ROW);
            setname("lookAtCameraZ", LOOK_AT_CAMERA_Z_ROW);
        }
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

void BSLookAtModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (currentIndex() == MAIN_WIDGET){
            (index == bsData->getId()) ? table->item(TARGET_OUT_OF_LIMIT_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
        }
    }else{
        LogFile::writeToLog("BSLookAtModifierUI::eventRenamed(): The data is nullptr!!");
    }
}
