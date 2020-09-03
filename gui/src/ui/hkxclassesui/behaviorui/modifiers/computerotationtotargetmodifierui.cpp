#include "computerotationtotargetmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbComputeRotationToTargetModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 9

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ROTATION_OUT_ROW 2
#define TARGET_POSITION_ROW 3
#define CURRENT_POSITION_ROW 4
#define CURRENT_ROTATION_ROW 5
#define LOCAL_AXIS_OF_ROTATION_ROW 6
#define LOCAL_FACING_DIRECTION_ROW 7
#define RESULT_IS_DELTA_ROW 8

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ComputeRotationToTargetModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ComputeRotationToTargetModifierUI::ComputeRotationToTargetModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      rotationOut(new QuadVariableWidget),
      targetPosition(new QuadVariableWidget),
      currentPosition(new QuadVariableWidget),
      currentRotation(new QuadVariableWidget),
      localAxisOfRotation(new QuadVariableWidget),
      localFacingDirection(new QuadVariableWidget),
      resultIsDelta(new CheckBox)
{
    setTitle("hkbComputeRotationToTargetModifier");
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
    table->setItem(ROTATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("rotationOut"));
    table->setItem(ROTATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(ROTATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATION_OUT_ROW, VALUE_COLUMN, rotationOut);
    table->setItem(TARGET_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("targetPosition"));
    table->setItem(TARGET_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_POSITION_ROW, VALUE_COLUMN, targetPosition);
    table->setItem(CURRENT_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("currentPosition"));
    table->setItem(CURRENT_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(CURRENT_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CURRENT_POSITION_ROW, VALUE_COLUMN, currentPosition);
    table->setItem(CURRENT_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("currentRotation"));
    table->setItem(CURRENT_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(CURRENT_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CURRENT_ROTATION_ROW, VALUE_COLUMN, currentRotation);
    table->setItem(LOCAL_AXIS_OF_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("localAxisOfRotation"));
    table->setItem(LOCAL_AXIS_OF_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(LOCAL_AXIS_OF_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOCAL_AXIS_OF_ROTATION_ROW, VALUE_COLUMN, localAxisOfRotation);
    table->setItem(LOCAL_FACING_DIRECTION_ROW, NAME_COLUMN, new TableWidgetItem("localFacingDirection"));
    table->setItem(LOCAL_FACING_DIRECTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(LOCAL_FACING_DIRECTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LOCAL_FACING_DIRECTION_ROW, VALUE_COLUMN, localFacingDirection);
    table->setItem(RESULT_IS_DELTA_ROW, NAME_COLUMN, new TableWidgetItem("resultIsDelta"));
    table->setItem(RESULT_IS_DELTA_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(RESULT_IS_DELTA_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RESULT_IS_DELTA_ROW, VALUE_COLUMN, resultIsDelta);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ComputeRotationToTargetModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()), Qt::UniqueConnection);
        connect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()), Qt::UniqueConnection);
        connect(currentPosition, SIGNAL(editingFinished()), this, SLOT(setCurrentPosition()), Qt::UniqueConnection);
        connect(currentRotation, SIGNAL(editingFinished()), this, SLOT(setCurrentRotation()), Qt::UniqueConnection);
        connect(localAxisOfRotation, SIGNAL(editingFinished()), this, SLOT(setLocalAxisOfRotation()), Qt::UniqueConnection);
        connect(localFacingDirection, SIGNAL(editingFinished()), this, SLOT(setLocalFacingDirection()), Qt::UniqueConnection);
        connect(resultIsDelta, SIGNAL(released()), this, SLOT(setResultIsDelta()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()));
        disconnect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()));
        disconnect(currentPosition, SIGNAL(editingFinished()), this, SLOT(setCurrentPosition()));
        disconnect(currentRotation, SIGNAL(editingFinished()), this, SLOT(setCurrentRotation()));
        disconnect(localAxisOfRotation, SIGNAL(editingFinished()), this, SLOT(setLocalAxisOfRotation()));
        disconnect(localFacingDirection, SIGNAL(editingFinished()), this, SLOT(setLocalFacingDirection()));
        disconnect(resultIsDelta, SIGNAL(released()), this, SLOT(setResultIsDelta()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void ComputeRotationToTargetModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ComputeRotationToTargetModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER){
            bsData = static_cast<hkbComputeRotationToTargetModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            rotationOut->setValue(bsData->getRotationOut());
            targetPosition->setValue(bsData->getTargetPosition());
            currentPosition->setValue(bsData->getCurrentPosition());
            currentRotation->setValue(bsData->getCurrentRotation());
            localAxisOfRotation->setValue(bsData->getLocalAxisOfRotation());
            localFacingDirection->setValue(bsData->getLocalFacingDirection());
            resultIsDelta->setChecked(bsData->getResultIsDelta());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ROTATION_OUT_ROW, BINDING_COLUMN, varBind, "rotationOut", table, bsData);
            UIHelper::loadBinding(TARGET_POSITION_ROW, BINDING_COLUMN, varBind, "targetPosition", table, bsData);
            UIHelper::loadBinding(CURRENT_POSITION_ROW, BINDING_COLUMN, varBind, "currentPosition", table, bsData);
            UIHelper::loadBinding(CURRENT_ROTATION_ROW, BINDING_COLUMN, varBind, "currentRotation", table, bsData);
            UIHelper::loadBinding(LOCAL_AXIS_OF_ROTATION_ROW, BINDING_COLUMN, varBind, "localAxisOfRotation", table, bsData);
            UIHelper::loadBinding(LOCAL_FACING_DIRECTION_ROW, BINDING_COLUMN, varBind, "localFacingDirection", table, bsData);
            UIHelper::loadBinding(RESULT_IS_DELTA_ROW, BINDING_COLUMN, varBind, "resultIsDelta", table, bsData);
        }else{
            LogFile::writeToLog("ComputeRotationToTargetModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void ComputeRotationToTargetModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::setName(): The data is nullptr!!");
    }
}

void ComputeRotationToTargetModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setEnable(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setRotationOut(){
    (bsData) ? bsData->setRotationOut(rotationOut->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setRotationOut(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setTargetPosition(){
    (bsData) ? bsData->setTargetPosition(targetPosition->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setTargetPosition(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setCurrentPosition(){
    (bsData) ? bsData->setCurrentPosition(targetPosition->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setCurrentPosition(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setCurrentRotation(){
    (bsData) ? bsData->setCurrentRotation(currentRotation->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setCurrentRotation(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setLocalAxisOfRotation(){
    (bsData) ? bsData->setLocalAxisOfRotation(localAxisOfRotation->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setLocalAxisOfRotation(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setLocalFacingDirection(){
    (bsData) ? bsData->setLocalFacingDirection(localFacingDirection->value()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setLocalFacingDirection(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::setResultIsDelta(){
    (bsData) ? bsData->setResultIsDelta(resultIsDelta->isChecked()) : LogFile::writeToLog("ComputeRotationToTargetModifierUI::setResultIsDelta(): The data is nullptr!!");
}

void ComputeRotationToTargetModifierUI::viewSelected(int row, int column){
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
            case ROTATION_OUT_ROW:
                checkisproperty(ROTATION_OUT_ROW, "rotationOut"); break;
            case TARGET_POSITION_ROW:
                checkisproperty(TARGET_POSITION_ROW, "targetPosition"); break;
            case CURRENT_POSITION_ROW:
                checkisproperty(CURRENT_POSITION_ROW, "currentPosition"); break;
            case CURRENT_ROTATION_ROW:
                checkisproperty(CURRENT_ROTATION_ROW, "currentRotation"); break;
            case LOCAL_AXIS_OF_ROTATION_ROW:
                checkisproperty(LOCAL_AXIS_OF_ROTATION_ROW, "localAxisOfRotation"); break;
            case LOCAL_FACING_DIRECTION_ROW:
                checkisproperty(LOCAL_FACING_DIRECTION_ROW, "localFacingDirection"); break;
            case RESULT_IS_DELTA_ROW:
                checkisproperty(RESULT_IS_DELTA_ROW, "resultIsDelta"); break;
            }
        }
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeRotationToTargetModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void ComputeRotationToTargetModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("rotationOut", ROTATION_OUT_ROW);
            setname("targetPosition", TARGET_POSITION_ROW);
            setname("currentPosition", CURRENT_POSITION_ROW);
            setname("currentRotation", CURRENT_ROTATION_ROW);
            setname("localAxisOfRotation", LOCAL_AXIS_OF_ROTATION_ROW);
            setname("localFacingDirection", LOCAL_FACING_DIRECTION_ROW);
            setname("resultIsDelta", RESULT_IS_DELTA_ROW);
        }
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeRotationToTargetModifierUI::setBindingVariable(int index, const QString &name){
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
        case ROTATION_OUT_ROW:
            checkisproperty(ROTATION_OUT_ROW, "rotationOut", VARIABLE_TYPE_VECTOR4); break;
        case TARGET_POSITION_ROW:
            checkisproperty(TARGET_POSITION_ROW, "targetPosition", VARIABLE_TYPE_VECTOR4); break;
        case CURRENT_POSITION_ROW:
            checkisproperty(CURRENT_POSITION_ROW, "currentPosition", VARIABLE_TYPE_VECTOR4); break;
        case CURRENT_ROTATION_ROW:
            checkisproperty(CURRENT_ROTATION_ROW, "currentRotation", VARIABLE_TYPE_QUATERNION); break;
        case LOCAL_AXIS_OF_ROTATION_ROW:
            checkisproperty(LOCAL_AXIS_OF_ROTATION_ROW, "localAxisOfRotation", VARIABLE_TYPE_VECTOR4); break;
        case LOCAL_FACING_DIRECTION_ROW:
            checkisproperty(LOCAL_FACING_DIRECTION_ROW, "localFacingDirection", VARIABLE_TYPE_VECTOR4); break;
        case RESULT_IS_DELTA_ROW:
            checkisproperty(RESULT_IS_DELTA_ROW, "resultIsDelta", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("ComputeRotationToTargetModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
