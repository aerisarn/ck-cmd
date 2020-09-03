#include "clipgeneratorui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/hkbcliptriggerarray.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/cliptriggerui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 17

#define NAME_ROW 0
#define ANIMATION_NAME_ROW 1
#define CROP_START_AMOUNT_LOCAL_TIME_ROW 2
#define CROP_END_AMOUNT_LOCAL_TIME_ROW 3
#define START_TIME_ROW 4
#define PLAYBACK_SPEED_ROW 5
#define ENFORCED_DURATION_ROW 6
#define USER_CONTROLLED_TIME_FRACTION_ROW 7
#define ANIMATION_BINDING_INDEX_ROW 8
#define MODE_ROW 9
#define FLAG_CONTINUE_MOTION_AT_END_ROW 10
#define FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW 11
#define FLAG_MIRROR_ROW 12
#define FLAG_FORCE_DENSE_POSE_ROW 13
#define FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW 14
#define FLAG_IGNORE_MOTION_ROW 15
#define ADD_TRIGGER_ROW 16

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ClipGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ClipGeneratorUI::ClipGeneratorUI()
    : bsData(nullptr),
      triggerUI(new ClipTriggerUI),
      groupBox(new QGroupBox),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      cropStartAmountLocalTime(new DoubleSpinBox),
      cropEndAmountLocalTime(new DoubleSpinBox),
      startTime(new DoubleSpinBox),
      playbackSpeed(new DoubleSpinBox),
      enforcedDuration(new DoubleSpinBox),
      userControlledTimeFraction(new DoubleSpinBox),
      animationBindingIndex(new SpinBox),
      mode(new ComboBox),
      flagContinueMotionAtEnd(new CheckBox),
      flagSyncHalfCycleInPingPongMode(new CheckBox),
      flagMirror(new CheckBox),
      flagForceDensePose(new CheckBox),
      flagDontConvertAnnotationsToTriggers(new CheckBox),
      flagIgnoreMotion(new CheckBox)
{
    groupBox->setTitle("hkbClipGenerator");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ANIMATION_NAME_ROW, NAME_COLUMN, new TableWidgetItem("animationName"));
    table->setItem(ANIMATION_NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(ANIMATION_NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ANIMATION_NAME_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QColor(Qt::black), VIEW_ANIMATIONS_TABLE_TIP));
    table->setItem(CROP_START_AMOUNT_LOCAL_TIME_ROW, NAME_COLUMN, new TableWidgetItem("cropStartAmountLocalTime"));
    table->setItem(CROP_START_AMOUNT_LOCAL_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CROP_START_AMOUNT_LOCAL_TIME_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CROP_START_AMOUNT_LOCAL_TIME_ROW, VALUE_COLUMN, cropStartAmountLocalTime);
    table->setItem(CROP_END_AMOUNT_LOCAL_TIME_ROW, NAME_COLUMN, new TableWidgetItem("cropEndAmountLocalTime"));
    table->setItem(CROP_END_AMOUNT_LOCAL_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(CROP_END_AMOUNT_LOCAL_TIME_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(CROP_END_AMOUNT_LOCAL_TIME_ROW, VALUE_COLUMN, cropEndAmountLocalTime);
    table->setItem(START_TIME_ROW, NAME_COLUMN, new TableWidgetItem("startTime"));
    table->setItem(START_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(START_TIME_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(START_TIME_ROW, VALUE_COLUMN, startTime);
    table->setItem(PLAYBACK_SPEED_ROW, NAME_COLUMN, new TableWidgetItem("playbackSpeed"));
    table->setItem(PLAYBACK_SPEED_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(PLAYBACK_SPEED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(PLAYBACK_SPEED_ROW, VALUE_COLUMN, playbackSpeed);
    table->setItem(ENFORCED_DURATION_ROW, NAME_COLUMN, new TableWidgetItem("enforcedDuration"));
    table->setItem(ENFORCED_DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ENFORCED_DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENFORCED_DURATION_ROW, VALUE_COLUMN, enforcedDuration);
    table->setItem(USER_CONTROLLED_TIME_FRACTION_ROW, NAME_COLUMN, new TableWidgetItem("userControlledTimeFraction"));
    table->setItem(USER_CONTROLLED_TIME_FRACTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(USER_CONTROLLED_TIME_FRACTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(USER_CONTROLLED_TIME_FRACTION_ROW, VALUE_COLUMN, userControlledTimeFraction);
    table->setItem(ANIMATION_BINDING_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("animationBindingIndex"));
    table->setItem(ANIMATION_BINDING_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ANIMATION_BINDING_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANIMATION_BINDING_INDEX_ROW, VALUE_COLUMN, animationBindingIndex);
    table->setItem(MODE_ROW, NAME_COLUMN, new TableWidgetItem("mode"));
    table->setItem(MODE_ROW, TYPE_COLUMN, new TableWidgetItem("Mode", Qt::AlignCenter));
    table->setItem(MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(MODE_ROW, VALUE_COLUMN, mode);
    table->setItem(FLAG_CONTINUE_MOTION_AT_END_ROW, NAME_COLUMN, new TableWidgetItem("flagContinueMotionAtEnd"));
    table->setItem(FLAG_CONTINUE_MOTION_AT_END_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_CONTINUE_MOTION_AT_END_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_CONTINUE_MOTION_AT_END_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_CONTINUE_MOTION_AT_END_ROW, VALUE_COLUMN, flagContinueMotionAtEnd);
    table->setItem(FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW, NAME_COLUMN, new TableWidgetItem("flagSyncHalfCycleInPingPongMode"));
    table->setItem(FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE_ROW, VALUE_COLUMN, flagSyncHalfCycleInPingPongMode);
    table->setItem(FLAG_MIRROR_ROW, NAME_COLUMN, new TableWidgetItem("flagMirror"));
    table->setItem(FLAG_MIRROR_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_MIRROR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_MIRROR_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_MIRROR_ROW, VALUE_COLUMN, flagMirror);
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, NAME_COLUMN, new TableWidgetItem("flagForceDensePose"));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_FORCE_DENSE_POSE_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_FORCE_DENSE_POSE_ROW, VALUE_COLUMN, flagForceDensePose);
    table->setItem(FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW, NAME_COLUMN, new TableWidgetItem("flagDontConvertAnnotationsToTriggers"));
    table->setItem(FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS_ROW, VALUE_COLUMN, flagDontConvertAnnotationsToTriggers);
    table->setItem(FLAG_IGNORE_MOTION_ROW, NAME_COLUMN, new TableWidgetItem("flagIgnoreMotion"));
    table->setItem(FLAG_IGNORE_MOTION_ROW, TYPE_COLUMN, new TableWidgetItem("Flags", Qt::AlignCenter));
    table->setItem(FLAG_IGNORE_MOTION_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(FLAG_IGNORE_MOTION_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(FLAG_IGNORE_MOTION_ROW, VALUE_COLUMN, flagIgnoreMotion);
    table->setItem(ADD_TRIGGER_ROW, NAME_COLUMN, new TableWidgetItem("Add Trigger", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new clip trigger"));
    table->setItem(ADD_TRIGGER_ROW, TYPE_COLUMN, new TableWidgetItem("hkTrigger", Qt::AlignCenter));
    table->setItem(ADD_TRIGGER_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Trigger", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected trigger"));
    table->setItem(ADD_TRIGGER_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Trigger", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected trigger"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(triggerUI);
    toggleSignals(true);
}

void ClipGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(cropStartAmountLocalTime, SIGNAL(editingFinished()), this, SLOT(setCropStartAmountLocalTime()), Qt::UniqueConnection);
        connect(cropEndAmountLocalTime, SIGNAL(editingFinished()), this, SLOT(setCropEndAmountLocalTime()), Qt::UniqueConnection);
        connect(startTime, SIGNAL(editingFinished()), this, SLOT(setStartTime()), Qt::UniqueConnection);
        connect(playbackSpeed, SIGNAL(editingFinished()), this, SLOT(setPlaybackSpeed()), Qt::UniqueConnection);
        connect(enforcedDuration, SIGNAL(editingFinished()), this, SLOT(setEnforcedDuration()), Qt::UniqueConnection);
        connect(userControlledTimeFraction, SIGNAL(editingFinished()), this, SLOT(setUserControlledTimeFraction()), Qt::UniqueConnection);
        connect(animationBindingIndex, SIGNAL(editingFinished()), this, SLOT(setAnimationBindingIndex()), Qt::UniqueConnection);
        connect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)), Qt::UniqueConnection);
        connect(flagContinueMotionAtEnd, SIGNAL(released()), this, SLOT(setFlagContinueMotionAtEnd()), Qt::UniqueConnection);
        connect(flagSyncHalfCycleInPingPongMode, SIGNAL(released()), this, SLOT(setFlagSyncHalfCycleInPingPongMode()), Qt::UniqueConnection);
        connect(flagMirror, SIGNAL(released()), this, SLOT(setFlagMirror()), Qt::UniqueConnection);
        connect(flagForceDensePose, SIGNAL(released()), this, SLOT(setFlagForceDensePose()), Qt::UniqueConnection);
        connect(flagDontConvertAnnotationsToTriggers, SIGNAL(released()), this, SLOT(setFlagDontConvertAnnotationsToTriggers()), Qt::UniqueConnection);
        connect(flagIgnoreMotion, SIGNAL(released()), this, SLOT(setFlagIgnoreMotion()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(triggerUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(triggerUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(cropStartAmountLocalTime, SIGNAL(editingFinished()), this, SLOT(setCropStartAmountLocalTime()));
        disconnect(cropEndAmountLocalTime, SIGNAL(editingFinished()), this, SLOT(setCropEndAmountLocalTime()));
        disconnect(startTime, SIGNAL(editingFinished()), this, SLOT(setStartTime()));
        disconnect(playbackSpeed, SIGNAL(editingFinished()), this, SLOT(setPlaybackSpeed()));
        disconnect(enforcedDuration, SIGNAL(editingFinished()), this, SLOT(setEnforcedDuration()));
        disconnect(userControlledTimeFraction, SIGNAL(editingFinished()), this, SLOT(setUserControlledTimeFraction()));
        disconnect(animationBindingIndex, SIGNAL(editingFinished()), this, SLOT(setAnimationBindingIndex()));
        disconnect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(setMode(int)));
        disconnect(flagContinueMotionAtEnd, SIGNAL(released()), this, SLOT(setFlagContinueMotionAtEnd()));
        disconnect(flagSyncHalfCycleInPingPongMode, SIGNAL(released()), this, SLOT(setFlagSyncHalfCycleInPingPongMode()));
        disconnect(flagMirror, SIGNAL(released()), this, SLOT(setFlagMirror()));
        disconnect(flagForceDensePose, SIGNAL(released()), this, SLOT(setFlagForceDensePose()));
        disconnect(flagDontConvertAnnotationsToTriggers, SIGNAL(released()), this, SLOT(setFlagDontConvertAnnotationsToTriggers()));
        disconnect(flagIgnoreMotion, SIGNAL(released()), this, SLOT(setFlagIgnoreMotion()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(triggerUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(triggerUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void ClipGeneratorUI::addTrigger(){
    if (bsData){
        auto triggers = bsData->getTriggers();
        if (!triggers){
            triggers = new hkbClipTriggerArray(bsData->getParentFile(), -1);
            bsData->triggers = HkxSharedPtr(triggers);
        }
        triggers->addTrigger();
        //triggers->triggers.last().event.id = 0;
        //static_cast<BehaviorFile *>(bsData->getParentFile())->appendClipTriggerToAnimData(bsData->getName());
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("ClipGeneratorUI::addTrigger(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::removeTrigger(int index){
    if (bsData){
        auto triggers = bsData->getTriggers();
        if (triggers){
            if (index < triggers->triggers.size() && index >= 0){
                triggers->removeTrigger(index);
                //static_cast<BehaviorFile *>(bsData->getParentFile())->removeClipTriggerToAnimDataAt(bsData->getName(), index);
                if (!triggers->getNumberOfTriggers()){
                    bsData->setTriggers(nullptr);
                    static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
                }
                loadDynamicTableRows();
            }else{
                WARNING_MESSAGE("ClipGeneratorUI::removeTrigger(): Invalid row index selected!!");
            }
        }else{
            LogFile::writeToLog("ClipGeneratorUI::removeTrigger(): Event data is nullptr!!");
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::removeTrigger(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        if (data->getSignature() == HKB_CLIP_GENERATOR){
            bsData = static_cast<hkbClipGenerator *>(data);
            name->setText(bsData->getName());
            table->item(ANIMATION_NAME_ROW, VALUE_COLUMN)->setText(bsData->getAnimationName());
            cropStartAmountLocalTime->setValue(bsData->getCropStartAmountLocalTime());
            cropEndAmountLocalTime->setValue(bsData->getCropEndAmountLocalTime());
            startTime->setValue(bsData->getStartTime());
            playbackSpeed->setValue(bsData->getPlaybackSpeed());
            enforcedDuration->setValue(bsData->getEnforcedDuration());
            userControlledTimeFraction->setValue(bsData->getUserControlledTimeFraction());
            animationBindingIndex->setValue(bsData->getAnimationBindingIndex());
            enforcedDuration->setValue(bsData->getEnforcedDuration());
            auto ok = true;
            hkbClipGenerator::ClipFlags flags(bsData->flags.toInt(&ok));
            auto testflag = [&](CheckBox * checkbox, hkbClipGenerator::ClipFlag flagtotest){
                (flags.testFlag(flagtotest)) ? checkbox->setChecked(true) : checkbox->setChecked(false);
            };
            if (ok){
                testflag(flagContinueMotionAtEnd, hkbClipGenerator::FLAG_CONTINUE_MOTION_AT_END);
                testflag(flagSyncHalfCycleInPingPongMode, hkbClipGenerator::FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE);
                testflag(flagMirror, hkbClipGenerator::FLAG_MIRROR);
                testflag(flagForceDensePose, hkbClipGenerator::FLAG_FORCE_DENSE_POSE);
                testflag(flagDontConvertAnnotationsToTriggers, hkbClipGenerator::FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS);
                testflag(flagIgnoreMotion, hkbClipGenerator::FLAG_IGNORE_MOTION);
            }else{
                LogFile::writeToLog(QString("ClipGeneratorUI::loadData(): The flags string is invalid!!!\nString: "+bsData->flags).toLocal8Bit().data());
            }
            (!mode->count()) ? mode->insertItems(0, bsData->PlaybackMode) : NULL;
            mode->setCurrentIndex(bsData->PlaybackMode.indexOf(bsData->mode));
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(CROP_START_AMOUNT_LOCAL_TIME_ROW, BINDING_COLUMN, varBind, "cropStartAmountLocalTime", table, bsData);
            UIHelper::loadBinding(CROP_END_AMOUNT_LOCAL_TIME_ROW, BINDING_COLUMN, varBind, "cropEndAmountLocalTime", table, bsData);
            UIHelper::loadBinding(START_TIME_ROW, BINDING_COLUMN, varBind, "startTime", table, bsData);
            UIHelper::loadBinding(PLAYBACK_SPEED_ROW, BINDING_COLUMN, varBind, "playbackSpeed", table, bsData);
            UIHelper::loadBinding(ENFORCED_DURATION_ROW, BINDING_COLUMN, varBind, "enforcedDuration", table, bsData);
            UIHelper::loadBinding(USER_CONTROLLED_TIME_FRACTION_ROW, BINDING_COLUMN, varBind, "userControlledTimeFraction", table, bsData);
            UIHelper::loadBinding(ANIMATION_BINDING_INDEX_ROW, BINDING_COLUMN, varBind, "animationBindingIndex", table, bsData);
            loadDynamicTableRows();
        }else{
            LogFile::writeToLog(QString("ClipGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void ClipGeneratorUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_TRIGGER_ROW + bsData->getNumberOfTriggers() + 1;
        (table->rowCount() != temp) ? table->setRowCount(temp) : NULL;
        auto triggers = static_cast<hkbClipTriggerArray *>(bsData->triggers.data());
        if (triggers){
            for (auto i = ADD_TRIGGER_ROW + 1, j = 0; j < bsData->getNumberOfTriggers(); i++, j++){
                UIHelper::setRowItems(i, static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(triggers->triggers.at(j).event.id), triggers->getClassname(), "Remove", "Edit", "Double click to remove this trigger", "Double click to edit this trigger", table);
            }
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case CROP_START_AMOUNT_LOCAL_TIME_ROW:
            checkisproperty(CROP_START_AMOUNT_LOCAL_TIME_ROW, "cropStartAmountLocalTime", VARIABLE_TYPE_REAL); break;
        case CROP_END_AMOUNT_LOCAL_TIME_ROW:
            checkisproperty(CROP_END_AMOUNT_LOCAL_TIME_ROW, "cropEndAmountLocalTime", VARIABLE_TYPE_REAL); break;
        case START_TIME_ROW:
            checkisproperty(START_TIME_ROW, "startTime", VARIABLE_TYPE_REAL); break;
        case PLAYBACK_SPEED_ROW:
            checkisproperty(PLAYBACK_SPEED_ROW, "playbackSpeed", VARIABLE_TYPE_REAL); break;
        case ENFORCED_DURATION_ROW:
            checkisproperty(ENFORCED_DURATION_ROW, "enforcedDuration", VARIABLE_TYPE_REAL); break;
        case USER_CONTROLLED_TIME_FRACTION_ROW:
            checkisproperty(USER_CONTROLLED_TIME_FRACTION_ROW, "userControlledTimeFraction", VARIABLE_TYPE_REAL); break;
        case ANIMATION_BINDING_INDEX_ROW:
            checkisproperty(ANIMATION_BINDING_INDEX_ROW, "animationBindingIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::setBindingVariable(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::returnToWidget(){
    loadDynamicTableRows();
    setCurrentIndex(MAIN_WIDGET);
}

void ClipGeneratorUI::setName(const QString & newname){
    if (bsData){
        bsData->setName(bsData->getName(), newname);
        bsData->updateIconNames();
        emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
        /*if (static_cast<BehaviorFile *>(bsData->getParentFile())->isClipGenNameAvailable(name->text())){
            bsData->setName(bsData->getName(), name->text());
            bsData->updateIconNames();
            emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
        }else{
            disconnect(name, SIGNAL(editingFinished()), this, SLOT(setName()));
            name->setText(bsData->getName());
            connect(name, SIGNAL(editingFinished()), this, SLOT(setName()), Qt::UniqueConnection);
            WARNING_MESSAGE("ClipGeneratorUI::setName(): This clip generator name is already is use elsewhere in the project!!");
        }*/
    }else{
        LogFile::writeToLog("ClipGeneratorUI::setName(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::setAnimationName(int index, const QString &name){
    if (bsData){
        bsData->setAnimationName(index, name);
        auto item = table->item(ANIMATION_NAME_ROW, VALUE_COLUMN);
        (item) ? item->setText(name) : LogFile::writeToLog("ClipGeneratorUI::setAnimationName(): The table item at 'ANIMATION_NAME_ROW' is nullptr!!");
    }else{
        LogFile::writeToLog("ClipGeneratorUI::setAnimationName(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::setCropStartAmountLocalTime(){
    (bsData) ? bsData->setCropStartAmountLocalTime(cropStartAmountLocalTime->value()) : LogFile::writeToLog("ClipGeneratorUI::setCropStartAmountLocalTime(): The data is nullptr!!");
}

void ClipGeneratorUI::setCropEndAmountLocalTime(){
    (bsData) ? bsData->setCropEndAmountLocalTime(cropEndAmountLocalTime->value()) : LogFile::writeToLog("ClipGeneratorUI::setCropEndAmountLocalTime(): The data is nullptr!!");
}

void ClipGeneratorUI::setStartTime(){
    (bsData) ? bsData->setStartTime(startTime->value()) : LogFile::writeToLog("ClipGeneratorUI::setStartTime(): The data is nullptr!!");
}

void ClipGeneratorUI::setPlaybackSpeed(){
    (bsData) ? bsData->setPlaybackSpeed(playbackSpeed->value()) : LogFile::writeToLog("ClipGeneratorUI::setPlaybackSpeed(): The data is nullptr!!");
}

void ClipGeneratorUI::setEnforcedDuration(){
    (bsData) ? bsData->setEnforcedDuration(enforcedDuration->value()) : LogFile::writeToLog("ClipGeneratorUI::setEnforcedDuration(): The data is nullptr!!");
}

void ClipGeneratorUI::setUserControlledTimeFraction(){
    (bsData) ? bsData->setUserControlledTimeFraction(userControlledTimeFraction->value()) : LogFile::writeToLog("ClipGeneratorUI::setUserControlledTimeFraction(): The data is nullptr!!");
}

void ClipGeneratorUI::setAnimationBindingIndex(){
    (bsData) ? bsData->setAnimationBindingIndex(animationBindingIndex->value()) : LogFile::writeToLog("ClipGeneratorUI::setAnimationBindingIndex(): The data is nullptr!!");
}

void ClipGeneratorUI::setMode(int index){
    (bsData) ? bsData->setMode(index) : LogFile::writeToLog("ClipGeneratorUI::setMode(): The data is nullptr!!");
}

void ClipGeneratorUI::setFlag(CheckBox *flagcheckbox, hkbClipGenerator::ClipFlag flagtoset){
    if (bsData){
        auto ok = true;
        hkbClipGenerator::ClipFlags flags(bsData->getFlags().toInt(&ok));
        if (ok){
            (flagcheckbox->isChecked()) ? flags |= flagtoset : flags &= ~(flagtoset);
            bsData->setFlags(QString::number(flags));
        }else{
            LogFile::writeToLog(QString("ClipGeneratorUI::setFlag(): The flags string is invalid!!!\nString: "+bsData->getFlags()).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::setFlag(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::setFlagContinueMotionAtEnd(){
    setFlag(flagContinueMotionAtEnd, hkbClipGenerator::FLAG_CONTINUE_MOTION_AT_END);
}

void ClipGeneratorUI::setFlagSyncHalfCycleInPingPongMode(){
    setFlag(flagSyncHalfCycleInPingPongMode, hkbClipGenerator::FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE);
}

void ClipGeneratorUI::setFlagMirror(){
    setFlag(flagMirror, hkbClipGenerator::FLAG_MIRROR);
}

void ClipGeneratorUI::setFlagForceDensePose(){
    setFlag(flagForceDensePose, hkbClipGenerator::FLAG_FORCE_DENSE_POSE);
}

void ClipGeneratorUI::setFlagDontConvertAnnotationsToTriggers(){
    setFlag(flagDontConvertAnnotationsToTriggers, hkbClipGenerator::FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS);
}

void ClipGeneratorUI::setFlagIgnoreMotion(){
    setFlag(flagIgnoreMotion, hkbClipGenerator::FLAG_IGNORE_MOTION);
}

void ClipGeneratorUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_TRIGGER_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case CROP_START_AMOUNT_LOCAL_TIME_ROW:
                    checkisproperty(CROP_START_AMOUNT_LOCAL_TIME_ROW, "cropStartAmountLocalTime"); break;
                case CROP_END_AMOUNT_LOCAL_TIME_ROW:
                    checkisproperty(CROP_END_AMOUNT_LOCAL_TIME_ROW, "cropEndAmountLocalTime"); break;
                case START_TIME_ROW:
                    checkisproperty(START_TIME_ROW, "startTime"); break;
                case PLAYBACK_SPEED_ROW:
                    checkisproperty(PLAYBACK_SPEED_ROW, "playbackSpeed"); break;
                case ENFORCED_DURATION_ROW:
                    checkisproperty(ENFORCED_DURATION_ROW, "enforcedDuration"); break;
                case USER_CONTROLLED_TIME_FRACTION_ROW:
                    checkisproperty(USER_CONTROLLED_TIME_FRACTION_ROW, "userControlledTimeFraction"); break;
                case ANIMATION_BINDING_INDEX_ROW:
                    checkisproperty(ANIMATION_BINDING_INDEX_ROW, "animationBindingIndex"); break;
                }
            }else if (row == ANIMATION_NAME_ROW && column == VALUE_COLUMN){
                emit viewAnimations(bsData->getAnimationName());
            }
        }else if (row == ADD_TRIGGER_ROW && column == NAME_COLUMN){
            addTrigger();
        }else if (row > ADD_TRIGGER_ROW && row < ADD_TRIGGER_ROW + bsData->getNumberOfTriggers() + 1){
            auto result = row - BASE_NUMBER_OF_ROWS;
            if (bsData->getNumberOfTriggers() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    triggerUI->loadData((BehaviorFile *)(bsData->getParentFile()), bsData, result, &static_cast<hkbClipTriggerArray *>(bsData->triggers.data())->triggers[result]); //TO DO...
                    setCurrentIndex(CHILD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the trigger \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeTrigger(result);
                    }
                }
            }else{
                LogFile::writeToLog("ClipGeneratorUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events, GenericTableWidget *animations){
    if (variables && events && properties && animations){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(animations, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), triggerUI, SLOT(setEventId(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(animations, SIGNAL(elementSelected(int,QString)), this, SLOT(setAnimationName(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewAnimations(QString)), animations, SLOT(showTable(QString)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ClipGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ClipGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("ClipGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (currentIndex() == CHILD_WIDGET) ? triggerUI->eventRenamed(name, index) : NULL;
    }else{
        LogFile::writeToLog("ClipGeneratorUI::eventRenamed(): The data is nullptr!!");
    }
}

void ClipGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        if (name != ""){
            index--;
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("cropStartAmountLocalTime", CROP_START_AMOUNT_LOCAL_TIME_ROW);
                setname("cropEndAmountLocalTime", CROP_END_AMOUNT_LOCAL_TIME_ROW);
                setname("startTime", START_TIME_ROW);
                setname("playbackSpeed", PLAYBACK_SPEED_ROW);
                setname("enforcedDuration", ENFORCED_DURATION_ROW);
                setname("userControlledTimeFraction", USER_CONTROLLED_TIME_FRACTION_ROW);
                setname("animationBindingIndex", ANIMATION_BINDING_INDEX_ROW);
            }
        }
    }else{
        LogFile::writeToLog("ClipGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}
