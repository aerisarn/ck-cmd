#include "handui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 14

#define TARGET_POSITION_ROW 0
#define TARGET_ROTATION_ROW 1
#define TARGET_NORMAL_ROW 2
#define TARGET_HANDLE_ROW 3
#define TRANSFORM_ON_FRACTION_ROW 4
#define NORMAL_ON_FRACTION_ROW 5
#define FADE_IN_DURATION_ROW 6
#define FADE_OUT_DURATION_ROW 7
#define EXTRAPOLATION_TIME_STEP_ROW 8
#define HANDLE_CHANGE_SPEED_ROW 9
#define HANDLE_CHANGE_MODE_ROW 10
#define FIXUP_ROW 11
#define HAND_INDEX_ROW 12
#define ENABLE_ROW 13

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList HandUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

HandUI::HandUI()
    : file(nullptr),
      bsBoneIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      targetPosition(new QuadVariableWidget),
      targetRotation(new QuadVariableWidget),
      targetNormal(new QuadVariableWidget),
      transformOnFraction(new DoubleSpinBox),
      normalOnFraction(new DoubleSpinBox),
      fadeInDuration(new DoubleSpinBox),
      fadeOutDuration(new DoubleSpinBox),
      extrapolationTimeStep(new DoubleSpinBox),
      handleChangeSpeed(new DoubleSpinBox),
      handleChangeMode(new ComboBox),
      fixUp(new CheckBox),
      handIndex(new ComboBox),
      enable(new CheckBox)
{
    setTitle("HandUI");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(HAND_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("index"));
    table->setItem(HAND_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(HAND_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HAND_INDEX_ROW, VALUE_COLUMN, handIndex);
    table->setItem(TARGET_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("targetPosition"));
    table->setItem(TARGET_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_POSITION_ROW, VALUE_COLUMN, targetPosition);
    table->setItem(TARGET_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("targetRotation"));
    table->setItem(TARGET_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_ROTATION_ROW, VALUE_COLUMN, targetRotation);
    table->setItem(TARGET_NORMAL_ROW, NAME_COLUMN, new TableWidgetItem("targetNormal"));
    table->setItem(TARGET_NORMAL_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_NORMAL_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_NORMAL_ROW, VALUE_COLUMN, targetNormal);
    table->setItem(TARGET_HANDLE_ROW, NAME_COLUMN, new TableWidgetItem("targetHandle"));
    table->setItem(TARGET_HANDLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkHandle", Qt::AlignCenter));
    table->setItem(TARGET_HANDLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(TARGET_HANDLE_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(TRANSFORM_ON_FRACTION_ROW, NAME_COLUMN, new TableWidgetItem("transformOnFraction"));
    table->setItem(TRANSFORM_ON_FRACTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TRANSFORM_ON_FRACTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSFORM_ON_FRACTION_ROW, VALUE_COLUMN, transformOnFraction);
    table->setItem(NORMAL_ON_FRACTION_ROW, NAME_COLUMN, new TableWidgetItem("normalOnFraction"));
    table->setItem(NORMAL_ON_FRACTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(NORMAL_ON_FRACTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NORMAL_ON_FRACTION_ROW, VALUE_COLUMN, normalOnFraction);
    table->setItem(FADE_IN_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("fadeInDuration"));
    table->setItem(FADE_IN_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FADE_IN_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FADE_IN_DURATION_ROW, VALUE_COLUMN, fadeInDuration);
    table->setItem(FADE_OUT_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("fadeOutDuration"));
    table->setItem(FADE_OUT_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FADE_OUT_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FADE_OUT_DURATION_ROW, VALUE_COLUMN, fadeOutDuration);
    table->setItem(EXTRAPOLATION_TIME_STEP_ROW, NAME_COLUMN, new TableWidgetItem("extrapolationTimeStep"));
    table->setItem(EXTRAPOLATION_TIME_STEP_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(EXTRAPOLATION_TIME_STEP_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(EXTRAPOLATION_TIME_STEP_ROW, VALUE_COLUMN, extrapolationTimeStep);
    table->setItem(HANDLE_CHANGE_SPEED_ROW, NAME_COLUMN, new TableWidgetItem("handleChangeSpeed"));
    table->setItem(HANDLE_CHANGE_SPEED_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(HANDLE_CHANGE_SPEED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HANDLE_CHANGE_SPEED_ROW, VALUE_COLUMN, handleChangeSpeed);
    table->setItem(HANDLE_CHANGE_MODE_ROW, NAME_COLUMN, new TableWidgetItem("handleChangeMode"));
    table->setItem(HANDLE_CHANGE_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("HandleChangeMode", Qt::AlignCenter));
    table->setItem(HANDLE_CHANGE_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(HANDLE_CHANGE_MODE_ROW, VALUE_COLUMN, handleChangeMode);
    table->setItem(FIXUP_ROW, NAME_COLUMN, new TableWidgetItem("fixUp"));
    table->setItem(FIXUP_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(FIXUP_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FIXUP_ROW, VALUE_COLUMN, fixUp);
    table->setItem(HAND_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("handIndex"));
    table->setItem(HAND_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(HAND_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HAND_INDEX_ROW, VALUE_COLUMN, handIndex);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void HandUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()), Qt::UniqueConnection);
        connect(targetRotation, SIGNAL(editingFinished()), this, SLOT(setTargetRotation()), Qt::UniqueConnection);
        connect(targetNormal, SIGNAL(editingFinished()), this, SLOT(setTargetNormal()), Qt::UniqueConnection);
        connect(transformOnFraction, SIGNAL(editingFinished()), this, SLOT(setTransformOnFraction()), Qt::UniqueConnection);
        connect(normalOnFraction, SIGNAL(editingFinished()), this, SLOT(setNormalOnFraction()), Qt::UniqueConnection);
        connect(fadeInDuration, SIGNAL(editingFinished()), this, SLOT(setFadeInDuration()), Qt::UniqueConnection);
        connect(fadeOutDuration, SIGNAL(editingFinished()), this, SLOT(setFadeOutDuration()), Qt::UniqueConnection);
        connect(extrapolationTimeStep, SIGNAL(editingFinished()), this, SLOT(setExtrapolationTimeStep()), Qt::UniqueConnection);
        connect(handleChangeSpeed, SIGNAL(editingFinished()), this, SLOT(setHandleChangeSpeed()), Qt::UniqueConnection);
        connect(handleChangeMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setHandleChangeMode(QString)), Qt::UniqueConnection);
        connect(fixUp, SIGNAL(released()), this, SLOT(setFixUp()), Qt::UniqueConnection);
        connect(handIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setHandIndex(int)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()));
        disconnect(targetRotation, SIGNAL(editingFinished()), this, SLOT(setTargetRotation()));
        disconnect(targetNormal, SIGNAL(editingFinished()), this, SLOT(setTargetNormal()));
        disconnect(transformOnFraction, SIGNAL(editingFinished()), this, SLOT(setTransformOnFraction()));
        disconnect(normalOnFraction, SIGNAL(editingFinished()), this, SLOT(setNormalOnFraction()));
        disconnect(fadeInDuration, SIGNAL(editingFinished()), this, SLOT(setFadeInDuration()));
        disconnect(fadeOutDuration, SIGNAL(editingFinished()), this, SLOT(setFadeOutDuration()));
        disconnect(extrapolationTimeStep, SIGNAL(editingFinished()), this, SLOT(setExtrapolationTimeStep()));
        disconnect(handleChangeSpeed, SIGNAL(editingFinished()), this, SLOT(setHandleChangeSpeed()));
        disconnect(handleChangeMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(setHandleChangeMode(QString)));
        disconnect(fixUp, SIGNAL(released()), this, SLOT(setFixUp()));
        disconnect(handIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setHandIndex(int)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void HandUI::loadData(BehaviorFile *parentFile, hkbHandIkControlsModifier::hkHand *bon, hkbHandIkControlsModifier *par, int ind){
    toggleSignals(false);
    if (parentFile && bon && par && ind > -1){
        parent = par;
        bsBoneIndex = ind;
        file = parentFile;
        bsData = bon;
        targetRotation->setValue(bsData->controlData.targetRotation);
        targetPosition->setValue(bsData->controlData.targetPosition);
        targetNormal->setValue(bsData->controlData.targetNormal);
        transformOnFraction->setValue(bsData->controlData.transformOnFraction);
        normalOnFraction->setValue(bsData->controlData.normalOnFraction);
        fadeInDuration->setValue(bsData->controlData.fadeInDuration);
        fadeOutDuration->setValue(bsData->controlData.fadeOutDuration);
        extrapolationTimeStep->setValue(bsData->controlData.extrapolationTimeStep);
        handleChangeSpeed->setValue(bsData->controlData.handleChangeSpeed);
        if (!handleChangeMode->count()){
            handleChangeMode->insertItems(0, parent->HandleChangeMode);
        }
        handleChangeMode->setCurrentIndex(parent->HandleChangeMode.indexOf(bsData->controlData.handleChangeMode));
        fixUp->setChecked(bsData->controlData.fixUp);
        if (!handIndex->count()){
            auto boneNames = QStringList("None") + file->getRigBoneNames();
            handIndex->insertItems(0, boneNames);
        }
        handIndex->setCurrentIndex(bsData->handIndex + 1);
        enable->setChecked(bsData->enable);
        auto varBind = parent->getVariableBindingSetData();
        UIHelper::loadBinding(TARGET_POSITION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/targetPosition", table, parent);
        UIHelper::loadBinding(TARGET_ROTATION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/boneIndex", table, parent);
        UIHelper::loadBinding(TARGET_NORMAL_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/targetNormal", table, parent);
        UIHelper::loadBinding(TRANSFORM_ON_FRACTION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/transformOnFraction", table, parent);
        UIHelper::loadBinding(NORMAL_ON_FRACTION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/normalOnFraction", table, parent);
        UIHelper::loadBinding(FADE_IN_DURATION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/fadeInDuration", table, parent);
        UIHelper::loadBinding(FADE_OUT_DURATION_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/fadeOutDuration", table, parent);
        UIHelper::loadBinding(EXTRAPOLATION_TIME_STEP_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/extrapolationTimeStep", table, parent);
        UIHelper::loadBinding(HANDLE_CHANGE_SPEED_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/handleChangeSpeed", table, parent);
        UIHelper::loadBinding(FIXUP_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/fixUp", table, parent);
        UIHelper::loadBinding(HAND_INDEX_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/handIndex", table, parent);
        UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "hands:"+QString::number(bsBoneIndex)+"/enable", table, parent);
    }else{
        LogFile::writeToLog("HandUI::loadData(): Behavior file, parent or data is null!!!");
    }
    toggleSignals(true);
}

void HandUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, parent);
        };
        switch (row){
        case TARGET_POSITION_ROW:
            checkisproperty(TARGET_POSITION_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetPosition", VARIABLE_TYPE_VECTOR4); break;
        case TARGET_ROTATION_ROW:
            checkisproperty(TARGET_ROTATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetRotation", VARIABLE_TYPE_QUATERNION); break;
        case TARGET_NORMAL_ROW:
            checkisproperty(TARGET_NORMAL_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetNormal", VARIABLE_TYPE_VECTOR4); break;
        case TRANSFORM_ON_FRACTION_ROW:
            checkisproperty(TRANSFORM_ON_FRACTION_ROW, "hands:"+QString::number(bsBoneIndex)+"/transformOnFraction", VARIABLE_TYPE_REAL); break;
        case NORMAL_ON_FRACTION_ROW:
            checkisproperty(NORMAL_ON_FRACTION_ROW, "hands:"+QString::number(bsBoneIndex)+"/normalOnFraction", VARIABLE_TYPE_REAL); break;
        case FADE_IN_DURATION_ROW:
            checkisproperty(FADE_IN_DURATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/fadeInDuration", VARIABLE_TYPE_REAL); break;
        case FADE_OUT_DURATION_ROW:
            checkisproperty(FADE_OUT_DURATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/fadeOutDuration", VARIABLE_TYPE_REAL); break;
        case EXTRAPOLATION_TIME_STEP_ROW:
            checkisproperty(EXTRAPOLATION_TIME_STEP_ROW, "hands:"+QString::number(bsBoneIndex)+"/extrapolationTimeStep", VARIABLE_TYPE_REAL); break;
        case HANDLE_CHANGE_SPEED_ROW:
            checkisproperty(HANDLE_CHANGE_SPEED_ROW, "hands:"+QString::number(bsBoneIndex)+"/handleChangeSpeed", VARIABLE_TYPE_REAL); break;
        case FIXUP_ROW:
            checkisproperty(FIXUP_ROW, "hands:"+QString::number(bsBoneIndex)+"/fixUp", VARIABLE_TYPE_BOOL); break;
        case HAND_INDEX_ROW:
            checkisproperty(HAND_INDEX_ROW, "hands:"+QString::number(bsBoneIndex)+"/handIndex", VARIABLE_TYPE_INT32); break;
        case ENABLE_ROW:
            checkisproperty(ENABLE_ROW, "hands:"+QString::number(bsBoneIndex)+"/enable", VARIABLE_TYPE_BOOL); break;
        default:
            return;
        }
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("HandUI::setBindingVariable(): The data is nullptr!!");
    }
}

void HandUI::setTargetPosition(){
    if (bsData && file){
        (bsData->controlData.targetPosition != targetPosition->value()) ? bsData->controlData.targetPosition = targetPosition->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::settargetPosition(): targetPosition not set!!");
    }else{
        LogFile::writeToLog("HandUI::settargetPosition(): Behavior file or data is null!!!");
    }
}

void HandUI::setTargetRotation(){
    if (bsData && file){
        (bsData->controlData.targetRotation != targetRotation->value()) ? bsData->controlData.targetRotation = targetRotation->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::settargetRotation(): targetRotation not set!!");
    }else{
        LogFile::writeToLog("HandUI::settargetRotation(): Behavior file or data is null!!!");
    }
}

void HandUI::setTargetNormal(){
    if (bsData && file){
        (bsData->controlData.targetNormal != targetNormal->value()) ? bsData->controlData.targetNormal = targetNormal->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::settargetNormal(): targetNormal not set!!");
    }else{
        LogFile::writeToLog("HandUI::settargetNormal(): Behavior file or data is null!!!");
    }
}

void HandUI::setTransformOnFraction(){
    if (bsData && file){
        (bsData->controlData.transformOnFraction != transformOnFraction->value()) ? bsData->controlData.transformOnFraction = transformOnFraction->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::settransformOnFraction(): transformOnFraction not set!!");
    }else{
        LogFile::writeToLog("HandUI::settransformOnFraction(): Behavior file or data is null!!!");
    }
}

void HandUI::setNormalOnFraction(){
    if (bsData && file){
        (bsData->controlData.normalOnFraction != normalOnFraction->value()) ? bsData->controlData.normalOnFraction = normalOnFraction->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setnormalOnFraction(): normalOnFraction not set!!");
    }else{
        LogFile::writeToLog("HandUI::setnormalOnFraction(): Behavior file or data is null!!!");
    }
}

void HandUI::setFadeInDuration(){
    if (bsData && file){
        (bsData->controlData.fadeInDuration != fadeInDuration->value()) ? bsData->controlData.fadeInDuration = fadeInDuration->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setfadeInDuration(): fadeInDuration not set!!");
    }else{
        LogFile::writeToLog("HandUI::setFadeInDuration(): Behavior file or data is null!!!");
    }
}

void HandUI::setFadeOutDuration(){
    if (bsData && file){
        (bsData->controlData.fadeOutDuration != fadeOutDuration->value()) ? bsData->controlData.fadeOutDuration = fadeOutDuration->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setfadeOutDuration(): fadeOutDuration not set!!");
    }else{
        LogFile::writeToLog("HandUI::setFadeOutDuration(): Behavior file or data is null!!!");
    }
}

void HandUI::setExtrapolationTimeStep(){
    if (bsData && file){
        (bsData->controlData.extrapolationTimeStep != extrapolationTimeStep->value()) ? bsData->controlData.extrapolationTimeStep = extrapolationTimeStep->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setextrapolationTimeStep(): extrapolationTimeStep not set!!");
    }else{
        LogFile::writeToLog("HandUI::setExtrapolationTimeStep(): Behavior file or data is null!!!");
    }
}

void HandUI::setHandleChangeSpeed(){
    if (bsData && file){
        (bsData->controlData.handleChangeSpeed != handleChangeSpeed->value()) ? bsData->controlData.handleChangeSpeed = handleChangeSpeed->value(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::sethandleChangeSpeed(): handleChangeSpeed not set!!");
    }else{
        LogFile::writeToLog("HandUI::setHandleChangeSpeed(): Behavior file or data is null!!!");
    }
}

void HandUI::setHandleChangeMode(const QString &mode){
    (bsData && file && bsData->controlData.handleChangeMode != mode) ? bsData->controlData.handleChangeMode = mode, file->setIsChanged(true) : LogFile::writeToLog("HandUI::setHandleChangeSpeed(): handleChangeMode was not set!!!");
}

void HandUI::setFixUp(){
    if (bsData && file){
        (bsData->controlData.fixUp != fixUp->isChecked()) ? bsData->controlData.fixUp = fixUp->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setfixUp(): fixUp not set!!");
    }else{
        LogFile::writeToLog("HandUI::setFixUp(): Behavior file or data is null!!!");
    }
}

void HandUI::setHandIndex(int index){
    auto boneindex = --index;
    (bsData && file && boneindex != bsData->handIndex) ? bsData->handIndex = boneindex, file->setIsChanged(true) : LogFile::writeToLog("HandUI::setindex(): handIndex was not set!!!");
}

void HandUI::setEnable(){
    (bsData && file && bsData->enable != enable->isChecked()) ? bsData->enable = enable->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("HandUI::setEnable(): Behavior file or data is null!!!");
}

void HandUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case TARGET_POSITION_ROW:
                checkisproperty(TARGET_POSITION_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetPosition"); break;
            case TARGET_ROTATION_ROW:
                checkisproperty(TARGET_ROTATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetRotation"); break;
            case TARGET_NORMAL_ROW:
                checkisproperty(TARGET_NORMAL_ROW, "hands:"+QString::number(bsBoneIndex)+"/targetNormal"); break;
            case TRANSFORM_ON_FRACTION_ROW:
                checkisproperty(TRANSFORM_ON_FRACTION_ROW, "hands:"+QString::number(bsBoneIndex)+"/transformOnFraction"); break;
            case NORMAL_ON_FRACTION_ROW:
                checkisproperty(NORMAL_ON_FRACTION_ROW, "hands:"+QString::number(bsBoneIndex)+"/normalOnFraction"); break;
            case FADE_IN_DURATION_ROW:
                checkisproperty(FADE_IN_DURATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/fadeInDuration"); break;
            case FADE_OUT_DURATION_ROW:
                checkisproperty(FADE_OUT_DURATION_ROW, "hands:"+QString::number(bsBoneIndex)+"/fadeOutDuration"); break;
            case EXTRAPOLATION_TIME_STEP_ROW:
                checkisproperty(EXTRAPOLATION_TIME_STEP_ROW, "hands:"+QString::number(bsBoneIndex)+"/extrapolationTimeStep"); break;
            case HANDLE_CHANGE_SPEED_ROW:
                checkisproperty(HANDLE_CHANGE_SPEED_ROW, "hands:"+QString::number(bsBoneIndex)+"/handleChangeSpeed"); break;
            case FIXUP_ROW:
                checkisproperty(FIXUP_ROW, "hands:"+QString::number(bsBoneIndex)+"/fixUp"); break;
            case HAND_INDEX_ROW:
                checkisproperty(HAND_INDEX_ROW, "hands:"+QString::number(bsBoneIndex)+"/handIndex"); break;
            case ENABLE_ROW:
                checkisproperty(ENABLE_ROW, "hands:"+QString::number(bsBoneIndex)+"/enable"); break;
            }
        }
    }else{
        LogFile::writeToLog("HandUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void HandUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
            if (parent->getVariableBindingSetData()){
                emit viewProperties(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (parent->getVariableBindingSetData()){
                emit viewVariables(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("HandUI::selectTableToView(): The data is nullptr!!");
    }
}

void HandUI::variableRenamed(const QString & name, int index){
    if (parent){
        index--;
        auto bind = parent->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("hands:"+QString::number(bsBoneIndex)+"/targetPosition", TARGET_POSITION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/targetRotation", TARGET_ROTATION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/targetNormal", TARGET_NORMAL_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/transformOnFraction", TRANSFORM_ON_FRACTION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/normalOnFraction", NORMAL_ON_FRACTION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/fadeInDuration", FADE_IN_DURATION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/fadeOutDuration", FADE_OUT_DURATION_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/extrapolationTimeStep", EXTRAPOLATION_TIME_STEP_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/handleChangeSpeed", HANDLE_CHANGE_SPEED_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/fixUp", FIXUP_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/handIndex", HAND_INDEX_ROW);
            setname("hands:"+QString::number(bsBoneIndex)+"/enable", ENABLE_ROW);
        }
    }else{
        LogFile::writeToLog("HandUI::variableRenamed(): parent is nullptr!!");
    }
}
