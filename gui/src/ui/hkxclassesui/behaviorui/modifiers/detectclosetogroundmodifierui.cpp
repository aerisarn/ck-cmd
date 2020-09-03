#include "detectclosetogroundmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/hkbDetectCloseToGroundModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 9

#define NAME_ROW 0
#define ENABLE_ROW 1
#define CLOSE_TO_GROUND_HEIGHT_ROW 2
#define RAYCAST_DISTANCE_DOWN_ROW 3
#define COLLISION_FILTER_INFO_ROW 4
#define BONE_INDEX_ROW 5
#define ANIM_BONE_INDEX_ROW 6
#define CLOSE_TO_GROUND_EVENT_ID_ROW 7
#define CLOSE_TO_GROUND_EVENT_PAYLOAD_ROW 8

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList DetectCloseToGroundModifierUI::collisionLayers = {
    "NONE",
    "L_UNIDENTIFIED",
    "L_STATIC",
    "L_ANIMSTATIC",
    "L_TRANSPARENT",
    "L_CLUTTER",
    "L_WEAPON",
    "L_PROJECTILE",
    "L_SPELL",
    "L_BIPED",
    "L_TREES",
    "L_PROPS",
    "L_WATER",
    "L_TRIGGER",
    "L_TERRAIN",
    "L_TRAP",
    "L_NONCOLLIDABLE",
    "L_CLOUDTRAP",
    "L_GROUND",
    "L_PORTAL",
    "L_DEBRIS_SMALL",
    "L_DEBRIS_LARGE",
    "L_ACOUSTIC_SPACE",
    "L_ACTOR_ZONE",
    "L_PROJECTILEZONE",
    "L_GASTRAP",
    "L_SHELLCASING",
    "L_TRANSPARENT_SMALL",
    "L_INVISABLE_WALL",
    "L_TRANSPARENT_SMALL_ANIM",
    "L_WARD",
    "L_CHARCONTROLLER",
    "L_STAIRHELPER",
    "L_DEADBIP",
    "L_BIPED_NO_CC",
    "L_AVOIDBOX",
    "L_COLLISIONBOX",
    "L_CAMERASPHERE",
    "L_DOORDETECTION",
    "L_CONEPROJECTILE",
    "L_CAMERA",
    "L_ITEMPICKER",
    "L_LOS",
    "L_PATHINGPICK",
    "L_CUSTOMPICK1",
    "L_CUSTOMPICK2",
    "L_SPELLEXPLOSION",
    "L_DROPPINGPICK",
    "L_DEADACTORZONE",
    "L_CRITTER",
    "L_TRIGGERFALLINGTRAP",
    "L_NAVCUT",
    "L_SPELLTRIGGER",
    "L_LIVING_AND_DEAD_ACTORS",
    "L_DETECTION",
    "L_TRAP_TRIGGER"
};

const QStringList DetectCloseToGroundModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

