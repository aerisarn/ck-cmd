#include "eventdrivenmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbeventdrivenmodifier.h"
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
#define ACTIVATE_EVENT_ID_ROW 2
#define DEACTIVATE_EVENT_ID_ROW 3
#define ACTIVE_BY_DEFAULT_ROW 4
#define MODIFIER_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList EventDrivenModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

EventDrivenModifierUI::EventDrivenModifierUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      activeByDefault(new CheckBox)
{
    setTitle("hkbEventDrivenModifier");
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
    table->setItem(ACTIVATE_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("activateEventId"));
    table->setItem(ACTIVATE_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ACTIVATE_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ACTIVATE_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(DEACTIVATE_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("deactivateEventId"));
    table->setItem(DEACTIVATE_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(DEACTIVATE_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(DEACTIVATE_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(ACTIVE_BY_DEFAULT_ROW, NAME_COLUMN, new TableWidgetItem("activeByDefault"));
    table->setItem(ACTIVE_BY_DEFAULT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ACTIVE_BY_DEFAULT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ACTIVE_BY_DEFAULT_ROW, VALUE_COLUMN, activeByDefault);
    table->setItem(MODIFIER_ROW, NAME_COLUMN, new TableWidgetItem("modifier"));
    table->setItem(MODIFIER_ROW, TYPE_COLUMN, new TableWidgetItem("hkbModifier", Qt::AlignCenter));
    table->setItem(MODIFIER_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(MODIFIER_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_MODIFIERS_TABLE_TIP));
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void EventDrivenModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(activeByDefault, SIGNAL(released()), this, SLOT(setActiveByDefault()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(activeByDefault, SIGNAL(released()), this, SLOT(setActiveByDefault()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void EventDrivenModifierUI::connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && modifiers && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(modifiers, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(modifiers, SIGNAL(elementSelected(int,QString)), this, SLOT(setModifier(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(eventTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewModifiers(int,QString,QStringList)), modifiers, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void EventDrivenModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_EVENT_DRIVEN_MODIFIER){
            bsData = static_cast<hkbEventDrivenModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto seteventtext = [&](QTableWidgetItem *item, const QString & text){
                (text != "") ? item->setText(text) : item->setText("NONE");
            };
            auto eventName = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getActivateEventId());
            seteventtext(table->item(ACTIVATE_EVENT_ID_ROW, VALUE_COLUMN), eventName);
            eventName = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getDeactivateEventId());
            seteventtext(table->item(DEACTIVATE_EVENT_ID_ROW, VALUE_COLUMN), eventName);
            activeByDefault->setChecked(bsData->getActiveByDefault());
            auto mod = bsData->getModifier();
            auto item = table->item(MODIFIER_ROW, VALUE_COLUMN);
            (mod) ? item->setText(mod->getName()) : item->setText("NONE");
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ACTIVE_BY_DEFAULT_ROW, BINDING_COLUMN, varBind, "activeByDefault", table, bsData);
        }else{
            LogFile::writeToLog(QString("EventDrivenModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void EventDrivenModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::setName(): The data is nullptr!!");
    }
}

void EventDrivenModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("EventDrivenModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void EventDrivenModifierUI::setActivateEventId(int index, const QString & name){
    if (bsData){
        bsData->setActivateEventId(index - 1);
        table->item(ACTIVATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::setActivateEventId(): The data is nullptr!!");
    }
}

void EventDrivenModifierUI::setDeactivateEventId(int index, const QString & name){
    if (bsData){
        bsData->setDeactivateEventId(index - 1);
        table->item(DEACTIVATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::setDeactivateEventId(): The data is nullptr!!");
    }
}

void EventDrivenModifierUI::setActiveByDefault(){
    (bsData) ? bsData->setActiveByDefault(activeByDefault->isChecked()) : LogFile::writeToLog("EventDrivenModifierUI::setActiveByDefault(): The 'bsData' pointer is nullptr!!");
}

void EventDrivenModifierUI::eventTableElementSelected(int index, const QString &name){
    switch (table->currentRow()){
    case ACTIVATE_EVENT_ID_ROW:
        setActivateEventId(index, name); break;
    case DEACTIVATE_EVENT_ID_ROW:
        setDeactivateEventId(index, name); break;
    default:
        WARNING_MESSAGE("EventDrivenModifierUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void EventDrivenModifierUI::setModifier(int index, const QString & name){
    UIHelper::setModifier(index, name, bsData, bsData->getModifier(), NULL_SIGNATURE, HkxObject::TYPE_MODIFIER, table, behaviorView, MODIFIER_ROW, VALUE_COLUMN);
}

void EventDrivenModifierUI::setBindingVariable(int index, const QString & name){
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
        case ACTIVE_BY_DEFAULT_ROW:
            checkisproperty(ACTIVE_BY_DEFAULT_ROW, "activeByDefault", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void EventDrivenModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("EventDrivenModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void EventDrivenModifierUI::viewSelected(int row, int column){
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
            case ACTIVE_BY_DEFAULT_ROW:
                checkisproperty(ACTIVE_BY_DEFAULT_ROW, "activeByDefault"); break;
            }
        }else if (row == MODIFIER_ROW && column == VALUE_COLUMN){
            emit viewModifiers(static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifier()) + 1, QString(), QStringList());
        }else if (column == VALUE_COLUMN){
            auto index = -1;
            switch (row){
            case ACTIVATE_EVENT_ID_ROW:
                index = bsData->getActivateEventId(); break;
            case DEACTIVATE_EVENT_ID_ROW:
                index = bsData->getDeactivateEventId(); break;
            }
            emit viewEvents(++index, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void EventDrivenModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("activeByDefault", ACTIVE_BY_DEFAULT_ROW);
        }
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void EventDrivenModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (index == bsData->getActivateEventId()) ? table->item(ACTIVATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
        (index == bsData->getDeactivateEventId()) ? table->item(DEACTIVATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void EventDrivenModifierUI::modifierRenamed(const QString &name, int index){
    if (bsData){
        index--;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData->getModifier())){
            table->item(MODIFIER_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("EventDrivenModifierUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void EventDrivenModifierUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}
