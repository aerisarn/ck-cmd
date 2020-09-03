#include "bsragdollcontactlistenermodifierui.h"


#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/hkxclasses/behavior/modifiers/BSRagdollContactListenerModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/boneindexarrayui.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 5

#define NAME_ROW 0
#define ENABLE_ROW 1
#define CONTACT_EVENT_ID_ROW 2
#define CONTACT_EVENT_PAYLOAD_ROW 3
#define BONES_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSRagdollContactListenerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSRagdollContactListenerModifierUI::BSRagdollContactListenerModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      groupBox(new QGroupBox),
      table(new TableWidget(QColor(Qt::white))),
      boneIndexUI(new BoneIndexArrayUI),
      name(new LineEdit),
      enable(new CheckBox),
      contactEventPayload(new QLineEdit),
      bones(new CheckButtonCombo("Edit"))
{
    groupBox->setTitle("BSRagdollContactListenerModifierUI");
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
    table->setItem(CONTACT_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("contactEventId"));
    table->setItem(CONTACT_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(CONTACT_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(CONTACT_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(CONTACT_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("contactEventPayload"));
    table->setItem(CONTACT_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(CONTACT_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(CONTACT_EVENT_PAYLOAD_ROW, VALUE_COLUMN, contactEventPayload);
    table->setItem(BONES_ROW, NAME_COLUMN, new TableWidgetItem("bones"));
    table->setItem(BONES_ROW, TYPE_COLUMN, new TableWidgetItem("hkbBoneIndexArray", Qt::AlignCenter));
    table->setItem(BONES_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(BONES_ROW, VALUE_COLUMN, bones);
    topLyt->addWidget(table, 0, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(boneIndexUI);
    toggleSignals(true);
}

void BSRagdollContactListenerModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(contactEventPayload, SIGNAL(editingFinished()), this, SLOT(setContactEventPayload()), Qt::UniqueConnection);
        connect(bones, SIGNAL(pressed()), this, SLOT(viewBones()), Qt::UniqueConnection);
        connect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(contactEventPayload, SIGNAL(editingFinished()), this, SLOT(setContactEventPayload()));
        disconnect(bones, SIGNAL(pressed()), this, SLOT(viewBones()));
        disconnect(bones, SIGNAL(enabled(bool)), this, SLOT(toggleBones(bool)));
        disconnect(boneIndexUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSRagdollContactListenerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events, GenericTableWidget *ragdollBones){
    if (variables && properties && events && ragdollBones){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(ragdollBones, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setContactEventId(int,QString)), Qt::UniqueConnection);
        connect(ragdollBones, SIGNAL(elementSelected(int,QString)), boneIndexUI, SLOT(setRagdollBone(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(boneIndexUI, SIGNAL(viewRagdollBones(int)), ragdollBones, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_RAGDOLL_CONTACT_LISTENER_MODIFIER){
            bsData = static_cast<BSRagdollContactListenerModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getContactEventID());
            (text != "") ? table->item(CONTACT_EVENT_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(CONTACT_EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
            auto payload = bsData->getContactEventPayload();
            (payload) ? contactEventPayload->setText(payload->getData()) : contactEventPayload->setText("");
            (bsData->getBones()) ? (bones->setChecked(true), bones->setText("Edit")) : (bones->setChecked(false), bones->setText("nullptr"));
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
        }else{
            LogFile::writeToLog("BSRagdollContactListenerModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSRagdollContactListenerModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::setName(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSRagdollContactListenerModifierUI::setEnable(): The data is nullptr!!");
}

void BSRagdollContactListenerModifierUI::setContactEventId(int index, const QString & name){
    if (bsData){
        bsData->setContactEventID(index - 1);
        table->item(CONTACT_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::setcontactEventId(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::setContactEventPayload(){
    if (bsData){
        auto payload = bsData->getContactEventPayload();
        if (contactEventPayload->text() != ""){
            if (payload){
                payload->setData(contactEventPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), contactEventPayload->text());
                bsData->setContactEventPayload(payload);
            }
        }else{
            bsData->setContactEventPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::setcontactEventPayload(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::toggleBones(bool enable){
    if (bsData){
        if (!enable){
            bsData->setBones(nullptr);
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }else if (!bsData->bones.data()){
            bsData->setBones(new hkbBoneIndexArray(bsData->getParentFile()));
            bones->setText("Edit");
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::toggleBones(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::viewBones(){
    if (bsData){
        boneIndexUI->loadData(bsData->getBones());
        setCurrentIndex(BONE_INDEX_WIDGET);
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::viewBones(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::viewSelected(int row, int column){
    if (bsData){
        auto isProperty = false;
        if (column == BINDING_COLUMN){
            if (row == ENABLE_ROW){
                (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
                selectTableToView(isProperty, "enable");
            }
        }else if (column == VALUE_COLUMN && row == CONTACT_EVENT_ID_ROW){
            emit viewEvents(bsData->getContactEventID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        (index == bsData->getContactEventID()) ? table->item(CONTACT_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto bindIndex = bind->getVariableIndexOfBinding("enable");
            (bindIndex == index) ? table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name) : NULL;
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSRagdollContactListenerModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto isProperty = false;
        auto row = table->currentRow();
        if (row == ENABLE_ROW){
            (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : NULL;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, "enable", VARIABLE_TYPE_BOOL, isProperty, table, bsData);
        }
    }else{
        LogFile::writeToLog("BSRagdollContactListenerModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