DetectCloseToGroundModifierUI::DetectCloseToGroundModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      closeToGroundEventPayload(new QLineEdit),
      closeToGroundHeight(new DoubleSpinBox),
      raycastDistanceDown(new DoubleSpinBox),
      collisionFilterInfo(new ComboBox),
      boneIndex(new ComboBox),
      animBoneIndex(new ComboBox)
{
    collisionFilterInfo->insertItems(0, collisionLayers);
    setTitle("hkbDetectCloseToGroundModifier");
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
    table->setItem(CLOSE_TO_GROUND_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("closeToGroundEventId"));
    table->setItem(CLOSE_TO_GROUND_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(CLOSE_TO_GROUND_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(CLOSE_TO_GROUND_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(CLOSE_TO_GROUND_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("closeToGroundEventPayload"));
    table->setItem(CLOSE_TO_GROUND_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(CLOSE_TO_GROUND_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(CLOSE_TO_GROUND_EVENT_PAYLOAD_ROW, VALUE_COLUMN, closeToGroundEventPayload);
    table->setItem(CLOSE_TO_GROUND_HEIGHT_ROW, NAME_COLUMN, new TableWidgetItem("closeToGroundHeight"));
    table->setItem(CLOSE_TO_GROUND_HEIGHT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CLOSE_TO_GROUND_HEIGHT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CLOSE_TO_GROUND_HEIGHT_ROW, VALUE_COLUMN, closeToGroundHeight);
    table->setItem(RAYCAST_DISTANCE_DOWN_ROW, NAME_COLUMN, new TableWidgetItem("raycastDistanceDown"));
    table->setItem(RAYCAST_DISTANCE_DOWN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(RAYCAST_DISTANCE_DOWN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RAYCAST_DISTANCE_DOWN_ROW, VALUE_COLUMN, raycastDistanceDown);
    table->setItem(COLLISION_FILTER_INFO_ROW, NAME_COLUMN, new TableWidgetItem("collisionFilterInfo"));
    table->setItem(COLLISION_FILTER_INFO_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(COLLISION_FILTER_INFO_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COLLISION_FILTER_INFO_ROW, VALUE_COLUMN, collisionFilterInfo);
    table->setItem(BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("boneIndex"));
    table->setItem(BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(BONE_INDEX_ROW, VALUE_COLUMN, boneIndex);
    table->setItem(ANIM_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("animBoneIndex"));
    table->setItem(ANIM_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ANIM_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANIM_BONE_INDEX_ROW, VALUE_COLUMN, animBoneIndex);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void DetectCloseToGroundModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(closeToGroundHeight, SIGNAL(editingFinished()), this, SLOT(setCloseToGroundHeight()), Qt::UniqueConnection);
        connect(raycastDistanceDown, SIGNAL(editingFinished()), this, SLOT(setRaycastDistanceDown()), Qt::UniqueConnection);
        connect(collisionFilterInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCollisionFilterInfo(int)), Qt::UniqueConnection);
        connect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)), Qt::UniqueConnection);
        connect(animBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnimBoneIndex(int)), Qt::UniqueConnection);
        connect(closeToGroundEventPayload, SIGNAL(editingFinished()), this, SLOT(setCloseToGroundEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(closeToGroundHeight, SIGNAL(editingFinished()), this, SLOT(setCloseToGroundHeight()));
        disconnect(raycastDistanceDown, SIGNAL(editingFinished()), this, SLOT(setRaycastDistanceDown()));
        disconnect(collisionFilterInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCollisionFilterInfo(int)));
        disconnect(boneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoneIndex(int)));
        disconnect(animBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnimBoneIndex(int)));
        disconnect(closeToGroundEventPayload, SIGNAL(editingFinished()), this, SLOT(setCloseToGroundEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void DetectCloseToGroundModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setCloseToGroundEventId(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_DETECT_CLOSE_TO_GROUND_MODIFIER){
            bsData = static_cast<hkbDetectCloseToGroundModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getCloseToGroundEventID());
            auto item = table->item(CLOSE_TO_GROUND_EVENT_ID_ROW, VALUE_COLUMN);
            (text != "") ? item->setText(text) : item->setText("None");
            auto payload = bsData->getCloseToGroundEventPayload();
            (payload) ? closeToGroundEventPayload->setText(payload->getData()) : closeToGroundEventPayload->setText("");
            closeToGroundHeight->setValue(bsData->getCloseToGroundHeight());
            raycastDistanceDown->setValue(bsData->getRaycastDistanceDown());
            if (bsData->getCollisionFilterInfo() + 1 >= collisionFilterInfo->count() || bsData->getCollisionFilterInfo() + 1 < 0){
                LogFile::writeToLog("DetectCloseToGroundModifierUI::loadData(): Invalid collisionFilterInfo!!!");
            }else{
                collisionFilterInfo->setCurrentIndex(bsData->getCollisionFilterInfo() + 1);
            }
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(boneIndex, bsData->getBoneIndex() + 1);
            loadbones(animBoneIndex, bsData->getAnimBoneIndex() + 1);
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(CLOSE_TO_GROUND_HEIGHT_ROW, BINDING_COLUMN, varBind, "closeToGroundHeight", table, bsData);
            UIHelper::loadBinding(RAYCAST_DISTANCE_DOWN_ROW, BINDING_COLUMN, varBind, "raycastDistanceDown", table, bsData);
            UIHelper::loadBinding(COLLISION_FILTER_INFO_ROW, BINDING_COLUMN, varBind, "collisionFilterInfo", table, bsData);
            UIHelper::loadBinding(BONE_INDEX_ROW, BINDING_COLUMN, varBind, "boneIndex", table, bsData);
            UIHelper::loadBinding(ANIM_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "animBoneIndex", table, bsData);
        }else{
            LogFile::writeToLog("DetectCloseToGroundModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void DetectCloseToGroundModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::setName(): The data is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void DetectCloseToGroundModifierUI::setCloseToGroundEventId(int index, const QString & name){
    if (bsData){
        bsData->setCloseToGroundEventID(index - 1);
        table->item(CLOSE_TO_GROUND_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::setcloseToGroundEventId(): The data is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::setCloseToGroundEventPayload(){
    if (bsData){
        auto payload = bsData->getCloseToGroundEventPayload();
        if (closeToGroundEventPayload->text() != ""){
            if (payload){
                payload->setData(closeToGroundEventPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), closeToGroundEventPayload->text());
                bsData->setCloseToGroundEventPayload(payload);
            }
        }else{
            bsData->setCloseToGroundEventPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::setcloseToGroundEventPayload(): The data is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::setCloseToGroundHeight(){
    (bsData) ? bsData->setCloseToGroundHeight(closeToGroundHeight->value()) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setCloseToGroundHeight(): The data is nullptr!!");
}

void DetectCloseToGroundModifierUI::setRaycastDistanceDown(){
    (bsData) ? bsData->setRaycastDistanceDown(raycastDistanceDown->value()) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setRaycastDistanceDown(): The data is nullptr!!");
}

void DetectCloseToGroundModifierUI::setCollisionFilterInfo(int index){
    (bsData) ? bsData->setCollisionFilterInfo(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setCollisionFilterInfo(): The 'bsData' pointer is nullptr!!");
}

void DetectCloseToGroundModifierUI::setBoneIndex(int index){
    (bsData) ? bsData->setBoneIndex(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void DetectCloseToGroundModifierUI::setAnimBoneIndex(int index){
    (bsData) ? bsData->setAnimBoneIndex(index - 1) : LogFile::writeToLog("DetectCloseToGroundModifierUI::setAnimBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void DetectCloseToGroundModifierUI::viewSelected(int row, int column){
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
            case CLOSE_TO_GROUND_HEIGHT_ROW:
                checkisproperty(CLOSE_TO_GROUND_HEIGHT_ROW, "closeToGroundHeight"); break;
            case RAYCAST_DISTANCE_DOWN_ROW:
                checkisproperty(RAYCAST_DISTANCE_DOWN_ROW, "raycastDistanceDown"); break;
            case COLLISION_FILTER_INFO_ROW:
                checkisproperty(COLLISION_FILTER_INFO_ROW, "collisionFilterInfo"); break;
            case BONE_INDEX_ROW:
                checkisproperty(BONE_INDEX_ROW, "boneIndex"); break;
            case ANIM_BONE_INDEX_ROW:
                checkisproperty(ANIM_BONE_INDEX_ROW, "animBoneIndex"); break;
            }
        }else if (column == VALUE_COLUMN && row == CLOSE_TO_GROUND_EVENT_ID_ROW){
            emit viewEvents(bsData->getCloseToGroundEventID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("DetectCloseToGroundModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (index == bsData->getCloseToGroundEventID()){
            table->item(CLOSE_TO_GROUND_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("closeToGroundHeight", CLOSE_TO_GROUND_HEIGHT_ROW);
            setname("raycastDistanceDown", RAYCAST_DISTANCE_DOWN_ROW);
            setname("collisionFilterInfo", COLLISION_FILTER_INFO_ROW);
            setname("boneIndex", BONE_INDEX_ROW);
            setname("animBoneIndex", ANIM_BONE_INDEX_ROW);
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void DetectCloseToGroundModifierUI::setBindingVariable(int index, const QString &name){
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
        case CLOSE_TO_GROUND_HEIGHT_ROW:
            checkisproperty(CLOSE_TO_GROUND_HEIGHT_ROW, "closeToGroundHeight", VARIABLE_TYPE_REAL); break;
        case RAYCAST_DISTANCE_DOWN_ROW:
            checkisproperty(RAYCAST_DISTANCE_DOWN_ROW, "raycastDistanceDown", VARIABLE_TYPE_REAL); break;
        case COLLISION_FILTER_INFO_ROW:
            checkisproperty(COLLISION_FILTER_INFO_ROW, "collisionFilterInfo", VARIABLE_TYPE_INT32); break;
        case BONE_INDEX_ROW:
            checkisproperty(BONE_INDEX_ROW, "boneIndex", VARIABLE_TYPE_INT32); break;
        case ANIM_BONE_INDEX_ROW:
            checkisproperty(ANIM_BONE_INDEX_ROW, "animBoneIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("DetectCloseToGroundModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
