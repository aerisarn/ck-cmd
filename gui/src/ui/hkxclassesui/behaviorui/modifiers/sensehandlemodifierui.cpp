#include "sensehandlemodifierui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbsensehandlemodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/rangesui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 18

#define NAME_ROW 0
#define ENABLE_ROW 1
#define SENSOR_LOCAL_OFFSET_ROW 2
#define HANDLE_OUT_ROW 3
#define HANDLE_IN_ROW 4
#define LOCAL_FRAME_NAME_ROW 5
#define SENSOR_LOCAL_FRAME_NAME_ROW 6
#define MIN_DISTANCE_ROW 7
#define MAX_DISTANCE_ROW 8
#define DISTANCE_OUT_ROW 9
#define COLLISION_FILTER_INFO_ROW 10
#define SENSOR_RAGDOLL_BONE_INDEX_ROW 11
#define SENSOR_ANIMATION_BONE_INDEX_ROW 12
#define SENSING_MODE_ROW 13
#define EXTRAPOLATE_SENSOR_POSITION_ROW 14
#define KEEP_FIRST_SENSED_HANDLE_ROW 15
#define FOUND_HANDLE_OUT_ROW 16
#define ADD_RANGE_ROW 17

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList SenseHandleModifierUI::collisionLayers = {
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

const QStringList SenseHandleModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

SenseHandleModifierUI::SenseHandleModifierUI()
    : bsData(nullptr),
      rangeUI(new RangesUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      sensorLocalOffset(new QuadVariableWidget),
      localFrameName(new ComboBox),
      sensorLocalFrameName(new ComboBox),
      minDistance(new DoubleSpinBox),
      maxDistance(new DoubleSpinBox),
      distanceOut(new DoubleSpinBox),
      collisionFilterInfo(new ComboBox),
      sensorRagdollBoneIndex(new ComboBox),
      sensorAnimationBoneIndex(new ComboBox),
      sensingMode(new ComboBox),
      extrapolateSensorPosition(new CheckBox),
      keepFirstSensedHandle(new CheckBox),
      foundHandleOut(new CheckBox)
{
    collisionFilterInfo->addItems(collisionLayers);
    groupBox->setTitle("hkbSenseHandleModifier");
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
    table->setItem(SENSOR_LOCAL_OFFSET_ROW, NAME_COLUMN, new TableWidgetItem("sensorLocalOffset"));
    table->setItem(SENSOR_LOCAL_OFFSET_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(SENSOR_LOCAL_OFFSET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SENSOR_LOCAL_OFFSET_ROW, VALUE_COLUMN, sensorLocalOffset);
    table->setItem(HANDLE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("handleOut"));
    table->setItem(HANDLE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkHandle", Qt::AlignCenter));
    table->setItem(HANDLE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(HANDLE_OUT_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(HANDLE_IN_ROW, NAME_COLUMN, new TableWidgetItem("handleIn"));
    table->setItem(HANDLE_IN_ROW, TYPE_COLUMN, new TableWidgetItem("hkHandle", Qt::AlignCenter));
    table->setItem(HANDLE_IN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(HANDLE_IN_ROW, VALUE_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(LOCAL_FRAME_NAME_ROW, NAME_COLUMN, new TableWidgetItem("localFrameName"));
    table->setItem(LOCAL_FRAME_NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(LOCAL_FRAME_NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(LOCAL_FRAME_NAME_ROW, VALUE_COLUMN, localFrameName);
    table->setItem(SENSOR_LOCAL_FRAME_NAME_ROW, NAME_COLUMN, new TableWidgetItem("sensorLocalFrameName"));
    table->setItem(SENSOR_LOCAL_FRAME_NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(SENSOR_LOCAL_FRAME_NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SENSOR_LOCAL_FRAME_NAME_ROW, VALUE_COLUMN, sensorLocalFrameName);
    table->setItem(MIN_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("minDistance"));
    table->setItem(MIN_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MIN_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MIN_DISTANCE_ROW, VALUE_COLUMN, minDistance);
    table->setItem(MAX_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("maxDistance"));
    table->setItem(MAX_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MAX_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MAX_DISTANCE_ROW, VALUE_COLUMN, maxDistance);
    table->setItem(DISTANCE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("distanceOut"));
    table->setItem(DISTANCE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DISTANCE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DISTANCE_OUT_ROW, VALUE_COLUMN, distanceOut);
    table->setItem(COLLISION_FILTER_INFO_ROW, NAME_COLUMN, new TableWidgetItem("collisionFilterInfo"));
    table->setItem(COLLISION_FILTER_INFO_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(COLLISION_FILTER_INFO_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COLLISION_FILTER_INFO_ROW, VALUE_COLUMN, collisionFilterInfo);
    table->setItem(SENSOR_RAGDOLL_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("sensorRagdollBoneIndex"));
    table->setItem(SENSOR_RAGDOLL_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(SENSOR_RAGDOLL_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SENSOR_RAGDOLL_BONE_INDEX_ROW, VALUE_COLUMN, sensorRagdollBoneIndex);
    table->setItem(SENSOR_ANIMATION_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("sensorAnimationBoneIndex"));
    table->setItem(SENSOR_ANIMATION_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(SENSOR_ANIMATION_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SENSOR_ANIMATION_BONE_INDEX_ROW, VALUE_COLUMN, sensorAnimationBoneIndex);
    table->setItem(SENSING_MODE_ROW, NAME_COLUMN, new TableWidgetItem("sensingMode"));
    table->setItem(SENSING_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("SensingMode", Qt::AlignCenter));
    table->setItem(SENSING_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SENSING_MODE_ROW, VALUE_COLUMN, sensingMode);
    table->setItem(EXTRAPOLATE_SENSOR_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("extrapolateSensorPosition"));
    table->setItem(EXTRAPOLATE_SENSOR_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(EXTRAPOLATE_SENSOR_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(EXTRAPOLATE_SENSOR_POSITION_ROW, VALUE_COLUMN, extrapolateSensorPosition);
    table->setItem(KEEP_FIRST_SENSED_HANDLE_ROW, NAME_COLUMN, new TableWidgetItem("keepFirstSensedHandle"));
    table->setItem(KEEP_FIRST_SENSED_HANDLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(KEEP_FIRST_SENSED_HANDLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(KEEP_FIRST_SENSED_HANDLE_ROW, VALUE_COLUMN, keepFirstSensedHandle);
    table->setItem(FOUND_HANDLE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("foundHandleOut"));
    table->setItem(FOUND_HANDLE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(FOUND_HANDLE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FOUND_HANDLE_OUT_ROW, VALUE_COLUMN, foundHandleOut);
    table->setItem(ADD_RANGE_ROW, NAME_COLUMN, new TableWidgetItem("Add Range", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new range"));
    table->setItem(ADD_RANGE_ROW, TYPE_COLUMN, new TableWidgetItem("hkRange", Qt::AlignCenter));
    table->setItem(ADD_RANGE_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Range", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected range"));
    table->setItem(ADD_RANGE_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Range", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected range"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(rangeUI);
    toggleSignals(true);
}

void SenseHandleModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(sensorLocalOffset, SIGNAL(editingFinished()), this, SLOT(setSensorLocalOffset()), Qt::UniqueConnection);
        connect(localFrameName, SIGNAL(currentIndexChanged(int)), this, SLOT(setLocalFrameName(int)), Qt::UniqueConnection);
        connect(sensorLocalFrameName, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorLocalFrameName(int)), Qt::UniqueConnection);
        connect(minDistance, SIGNAL(editingFinished()), this, SLOT(setMinDistance()), Qt::UniqueConnection);
        connect(maxDistance, SIGNAL(editingFinished()), this, SLOT(setMaxDistance()), Qt::UniqueConnection);
        connect(distanceOut, SIGNAL(editingFinished()), this, SLOT(setDistanceOut()), Qt::UniqueConnection);
        connect(collisionFilterInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCollisionFilterInfo(int)), Qt::UniqueConnection);
        connect(sensorRagdollBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorRagdollBoneIndex(int)), Qt::UniqueConnection);
        connect(sensorAnimationBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorAnimationBoneIndex(int)), Qt::UniqueConnection);
        connect(sensingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensingMode(int)), Qt::UniqueConnection);
        connect(extrapolateSensorPosition, SIGNAL(released()), this, SLOT(setExtrapolateSensorPosition()), Qt::UniqueConnection);
        connect(keepFirstSensedHandle, SIGNAL(released()), this, SLOT(setKeepFirstSensedHandle()), Qt::UniqueConnection);
        connect(foundHandleOut, SIGNAL(released()), this, SLOT(setFoundHandleOut()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(rangeUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(sensorLocalOffset, SIGNAL(editingFinished()), this, SLOT(setSensorLocalOffset()));
        disconnect(localFrameName, SIGNAL(currentIndexChanged(int)), this, SLOT(setLocalFrameName(int)));
        disconnect(sensorLocalFrameName, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorLocalFrameName(int)));
        disconnect(minDistance, SIGNAL(editingFinished()), this, SLOT(setMinDistance()));
        disconnect(maxDistance, SIGNAL(editingFinished()), this, SLOT(setMaxDistance()));
        disconnect(distanceOut, SIGNAL(editingFinished()), this, SLOT(setDistanceOut()));
        disconnect(collisionFilterInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCollisionFilterInfo(int)));
        disconnect(sensorRagdollBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorRagdollBoneIndex(int)));
        disconnect(sensorAnimationBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensorAnimationBoneIndex(int)));
        disconnect(sensingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSensingMode(int)));
        disconnect(extrapolateSensorPosition, SIGNAL(released()), this, SLOT(setExtrapolateSensorPosition()));
        disconnect(keepFirstSensedHandle, SIGNAL(released()), this, SLOT(setKeepFirstSensedHandle()));
        disconnect(foundHandleOut, SIGNAL(released()), this, SLOT(setFoundHandleOut()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(rangeUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(rangeUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(rangeUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(rangeUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void SenseHandleModifierUI::addRange(){
    (bsData) ? bsData->addRange(), loadDynamicTableRows() : LogFile::writeToLog("SenseHandleModifierUI::addRange(): The data is nullptr!!");
}

void SenseHandleModifierUI::removeRange(int index){
    (bsData) ? bsData->removeRange(index), loadDynamicTableRows() : LogFile::writeToLog("SenseHandleModifierUI::removeRange(): The data is nullptr!!");
}

void SenseHandleModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_SENSE_HANDLE_MODIFIER){
            bsData = static_cast<hkbSenseHandleModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            sensorLocalOffset->setValue(bsData->getSensorLocalOffset());
            auto loadlocalframes = [&](ComboBox *combobox, const QString & localframename){
                if (!combobox->count()){
                    auto localFrames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getLocalFrameNames();
                    combobox->insertItems(0, localFrames);
                }
                auto index = combobox->findText(localframename);
                if (index < 0 || index >= combobox->count()){
                    LogFile::writeToLog("The local frame name was not loaded correctly!!!");
                }else{
                    combobox->setCurrentIndex(index);
                }
            };
            loadlocalframes(localFrameName, bsData->getLocalFrameName());
            loadlocalframes(sensorLocalFrameName, bsData->getSensorLocalFrameName());
            minDistance->setValue(bsData->getMinDistance());
            maxDistance->setValue(bsData->getMaxDistance());
            distanceOut->setValue(bsData->getDistanceOut());
            auto collinfo = bsData->getCollisionFilterInfo();
            if (collinfo < collisionLayers.size() && collinfo > -1){
                auto index = collisionFilterInfo->findText(collisionLayers.at(collinfo));
                (index < 0) ? LogFile::writeToLog("SenseHandleModifierUI::loadData(): Invalid collisionFilterInfo!!!") : NULL;
                collisionFilterInfo->setCurrentIndex(index);
            }else{
                LogFile::writeToLog("SenseHandleModifierUI::loadData(): Invalid collisionFilterInfo!!!");
            }
            auto loadbones = [&](ComboBox *combobox, int indextoset, const QStringList & bonenames){
                if (!combobox->count()){
                    auto bones = QStringList("None") + bonenames;
                    combobox->insertItems(0, bones);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(sensorRagdollBoneIndex, bsData->getSensorRagdollBoneIndex() + 1, static_cast<BehaviorFile *>(bsData->getParentFile())->getRagdollBoneNames());
            loadbones(sensorAnimationBoneIndex, bsData->getSensorAnimationBoneIndex() + 1, static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames());
            (!sensingMode->count()) ? sensingMode->insertItems(0, bsData->SensingMode) : NULL;
            sensingMode->setCurrentIndex(bsData->SensingMode.indexOf(bsData->getSensingMode()));
            extrapolateSensorPosition->setChecked(bsData->getExtrapolateSensorPosition());
            keepFirstSensedHandle->setChecked(bsData->getKeepFirstSensedHandle());
            foundHandleOut->setChecked(bsData->getFoundHandleOut());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(SENSOR_LOCAL_OFFSET_ROW, BINDING_COLUMN, varBind, "sensorLocalOffset", table, bsData);
            UIHelper::loadBinding(HANDLE_OUT_ROW, BINDING_COLUMN, varBind, "handleOut", table, bsData);
            UIHelper::loadBinding(HANDLE_IN_ROW, BINDING_COLUMN, varBind, "handleIn", table, bsData);
            UIHelper::loadBinding(MIN_DISTANCE_ROW, BINDING_COLUMN, varBind, "minDistance", table, bsData);
            UIHelper::loadBinding(MAX_DISTANCE_ROW, BINDING_COLUMN, varBind, "maxDistance", table, bsData);
            UIHelper::loadBinding(DISTANCE_OUT_ROW, BINDING_COLUMN, varBind, "distanceOut", table, bsData);
            UIHelper::loadBinding(COLLISION_FILTER_INFO_ROW, BINDING_COLUMN, varBind, "collisionFilterInfo", table, bsData);
            UIHelper::loadBinding(SENSOR_RAGDOLL_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "sensorRagdollBoneIndex", table, bsData);
            UIHelper::loadBinding(SENSOR_ANIMATION_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "sensorAnimationBoneIndex", table, bsData);
            UIHelper::loadBinding(EXTRAPOLATE_SENSOR_POSITION_ROW, BINDING_COLUMN, varBind, "extrapolateSensorPosition", table, bsData);
            UIHelper::loadBinding(KEEP_FIRST_SENSED_HANDLE_ROW, BINDING_COLUMN, varBind, "keepFirstSensedHandle", table, bsData);
            UIHelper::loadBinding(FOUND_HANDLE_OUT_ROW, BINDING_COLUMN, varBind, "foundHandleOut", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("SenseHandleModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void SenseHandleModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_RANGE_ROW + bsData->getNumberOfRanges() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_RANGE_ROW + 1, j = 0; j < bsData->getNumberOfRanges(); i++, j++){
            UIHelper::setRowItems(i, "Range "+QString::number(j), "hkRange", "Remove", "Edit", "Double click to remove this range", "Double click to edit this range", table);
        }
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::setBindingVariable(int index, const QString & name){
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
        case SENSOR_LOCAL_OFFSET_ROW:
            checkisproperty(SENSOR_LOCAL_OFFSET_ROW, "sensorLocalOffset", VARIABLE_TYPE_VECTOR4); break;
        case HANDLE_OUT_ROW:
            checkisproperty(HANDLE_OUT_ROW, "handleOut", VARIABLE_TYPE_POINTER); break;
        case HANDLE_IN_ROW:
            checkisproperty(HANDLE_IN_ROW, "handleIn", VARIABLE_TYPE_POINTER); break;
        case MIN_DISTANCE_ROW:
            checkisproperty(MIN_DISTANCE_ROW, "minDistance", VARIABLE_TYPE_REAL); break;
        case MAX_DISTANCE_ROW:
            checkisproperty(MAX_DISTANCE_ROW, "maxDistance", VARIABLE_TYPE_REAL); break;
        case DISTANCE_OUT_ROW:
            checkisproperty(DISTANCE_OUT_ROW, "distanceOut", VARIABLE_TYPE_REAL); break;
        case COLLISION_FILTER_INFO_ROW:
            checkisproperty(COLLISION_FILTER_INFO_ROW, "collisionFilterInfo", VARIABLE_TYPE_INT32); break;
        case SENSOR_RAGDOLL_BONE_INDEX_ROW:
            checkisproperty(SENSOR_RAGDOLL_BONE_INDEX_ROW, "sensorRagdollBoneIndex", VARIABLE_TYPE_INT32); break;
        case SENSOR_ANIMATION_BONE_INDEX_ROW:
            checkisproperty(SENSOR_ANIMATION_BONE_INDEX_ROW, "sensorAnimationBoneIndex", VARIABLE_TYPE_INT32); break;
        case EXTRAPOLATE_SENSOR_POSITION_ROW:
            checkisproperty(EXTRAPOLATE_SENSOR_POSITION_ROW, "extrapolateSensorPosition", VARIABLE_TYPE_BOOL); break;
        case KEEP_FIRST_SENSED_HANDLE_ROW:
            checkisproperty(KEEP_FIRST_SENSED_HANDLE_ROW, "keepFirstSensedHandle", VARIABLE_TYPE_BOOL); break;
        case FOUND_HANDLE_OUT_ROW:
            checkisproperty(FOUND_HANDLE_OUT_ROW, "foundHandleOut", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void SenseHandleModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::setName(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("SenseHandleModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setSensorLocalOffset(){
    (bsData) ? bsData->setSensorLocalOffset(sensorLocalOffset->value()) : LogFile::writeToLog("SenseHandleModifierUI::setSensorLocalOffset(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setMinDistance(){
    (bsData) ? bsData->setMinDistance(minDistance->value()) : LogFile::writeToLog("SenseHandleModifierUI::setMinDistance(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setMaxDistance(){
    (bsData) ? bsData->setMaxDistance(maxDistance->value()) : LogFile::writeToLog("SenseHandleModifierUI::setMaxDistance(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setDistanceOut(){
    (bsData) ? bsData->setDistanceOut(distanceOut->value()) : LogFile::writeToLog("SenseHandleModifierUI::setDistanceOut(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setLocalFrameName(int index){
    (bsData) ? bsData->setLocalFrameName(index - 1) : LogFile::writeToLog("SenseHandleModifierUI::setLocalFrameName(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setSensorLocalFrameName(int index){
    (bsData) ? bsData->setSensorLocalFrameName(index - 1) : LogFile::writeToLog("SenseHandleModifierUI::setSensorLocalFrameName(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setCollisionFilterInfo(int index){
    (bsData) ? bsData->setCollisionFilterInfo(index - 1) : LogFile::writeToLog("SenseHandleModifierUI::setCollisionFilterInfo(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setSensorRagdollBoneIndex(int index){
    (bsData) ? bsData->setSensorRagdollBoneIndex(index - 1) : LogFile::writeToLog("SenseHandleModifierUI::setSensorRagdollBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setSensorAnimationBoneIndex(int index){
    (bsData) ? bsData->setSensorAnimationBoneIndex(index - 1) : LogFile::writeToLog("SenseHandleModifierUI::setSensorAnimationBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setSensingMode(int index){
    (bsData) ? bsData->setSensingMode(index) : LogFile::writeToLog("SenseHandleModifierUI::setSensingMode(): The data is nullptr!!");
}

void SenseHandleModifierUI::setExtrapolateSensorPosition(){
    (bsData) ? bsData->setExtrapolateSensorPosition(extrapolateSensorPosition->isChecked()) : LogFile::writeToLog("SenseHandleModifierUI::setExtrapolateSensorPosition(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setKeepFirstSensedHandle(){
    (bsData) ? bsData->setKeepFirstSensedHandle(keepFirstSensedHandle->isChecked()) : LogFile::writeToLog("SenseHandleModifierUI::setKeepFirstSensedHandle(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::setFoundHandleOut(){
    (bsData) ? bsData->setFoundHandleOut(foundHandleOut->isChecked()) : LogFile::writeToLog("SenseHandleModifierUI::setFoundHandleOut(): The 'bsData' pointer is nullptr!!");
}

void SenseHandleModifierUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_RANGE_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case ENABLE_ROW:
                    checkisproperty(ENABLE_ROW, "enable"); break;
                case SENSOR_LOCAL_OFFSET_ROW:
                    checkisproperty(SENSOR_LOCAL_OFFSET_ROW, "sensorLocalOffset"); break;
                case HANDLE_OUT_ROW:
                    checkisproperty(HANDLE_OUT_ROW, "handleOut"); break;
                case HANDLE_IN_ROW:
                    checkisproperty(HANDLE_IN_ROW, "handleIn"); break;
                case MIN_DISTANCE_ROW:
                    checkisproperty(MIN_DISTANCE_ROW, "minDistance"); break;
                case MAX_DISTANCE_ROW:
                    checkisproperty(MAX_DISTANCE_ROW, "maxDistance"); break;
                case DISTANCE_OUT_ROW:
                    checkisproperty(DISTANCE_OUT_ROW, "distanceOut"); break;
                case COLLISION_FILTER_INFO_ROW:
                    checkisproperty(COLLISION_FILTER_INFO_ROW, "collisionFilterInfo"); break;
                case SENSOR_RAGDOLL_BONE_INDEX_ROW:
                    checkisproperty(SENSOR_RAGDOLL_BONE_INDEX_ROW, "sensorRagdollBoneIndex"); break;
                case SENSOR_ANIMATION_BONE_INDEX_ROW:
                    checkisproperty(SENSOR_ANIMATION_BONE_INDEX_ROW, "sensorAnimationBoneIndex"); break;
                case EXTRAPOLATE_SENSOR_POSITION_ROW:
                    checkisproperty(EXTRAPOLATE_SENSOR_POSITION_ROW, "extrapolateSensorPosition"); break;
                case KEEP_FIRST_SENSED_HANDLE_ROW:
                    checkisproperty(KEEP_FIRST_SENSED_HANDLE_ROW, "keepFirstSensedHandle"); break;
                case FOUND_HANDLE_OUT_ROW:
                    checkisproperty(FOUND_HANDLE_OUT_ROW, "foundHandleOut"); break;
                }
            }
        }else if (row == ADD_RANGE_ROW && column == NAME_COLUMN){
            addRange();
        }else if (row > ADD_RANGE_ROW && row < ADD_RANGE_ROW + bsData->getNumberOfRanges() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfRanges() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    rangeUI->loadData(((BehaviorFile *)(bsData->getParentFile())), &bsData->ranges[result], bsData, result);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the range \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeRange(result);
                    }
                }
            }else{
                LogFile::writeToLog("SenseHandleModifierUI::viewSelectedChild(): Invalid index of range to view!!");
            }
        }
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case CHILD_WIDGET:
        rangeUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("SenseHandleModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void SenseHandleModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && events && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), rangeUI, SLOT(setEventId(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void SenseHandleModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("SenseHandleModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (currentIndex() == CHILD_WIDGET) ? rangeUI->eventRenamed(name, index) : NULL;
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void SenseHandleModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("sensorLocalOffset", SENSOR_LOCAL_OFFSET_ROW);
            setname("handleOut", HANDLE_OUT_ROW);
            setname("handleIn", HANDLE_IN_ROW);
            setname("minDistance", MIN_DISTANCE_ROW);
            setname("maxDistance", MAX_DISTANCE_ROW);
            setname("distanceOut", DISTANCE_OUT_ROW);
            setname("collisionFilterInfo", COLLISION_FILTER_INFO_ROW);
            setname("sensorRagdollBoneIndex", SENSOR_RAGDOLL_BONE_INDEX_ROW);
            setname("sensorAnimationBoneIndex", SENSOR_ANIMATION_BONE_INDEX_ROW);
            setname("extrapolateSensorPosition", EXTRAPOLATE_SENSOR_POSITION_ROW);
            setname("keepFirstSensedHandle", KEEP_FIRST_SENSED_HANDLE_ROW);
            setname("foundHandleOut", FOUND_HANDLE_OUT_ROW);
        }else{
            rangeUI->variableRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("SenseHandleModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

