#include "bstweenermodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/BSTweenerModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 8

#define NAME_ROW 0
#define ENABLE_ROW 1
#define TWEEN_POSITION_ROW 2
#define TWEEN_ROTATION_ROW 3
#define USE_TWEEN_DURATION_ROW 4
#define TWEEN_DURATION_ROW 5
#define TARGET_POSITION_ROW 6
#define TARGET_ROTATION_ROW 7

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSTweenerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSTweenerModifierUI::BSTweenerModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      tweenPosition(new CheckBox),
      tweenRotation(new CheckBox),
      useTweenDuration(new CheckBox),
      tweenDuration(new DoubleSpinBox),
      targetPosition(new QuadVariableWidget),
      targetRotation(new QuadVariableWidget)
{
    setTitle("BSTweenerModifier");
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
    table->setItem(TWEEN_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("tweenPosition"));
    table->setItem(TWEEN_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(TWEEN_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TWEEN_POSITION_ROW, VALUE_COLUMN, tweenPosition);
    table->setItem(TWEEN_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("tweenRotation"));
    table->setItem(TWEEN_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(TWEEN_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TWEEN_ROTATION_ROW, VALUE_COLUMN, tweenRotation);
    table->setItem(USE_TWEEN_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("useTweenDuration"));
    table->setItem(USE_TWEEN_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(USE_TWEEN_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(USE_TWEEN_DURATION_ROW, VALUE_COLUMN, useTweenDuration);
    table->setItem(TWEEN_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("tweenDuration"));
    table->setItem(TWEEN_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TWEEN_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TWEEN_DURATION_ROW, VALUE_COLUMN, tweenDuration);
    table->setItem(TARGET_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("targetPosition"));
    table->setItem(TARGET_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_POSITION_ROW, VALUE_COLUMN, targetPosition);
    table->setItem(TARGET_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("targetRotation"));
    table->setItem(TARGET_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(TARGET_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_ROTATION_ROW, VALUE_COLUMN, targetRotation);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSTweenerModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(tweenPosition, SIGNAL(released()), this, SLOT(setTweenPosition()), Qt::UniqueConnection);
        connect(tweenRotation, SIGNAL(released()), this, SLOT(setTweenRotation()), Qt::UniqueConnection);
        connect(useTweenDuration, SIGNAL(released()), this, SLOT(setUseTweenDuration()), Qt::UniqueConnection);
        connect(tweenDuration, SIGNAL(editingFinished()), this, SLOT(setTweenDuration()), Qt::UniqueConnection);
        connect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()), Qt::UniqueConnection);
        connect(targetRotation, SIGNAL(editingFinished()), this, SLOT(setTargetRotation()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(tweenPosition, SIGNAL(released()), this, SLOT(setTweenPosition()));
        disconnect(tweenRotation, SIGNAL(released()), this, SLOT(setTweenRotation()));
        disconnect(useTweenDuration, SIGNAL(released()), this, SLOT(setUseTweenDuration()));
        disconnect(tweenDuration, SIGNAL(editingFinished()), this, SLOT(setTweenDuration()));
        disconnect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()));
        disconnect(targetRotation, SIGNAL(editingFinished()), this, SLOT(setTargetRotation()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSTweenerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSTweenerModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_TWEENER_MODIFIER){
            bsData = static_cast<BSTweenerModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            tweenPosition->setChecked(bsData->getTweenPosition());
            tweenRotation->setChecked(bsData->getTweenRotation());
            useTweenDuration->setChecked(bsData->getUseTweenDuration());
            tweenDuration->setValue(bsData->getTweenDuration());
            targetPosition->setValue(bsData->getTargetPosition());
            targetRotation->setValue(bsData->getTargetRotation());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(TWEEN_POSITION_ROW, BINDING_COLUMN, varBind, "tweenPosition", table, bsData);
            UIHelper::loadBinding(TWEEN_ROTATION_ROW, BINDING_COLUMN, varBind, "tweenRotation", table, bsData);
            UIHelper::loadBinding(USE_TWEEN_DURATION_ROW, BINDING_COLUMN, varBind, "useTweenDuration", table, bsData);
            UIHelper::loadBinding(TWEEN_DURATION_ROW, BINDING_COLUMN, varBind, "tweenDuration", table, bsData);
            UIHelper::loadBinding(TARGET_POSITION_ROW, BINDING_COLUMN, varBind, "targetPosition", table, bsData);
            UIHelper::loadBinding(TARGET_ROTATION_ROW, BINDING_COLUMN, varBind, "targetRotation", table, bsData);
        }else{
            LogFile::writeToLog("BSTweenerModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSTweenerModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::setName(): The data is nullptr!!");
    }
}

void BSTweenerModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSTweenerModifierUI::setEnable(): The data is nullptr!!");
}

void BSTweenerModifierUI::setTweenPosition(){
    (bsData) ? bsData->setTweenPosition(tweenPosition->isChecked()) : LogFile::writeToLog("BSTweenerModifierUI::setTweenPosition(): The data is nullptr!!");
}

void BSTweenerModifierUI::setTweenRotation(){
    (bsData) ? bsData->setTweenRotation(tweenRotation->isChecked()) : LogFile::writeToLog("BSTweenerModifierUI::setTweenRotation(): The data is nullptr!!");
}

void BSTweenerModifierUI::setUseTweenDuration(){
    (bsData) ? bsData->setUseTweenDuration(useTweenDuration->isChecked()) : LogFile::writeToLog("BSTweenerModifierUI::setUseTweenDuration(): The data is nullptr!!");
}

void BSTweenerModifierUI::setTweenDuration(){
    (bsData) ? bsData->setTweenDuration(tweenDuration->value()) : LogFile::writeToLog("BSTweenerModifierUI::setTweenDuration(): The data is nullptr!!");
}

void BSTweenerModifierUI::setTargetPosition(){
    (bsData) ? bsData->setTargetPosition(targetPosition->value()) : LogFile::writeToLog("BSTweenerModifierUI::setTargetPosition(): The data is nullptr!!");
}

void BSTweenerModifierUI::setTargetRotation(){
    (bsData) ? bsData->setTargetRotation(targetRotation->value()) : LogFile::writeToLog("BSTweenerModifierUI::setTargetRotation(): The data is nullptr!!");
}

void BSTweenerModifierUI::viewSelected(int row, int column){
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
            case TWEEN_POSITION_ROW:
                checkisproperty(TWEEN_POSITION_ROW, "tweenPosition"); break;
            case TWEEN_ROTATION_ROW:
                checkisproperty(TWEEN_ROTATION_ROW, "tweenRotation"); break;
            case USE_TWEEN_DURATION_ROW:
                checkisproperty(USE_TWEEN_DURATION_ROW, "useTweenDuration"); break;
            case TWEEN_DURATION_ROW:
                checkisproperty(TWEEN_DURATION_ROW, "tweenDuration"); break;
            case TARGET_POSITION_ROW:
                checkisproperty(TARGET_POSITION_ROW, "targetPosition"); break;
            case TARGET_ROTATION_ROW:
                checkisproperty(TARGET_ROTATION_ROW, "targetRotation"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSTweenerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSTweenerModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSTweenerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("tweenPosition", TWEEN_POSITION_ROW);
            setname("tweenRotation", TWEEN_ROTATION_ROW);
            setname("useTweenDuration", USE_TWEEN_DURATION_ROW);
            setname("tweenDuration", TWEEN_DURATION_ROW);
            setname("targetPosition", TARGET_POSITION_ROW);
            setname("targetRotation", TARGET_ROTATION_ROW);
        }
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSTweenerModifierUI::setBindingVariable(int index, const QString &name){
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
        case TWEEN_POSITION_ROW:
            checkisproperty(TWEEN_POSITION_ROW, "tweenPosition", VARIABLE_TYPE_BOOL); break;
        case TWEEN_ROTATION_ROW:
            checkisproperty(TWEEN_ROTATION_ROW, "tweenRotation", VARIABLE_TYPE_BOOL); break;
        case USE_TWEEN_DURATION_ROW:
            checkisproperty(USE_TWEEN_DURATION_ROW, "useTweenDuration", VARIABLE_TYPE_BOOL); break;
        case TWEEN_DURATION_ROW:
            checkisproperty(TWEEN_DURATION_ROW, "tweenDuration", VARIABLE_TYPE_REAL); break;
        case TARGET_POSITION_ROW:
            checkisproperty(TARGET_POSITION_ROW, "targetPosition", VARIABLE_TYPE_VECTOR4); break;
        case TARGET_ROTATION_ROW:
            checkisproperty(TARGET_ROTATION_ROW, "targetRotation", VARIABLE_TYPE_QUATERNION); break;
        }
    }else{
        LogFile::writeToLog("BSTweenerModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
