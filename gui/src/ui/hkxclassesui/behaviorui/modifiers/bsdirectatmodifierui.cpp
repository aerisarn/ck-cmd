#include "bsdirectatmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bsdirectatmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 21

#define NAME_ROW 0
#define ENABLE_ROW 1
#define DIRECT_AT_TARGET_ROW 2
#define SOURCE_BONE_INDEX_ROW 3
#define START_BONE_INDEX_ROW 4
#define END_BONE_INDEX_ROW 5
#define LIMIT_HEADING_DEGREES_ROW 6
#define LIMIT_PITCH_DEGREES_ROW 7
#define OFFSET_HEADING_DEGREES_ROW 8
#define OFFSET_PITCH_DEGREES_ROW 9
#define ON_GAIN_ROW 10
#define OFF_GAIN_ROW 11
#define TARGET_LOCATION_ROW 12
#define USER_INFO_ROW 13
#define DIRECT_AT_CAMERA_ROW 14
#define DIRECT_AT_CAMERA_X_ROW 15
#define DIRECT_AT_CAMERA_Y_ROW 16
#define DIRECT_AT_CAMERA_Z_ROW 17
#define ACTIVE_ROW 18
#define CURRENT_HEADING_OFFSET_ROW 19
#define CURRENT_PITCH_OFFSET_ROW 20

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSDirectAtModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSDirectAtModifierUI::BSDirectAtModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      directAtTarget(new CheckBox),
      sourceBoneIndex(new ComboBox),
      startBoneIndex(new ComboBox),
      endBoneIndex(new ComboBox),
      limitHeadingDegrees(new DoubleSpinBox),
      limitPitchDegrees(new DoubleSpinBox),
      offsetHeadingDegrees(new DoubleSpinBox),
      offsetPitchDegrees(new DoubleSpinBox),
      onGain(new DoubleSpinBox),
      offGain(new DoubleSpinBox),
      targetLocation(new QuadVariableWidget),
      userInfo(new SpinBox),
      directAtCamera(new CheckBox),
      directAtCameraX(new DoubleSpinBox),
      directAtCameraY(new DoubleSpinBox),
      directAtCameraZ(new DoubleSpinBox),
      active(new CheckBox),
      currentHeadingOffset(new DoubleSpinBox),
      currentPitchOffset(new DoubleSpinBox)
{
    setTitle("BSDirectAtModifier");
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
    table->setItem(DIRECT_AT_TARGET_ROW, NAME_COLUMN, new TableWidgetItem("directAtTarget"));
    table->setItem(DIRECT_AT_TARGET_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(DIRECT_AT_TARGET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECT_AT_TARGET_ROW, VALUE_COLUMN, directAtTarget);
    table->setItem(SOURCE_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("sourceBoneIndex"));
    table->setItem(SOURCE_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(SOURCE_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SOURCE_BONE_INDEX_ROW, VALUE_COLUMN, sourceBoneIndex);
    table->setItem(START_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("startBoneIndex"));
    table->setItem(START_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(START_BONE_INDEX_ROW, VALUE_COLUMN, startBoneIndex);
    table->setItem(END_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("endBoneIndex"));
    table->setItem(END_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(END_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(END_BONE_INDEX_ROW, VALUE_COLUMN, endBoneIndex);
    table->setItem(LIMIT_HEADING_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitHeadingDegrees"));
    table->setItem(LIMIT_HEADING_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_HEADING_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_HEADING_DEGREES_ROW, VALUE_COLUMN, limitHeadingDegrees);
    table->setItem(LIMIT_PITCH_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitPitchDegrees"));
    table->setItem(LIMIT_PITCH_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_PITCH_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_PITCH_DEGREES_ROW, VALUE_COLUMN, limitPitchDegrees);
    table->setItem(OFFSET_HEADING_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("offsetHeadingDegrees"));
    table->setItem(OFFSET_HEADING_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFFSET_HEADING_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFFSET_HEADING_DEGREES_ROW, VALUE_COLUMN, offsetHeadingDegrees);
    table->setItem(OFFSET_PITCH_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("offsetPitchDegrees"));
    table->setItem(OFFSET_PITCH_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFFSET_PITCH_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFFSET_PITCH_DEGREES_ROW, VALUE_COLUMN, offsetPitchDegrees);
    table->setItem(ON_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("onGain"));
    table->setItem(ON_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ON_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ON_GAIN_ROW, VALUE_COLUMN, onGain);
    table->setItem(OFF_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("offGain"));
    table->setItem(OFF_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFF_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFF_GAIN_ROW, VALUE_COLUMN, offGain);
    table->setItem(TARGET_LOCATION_ROW, NAME_COLUMN, new TableWidgetItem("targetLocation"));
    table->setItem(TARGET_LOCATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_LOCATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_LOCATION_ROW, VALUE_COLUMN, targetLocation);
    table->setItem(USER_INFO_ROW, NAME_COLUMN, new TableWidgetItem("userInfo"));
    table->setItem(USER_INFO_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(USER_INFO_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(USER_INFO_ROW, VALUE_COLUMN, userInfo);
    table->setItem(DIRECT_AT_CAMERA_ROW, NAME_COLUMN, new TableWidgetItem("directAtCamera"));
    table->setItem(DIRECT_AT_CAMERA_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(DIRECT_AT_CAMERA_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECT_AT_CAMERA_ROW, VALUE_COLUMN, directAtCamera);
    table->setItem(DIRECT_AT_CAMERA_X_ROW, NAME_COLUMN, new TableWidgetItem("directAtCameraX"));
    table->setItem(DIRECT_AT_CAMERA_X_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DIRECT_AT_CAMERA_X_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECT_AT_CAMERA_X_ROW, VALUE_COLUMN, directAtCameraX);
    table->setItem(DIRECT_AT_CAMERA_Y_ROW, NAME_COLUMN, new TableWidgetItem("directAtCameraY"));
    table->setItem(DIRECT_AT_CAMERA_Y_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DIRECT_AT_CAMERA_Y_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECT_AT_CAMERA_Y_ROW, VALUE_COLUMN, directAtCameraY);
    table->setItem(DIRECT_AT_CAMERA_Z_ROW, NAME_COLUMN, new TableWidgetItem("directAtCameraZ"));
    table->setItem(DIRECT_AT_CAMERA_Z_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DIRECT_AT_CAMERA_Z_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DIRECT_AT_CAMERA_Z_ROW, VALUE_COLUMN, directAtCameraZ);
    table->setItem(ACTIVE_ROW, NAME_COLUMN, new TableWidgetItem("active"));
    table->setItem(ACTIVE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ACTIVE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ACTIVE_ROW, VALUE_COLUMN, active);
    table->setItem(CURRENT_HEADING_OFFSET_ROW, NAME_COLUMN, new TableWidgetItem("currentHeadingOffset"));
    table->setItem(CURRENT_HEADING_OFFSET_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CURRENT_HEADING_OFFSET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CURRENT_HEADING_OFFSET_ROW, VALUE_COLUMN, currentHeadingOffset);
    table->setItem(CURRENT_PITCH_OFFSET_ROW, NAME_COLUMN, new TableWidgetItem("currentPitchOffset"));
    table->setItem(CURRENT_PITCH_OFFSET_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CURRENT_PITCH_OFFSET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CURRENT_PITCH_OFFSET_ROW, VALUE_COLUMN, currentPitchOffset);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSDirectAtModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(directAtTarget, SIGNAL(released()), this, SLOT(setDirectAtTarget()), Qt::UniqueConnection);
        connect(sourceBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceBoneIndex(int)), Qt::UniqueConnection);
        connect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)), Qt::UniqueConnection);
        connect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)), Qt::UniqueConnection);
        connect(limitHeadingDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitHeadingDegrees()), Qt::UniqueConnection);
        connect(limitPitchDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitPitchDegrees()), Qt::UniqueConnection);
        connect(offsetHeadingDegrees, SIGNAL(editingFinished()), this, SLOT(setOffsetHeadingDegrees()), Qt::UniqueConnection);
        connect(offsetPitchDegrees, SIGNAL(editingFinished()), this, SLOT(setOffsetPitchDegrees()), Qt::UniqueConnection);
        connect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()), Qt::UniqueConnection);
        connect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()), Qt::UniqueConnection);
        connect(targetLocation, SIGNAL(editingFinished()), this, SLOT(setTargetLocation()), Qt::UniqueConnection);
        connect(userInfo, SIGNAL(editingFinished()), this, SLOT(setUserInfo()), Qt::UniqueConnection);
        connect(directAtCamera, SIGNAL(released()), this, SLOT(setDirectAtCamera()), Qt::UniqueConnection);
        connect(directAtCameraX, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraX()), Qt::UniqueConnection);
        connect(directAtCameraY, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraY()), Qt::UniqueConnection);
        connect(directAtCameraZ, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraZ()), Qt::UniqueConnection);
        connect(active, SIGNAL(released()), this, SLOT(setActive()), Qt::UniqueConnection);
        connect(currentHeadingOffset, SIGNAL(editingFinished()), this, SLOT(setCurrentHeadingOffset()), Qt::UniqueConnection);
        connect(currentPitchOffset, SIGNAL(editingFinished()), this, SLOT(setCurrentPitchOffset()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(directAtTarget, SIGNAL(released()), this, SLOT(setDirectAtTarget()));
        disconnect(sourceBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceBoneIndex(int)));
        disconnect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)));
        disconnect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)));
        disconnect(limitHeadingDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitHeadingDegrees()));
        disconnect(limitPitchDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitPitchDegrees()));
        disconnect(offsetHeadingDegrees, SIGNAL(editingFinished()), this, SLOT(setOffsetHeadingDegrees()));
        disconnect(offsetPitchDegrees, SIGNAL(editingFinished()), this, SLOT(setOffsetPitchDegrees()));
        disconnect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()));
        disconnect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()));
        disconnect(targetLocation, SIGNAL(editingFinished()), this, SLOT(setTargetLocation()));
        disconnect(userInfo, SIGNAL(editingFinished()), this, SLOT(setUserInfo()));
        disconnect(directAtCamera, SIGNAL(released()), this, SLOT(setDirectAtCamera()));
        disconnect(directAtCameraX, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraX()));
        disconnect(directAtCameraY, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraY()));
        disconnect(directAtCameraZ, SIGNAL(editingFinished()), this, SLOT(setDirectAtCameraZ()));
        disconnect(active, SIGNAL(released()), this, SLOT(setActive()));
        disconnect(currentHeadingOffset, SIGNAL(editingFinished()), this, SLOT(setCurrentHeadingOffset()));
        disconnect(currentPitchOffset, SIGNAL(editingFinished()), this, SLOT(setCurrentPitchOffset()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSDirectAtModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSDirectAtModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_DIRECT_AT_MODIFIER){
            bsData = static_cast<BSDirectAtModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            directAtTarget->setChecked(bsData->getDirectAtTarget());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(startBoneIndex, bsData->getStartBoneIndex() + 1);
            loadbones(sourceBoneIndex, bsData->getSourceBoneIndex() + 1);
            loadbones(endBoneIndex, bsData->getEndBoneIndex() + 1);
            limitHeadingDegrees->setValue(bsData->getLimitHeadingDegrees());
            limitPitchDegrees->setValue(bsData->getLimitPitchDegrees());
            offsetHeadingDegrees->setValue(bsData->getOffsetHeadingDegrees());
            offsetPitchDegrees->setValue(bsData->getOffsetPitchDegrees());
            onGain->setValue(bsData->getOnGain());
            offGain->setValue(bsData->getOffGain());
            targetLocation->setValue(bsData->getTargetLocation());
            userInfo->setValue(bsData->getUserInfo());
            directAtCamera->setChecked(bsData->getDirectAtCamera());
            directAtCameraX->setValue(bsData->getDirectAtCameraX());
            directAtCameraY->setValue(bsData->getDirectAtCameraY());
            directAtCameraZ->setValue(bsData->getDirectAtCameraZ());
            active->setChecked(bsData->getActive());
            currentHeadingOffset->setValue(bsData->getCurrentHeadingOffset());
            currentPitchOffset->setValue(bsData->getCurrentPitchOffset());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(DIRECT_AT_TARGET_ROW, BINDING_COLUMN, varBind, "directAtTarget", table, bsData);
            UIHelper::loadBinding(SOURCE_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "sourceBoneIndex", table, bsData);
            UIHelper::loadBinding(START_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "startBoneIndex", table, bsData);
            UIHelper::loadBinding(END_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "endBoneIndex", table, bsData);
            UIHelper::loadBinding(LIMIT_HEADING_DEGREES_ROW, BINDING_COLUMN, varBind, "limitHeadingDegrees", table, bsData);
            UIHelper::loadBinding(LIMIT_PITCH_DEGREES_ROW, BINDING_COLUMN, varBind, "limitPitchDegrees", table, bsData);
            UIHelper::loadBinding(OFFSET_HEADING_DEGREES_ROW, BINDING_COLUMN, varBind, "offsetHeadingDegrees", table, bsData);
            UIHelper::loadBinding(OFFSET_PITCH_DEGREES_ROW, BINDING_COLUMN, varBind, "offsetPitchDegrees", table, bsData);
            UIHelper::loadBinding(ON_GAIN_ROW, BINDING_COLUMN, varBind, "onGain", table, bsData);
            UIHelper::loadBinding(OFF_GAIN_ROW, BINDING_COLUMN, varBind, "offGain", table, bsData);
            UIHelper::loadBinding(TARGET_LOCATION_ROW, BINDING_COLUMN, varBind, "targetLocation", table, bsData);
            UIHelper::loadBinding(USER_INFO_ROW, BINDING_COLUMN, varBind, "userInfo", table, bsData);
            UIHelper::loadBinding(DIRECT_AT_CAMERA_ROW, BINDING_COLUMN, varBind, "directAtCamera", table, bsData);
            UIHelper::loadBinding(DIRECT_AT_CAMERA_X_ROW, BINDING_COLUMN, varBind, "directAtCameraX", table, bsData);
            UIHelper::loadBinding(DIRECT_AT_CAMERA_Y_ROW, BINDING_COLUMN, varBind, "directAtCameraY", table, bsData);
            UIHelper::loadBinding(DIRECT_AT_CAMERA_Z_ROW, BINDING_COLUMN, varBind, "directAtCameraZ", table, bsData);
            UIHelper::loadBinding(ACTIVE_ROW, BINDING_COLUMN, varBind, "active", table, bsData);
            UIHelper::loadBinding(CURRENT_HEADING_OFFSET_ROW, BINDING_COLUMN, varBind, "currentHeadingOffset", table, bsData);
            UIHelper::loadBinding(CURRENT_PITCH_OFFSET_ROW, BINDING_COLUMN, varBind, "currentPitchOffset", table, bsData);
        }else{
            LogFile::writeToLog("BSDirectAtModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSDirectAtModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::setName(): The data is nullptr!!");
    }
}

void BSDirectAtModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSDirectAtModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setDirectAtTarget(){
    (bsData) ? bsData->setDirectAtTarget(directAtTarget->isChecked()) : LogFile::writeToLog("BSDirectAtModifierUI::setDirectAtTarget(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setSourceBoneIndex(int index){
    (bsData) ? bsData->setSourceBoneIndex(index - 1) : LogFile::writeToLog("BSDirectAtModifierUI::setSourceBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setStartBoneIndex(int index){
    (bsData) ? bsData->setStartBoneIndex(index - 1) : LogFile::writeToLog("BSDirectAtModifierUI::setStartBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setEndBoneIndex(int index){
    (bsData) ? bsData->setEndBoneIndex(index - 1) : LogFile::writeToLog("BSDirectAtModifierUI::setEndBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setLimitHeadingDegrees(){
    (bsData) ? bsData->setLimitHeadingDegrees(limitHeadingDegrees->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setLimitHeadingDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setLimitPitchDegrees(){
    (bsData) ? bsData->setLimitPitchDegrees(limitPitchDegrees->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setLimitPitchDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setOffsetHeadingDegrees(){
    (bsData) ? bsData->setOffsetHeadingDegrees(offsetHeadingDegrees->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setOffsetHeadingDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setOffsetPitchDegrees(){
    (bsData) ? bsData->setOffsetPitchDegrees(offsetPitchDegrees->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setOffsetPitchDegrees(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setOnGain(){
    (bsData) ? bsData->setOnGain(onGain->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setOnGain(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setOffGain(){
    (bsData) ? bsData->setOffGain(offGain->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setOffGain(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setTargetLocation(){
    (bsData) ? bsData->setTargetLocation(targetLocation->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setTargetLocation(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setUserInfo(){
    (bsData) ? bsData->setUserInfo(userInfo->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setUserInfo(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setDirectAtCamera(){
    (bsData) ? bsData->setDirectAtCamera(directAtCamera->isChecked()) : LogFile::writeToLog("BSDirectAtModifierUI::setDirectAtCamera(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setDirectAtCameraX(){
    (bsData) ? bsData->setDirectAtCameraX(directAtCameraX->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setDirectAtCameraX(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setDirectAtCameraY(){
    (bsData) ? bsData->setDirectAtCameraY(directAtCameraY->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setDirectAtCameraY(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setDirectAtCameraZ(){
    (bsData) ? bsData->setDirectAtCameraZ(directAtCameraZ->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setDirectAtCameraZ(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setActive(){
    (bsData) ? bsData->setActive(active->isChecked()) : LogFile::writeToLog("BSDirectAtModifierUI::setActive(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setCurrentHeadingOffset(){
    (bsData) ? bsData->setCurrentHeadingOffset(currentHeadingOffset->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setCurrentHeadingOffset(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::setCurrentPitchOffset(){
    (bsData) ? bsData->setCurrentPitchOffset(currentPitchOffset->value()) : LogFile::writeToLog("BSDirectAtModifierUI::setCurrentPitchOffset(): The 'bsData' pointer is nullptr!!");
}

void BSDirectAtModifierUI::viewSelected(int row, int column){
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
            case DIRECT_AT_TARGET_ROW:
                checkisproperty(DIRECT_AT_TARGET_ROW, "directAtTarget"); break;
            case SOURCE_BONE_INDEX_ROW:
                checkisproperty(SOURCE_BONE_INDEX_ROW, "sourceBoneIndex"); break;
            case START_BONE_INDEX_ROW:
                checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex"); break;
            case END_BONE_INDEX_ROW:
                checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex"); break;
            case LIMIT_HEADING_DEGREES_ROW:
                checkisproperty(LIMIT_HEADING_DEGREES_ROW, "limitHeadingDegrees"); break;
            case LIMIT_PITCH_DEGREES_ROW:
                checkisproperty(LIMIT_PITCH_DEGREES_ROW, "limitPitchDegrees"); break;
            case OFFSET_HEADING_DEGREES_ROW:
                checkisproperty(OFFSET_HEADING_DEGREES_ROW, "offsetHeadingDegrees"); break;
            case OFFSET_PITCH_DEGREES_ROW:
                checkisproperty(OFFSET_PITCH_DEGREES_ROW, "offsetPitchDegrees"); break;
            case ON_GAIN_ROW:
                checkisproperty(ON_GAIN_ROW, "onGain"); break;
            case OFF_GAIN_ROW:
                checkisproperty(OFF_GAIN_ROW, "offGain"); break;
            case TARGET_LOCATION_ROW:
                checkisproperty(TARGET_LOCATION_ROW, "targetLocation"); break;
            case USER_INFO_ROW:
                checkisproperty(USER_INFO_ROW, "userInfo"); break;
            case DIRECT_AT_CAMERA_ROW:
                checkisproperty(DIRECT_AT_CAMERA_ROW, "directAtCamera"); break;
            case DIRECT_AT_CAMERA_X_ROW:
                checkisproperty(DIRECT_AT_CAMERA_X_ROW, "directAtCameraX"); break;
            case DIRECT_AT_CAMERA_Y_ROW:
                checkisproperty(DIRECT_AT_CAMERA_Y_ROW, "directAtCameraY"); break;
            case DIRECT_AT_CAMERA_Z_ROW:
                checkisproperty(DIRECT_AT_CAMERA_Z_ROW, "directAtCameraZ"); break;
            case ACTIVE_ROW:
                checkisproperty(ACTIVE_ROW, "active"); break;
            case CURRENT_HEADING_OFFSET_ROW:
                checkisproperty(CURRENT_HEADING_OFFSET_ROW, "currentHeadingOffset"); break;
            case CURRENT_PITCH_OFFSET_ROW:
                checkisproperty(CURRENT_PITCH_OFFSET_ROW, "currentPitchOffset"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSDirectAtModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSDirectAtModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSDirectAtModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("directAtTarget", DIRECT_AT_TARGET_ROW);
            setname("sourceBoneIndex", SOURCE_BONE_INDEX_ROW);
            setname("startBoneIndex", START_BONE_INDEX_ROW);
            setname("endBoneIndex", END_BONE_INDEX_ROW);
            setname("limitHeadingDegrees", LIMIT_HEADING_DEGREES_ROW);
            setname("limitPitchDegrees", LIMIT_PITCH_DEGREES_ROW);
            setname("offsetHeadingDegrees", OFFSET_HEADING_DEGREES_ROW);
            setname("offsetPitchDegrees", OFFSET_PITCH_DEGREES_ROW);
            setname("onGain", ON_GAIN_ROW);
            setname("offGain", OFF_GAIN_ROW);
            setname("targetLocation", TARGET_LOCATION_ROW);
            setname("userInfo", USER_INFO_ROW);
            setname("directAtCamera", DIRECT_AT_CAMERA_ROW);
            setname("directAtCameraX", DIRECT_AT_CAMERA_X_ROW);
            setname("directAtCameraY", DIRECT_AT_CAMERA_Y_ROW);
            setname("directAtCameraZ", DIRECT_AT_CAMERA_Z_ROW);
            setname("active", ACTIVE_ROW);
            setname("currentHeadingOffset", CURRENT_HEADING_OFFSET_ROW);
            setname("currentPitchOffset", CURRENT_PITCH_OFFSET_ROW);
        }
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSDirectAtModifierUI::setBindingVariable(int index, const QString &name){
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
        case DIRECT_AT_TARGET_ROW:
            checkisproperty(DIRECT_AT_TARGET_ROW, "directAtTarget", VARIABLE_TYPE_BOOL); break;
        case SOURCE_BONE_INDEX_ROW:
            checkisproperty(SOURCE_BONE_INDEX_ROW, "sourceBoneIndex", VARIABLE_TYPE_INT32); break;
        case START_BONE_INDEX_ROW:
            checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex", VARIABLE_TYPE_INT32); break;
        case END_BONE_INDEX_ROW:
            checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex", VARIABLE_TYPE_INT32); break;
        case LIMIT_HEADING_DEGREES_ROW:
            checkisproperty(LIMIT_HEADING_DEGREES_ROW, "limitHeadingDegrees", VARIABLE_TYPE_REAL); break;
        case LIMIT_PITCH_DEGREES_ROW:
            checkisproperty(LIMIT_PITCH_DEGREES_ROW, "limitPitchDegrees", VARIABLE_TYPE_REAL); break;
        case OFFSET_HEADING_DEGREES_ROW:
            checkisproperty(OFFSET_HEADING_DEGREES_ROW, "offsetHeadingDegrees", VARIABLE_TYPE_REAL); break;
        case OFFSET_PITCH_DEGREES_ROW:
            checkisproperty(OFFSET_PITCH_DEGREES_ROW, "offsetPitchDegrees", VARIABLE_TYPE_REAL); break;
        case ON_GAIN_ROW:
            checkisproperty(ON_GAIN_ROW, "onGain", VARIABLE_TYPE_REAL); break;
        case OFF_GAIN_ROW:
            checkisproperty(OFF_GAIN_ROW, "offGain", VARIABLE_TYPE_REAL); break;
        case TARGET_LOCATION_ROW:
            checkisproperty(TARGET_LOCATION_ROW, "targetLocation", VARIABLE_TYPE_VECTOR4); break;
        case USER_INFO_ROW:
            checkisproperty(USER_INFO_ROW, "userInfo", VARIABLE_TYPE_INT32); break;
        case DIRECT_AT_CAMERA_ROW:
            checkisproperty(DIRECT_AT_CAMERA_ROW, "directAtCamera", VARIABLE_TYPE_BOOL); break;
        case DIRECT_AT_CAMERA_X_ROW:
            checkisproperty(DIRECT_AT_CAMERA_X_ROW, "directAtCameraX", VARIABLE_TYPE_REAL); break;
        case DIRECT_AT_CAMERA_Y_ROW:
            checkisproperty(DIRECT_AT_CAMERA_Y_ROW, "directAtCameraY", VARIABLE_TYPE_REAL); break;
        case DIRECT_AT_CAMERA_Z_ROW:
            checkisproperty(DIRECT_AT_CAMERA_Z_ROW, "directAtCameraZ", VARIABLE_TYPE_REAL); break;
        case ACTIVE_ROW:
            checkisproperty(ACTIVE_ROW, "active", VARIABLE_TYPE_REAL); break;
        case CURRENT_HEADING_OFFSET_ROW:
            checkisproperty(CURRENT_HEADING_OFFSET_ROW, "currentHeadingOffset", VARIABLE_TYPE_REAL); break;
        case CURRENT_PITCH_OFFSET_ROW:
            checkisproperty(CURRENT_PITCH_OFFSET_ROW, "currentPitchOffset", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSDirectAtModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
