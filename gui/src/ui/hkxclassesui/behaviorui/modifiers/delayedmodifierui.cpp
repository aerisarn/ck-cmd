#include "delayedmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbDelayedModifier.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/TreeGraphicsView.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"


#include <QHeaderView>
#include <QGroupBox>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define DELAY_SECONDS_ROW 2
#define DURATION_SECONDS_ROW 3
#define SECONDS_ELAPSED_ROW 4
#define MODIFIER_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList DelayedModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

DelayedModifierUI::DelayedModifierUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      delaySeconds(new DoubleSpinBox),
      durationSeconds(new DoubleSpinBox),
      secondsElapsed(new DoubleSpinBox)
{
    setTitle("hkbDelayedModifier");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignVCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(DELAY_SECONDS_ROW, NAME_COLUMN, new TableWidgetItem("delaySeconds"));
    table->setItem(DELAY_SECONDS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DELAY_SECONDS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DELAY_SECONDS_ROW, VALUE_COLUMN, delaySeconds);
    table->setItem(DURATION_SECONDS_ROW, NAME_COLUMN, new TableWidgetItem("durationSeconds"));
    table->setItem(DURATION_SECONDS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DURATION_SECONDS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DURATION_SECONDS_ROW, VALUE_COLUMN, durationSeconds);
    table->setItem(SECONDS_ELAPSED_ROW, NAME_COLUMN, new TableWidgetItem("secondsElapsed"));
    table->setItem(SECONDS_ELAPSED_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SECONDS_ELAPSED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SECONDS_ELAPSED_ROW, VALUE_COLUMN, secondsElapsed);
    table->setItem(MODIFIER_ROW, NAME_COLUMN, new TableWidgetItem("modifier"));
    table->setItem(MODIFIER_ROW, TYPE_COLUMN, new TableWidgetItem("hkbModifier", Qt::AlignCenter));
    table->setItem(MODIFIER_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(MODIFIER_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_MODIFIERS_TABLE_TIP));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void DelayedModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(delaySeconds, SIGNAL(editingFinished()), this, SLOT(setDelaySeconds()), Qt::UniqueConnection);
        connect(durationSeconds, SIGNAL(editingFinished()), this, SLOT(setDurationSeconds()), Qt::UniqueConnection);
        connect(secondsElapsed, SIGNAL(editingFinished()), this, SLOT(setSecondsElapsed()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(delaySeconds, SIGNAL(editingFinished()), this, SLOT(setDelaySeconds()));
        disconnect(durationSeconds, SIGNAL(editingFinished()), this, SLOT(setDurationSeconds()));
        disconnect(secondsElapsed, SIGNAL(editingFinished()), this, SLOT(setSecondsElapsed()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void DelayedModifierUI::connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties && modifiers){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(modifiers, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(modifiers, SIGNAL(elementSelected(int,QString)), this, SLOT(setModifier(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewModifiers(int,QString,QStringList)), modifiers, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("DelayedModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void DelayedModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_DELAYED_MODIFIER){
            bsData = static_cast<hkbDelayedModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            delaySeconds->setValue(bsData->getDelaySeconds());
            durationSeconds->setValue(bsData->getDurationSeconds());
            secondsElapsed->setValue(bsData->getSecondsElapsed());
            auto item = table->item(MODIFIER_ROW, VALUE_COLUMN);
            auto mod = bsData->getModifier();
            (mod) ? item->setText(mod->getName()) : item->setText("NONE");
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(DELAY_SECONDS_ROW, BINDING_COLUMN, varBind, "delaySeconds", table, bsData);
            UIHelper::loadBinding(DURATION_SECONDS_ROW, BINDING_COLUMN, varBind, "durationSeconds", table, bsData);
            UIHelper::loadBinding(SECONDS_ELAPSED_ROW, BINDING_COLUMN, varBind, "secondsElapsed", table, bsData);
        }else{
            LogFile::writeToLog(QString("DelayedModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("DelayedModifierUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void DelayedModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("DelayedModifierUI::setName(): The data is nullptr!!");
    }
}

void DelayedModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("DelayedModifierUI::setEnable(): The data is nullptr!!");
}

void DelayedModifierUI::setDelaySeconds(){
    (bsData) ? bsData->setDelaySeconds(delaySeconds->value()) : LogFile::writeToLog("DelayedModifierUI::setDelaySeconds(): The data is nullptr!!");
}

void DelayedModifierUI::setDurationSeconds(){
    (bsData) ? bsData->setDurationSeconds(durationSeconds->value()) : LogFile::writeToLog("DelayedModifierUI::setDurationSeconds(): The data is nullptr!!");
}

void DelayedModifierUI::setSecondsElapsed(){
    (bsData) ? bsData->setSecondsElapsed(secondsElapsed->value()) : LogFile::writeToLog("DelayedModifierUI::setSecondsElapsed(): The data is nullptr!!");
}

void DelayedModifierUI::setModifier(int index, const QString & name){
    UIHelper::setModifier(index, name, bsData, bsData->getModifier(), NULL_SIGNATURE, HkxObject::TYPE_MODIFIER, table, behaviorView, MODIFIER_ROW, VALUE_COLUMN);
}

void DelayedModifierUI::setBindingVariable(int index, const QString & name){
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
        case DELAY_SECONDS_ROW:
            checkisproperty(DELAY_SECONDS_ROW, "delaySeconds", VARIABLE_TYPE_REAL); break;
        case DURATION_SECONDS_ROW:
            checkisproperty(DURATION_SECONDS_ROW, "durationSeconds", VARIABLE_TYPE_REAL); break;
        case SECONDS_ELAPSED_ROW:
            checkisproperty(SECONDS_ELAPSED_ROW, "secondsElapsed", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("DelayedModifierUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void DelayedModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("DelayedModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void DelayedModifierUI::viewSelected(int row, int column){
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
            case DELAY_SECONDS_ROW:
                checkisproperty(DELAY_SECONDS_ROW, "delaySeconds"); break;
            case DURATION_SECONDS_ROW:
                checkisproperty(DURATION_SECONDS_ROW, "durationSeconds"); break;
            case SECONDS_ELAPSED_ROW:
                checkisproperty(SECONDS_ELAPSED_ROW, "secondsElapsed"); break;
            }
        }else if (row == MODIFIER_ROW && column == VALUE_COLUMN){
            emit viewModifiers(static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifier()) + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("DelayedModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void DelayedModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("delaySeconds", DELAY_SECONDS_ROW);
            setname("durationSeconds", DURATION_SECONDS_ROW);
            setname("secondsElapsed", SECONDS_ELAPSED_ROW);
        }
    }else{
        LogFile::writeToLog("DelayedModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void DelayedModifierUI::modifierRenamed(const QString &name, int index){
    if (bsData){
        index--;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifier())){
            table->item(MODIFIER_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("DelayedModifierUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void DelayedModifierUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}
