#include "lookatmodifierui.h"


#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbLookAtModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 20

#define NAME_ROW 0
#define ENABLE_ROW 1
#define TARGET_WS_ROW 2
#define HEAD_FORWARD_LS_ROW 3
#define NECK_FORWARD_LS_ROW 4
#define NECK_RIGHT_LS_ROW 5
#define EYE_POSITION_HS_ROW 6
#define NEW_TARGET_GAIN_ROW 7
#define ON_GAIN_ROW 8
#define OFF_GAIN_ROW 9
#define LIMIT_ANGLE_DEGREES_ROW 10
#define LIMIT_ANGLE_LEFT_ROW 11
#define LIMIT_ANGLE_RIGHT_ROW 12
#define LIMIT_ANGLE_UP_ROW 13
#define LIMIT_ANGLE_DOWN_ROW 14
#define HEAD_INDEX_ROW 15
#define NECK_INDEX_ROW 16
#define IS_ON_ROW 17
#define INDIVIDUAL_LIMITS_ON_ROW 18
#define IS_TARGET_INSIDE_LIMIT_CONE_ROW 19

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList LookAtModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

LookAtModifierUI::LookAtModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      targetWS(new QuadVariableWidget),
      headForwardLS(new QuadVariableWidget),
      neckForwardLS(new QuadVariableWidget),
      neckRightLS(new QuadVariableWidget),
      eyePositionHS(new QuadVariableWidget),
      newTargetGain(new DoubleSpinBox),
      onGain(new DoubleSpinBox),
      offGain(new DoubleSpinBox),
      limitAngleDegrees(new DoubleSpinBox),
      limitAngleLeft(new DoubleSpinBox),
      limitAngleRight(new DoubleSpinBox),
      limitAngleUp(new DoubleSpinBox),
      limitAngleDown(new DoubleSpinBox),
      headIndex(new ComboBox),
      neckIndex(new ComboBox),
      isOn(new CheckBox),
      individualLimitsOn(new CheckBox),
      isTargetInsideLimitCone(new CheckBox)
{
    setTitle("hkbLookAtModifier");
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
    table->setItem(TARGET_WS_ROW, NAME_COLUMN, new TableWidgetItem("targetWS"));
    table->setItem(TARGET_WS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_WS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_WS_ROW, VALUE_COLUMN, targetWS);
    table->setItem(HEAD_FORWARD_LS_ROW, NAME_COLUMN, new TableWidgetItem("headForwardLS"));
    table->setItem(HEAD_FORWARD_LS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(HEAD_FORWARD_LS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HEAD_FORWARD_LS_ROW, VALUE_COLUMN, headForwardLS);
    table->setItem(NECK_FORWARD_LS_ROW, NAME_COLUMN, new TableWidgetItem("neckForwardLS"));
    table->setItem(NECK_FORWARD_LS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(NECK_FORWARD_LS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NECK_FORWARD_LS_ROW, VALUE_COLUMN, neckForwardLS);
    table->setItem(NECK_RIGHT_LS_ROW, NAME_COLUMN, new TableWidgetItem("neckRightLS"));
    table->setItem(NECK_RIGHT_LS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(NECK_RIGHT_LS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NECK_RIGHT_LS_ROW, VALUE_COLUMN, neckRightLS);
    table->setItem(EYE_POSITION_HS_ROW, NAME_COLUMN, new TableWidgetItem("eyePositionHS"));
    table->setItem(EYE_POSITION_HS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(EYE_POSITION_HS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(EYE_POSITION_HS_ROW, VALUE_COLUMN, eyePositionHS);
    table->setItem(NEW_TARGET_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("newTargetGain"));
    table->setItem(NEW_TARGET_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(NEW_TARGET_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NEW_TARGET_GAIN_ROW, VALUE_COLUMN, newTargetGain);
    table->setItem(ON_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("onGain"));
    table->setItem(ON_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ON_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ON_GAIN_ROW, VALUE_COLUMN, onGain);
    table->setItem(OFF_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("offGain"));
    table->setItem(OFF_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFF_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFF_GAIN_ROW, VALUE_COLUMN, offGain);
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleDegrees"));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_DEGREES_ROW, VALUE_COLUMN, limitAngleDegrees);
    table->setItem(LIMIT_ANGLE_LEFT_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleLeft"));
    table->setItem(LIMIT_ANGLE_LEFT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_LEFT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_LEFT_ROW, VALUE_COLUMN, limitAngleLeft);
    table->setItem(LIMIT_ANGLE_RIGHT_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleRight"));
    table->setItem(LIMIT_ANGLE_RIGHT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_RIGHT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_RIGHT_ROW, VALUE_COLUMN, limitAngleRight);
    table->setItem(LIMIT_ANGLE_UP_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleUp"));
    table->setItem(LIMIT_ANGLE_UP_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_UP_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_UP_ROW, VALUE_COLUMN, limitAngleUp);
    table->setItem(LIMIT_ANGLE_DOWN_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleDown"));
    table->setItem(LIMIT_ANGLE_DOWN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_DOWN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_DOWN_ROW, VALUE_COLUMN, limitAngleDown);
    table->setItem(HEAD_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("headIndex"));
    table->setItem(HEAD_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(HEAD_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HEAD_INDEX_ROW, VALUE_COLUMN, headIndex);
    table->setItem(NECK_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("neckIndex"));
    table->setItem(NECK_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(NECK_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NECK_INDEX_ROW, VALUE_COLUMN, neckIndex);
    table->setItem(IS_ON_ROW, NAME_COLUMN, new TableWidgetItem("isOn"));
    table->setItem(IS_ON_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ON_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ON_ROW, VALUE_COLUMN, isOn);
    table->setItem(INDIVIDUAL_LIMITS_ON_ROW, NAME_COLUMN, new TableWidgetItem("individualLimitsOn"));
    table->setItem(INDIVIDUAL_LIMITS_ON_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(INDIVIDUAL_LIMITS_ON_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INDIVIDUAL_LIMITS_ON_ROW, VALUE_COLUMN, individualLimitsOn);
    table->setItem(IS_TARGET_INSIDE_LIMIT_CONE_ROW, NAME_COLUMN, new TableWidgetItem("isTargetInsideLimitCone"));
    table->setItem(IS_TARGET_INSIDE_LIMIT_CONE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_TARGET_INSIDE_LIMIT_CONE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_TARGET_INSIDE_LIMIT_CONE_ROW, VALUE_COLUMN, isTargetInsideLimitCone);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void LookAtModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(targetWS, SIGNAL(released()), this, SLOT(setTargetWS()), Qt::UniqueConnection);
        connect(headForwardLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setHeadForwardLS(int)), Qt::UniqueConnection);
        connect(neckForwardLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setNeckForwardLS(int)), Qt::UniqueConnection);
        connect(neckRightLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setNeckRightLS(int)), Qt::UniqueConnection);
        connect(eyePositionHS, SIGNAL(editingFinished()), this, SLOT(setEyePositionHS()), Qt::UniqueConnection);
        connect(newTargetGain, SIGNAL(editingFinished()), this, SLOT(setNewTargetGain()), Qt::UniqueConnection);
        connect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()), Qt::UniqueConnection);
        connect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()), Qt::UniqueConnection);
        connect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()), Qt::UniqueConnection);
        connect(limitAngleLeft, SIGNAL(editingFinished()), this, SLOT(setLimitAngleLeft()), Qt::UniqueConnection);
        connect(limitAngleRight, SIGNAL(editingFinished()), this, SLOT(setLimitAngleRight()), Qt::UniqueConnection);
        connect(limitAngleUp, SIGNAL(editingFinished()), this, SLOT(setLimitAngleUp()), Qt::UniqueConnection);
        connect(limitAngleDown, SIGNAL(released()), this, SLOT(setLimitAngleDown()), Qt::UniqueConnection);
        connect(headIndex, SIGNAL(editingFinished()), this, SLOT(setHeadIndex()), Qt::UniqueConnection);
        connect(neckIndex, SIGNAL(editingFinished()), this, SLOT(setNeckIndex()), Qt::UniqueConnection);
        connect(isOn, SIGNAL(editingFinished()), this, SLOT(setIsOn()), Qt::UniqueConnection);
        connect(individualLimitsOn, SIGNAL(released()), this, SLOT(setIndividualLimitsOn()), Qt::UniqueConnection);
        connect(isTargetInsideLimitCone, SIGNAL(editingFinished()), this, SLOT(setIsTargetInsideLimitCone()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(targetWS, SIGNAL(released()), this, SLOT(setTargetWS()));
        disconnect(headForwardLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setHeadForwardLS(int)));
        disconnect(neckForwardLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setNeckForwardLS(int)));
        disconnect(neckRightLS, SIGNAL(currentIndexChanged(int)), this, SLOT(setNeckRightLS(int)));
        disconnect(eyePositionHS, SIGNAL(editingFinished()), this, SLOT(setEyePositionHS()));
        disconnect(newTargetGain, SIGNAL(editingFinished()), this, SLOT(setNewTargetGain()));
        disconnect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()));
        disconnect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()));
        disconnect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()));
        disconnect(limitAngleLeft, SIGNAL(editingFinished()), this, SLOT(setLimitAngleLeft()));
        disconnect(limitAngleRight, SIGNAL(editingFinished()), this, SLOT(setLimitAngleRight()));
        disconnect(limitAngleUp, SIGNAL(editingFinished()), this, SLOT(setLimitAngleUp()));
        disconnect(limitAngleDown, SIGNAL(released()), this, SLOT(setLimitAngleDown()));
        disconnect(headIndex, SIGNAL(editingFinished()), this, SLOT(setHeadIndex()));
        disconnect(neckIndex, SIGNAL(editingFinished()), this, SLOT(setNeckIndex()));
        disconnect(isOn, SIGNAL(editingFinished()), this, SLOT(setIsOn()));
        disconnect(individualLimitsOn, SIGNAL(released()), this, SLOT(setIndividualLimitsOn()));
        disconnect(isTargetInsideLimitCone, SIGNAL(editingFinished()), this, SLOT(setIsTargetInsideLimitCone()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void LookAtModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("LookAtModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void LookAtModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_LOOK_AT_MODIFIER){
            bsData = static_cast<hkbLookAtModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            targetWS->setValue(bsData->getTargetWS());
            headForwardLS->setValue(bsData->getHeadForwardLS());
            neckForwardLS->setValue(bsData->getNeckForwardLS());
            neckRightLS->setValue(bsData->getNeckRightLS());
            eyePositionHS->setValue(bsData->getEyePositionHS());
            newTargetGain->setValue(bsData->getNewTargetGain());
            onGain->setValue(bsData->getOnGain());
            offGain->setValue(bsData->getOffGain());
            limitAngleDegrees->setValue(bsData->getLimitAngleDegrees());
            limitAngleLeft->setValue(bsData->getLimitAngleLeft());
            limitAngleRight->setValue(bsData->getLimitAngleRight());
            limitAngleUp->setValue(bsData->getLimitAngleUp());
            limitAngleDown->setValue(bsData->getLimitAngleDown());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(headIndex, bsData->getHeadIndex() + 1);
            loadbones(neckIndex, bsData->getNeckIndex() + 1);
            isOn->setChecked(bsData->getIsOn());
            individualLimitsOn->setChecked(bsData->getIndividualLimitsOn());
            isTargetInsideLimitCone->setChecked(bsData->getIsTargetInsideLimitCone());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(TARGET_WS_ROW, BINDING_COLUMN, varBind, "targetWS", table, bsData);
            UIHelper::loadBinding(HEAD_FORWARD_LS_ROW, BINDING_COLUMN, varBind, "headForwardLS", table, bsData);
            UIHelper::loadBinding(NECK_FORWARD_LS_ROW, BINDING_COLUMN, varBind, "neckForwardLS", table, bsData);
            UIHelper::loadBinding(NECK_RIGHT_LS_ROW, BINDING_COLUMN, varBind, "neckRightLS", table, bsData);
            UIHelper::loadBinding(EYE_POSITION_HS_ROW, BINDING_COLUMN, varBind, "eyePositionHS", table, bsData);
            UIHelper::loadBinding(NEW_TARGET_GAIN_ROW, BINDING_COLUMN, varBind, "newTargetGain", table, bsData);
            UIHelper::loadBinding(ON_GAIN_ROW, BINDING_COLUMN, varBind, "onGain", table, bsData);
            UIHelper::loadBinding(OFF_GAIN_ROW, BINDING_COLUMN, varBind, "offGain", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, varBind, "limitAngleDegrees", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_LEFT_ROW, BINDING_COLUMN, varBind, "limitAngleLeft", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_RIGHT_ROW, BINDING_COLUMN, varBind, "limitAngleRight", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_UP_ROW, BINDING_COLUMN, varBind, "limitAngleUp", table, bsData);
            UIHelper::loadBinding(LIMIT_ANGLE_DOWN_ROW, BINDING_COLUMN, varBind, "limitAngleDown", table, bsData);
            UIHelper::loadBinding(HEAD_INDEX_ROW, BINDING_COLUMN, varBind, "headIndex", table, bsData);
            UIHelper::loadBinding(NECK_INDEX_ROW, BINDING_COLUMN, varBind, "neckIndex", table, bsData);
            UIHelper::loadBinding(IS_ON_ROW, BINDING_COLUMN, varBind, "isOn", table, bsData);
            UIHelper::loadBinding(INDIVIDUAL_LIMITS_ON_ROW, BINDING_COLUMN, varBind, "individualLimitsOn", table, bsData);
            UIHelper::loadBinding(IS_TARGET_INSIDE_LIMIT_CONE_ROW, BINDING_COLUMN, varBind, "isTargetInsideLimitCone", table, bsData);
        }else{
            LogFile::writeToLog("LookAtModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("LookAtModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void LookAtModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("LookAtModifierUI::setName(): The data is nullptr!!");
    }
}

void LookAtModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("LookAtModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setTargetWS(){
    (bsData) ? bsData->setTargetWS(targetWS->value()) : LogFile::writeToLog("LookAtModifierUI::setTargetWS(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setHeadForwardLS(){
    (bsData) ? bsData->setHeadForwardLS(headForwardLS->value()) : LogFile::writeToLog("LookAtModifierUI::setHeadForwardLS(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setNeckForwardLS(){
    (bsData) ? bsData->setNeckForwardLS(neckForwardLS->value()) : LogFile::writeToLog("LookAtModifierUI::setNeckForwardLS(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setNeckRightLS(){
    (bsData) ? bsData->setNeckRightLS(neckRightLS->value()) : LogFile::writeToLog("LookAtModifierUI::setNeckRightLS(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setEyePositionHS(){
    (bsData) ? bsData->setEyePositionHS(eyePositionHS->value()) : LogFile::writeToLog("LookAtModifierUI::setEyePositionHS(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setNewTargetGain(){
    (bsData) ? bsData->setNewTargetGain(newTargetGain->value()) : LogFile::writeToLog("LookAtModifierUI::setNewTargetGain(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setOnGain(){
    (bsData) ? bsData->setOnGain(onGain->value()) : LogFile::writeToLog("LookAtModifierUI::setOnGain(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setOffGain(){
    (bsData) ? bsData->setOffGain(offGain->value()) : LogFile::writeToLog("LookAtModifierUI::setOffGain(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setLimitAngleDegrees(){
    (bsData) ? bsData->setLimitAngleDegrees(limitAngleDegrees->value()) : LogFile::writeToLog("LookAtModifierUI::setLimitAngleDegrees(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setLimitAngleLeft(){
    (bsData) ? bsData->setLimitAngleLeft(limitAngleLeft->value()) : LogFile::writeToLog("LookAtModifierUI::setLimitAngleLeft(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setLimitAngleRight(){
    (bsData) ? bsData->setLimitAngleRight(limitAngleRight->value()) : LogFile::writeToLog("LookAtModifierUI::setLimitAngleRight(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setLimitAngleUp(){
    (bsData) ? bsData->setLimitAngleUp(limitAngleUp->value()) : LogFile::writeToLog("LookAtModifierUI::setLimitAngleUp(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setLimitAngleDown(){
    (bsData) ? bsData->setLimitAngleDown(limitAngleDown->value()) : LogFile::writeToLog("LookAtModifierUI::setLimitAngleDown(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setHeadIndex(int index){
    (bsData) ? bsData->setHeadIndex(index - 1) : LogFile::writeToLog("LookAtModifierUI::setHeadIndex(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setNeckIndex(int index){
    (bsData) ? bsData->setNeckIndex(index - 1) : LogFile::writeToLog("LookAtModifierUI::setNeckIndex(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setIsOn(){
    (bsData) ? bsData->setIsOn(isOn->isChecked()) : LogFile::writeToLog("LookAtModifierUI::setIsOn(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setIndividualLimitsOn(){
    (bsData) ? bsData->setIndividualLimitsOn(individualLimitsOn->isChecked()) : LogFile::writeToLog("LookAtModifierUI::setIndividualLimitsOn(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::setIsTargetInsideLimitCone(){
    (bsData) ? bsData->setIsTargetInsideLimitCone(isTargetInsideLimitCone->isChecked()) : LogFile::writeToLog("LookAtModifierUI::setIsTargetInsideLimitCone(): The 'bsData' pointer is nullptr!!");
}

void LookAtModifierUI::viewSelected(int row, int column){
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
            case TARGET_WS_ROW:
                checkisproperty(TARGET_WS_ROW, "targetWS"); break;
            case HEAD_FORWARD_LS_ROW:
                checkisproperty(HEAD_FORWARD_LS_ROW, "headForwardLS"); break;
            case NECK_FORWARD_LS_ROW:
                checkisproperty(NECK_FORWARD_LS_ROW, "neckForwardLS"); break;
            case NECK_RIGHT_LS_ROW:
                checkisproperty(NECK_RIGHT_LS_ROW, "neckRightLS"); break;
            case EYE_POSITION_HS_ROW:
                checkisproperty(EYE_POSITION_HS_ROW, "eyePositionHS"); break;
            case NEW_TARGET_GAIN_ROW:
                checkisproperty(NEW_TARGET_GAIN_ROW, "newTargetGain"); break;
            case ON_GAIN_ROW:
                checkisproperty(ON_GAIN_ROW, "onGain"); break;
            case OFF_GAIN_ROW:
                checkisproperty(OFF_GAIN_ROW, "offGain"); break;
            case LIMIT_ANGLE_DEGREES_ROW:
                checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees"); break;
            case LIMIT_ANGLE_LEFT_ROW:
                checkisproperty(LIMIT_ANGLE_LEFT_ROW, "limitAngleLeft"); break;
            case LIMIT_ANGLE_RIGHT_ROW:
                checkisproperty(LIMIT_ANGLE_RIGHT_ROW, "limitAngleRight"); break;
            case LIMIT_ANGLE_UP_ROW:
                checkisproperty(LIMIT_ANGLE_UP_ROW, "limitAngleUp"); break;
            case LIMIT_ANGLE_DOWN_ROW:
                checkisproperty(LIMIT_ANGLE_DOWN_ROW, "limitAngleDown"); break;
            case HEAD_INDEX_ROW:
                checkisproperty(HEAD_INDEX_ROW, "headIndex"); break;
            case NECK_INDEX_ROW:
                checkisproperty(NECK_INDEX_ROW, "neckIndex"); break;
            case IS_ON_ROW:
                checkisproperty(IS_ON_ROW, "isOn"); break;
            case INDIVIDUAL_LIMITS_ON_ROW:
                checkisproperty(INDIVIDUAL_LIMITS_ON_ROW, "individualLimitsOn"); break;
            case IS_TARGET_INSIDE_LIMIT_CONE_ROW:
                checkisproperty(IS_TARGET_INSIDE_LIMIT_CONE_ROW, "isTargetInsideLimitCone"); break;
            }
        }
    }else{
        LogFile::writeToLog("LookAtModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void LookAtModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("LookAtModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void LookAtModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("targetWS", TARGET_WS_ROW);
            setname("headForwardLS", HEAD_FORWARD_LS_ROW);
            setname("neckForwardLS", NECK_FORWARD_LS_ROW);
            setname("neckRightLS", NECK_RIGHT_LS_ROW);
            setname("eyePositionHS", EYE_POSITION_HS_ROW);
            setname("newTargetGain", NEW_TARGET_GAIN_ROW);
            setname("onGain", ON_GAIN_ROW);
            setname("offGain", OFF_GAIN_ROW);
            setname("limitAngleDegrees", LIMIT_ANGLE_DEGREES_ROW);
            setname("limitAngleLeft", LIMIT_ANGLE_LEFT_ROW);
            setname("limitAngleRight", LIMIT_ANGLE_RIGHT_ROW);
            setname("limitAngleUp", LIMIT_ANGLE_UP_ROW);
            setname("limitAngleDown", LIMIT_ANGLE_DOWN_ROW);
            setname("headIndex", HEAD_INDEX_ROW);
            setname("neckIndex", NECK_INDEX_ROW);
            setname("isOn", IS_ON_ROW);
            setname("individualLimitsOn", INDIVIDUAL_LIMITS_ON_ROW);
            setname("isTargetInsideLimitCone", IS_TARGET_INSIDE_LIMIT_CONE_ROW);
        }
    }else{
        LogFile::writeToLog("LookAtModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void LookAtModifierUI::setBindingVariable(int index, const QString &name){
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
        case TARGET_WS_ROW:
            checkisproperty(TARGET_WS_ROW, "targetWS", VARIABLE_TYPE_VECTOR4); break;
        case HEAD_FORWARD_LS_ROW:
            checkisproperty(HEAD_FORWARD_LS_ROW, "headForwardLS", VARIABLE_TYPE_VECTOR4); break;
        case NECK_FORWARD_LS_ROW:
            checkisproperty(NECK_FORWARD_LS_ROW, "neckForwardLS", VARIABLE_TYPE_VECTOR4); break;
        case NECK_RIGHT_LS_ROW:
            checkisproperty(NECK_RIGHT_LS_ROW, "neckRightLS", VARIABLE_TYPE_VECTOR4); break;
        case EYE_POSITION_HS_ROW:
            checkisproperty(EYE_POSITION_HS_ROW, "eyePositionHS", VARIABLE_TYPE_VECTOR4); break;
        case NEW_TARGET_GAIN_ROW:
            checkisproperty(NEW_TARGET_GAIN_ROW, "newTargetGain", VARIABLE_TYPE_REAL); break;
        case ON_GAIN_ROW:
            checkisproperty(ON_GAIN_ROW, "onGain", VARIABLE_TYPE_REAL); break;
        case OFF_GAIN_ROW:
            checkisproperty(OFF_GAIN_ROW, "offGain", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_DEGREES_ROW:
            checkisproperty(LIMIT_ANGLE_DEGREES_ROW, "limitAngleDegrees", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_LEFT_ROW:
            checkisproperty(LIMIT_ANGLE_LEFT_ROW, "limitAngleLeft", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_RIGHT_ROW:
            checkisproperty(LIMIT_ANGLE_RIGHT_ROW, "limitAngleRight", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_UP_ROW:
            checkisproperty(LIMIT_ANGLE_UP_ROW, "limitAngleUp", VARIABLE_TYPE_REAL); break;
        case LIMIT_ANGLE_DOWN_ROW:
            checkisproperty(LIMIT_ANGLE_DOWN_ROW, "limitAngleDown", VARIABLE_TYPE_REAL); break;
        case HEAD_INDEX_ROW:
            checkisproperty(HEAD_INDEX_ROW, "headIndex", VARIABLE_TYPE_INT32); break;
        case NECK_INDEX_ROW:
            checkisproperty(NECK_INDEX_ROW, "neckIndex", VARIABLE_TYPE_INT32); break;
        case IS_ON_ROW:
            checkisproperty(IS_ON_ROW, "isOn", VARIABLE_TYPE_BOOL); break;
        case INDIVIDUAL_LIMITS_ON_ROW:
            checkisproperty(INDIVIDUAL_LIMITS_ON_ROW, "individualLimitsOn", VARIABLE_TYPE_BOOL); break;
        case IS_TARGET_INSIDE_LIMIT_CONE_ROW:
            checkisproperty(IS_TARGET_INSIDE_LIMIT_CONE_ROW, "isTargetInsideLimitCone", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("LookAtModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
