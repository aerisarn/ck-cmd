#include "evaluatehandlemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbevaluatehandlemodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 9

#define NAME_ROW 0
#define ENABLE_ROW 1
#define HANDLE_ROW 2
#define HANDLE_POSITION_OUT_ROW 3
#define HANDLE_ROTATION_OUT_ROW 4
#define IS_VALID_OUT_ROW 5
#define EXTRAPOLATION_TIME_STEP_ROW 6
#define HANDLE_CHANGE_SPEED_ROW 7
#define HANDLE_CHANGE_MODE_ROW 8

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList EvaluateHandleModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

EvaluateHandleModifierUI::EvaluateHandleModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      handlePositionOut(new QuadVariableWidget),
      handleRotationOut(new QuadVariableWidget),
      isValidOut(new CheckBox),
      extrapolationTimeStep(new DoubleSpinBox),
      handleChangeSpeed(new DoubleSpinBox),
      handleChangeMode(new ComboBox)
{
    setTitle("hkbEvaluateHandleModifier");
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
    table->setItem(HANDLE_ROW, NAME_COLUMN, new TableWidgetItem("handle"));
    table->setItem(HANDLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkHandle", Qt::AlignCenter));
    table->setItem(HANDLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(HANDLE_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(HANDLE_POSITION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("handlePositionOut"));
    table->setItem(HANDLE_POSITION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(HANDLE_POSITION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HANDLE_POSITION_OUT_ROW, VALUE_COLUMN, handlePositionOut);
    table->setItem(HANDLE_ROTATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("handleRotationOut"));
    table->setItem(HANDLE_ROTATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(HANDLE_ROTATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HANDLE_ROTATION_OUT_ROW, VALUE_COLUMN, handleRotationOut);
    table->setItem(IS_VALID_OUT_ROW, NAME_COLUMN, new TableWidgetItem("isValidOut"));
    table->setItem(IS_VALID_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_VALID_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_VALID_OUT_ROW, VALUE_COLUMN, isValidOut);
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
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void EvaluateHandleModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(handlePositionOut, SIGNAL(editingFinished()), this, SLOT(setHandlePositionOut()), Qt::UniqueConnection);
        connect(handleRotationOut, SIGNAL(editingFinished()), this, SLOT(setHandleRotationOut()), Qt::UniqueConnection);
        connect(isValidOut, SIGNAL(released()), this, SLOT(setIsValidOut()), Qt::UniqueConnection);
        connect(extrapolationTimeStep, SIGNAL(editingFinished()), this, SLOT(setExtrapolationTimeStep()), Qt::UniqueConnection);
        connect(handleChangeSpeed, SIGNAL(editingFinished()), this, SLOT(setHandleChangeSpeed()), Qt::UniqueConnection);
        connect(handleChangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setHandleChangeMode(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(handlePositionOut, SIGNAL(editingFinished()), this, SLOT(setHandlePositionOut()));
        disconnect(handleRotationOut, SIGNAL(editingFinished()), this, SLOT(setHandleRotationOut()));
        disconnect(isValidOut, SIGNAL(released()), this, SLOT(setIsValidOut()));
        disconnect(extrapolationTimeStep, SIGNAL(editingFinished()), this, SLOT(setExtrapolationTimeStep()));
        disconnect(handleChangeSpeed, SIGNAL(editingFinished()), this, SLOT(setHandleChangeSpeed()));
        disconnect(handleChangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setHandleChangeMode(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void EvaluateHandleModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void EvaluateHandleModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_EVALUATE_HANDLE_MODIFIER){
            bsData = static_cast<hkbEvaluateHandleModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            handlePositionOut->setValue(bsData->getHandlePositionOut());
            handleRotationOut->setValue(bsData->getHandleRotationOut());
            isValidOut->setChecked(bsData->getIsValidOut());
            extrapolationTimeStep->setValue(bsData->getExtrapolationTimeStep());
            handleChangeSpeed->setValue(bsData->getHandleChangeSpeed());
            (!handleChangeMode->count()) ? handleChangeMode->insertItems(0, bsData->HandleChangeMode) : NULL;
            handleChangeMode->setCurrentIndex(bsData->HandleChangeMode.indexOf(bsData->getHandleChangeMode()));
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(HANDLE_ROW, BINDING_COLUMN, varBind, "handle", table, bsData);
            UIHelper::loadBinding(HANDLE_POSITION_OUT_ROW, BINDING_COLUMN, varBind, "handlePositionOut", table, bsData);
            UIHelper::loadBinding(HANDLE_ROTATION_OUT_ROW, BINDING_COLUMN, varBind, "handleRotationOut", table, bsData);
            UIHelper::loadBinding(IS_VALID_OUT_ROW, BINDING_COLUMN, varBind, "isValidOut", table, bsData);
            UIHelper::loadBinding(EXTRAPOLATION_TIME_STEP_ROW, BINDING_COLUMN, varBind, "extrapolationTimeStep", table, bsData);
            UIHelper::loadBinding(HANDLE_CHANGE_SPEED_ROW, BINDING_COLUMN, varBind, "handleChangeSpeed", table, bsData);
        }else{
            LogFile::writeToLog("EvaluateHandleModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void EvaluateHandleModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::setName(): The data is nullptr!!");
    }
}

void EvaluateHandleModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("EvaluateHandleModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setHandlePositionOut(){
    (bsData) ? bsData->setHandlePositionOut(handlePositionOut->value()) : LogFile::writeToLog("EvaluateHandleModifierUI::setHandlePositionOut(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setHandleRotationOut(){
    (bsData) ? bsData->setHandleRotationOut(handleRotationOut->value()) : LogFile::writeToLog("EvaluateHandleModifierUI::setHandleRotationOut(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setIsValidOut(){
    (bsData) ? bsData->setIsValidOut(isValidOut->isChecked()) : LogFile::writeToLog("EvaluateHandleModifierUI::setIsValidOut(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setExtrapolationTimeStep(){
    (bsData) ? bsData->setExtrapolationTimeStep(extrapolationTimeStep->value()) : LogFile::writeToLog("EvaluateHandleModifierUI::setExtrapolationTimeStep(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setHandleChangeSpeed(){
    (bsData) ? bsData->setHandleChangeSpeed(handleChangeSpeed->value()) : LogFile::writeToLog("EvaluateHandleModifierUI::setHandleChangeSpeed(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::setHandleChangeMode(int index){
    (bsData) ? bsData->setHandleChangeMode(index - 1) : LogFile::writeToLog("EvaluateHandleModifierUI::setHandleChangeMode(): The 'bsData' pointer is nullptr!!");
}

void EvaluateHandleModifierUI::viewSelected(int row, int column){
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
            case HANDLE_ROW:
                checkisproperty(HANDLE_ROW, "handle"); break;
            case HANDLE_POSITION_OUT_ROW:
                checkisproperty(HANDLE_POSITION_OUT_ROW, "handlePositionOut"); break;
            case HANDLE_ROTATION_OUT_ROW:
                checkisproperty(HANDLE_ROTATION_OUT_ROW, "handleRotationOut"); break;
            case IS_VALID_OUT_ROW:
                checkisproperty(IS_VALID_OUT_ROW, "isValidOut"); break;
            case EXTRAPOLATION_TIME_STEP_ROW:
                checkisproperty(EXTRAPOLATION_TIME_STEP_ROW, "extrapolationTimeStep"); break;
            case HANDLE_CHANGE_SPEED_ROW:
                checkisproperty(HANDLE_CHANGE_SPEED_ROW, "handleChangeSpeed"); break;
            }
        }
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void EvaluateHandleModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("EvaluateHandleModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void EvaluateHandleModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("handle", HANDLE_ROW);
            setname("handlePositionOut", HANDLE_POSITION_OUT_ROW);
            setname("handleRotationOut", HANDLE_ROTATION_OUT_ROW);
            setname("isValidOut", IS_VALID_OUT_ROW);
            setname("extrapolationTimeStep", EXTRAPOLATION_TIME_STEP_ROW);
            setname("handleChangeSpeed", HANDLE_CHANGE_SPEED_ROW);
        }
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void EvaluateHandleModifierUI::setBindingVariable(int index, const QString &name){
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
        case HANDLE_ROW:
            checkisproperty(HANDLE_ROW, "handle", VARIABLE_TYPE_POINTER); break;
        case HANDLE_POSITION_OUT_ROW:
            checkisproperty(HANDLE_POSITION_OUT_ROW, "handlePositionOut", VARIABLE_TYPE_VECTOR4); break;
        case HANDLE_ROTATION_OUT_ROW:
            checkisproperty(HANDLE_ROTATION_OUT_ROW, "handleRotationOut", VARIABLE_TYPE_VECTOR4); break;
        case IS_VALID_OUT_ROW:
            checkisproperty(IS_VALID_OUT_ROW, "isValidOut", VARIABLE_TYPE_BOOL); break;
        case EXTRAPOLATION_TIME_STEP_ROW:
            checkisproperty(EXTRAPOLATION_TIME_STEP_ROW, "extrapolationTimeStep", VARIABLE_TYPE_REAL); break;
        case HANDLE_CHANGE_SPEED_ROW:
            checkisproperty(HANDLE_CHANGE_SPEED_ROW, "handleChangeSpeed", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("EvaluateHandleModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
