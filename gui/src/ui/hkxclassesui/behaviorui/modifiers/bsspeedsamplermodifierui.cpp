#include "bsspeedsamplermodifierui.h"


#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/BSSpeedSamplerModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define STATE_ROW 2
#define DIRECTION_ROW 3
#define GOAL_SPEED_ROW 4
#define SPEED_OUT_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSSpeedSamplerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSSpeedSamplerModifierUI::BSSpeedSamplerModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      state(new DoubleSpinBox),
      direction(new DoubleSpinBox),
      goalSpeed(new DoubleSpinBox),
      speedOut(new DoubleSpinBox)
{
    setTitle("BSSpeedSamplerModifier");
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
    table->setItem(STATE_ROW, NAME_COLUMN, new TableWidgetItem("state"));
    table->setItem(STATE_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(STATE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(STATE_ROW, VALUE_COLUMN, state);
    table->setItem(DIRECTION_ROW, NAME_COLUMN, new TableWidgetItem("direction"));
    table->setItem(DIRECTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DIRECTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECTION_ROW, VALUE_COLUMN, direction);
    table->setItem(GOAL_SPEED_ROW, NAME_COLUMN, new TableWidgetItem("goalSpeed"));
    table->setItem(GOAL_SPEED_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GOAL_SPEED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GOAL_SPEED_ROW, VALUE_COLUMN, goalSpeed);
    table->setItem(SPEED_OUT_ROW, NAME_COLUMN, new TableWidgetItem("speedOut"));
    table->setItem(SPEED_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SPEED_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SPEED_OUT_ROW, VALUE_COLUMN, speedOut);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSSpeedSamplerModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(state, SIGNAL(editingFinished()), this, SLOT(setState()), Qt::UniqueConnection);
        connect(direction, SIGNAL(editingFinished()), this, SLOT(setDirection()), Qt::UniqueConnection);
        connect(goalSpeed, SIGNAL(editingFinished()), this, SLOT(setGoalSpeed()), Qt::UniqueConnection);
        connect(speedOut, SIGNAL(editingFinished()), this, SLOT(setSpeedOut()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(state, SIGNAL(editingFinished()), this, SLOT(setState()));
        disconnect(direction, SIGNAL(editingFinished()), this, SLOT(setDirection()));
        disconnect(goalSpeed, SIGNAL(editingFinished()), this, SLOT(setGoalSpeed()));
        disconnect(speedOut, SIGNAL(editingFinished()), this, SLOT(setSpeedOut()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSSpeedSamplerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSSpeedSamplerModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_SPEED_SAMPLER_MODIFIER){
            bsData = static_cast<BSSpeedSamplerModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            state->setValue(bsData->getState());
            direction->setValue(bsData->getDirection());
            goalSpeed->setValue(bsData->getGoalSpeed());
            speedOut->setValue(bsData->getSpeedOut());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(STATE_ROW, BINDING_COLUMN, varBind, "state", table, bsData);
            UIHelper::loadBinding(DIRECTION_ROW, BINDING_COLUMN, varBind, "direction", table, bsData);
            UIHelper::loadBinding(GOAL_SPEED_ROW, BINDING_COLUMN, varBind, "goalSpeed", table, bsData);
            UIHelper::loadBinding(SPEED_OUT_ROW, BINDING_COLUMN, varBind, "speedOut", table, bsData);
        }else{
            LogFile::writeToLog("BSSpeedSamplerModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSSpeedSamplerModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::setName(): The data is nullptr!!");
    }
}

void BSSpeedSamplerModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSSpeedSamplerModifierUI::setEnable(): The data is nullptr!!");
}

void BSSpeedSamplerModifierUI::setState(){
    (bsData) ? bsData->setState(state->value()) : LogFile::writeToLog("BSSpeedSamplerModifierUI::setState(): The data is nullptr!!");
}

void BSSpeedSamplerModifierUI::setDirection(){
    (bsData) ? bsData->setDirection(direction->value()) : LogFile::writeToLog("BSSpeedSamplerModifierUI::setDirection(): The data is nullptr!!");
}

void BSSpeedSamplerModifierUI::setGoalSpeed(){
    (bsData) ? bsData->setGoalSpeed(goalSpeed->value()) : LogFile::writeToLog("BSSpeedSamplerModifierUI::setGoalSpeed(): The data is nullptr!!");
}

void BSSpeedSamplerModifierUI::setSpeedOut(){
    (bsData) ? bsData->setSpeedOut(speedOut->value()) : LogFile::writeToLog("BSSpeedSamplerModifierUI::setSpeedOut(): The data is nullptr!!");
}

void BSSpeedSamplerModifierUI::viewSelected(int row, int column){
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
            case STATE_ROW:
                checkisproperty(STATE_ROW, "state"); break;
            case DIRECTION_ROW:
                checkisproperty(DIRECTION_ROW, "direction"); break;
            case GOAL_SPEED_ROW:
                checkisproperty(GOAL_SPEED_ROW, "goalSpeed"); break;
            case SPEED_OUT_ROW:
                checkisproperty(SPEED_OUT_ROW, "speedOut"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSSpeedSamplerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSSpeedSamplerModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSSpeedSamplerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("state", STATE_ROW);
            setname("direction", DIRECTION_ROW);
            setname("goalSpeed", GOAL_SPEED_ROW);
            setname("speedOut", SPEED_OUT_ROW);
        }
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSSpeedSamplerModifierUI::setBindingVariable(int index, const QString &name){
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
        case STATE_ROW:
            checkisproperty(STATE_ROW, "state", VARIABLE_TYPE_INT32); break;
        case DIRECTION_ROW:
            checkisproperty(DIRECTION_ROW, "direction", VARIABLE_TYPE_REAL); break;
        case GOAL_SPEED_ROW:
            checkisproperty(GOAL_SPEED_ROW, "goalSpeed", VARIABLE_TYPE_REAL); break;
        case SPEED_OUT_ROW:
            checkisproperty(SPEED_OUT_ROW, "speedOut", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSSpeedSamplerModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
