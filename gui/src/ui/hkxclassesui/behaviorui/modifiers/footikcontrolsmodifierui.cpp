#include "footikcontrolsmodifierui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbFootIkControlsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/legui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 17

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ON_OFF_GAIN_ROW 2
#define GROUND_ASCENDING_GAIN_ROW 3
#define GROUND_DESCENDING_GAIN_ROW 4
#define FOOT_PLANTED_GAIN_ROW 5
#define FOOT_RAISED_GAIN_ROW 6
#define FOOT_UNLOCK_GAIN_ROW 7
#define WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW 8
#define ERROR_UP_DOWN_BIAS_ROW 9
#define ALIGN_WORLD_FROM_MODEL_GAIN_ROW 10
#define HIP_ORIENTATION_GAIN_ROW 11
#define MAX_KNEE_ANGLE_DIFFERENCE_ROW 12
#define ANKLE_ORIENTATION_GAIN_ROW 13
#define ERROR_OUT_TRANSLATION_ROW 14
#define ALIGN_WITH_GROUND_ROTATION 15
#define ADD_LEG_ROW 16

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList FootIkControlsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

FootIkControlsModifierUI::FootIkControlsModifierUI()
    : bsData(nullptr),
      legUI(new LegUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      onOffGain(new DoubleSpinBox),
      groundAscendingGain(new DoubleSpinBox),
      groundDescendingGain(new DoubleSpinBox),
      footPlantedGain(new DoubleSpinBox),
      footRaisedGain(new DoubleSpinBox),
      footUnlockGain(new DoubleSpinBox),
      worldFromModelFeedbackGain(new DoubleSpinBox),
      errorUpDownBias(new DoubleSpinBox),
      alignWorldFromModelGain(new DoubleSpinBox),
      hipOrientationGain(new DoubleSpinBox),
      maxKneeAngleDifference(new DoubleSpinBox),
      ankleOrientationGain(new DoubleSpinBox),
      errorOutTranslation(new QuadVariableWidget),
      alignWithGroundRotation(new QuadVariableWidget)
{
    groupBox->setTitle("hkbFootIkControlsModifier");
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
    table->setItem(ON_OFF_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("onOffGain"));
    table->setItem(ON_OFF_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ON_OFF_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ON_OFF_GAIN_ROW, VALUE_COLUMN, onOffGain);
    table->setItem(GROUND_ASCENDING_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("groundAscendingGain"));
    table->setItem(GROUND_ASCENDING_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GROUND_ASCENDING_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GROUND_ASCENDING_GAIN_ROW, VALUE_COLUMN, groundAscendingGain);
    table->setItem(GROUND_DESCENDING_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("groundDescendingGain"));
    table->setItem(GROUND_DESCENDING_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GROUND_DESCENDING_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GROUND_DESCENDING_GAIN_ROW, VALUE_COLUMN, groundDescendingGain);
    table->setItem(FOOT_PLANTED_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("footPlantedGain"));
    table->setItem(FOOT_PLANTED_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOOT_PLANTED_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FOOT_PLANTED_GAIN_ROW, VALUE_COLUMN, footPlantedGain);
    table->setItem(FOOT_RAISED_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("footRaisedGain"));
    table->setItem(FOOT_RAISED_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOOT_RAISED_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FOOT_RAISED_GAIN_ROW, VALUE_COLUMN, footRaisedGain);
    table->setItem(FOOT_UNLOCK_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("footUnlockGain"));
    table->setItem(FOOT_UNLOCK_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(FOOT_UNLOCK_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FOOT_UNLOCK_GAIN_ROW, VALUE_COLUMN, footUnlockGain);
    table->setItem(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("worldFromModelFeedbackGain"));
    table->setItem(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, VALUE_COLUMN, worldFromModelFeedbackGain);
    table->setItem(ERROR_UP_DOWN_BIAS_ROW, NAME_COLUMN, new TableWidgetItem("errorUpDownBias"));
    table->setItem(ERROR_UP_DOWN_BIAS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ERROR_UP_DOWN_BIAS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ERROR_UP_DOWN_BIAS_ROW, VALUE_COLUMN, errorUpDownBias);
    table->setItem(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("alignWorldFromModelGain"));
    table->setItem(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, VALUE_COLUMN, alignWorldFromModelGain);
    table->setItem(HIP_ORIENTATION_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("hipOrientationGain"));
    table->setItem(HIP_ORIENTATION_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(HIP_ORIENTATION_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HIP_ORIENTATION_GAIN_ROW, VALUE_COLUMN, hipOrientationGain);
    table->setItem(MAX_KNEE_ANGLE_DIFFERENCE_ROW, NAME_COLUMN, new TableWidgetItem("maxKneeAngleDifference"));
    table->setItem(MAX_KNEE_ANGLE_DIFFERENCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MAX_KNEE_ANGLE_DIFFERENCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MAX_KNEE_ANGLE_DIFFERENCE_ROW, VALUE_COLUMN, maxKneeAngleDifference);
    table->setItem(ANKLE_ORIENTATION_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("ankleOrientationGain"));
    table->setItem(ANKLE_ORIENTATION_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ANKLE_ORIENTATION_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANKLE_ORIENTATION_GAIN_ROW, VALUE_COLUMN, ankleOrientationGain);
    table->setItem(ERROR_OUT_TRANSLATION_ROW, NAME_COLUMN, new TableWidgetItem("errorOutTranslation"));
    table->setItem(ERROR_OUT_TRANSLATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(ERROR_OUT_TRANSLATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ERROR_OUT_TRANSLATION_ROW, VALUE_COLUMN, errorOutTranslation);
    table->setItem(ALIGN_WITH_GROUND_ROTATION, NAME_COLUMN, new TableWidgetItem("alignWithGroundRotation"));
    table->setItem(ALIGN_WITH_GROUND_ROTATION, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(ALIGN_WITH_GROUND_ROTATION, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ALIGN_WITH_GROUND_ROTATION, VALUE_COLUMN, alignWithGroundRotation);
    table->setItem(ADD_LEG_ROW, NAME_COLUMN, new TableWidgetItem("Add Leg", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new Leg"));
    table->setItem(ADD_LEG_ROW, TYPE_COLUMN, new TableWidgetItem("hkLeg", Qt::AlignCenter));
    table->setItem(ADD_LEG_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Leg", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected Leg"));
    table->setItem(ADD_LEG_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Leg", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected Leg"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(legUI);
    toggleSignals(true);
}

void FootIkControlsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(onOffGain, SIGNAL(editingFinished()), this, SLOT(setOnOffGain()), Qt::UniqueConnection);
        connect(groundAscendingGain, SIGNAL(editingFinished()), this, SLOT(setGroundAscendingGain()), Qt::UniqueConnection);
        connect(groundDescendingGain, SIGNAL(editingFinished()), this, SLOT(setGroundDescendingGain()), Qt::UniqueConnection);
        connect(footPlantedGain, SIGNAL(editingFinished()), this, SLOT(setFootPlantedGain()), Qt::UniqueConnection);
        connect(footRaisedGain, SIGNAL(editingFinished()), this, SLOT(setFootRaisedGain()), Qt::UniqueConnection);
        connect(footUnlockGain, SIGNAL(editingFinished()), this, SLOT(setFootUnlockGain()), Qt::UniqueConnection);
        connect(worldFromModelFeedbackGain, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelFeedbackGain()), Qt::UniqueConnection);
        connect(errorUpDownBias, SIGNAL(editingFinished()), this, SLOT(setErrorUpDownBias()), Qt::UniqueConnection);
        connect(alignWorldFromModelGain, SIGNAL(editingFinished()), this, SLOT(setAlignWorldFromModelGain()), Qt::UniqueConnection);
        connect(hipOrientationGain, SIGNAL(editingFinished()), this, SLOT(setHipOrientationGain()), Qt::UniqueConnection);
        connect(maxKneeAngleDifference, SIGNAL(editingFinished()), this, SLOT(setMaxKneeAngleDifference()), Qt::UniqueConnection);
        connect(ankleOrientationGain, SIGNAL(editingFinished()), this, SLOT(setAnkleOrientationGain()), Qt::UniqueConnection);
        connect(errorOutTranslation, SIGNAL(editingFinished()), this, SLOT(setErrorOutTranslation()), Qt::UniqueConnection);
        connect(alignWithGroundRotation, SIGNAL(editingFinished()), this, SLOT(setAlignWithGroundRotation()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(legUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(legUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(legUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(onOffGain, SIGNAL(editingFinished()), this, SLOT(setOnOffGain()));
        disconnect(groundAscendingGain, SIGNAL(editingFinished()), this, SLOT(setGroundAscendingGain()));
        disconnect(groundDescendingGain, SIGNAL(editingFinished()), this, SLOT(setGroundDescendingGain()));
        disconnect(footPlantedGain, SIGNAL(editingFinished()), this, SLOT(setFootPlantedGain()));
        disconnect(footRaisedGain, SIGNAL(editingFinished()), this, SLOT(setFootRaisedGain()));
        disconnect(footUnlockGain, SIGNAL(editingFinished()), this, SLOT(setFootUnlockGain()));
        disconnect(worldFromModelFeedbackGain, SIGNAL(editingFinished()), this, SLOT(setWorldFromModelFeedbackGain()));
        disconnect(errorUpDownBias, SIGNAL(editingFinished()), this, SLOT(setErrorUpDownBias()));
        disconnect(alignWorldFromModelGain, SIGNAL(editingFinished()), this, SLOT(setAlignWorldFromModelGain()));
        disconnect(hipOrientationGain, SIGNAL(editingFinished()), this, SLOT(setHipOrientationGain()));
        disconnect(maxKneeAngleDifference, SIGNAL(editingFinished()), this, SLOT(setMaxKneeAngleDifference()));
        disconnect(ankleOrientationGain, SIGNAL(editingFinished()), this, SLOT(setAnkleOrientationGain()));
        disconnect(errorOutTranslation, SIGNAL(editingFinished()), this, SLOT(setErrorOutTranslation()));
        disconnect(alignWithGroundRotation, SIGNAL(editingFinished()), this, SLOT(setAlignWithGroundRotation()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(legUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(legUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(legUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void FootIkControlsModifierUI::addLeg(){
    (bsData) ? bsData->addLeg(), loadDynamicTableRows() : LogFile::writeToLog("FootIkControlsModifierUI::addFoot(): The data is nullptr!!");
}

void FootIkControlsModifierUI::removeLeg(int index){
    (bsData) ? bsData->removeLeg(index), loadDynamicTableRows() : LogFile::writeToLog("FootIkControlsModifierUI::removeFoot(): The data is nullptr!!");
}

void FootIkControlsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_FOOT_IK_CONTROLS_MODIFIER){
            bsData = static_cast<hkbFootIkControlsModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            onOffGain->setValue(bsData->getOnOffGain());
            groundAscendingGain->setValue(bsData->getGroundAscendingGain());
            groundDescendingGain->setValue(bsData->getGroundDescendingGain());
            footPlantedGain->setValue(bsData->getFootPlantedGain());
            footRaisedGain->setValue(bsData->getFootRaisedGain());
            footUnlockGain->setValue(bsData->getFootUnlockGain());
            worldFromModelFeedbackGain->setValue(bsData->getWorldFromModelFeedbackGain());
            errorUpDownBias->setValue(bsData->getErrorUpDownBias());
            alignWorldFromModelGain->setValue(bsData->getAlignWorldFromModelGain());
            hipOrientationGain->setValue(bsData->getHipOrientationGain());
            maxKneeAngleDifference->setValue(bsData->getMaxKneeAngleDifference());
            ankleOrientationGain->setValue(bsData->getAnkleOrientationGain());
            errorOutTranslation->setValue(bsData->getErrorOutTranslation());
            alignWithGroundRotation->setValue(bsData->getAlignWithGroundRotation());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ON_OFF_GAIN_ROW, BINDING_COLUMN, varBind, "onOffGain", table, bsData);
            UIHelper::loadBinding(GROUND_ASCENDING_GAIN_ROW, BINDING_COLUMN, varBind, "groundAscendingGain", table, bsData);
            UIHelper::loadBinding(GROUND_DESCENDING_GAIN_ROW, BINDING_COLUMN, varBind, "groundDescendingGain", table, bsData);
            UIHelper::loadBinding(FOOT_PLANTED_GAIN_ROW, BINDING_COLUMN, varBind, "footPlantedGain", table, bsData);
            UIHelper::loadBinding(FOOT_RAISED_GAIN_ROW, BINDING_COLUMN, varBind, "footRaisedGain", table, bsData);
            UIHelper::loadBinding(FOOT_UNLOCK_GAIN_ROW, BINDING_COLUMN, varBind, "footUnlockGain", table, bsData);
            UIHelper::loadBinding(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, BINDING_COLUMN, varBind, "worldFromModelFeedbackGain", table, bsData);
            UIHelper::loadBinding(ERROR_UP_DOWN_BIAS_ROW, BINDING_COLUMN, varBind, "errorUpDownBias", table, bsData);
            UIHelper::loadBinding(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, BINDING_COLUMN, varBind, "alignWorldFromModelGain", table, bsData);
            UIHelper::loadBinding(HIP_ORIENTATION_GAIN_ROW, BINDING_COLUMN, varBind, "hipOrientationGain", table, bsData);
            UIHelper::loadBinding(MAX_KNEE_ANGLE_DIFFERENCE_ROW, BINDING_COLUMN, varBind, "maxKneeAngleDifference", table, bsData);
            UIHelper::loadBinding(ANKLE_ORIENTATION_GAIN_ROW, BINDING_COLUMN, varBind, "ankleOrientationGain", table, bsData);
            UIHelper::loadBinding(ERROR_OUT_TRANSLATION_ROW, BINDING_COLUMN, varBind, "errorOutTranslation", table, bsData);
            UIHelper::loadBinding(ALIGN_WITH_GROUND_ROTATION, BINDING_COLUMN, varBind, "alignWithGroundRotation", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("FootIkControlsModifierUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void FootIkControlsModifierUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_LEG_ROW + bsData->getNumberOfLegs() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        for (auto i = ADD_LEG_ROW + 1, j = 0; j < bsData->getNumberOfLegs(); i++, j++){
            UIHelper::setRowItems(i, "Leg "+QString::number(j), "hkLeg", "Remove", "Edit", "Double click to remove this Leg", "Double click to edit this Leg", table);
        }
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void FootIkControlsModifierUI::setBindingVariable(int index, const QString & name){
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
        case ON_OFF_GAIN_ROW:
            checkisproperty(ON_OFF_GAIN_ROW, "onOffGain", VARIABLE_TYPE_REAL); break;
        case GROUND_ASCENDING_GAIN_ROW:
            checkisproperty(GROUND_ASCENDING_GAIN_ROW, "groundAscendingGain", VARIABLE_TYPE_REAL); break;
        case GROUND_DESCENDING_GAIN_ROW:
            checkisproperty(GROUND_DESCENDING_GAIN_ROW, "groundDescendingGain", VARIABLE_TYPE_REAL); break;
        case FOOT_PLANTED_GAIN_ROW:
            checkisproperty(FOOT_PLANTED_GAIN_ROW, "footPlantedGain", VARIABLE_TYPE_REAL); break;
        case FOOT_RAISED_GAIN_ROW:
            checkisproperty(FOOT_RAISED_GAIN_ROW, "footRaisedGain", VARIABLE_TYPE_REAL); break;
        case FOOT_UNLOCK_GAIN_ROW:
            checkisproperty(FOOT_UNLOCK_GAIN_ROW, "footUnlockGain", VARIABLE_TYPE_REAL); break;
        case WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW:
            checkisproperty(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, "worldFromModelFeedbackGain", VARIABLE_TYPE_REAL); break;
        case ERROR_UP_DOWN_BIAS_ROW:
            checkisproperty(ERROR_UP_DOWN_BIAS_ROW, "errorUpDownBias", VARIABLE_TYPE_REAL); break;
        case ALIGN_WORLD_FROM_MODEL_GAIN_ROW:
            checkisproperty(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, "alignWorldFromModelGain", VARIABLE_TYPE_REAL); break;
        case HIP_ORIENTATION_GAIN_ROW:
            checkisproperty(HIP_ORIENTATION_GAIN_ROW, "hipOrientationGain", VARIABLE_TYPE_REAL); break;
        case MAX_KNEE_ANGLE_DIFFERENCE_ROW:
            checkisproperty(MAX_KNEE_ANGLE_DIFFERENCE_ROW, "maxKneeAngleDifference", VARIABLE_TYPE_REAL); break;
        case ANKLE_ORIENTATION_GAIN_ROW:
            checkisproperty(ANKLE_ORIENTATION_GAIN_ROW, "ankleOrientationGain", VARIABLE_TYPE_REAL); break;
        case ERROR_OUT_TRANSLATION_ROW:
            checkisproperty(ERROR_OUT_TRANSLATION_ROW, "errorOutTranslation", VARIABLE_TYPE_VECTOR4); break;
        case ALIGN_WITH_GROUND_ROTATION:
            checkisproperty(ALIGN_WITH_GROUND_ROTATION, "alignWithGroundRotation", VARIABLE_TYPE_QUATERNION); break;
        }
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}

void FootIkControlsModifierUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void FootIkControlsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::setName(): The data is nullptr!!");
    }
}

void FootIkControlsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("FootIkControlsModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setOnOffGain(){
    (bsData) ? bsData->setOnOffGain(onOffGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setOnOffGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setFootPlantedGain(){
    (bsData) ? bsData->setFootPlantedGain(footPlantedGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setFootPlantedGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setFootRaisedGain(){
    (bsData) ? bsData->setFootRaisedGain(footRaisedGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setFootRaisedGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setFootUnlockGain(){
    (bsData) ? bsData->setFootUnlockGain(footUnlockGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setFootUnlockGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setGroundAscendingGain(){
    (bsData) ? bsData->setGroundAscendingGain(groundAscendingGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setGroundAscendingGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setGroundDescendingGain(){
    (bsData) ? bsData->setGroundDescendingGain(groundDescendingGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setGroundDescendingGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setWorldFromModelFeedbackGain(){
    (bsData) ? bsData->setWorldFromModelFeedbackGain(worldFromModelFeedbackGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setWorldFromModelFeedbackGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setErrorUpDownBias(){
    (bsData) ? bsData->setErrorUpDownBias(errorUpDownBias->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setErrorUpDownBias(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setAlignWorldFromModelGain(){
    (bsData) ? bsData->setAlignWorldFromModelGain(alignWorldFromModelGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setAlignWorldFromModelGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setHipOrientationGain(){
    (bsData) ? bsData->setHipOrientationGain(hipOrientationGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setHipOrientationGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setMaxKneeAngleDifference(){
    (bsData) ? bsData->setMaxKneeAngleDifference(maxKneeAngleDifference->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setMaxKneeAngleDifference(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setAnkleOrientationGain(){
    (bsData) ? bsData->setAnkleOrientationGain(ankleOrientationGain->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setAnkleOrientationGain(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setErrorOutTranslation(){
    (bsData) ? bsData->setErrorOutTranslation(errorOutTranslation->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setErrorOutTranslation(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::setAlignWithGroundRotation(){
    (bsData) ? bsData->setAlignWithGroundRotation(alignWithGroundRotation->value()) : LogFile::writeToLog("FootIkControlsModifierUI::setAlignWithGroundRotation(): The 'bsData' pointer is nullptr!!");
}

void FootIkControlsModifierUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_LEG_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case ENABLE_ROW:
                    checkisproperty(ENABLE_ROW, "enable"); break;
                case ON_OFF_GAIN_ROW:
                    checkisproperty(ON_OFF_GAIN_ROW, "onOffGain"); break;
                case GROUND_ASCENDING_GAIN_ROW:
                    checkisproperty(GROUND_ASCENDING_GAIN_ROW, "groundAscendingGain"); break;
                case GROUND_DESCENDING_GAIN_ROW:
                    checkisproperty(GROUND_DESCENDING_GAIN_ROW, "groundDescendingGain"); break;
                case FOOT_PLANTED_GAIN_ROW:
                    checkisproperty(FOOT_PLANTED_GAIN_ROW, "footPlantedGain"); break;
                case FOOT_RAISED_GAIN_ROW:
                    checkisproperty(FOOT_RAISED_GAIN_ROW, "footRaisedGain"); break;
                case FOOT_UNLOCK_GAIN_ROW:
                    checkisproperty(FOOT_UNLOCK_GAIN_ROW, "footUnlockGain"); break;
                case WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW:
                    checkisproperty(WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW, "worldFromModelFeedbackGain"); break;
                case ERROR_UP_DOWN_BIAS_ROW:
                    checkisproperty(ERROR_UP_DOWN_BIAS_ROW, "errorUpDownBias"); break;
                case ALIGN_WORLD_FROM_MODEL_GAIN_ROW:
                    checkisproperty(ALIGN_WORLD_FROM_MODEL_GAIN_ROW, "alignWorldFromModelGain"); break;
                case HIP_ORIENTATION_GAIN_ROW:
                    checkisproperty(HIP_ORIENTATION_GAIN_ROW, "hipOrientationGain"); break;
                case MAX_KNEE_ANGLE_DIFFERENCE_ROW:
                    checkisproperty(MAX_KNEE_ANGLE_DIFFERENCE_ROW, "maxKneeAngleDifference"); break;
                case ANKLE_ORIENTATION_GAIN_ROW:
                    checkisproperty(ANKLE_ORIENTATION_GAIN_ROW, "ankleOrientationGain"); break;
                case ERROR_OUT_TRANSLATION_ROW:
                    checkisproperty(ERROR_OUT_TRANSLATION_ROW, "errorOutTranslation"); break;
                case ALIGN_WITH_GROUND_ROTATION:
                    checkisproperty(ALIGN_WITH_GROUND_ROTATION, "alignWithGroundRotation"); break;
                }
            }
        }else if (row == ADD_LEG_ROW && column == NAME_COLUMN){
            addLeg();
        }else if (row > ADD_LEG_ROW && row < ADD_LEG_ROW + bsData->getNumberOfLegs() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfLegs() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    legUI->loadData(((BehaviorFile *)(bsData->getParentFile())), &bsData->legs[result], bsData, result);
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the range \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeLeg(result);
                    }
                }
            }else{
                LogFile::writeToLog("FootIkControlsModifierUI::viewSelectedChild(): Invalid index of range to view!!");
            }
        }
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void FootIkControlsModifierUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setBindingVariable(index, name); break;
    case CHILD_WIDGET:
        legUI->setBindingVariable(index, name); break;
    default:
        WARNING_MESSAGE("FootIkControlsModifierUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void FootIkControlsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && events && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), legUI, SLOT(setEventId(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void FootIkControlsModifierUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("FootIkControlsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void FootIkControlsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (currentIndex() == MAIN_WIDGET){
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("enable", ENABLE_ROW);
                setname("onOffGain", ON_OFF_GAIN_ROW);
                setname("groundAscendingGain", GROUND_ASCENDING_GAIN_ROW);
                setname("groundDescendingGain", GROUND_DESCENDING_GAIN_ROW);
                setname("footPlantedGain", FOOT_PLANTED_GAIN_ROW);
                setname("footRaisedGain", FOOT_RAISED_GAIN_ROW);
                setname("footUnlockGain", FOOT_UNLOCK_GAIN_ROW);
                setname("worldFromModelFeedbackGain", WORLD_FROM_MODEL_FEEDBACK_GAIN_ROW);
                setname("errorUpDownBias", ERROR_UP_DOWN_BIAS_ROW);
                setname("alignWorldFromModelGain", ALIGN_WORLD_FROM_MODEL_GAIN_ROW);
                setname("hipOrientationGain", HIP_ORIENTATION_GAIN_ROW);
                setname("maxKneeAngleDifference", MAX_KNEE_ANGLE_DIFFERENCE_ROW);
                setname("ankleOrientationGain", ANKLE_ORIENTATION_GAIN_ROW);
                setname("errorOutTranslation", ERROR_OUT_TRANSLATION_ROW);
                setname("alignWithGroundRotation", ALIGN_WITH_GROUND_ROTATION);
            }
        }else{
            legUI->variableRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("FootIkControlsModifierUI::variableRenamed(): The data is nullptr!!");
    }
}

