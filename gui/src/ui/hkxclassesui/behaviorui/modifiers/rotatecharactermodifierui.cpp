#include "rotatecharactermodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbrotatecharactermodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 5

#define NAME_ROW 0
#define ENABLE_ROW 1
#define DEGREES_PER_SECOND_ROW 2
#define SPEED_MULTIPLIER_ROW 3
#define AXIS_OF_ROTATION_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList RotateCharacterModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

RotateCharacterModifierUI::RotateCharacterModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      degreesPerSecond(new DoubleSpinBox),
      speedMultiplier(new DoubleSpinBox),
      axisOfRotation(new QuadVariableWidget)
{
    setTitle("hkbRotateCharacterModifier");
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
    table->setItem(DEGREES_PER_SECOND_ROW, NAME_COLUMN, new TableWidgetItem("degreesPerSecond"));
    table->setItem(DEGREES_PER_SECOND_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DEGREES_PER_SECOND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DEGREES_PER_SECOND_ROW, VALUE_COLUMN, degreesPerSecond);
    table->setItem(SPEED_MULTIPLIER_ROW, NAME_COLUMN, new TableWidgetItem("speedMultiplier"));
    table->setItem(SPEED_MULTIPLIER_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SPEED_MULTIPLIER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SPEED_MULTIPLIER_ROW, VALUE_COLUMN, speedMultiplier);
    table->setItem(AXIS_OF_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("axisOfRotation"));
    table->setItem(AXIS_OF_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(AXIS_OF_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(AXIS_OF_ROTATION_ROW, VALUE_COLUMN, axisOfRotation);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void RotateCharacterModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(degreesPerSecond, SIGNAL(editingFinished()), this, SLOT(setDegreesPerSecond()), Qt::UniqueConnection);
        connect(speedMultiplier, SIGNAL(editingFinished()), this, SLOT(setSpeedMultiplier()), Qt::UniqueConnection);
        connect(axisOfRotation, SIGNAL(editingFinished()), this, SLOT(setAxisOfRotation()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(degreesPerSecond, SIGNAL(editingFinished()), this, SLOT(setDegreesPerSecond()));
        disconnect(speedMultiplier, SIGNAL(editingFinished()), this, SLOT(setSpeedMultiplier()));
        disconnect(axisOfRotation, SIGNAL(editingFinished()), this, SLOT(setAxisOfRotation()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void RotateCharacterModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void RotateCharacterModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_ROTATE_CHARACTER_MODIFIER){
            bsData = static_cast<hkbRotateCharacterModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            degreesPerSecond->setValue(bsData->getDegreesPerSecond());
            speedMultiplier->setValue(bsData->getSpeedMultiplier());
            axisOfRotation->setValue(bsData->getAxisOfRotation());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(DEGREES_PER_SECOND_ROW, BINDING_COLUMN, varBind, "degreesPerSecond", table, bsData);
            UIHelper::loadBinding(SPEED_MULTIPLIER_ROW, BINDING_COLUMN, varBind, "speedMultiplier", table, bsData);
            UIHelper::loadBinding(AXIS_OF_ROTATION_ROW, BINDING_COLUMN, varBind, "axisOfRotation", table, bsData);
        }else{
            LogFile::writeToLog("RotateCharacterModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void RotateCharacterModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::setName(): The data is nullptr!!");
    }
}

void RotateCharacterModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("RotateCharacterModifierUI::setEnable(): The data is nullptr!!");
}

void RotateCharacterModifierUI::setDegreesPerSecond(){
    (bsData) ? bsData->setDegreesPerSecond(degreesPerSecond->value()) : LogFile::writeToLog("RotateCharacterModifierUI::setDegreesPerSecond(): The data is nullptr!!");
}

void RotateCharacterModifierUI::setSpeedMultiplier(){
    (bsData) ? bsData->setSpeedMultiplier(speedMultiplier->value()) : LogFile::writeToLog("RotateCharacterModifierUI::setSpeedMultiplier(): The data is nullptr!!");
}

void RotateCharacterModifierUI::setAxisOfRotation(){
    (bsData) ? bsData->setAxisOfRotation(axisOfRotation->value()) : LogFile::writeToLog("RotateCharacterModifierUI::setAxisOfRotation(): The data is nullptr!!");
}

void RotateCharacterModifierUI::viewSelected(int row, int column){
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
            case DEGREES_PER_SECOND_ROW:
                checkisproperty(DEGREES_PER_SECOND_ROW, "degreesPerSecond"); break;
            case SPEED_MULTIPLIER_ROW:
                checkisproperty(SPEED_MULTIPLIER_ROW, "speedMultiplier"); break;
            case AXIS_OF_ROTATION_ROW:
                checkisproperty(AXIS_OF_ROTATION_ROW, "axisOfRotation"); break;
            }
        }
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void RotateCharacterModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("RotateCharacterModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void RotateCharacterModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("degreesPerSecond", DEGREES_PER_SECOND_ROW);
            setname("speedMultiplier", SPEED_MULTIPLIER_ROW);
            setname("axisOfRotation", AXIS_OF_ROTATION_ROW);
        }
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void RotateCharacterModifierUI::setBindingVariable(int index, const QString &name){
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
        case DEGREES_PER_SECOND_ROW:
            checkisproperty(DEGREES_PER_SECOND_ROW, "degreesPerSecond", VARIABLE_TYPE_REAL); break;
        case SPEED_MULTIPLIER_ROW:
            checkisproperty(SPEED_MULTIPLIER_ROW, "speedMultiplier", VARIABLE_TYPE_REAL); break;
        case AXIS_OF_ROTATION_ROW:
            checkisproperty(AXIS_OF_ROTATION_ROW, "axisOfRotation", VARIABLE_TYPE_VECTOR4); break;
        }
    }else{
        LogFile::writeToLog("RotateCharacterModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
