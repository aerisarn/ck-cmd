#include "computerotationfromaxisanglemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbComputeRotationFromAxisAngleModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 5

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ROTATION_OUT_ROW 2
#define AXIS_ROW 3
#define ANGLE_DEGREES_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ComputeRotationFromAxisAngleModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ComputeRotationFromAxisAngleModifierUI::ComputeRotationFromAxisAngleModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      rotationOut(new QuadVariableWidget),
      axis(new QuadVariableWidget),
      angleDegrees(new DoubleSpinBox)
{
    setTitle("hkbComputeRotationFromAxisAngleModifier");
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
    table->setItem(AXIS_ROW, NAME_COLUMN, new TableWidgetItem("axis"));
    table->setItem(AXIS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(AXIS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(AXIS_ROW, VALUE_COLUMN, axis);
    table->setItem(ANGLE_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("angleDegrees"));
    table->setItem(ANGLE_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ANGLE_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANGLE_DEGREES_ROW, VALUE_COLUMN, angleDegrees);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ComputeRotationFromAxisAngleModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setAxis()), Qt::UniqueConnection);
        connect(axis, SIGNAL(editingFinished()), this, SLOT(setAxis()), Qt::UniqueConnection);
        connect(angleDegrees, SIGNAL(editingFinished()), this, SLOT(setAngleDegrees()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setAxis()));
        disconnect(axis, SIGNAL(editingFinished()), this, SLOT(setAxis()));
        disconnect(angleDegrees, SIGNAL(editingFinished()), this, SLOT(setAngleDegrees()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void ComputeRotationFromAxisAngleModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ComputeRotationFromAxisAngleModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER){
            bsData = static_cast<hkbComputeRotationFromAxisAngleModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->enable);
            rotationOut->setValue(bsData->rotationOut);
            axis->setValue(bsData->axis);
            angleDegrees->setValue(bsData->angleDegrees);
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ROTATION_OUT_ROW, BINDING_COLUMN, varBind, "rotationOut", table, bsData);
            UIHelper::loadBinding(AXIS_ROW, BINDING_COLUMN, varBind, "axis", table, bsData);
            UIHelper::loadBinding(ANGLE_DEGREES_ROW, BINDING_COLUMN, varBind, "angleDegrees", table, bsData);
        }else{
            LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void ComputeRotationFromAxisAngleModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::setName(): The data is nullptr!!");
    }
}

void ComputeRotationFromAxisAngleModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setEnable(): The data is nullptr!!");
}

void ComputeRotationFromAxisAngleModifierUI::setRotationOut(){
    (bsData) ? bsData->setRotationOut(rotationOut->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setRotationOut(): The data is nullptr!!");
}

void ComputeRotationFromAxisAngleModifierUI::setAxis(){
    (bsData) ? bsData->setAxis(axis->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setAxis(): The data is nullptr!!");
}

void ComputeRotationFromAxisAngleModifierUI::setAngleDegrees(){
    (bsData) ? bsData->setAngleDegrees(angleDegrees->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setAngleDegrees(): The data is nullptr!!");
}

void ComputeRotationFromAxisAngleModifierUI::viewSelected(int row, int column){
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
            case AXIS_ROW:
                checkisproperty(AXIS_ROW, "axis"); break;
            case ANGLE_DEGREES_ROW:
                checkisproperty(ANGLE_DEGREES_ROW, "angleDegrees"); break;
            }
        }
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeRotationFromAxisAngleModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void ComputeRotationFromAxisAngleModifierUI::variableRenamed(const QString & name, int index){
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
            setname("axis", AXIS_ROW);
            setname("angleDegrees", ANGLE_DEGREES_ROW);
        }
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeRotationFromAxisAngleModifierUI::setBindingVariable(int index, const QString &name){
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
            checkisproperty(ROTATION_OUT_ROW, "rotationOut", VARIABLE_TYPE_QUATERNION); break;
        case AXIS_ROW:
            checkisproperty(AXIS_ROW, "axis", VARIABLE_TYPE_VECTOR4); break;
        case ANGLE_DEGREES_ROW:
            checkisproperty(ANGLE_DEGREES_ROW, "angleDegrees", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("ComputeRotationFromAxisAngleModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
