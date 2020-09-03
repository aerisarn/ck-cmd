#include "bseventondeactivatemodifierui.h"


#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSEventOnDeactivateModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define ENABLE_ROW 1
#define EVENT_ID_ROW 2
#define EVENT_PAYLOAD_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSEventOnDeactivateModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSEventOnDeactivateModifierUI::BSEventOnDeactivateModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      eventPayload(new QLineEdit)
{
    setTitle("BSEventOnDeactivateModifierUI");
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
    table->setItem(EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventId"));
    table->setItem(EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventPayload"));
    table->setItem(EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_PAYLOAD_ROW, VALUE_COLUMN, eventPayload);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSEventOnDeactivateModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(eventPayload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(eventPayload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSEventOnDeactivateModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setEventId(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_EVENT_ON_DEACTIVATE_MODIFIER){
            bsData = static_cast<BSEventOnDeactivateModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventID());
            (text != "") ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
            auto payload = static_cast<hkbStringEventPayload *>(bsData->getEventPayload());
            (payload) ? eventPayload->setText(payload->getData()) : eventPayload->setText("");
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
        }else{
            LogFile::writeToLog("BSEventOnDeactivateModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSEventOnDeactivateModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::setName(): The data is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSEventOnDeactivateModifierUI::setEnable(): The data is nullptr!!");
}

void BSEventOnDeactivateModifierUI::setEventId(int index, const QString & name){
    if (bsData){
        bsData->setEventID(index - 1);
        table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::seteventId(): The data is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::setEventPayload(){
    if (bsData){
        auto payload = bsData->getEventPayload();
        if (eventPayload->text() != ""){
            if (payload){
                payload->setData(eventPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventPayload->text());
                bsData->setEventPayload(payload);
            }
        }else{
            bsData->setEventPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::seteventPayload(): The data is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::viewSelected(int row, int column){
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
            }
        }else if (column == VALUE_COLUMN && row == EVENT_ID_ROW){
            emit viewEvents(bsData->getEventID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        (index == bsData->getEventID()) ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto bindIndex = bind->getVariableIndexOfBinding("enable");
            (bindIndex == index) ? table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name) : NULL;
        }
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventOnDeactivateModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto isProperty = false;
        auto row = table->currentRow();
        switch (row){
        case ENABLE_ROW:
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("BSEventOnDeactivateModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
