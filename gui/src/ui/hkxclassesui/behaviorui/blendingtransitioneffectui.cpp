#include "blendingtransitioneffectui.h"
#include "src/hkxclasses/behavior/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QSpinBox>
#include <QGroupBox>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 10

#define NAME_ROW 0
#define SELF_TRANSITION_MODE_ROW 1
#define EVENT_MODE_ROW 2
#define DURATION_ROW 3
#define TO_GENERATOR_START_TIME_FRACTION_ROW 4
#define FLAG_SYNC_ROW 5
#define FLAG_IGNORE_FROM_WORLD_FROM_MODEL_ROW 6
#define FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROW 7
#define END_MODE_ROW 8
#define BLEND_CURVE_ROW 9

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BlendingTransitionEffectUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BlendingTransitionEffectUI::BlendingTransitionEffectUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      name(new LineEdit),
      selfTransitionMode(new ComboBox),
      eventMode(new ComboBox),
      duration(new DoubleSpinBox),
      toGeneratorStartTimeFraction(new DoubleSpinBox),
      flagSync(new CheckBox),
      flagIgnoreFromWorldFromModel(new CheckBox),
      flagIgnoreToWorldFromModel(new CheckBox),
      endMode(new ComboBox),
      blendCurve(new ComboBox)
{
    setTitle("hkbBlendingTransitionEffect");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(SELF_TRANSITION_MODE_ROW, NAME_COLUMN, new TableWidgetItem("selfTransitionMode"));
    table->setItem(SELF_TRANSITION_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("SelfTransitionMode", Qt::AlignCenter));
    table->setItem(SELF_TRANSITION_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SELF_TRANSITION_MODE_ROW, VALUE_COLUMN, selfTransitionMode);
    table->setItem(EVENT_MODE_ROW, NAME_COLUMN, new TableWidgetItem("eventMode"));
    table->setItem(EVENT_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("EventMode", Qt::AlignCenter));
    table->setItem(EVENT_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_MODE_ROW, VALUE_COLUMN, eventMode);
    table->setItem(DURATION_ROW, NAME_COLUMN, new TableWidgetItem("duration"));
    table->setItem(DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DURATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(DURATION_ROW, VALUE_COLUMN, duration);
    table->setItem(TO_GENERATOR_START_TIME_FRACTION_ROW, NAME_COLUMN, new TableWidgetItem("toGeneratorStartTimeFraction"));
    table->setItem(TO_GENERATOR_START_TIME_FRACTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TO_GENERATOR_START_TIME_FRACTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TO_GENERATOR_START_TIME_FRACTION_ROW, VALUE_COLUMN, toGeneratorStartTimeFraction);
    table->setItem(FLAG_SYNC_ROW, NAME_COLUMN, new TableWidgetItem("flagSync"));
    table->setItem(FLAG_SYNC_ROW, TYPE_COLUMN, new TableWidgetItem("FlagBits", Qt::AlignCenter));
    table->setItem(FLAG_SYNC_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(FLAG_SYNC_ROW, VALUE_COLUMN, flagSync);
    table->setItem(FLAG_IGNORE_FROM_WORLD_FROM_MODEL_ROW, NAME_COLUMN, new TableWidgetItem("flagIgnoreFromWorldFromModel"));
    table->setItem(FLAG_IGNORE_FROM_WORLD_FROM_MODEL_ROW, TYPE_COLUMN, new TableWidgetItem("FlagBits", Qt::AlignCenter));
    table->setItem(FLAG_IGNORE_FROM_WORLD_FROM_MODEL_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(FLAG_IGNORE_FROM_WORLD_FROM_MODEL_ROW, VALUE_COLUMN, flagIgnoreFromWorldFromModel);
    table->setItem(FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROW, NAME_COLUMN, new TableWidgetItem("flagIgnoreToWorldFromModel"));
    table->setItem(FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROW, TYPE_COLUMN, new TableWidgetItem("FlagBits", Qt::AlignCenter));
    table->setItem(FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(FLAG_IGNORE_TO_WORLD_FROM_MODEL_ROW, VALUE_COLUMN, flagIgnoreToWorldFromModel);
    table->setItem(END_MODE_ROW, NAME_COLUMN, new TableWidgetItem("endMode"));
    table->setItem(END_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("EndMode", Qt::AlignCenter));
    table->setItem(END_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(END_MODE_ROW, VALUE_COLUMN, endMode);
    table->setItem(BLEND_CURVE_ROW, NAME_COLUMN, new TableWidgetItem("blendCurve"));
    table->setItem(BLEND_CURVE_ROW, TYPE_COLUMN, new TableWidgetItem("BlendCurve", Qt::AlignCenter));
    table->setItem(BLEND_CURVE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(BLEND_CURVE_ROW, VALUE_COLUMN, blendCurve);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BlendingTransitionEffectUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(selfTransitionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelfTransitionMode(int)), Qt::UniqueConnection);
        connect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)), Qt::UniqueConnection);
        connect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()), Qt::UniqueConnection);
        connect(toGeneratorStartTimeFraction, SIGNAL(editingFinished()), this, SLOT(setToGeneratorStartTimeFraction()), Qt::UniqueConnection);
        connect(flagSync, SIGNAL(released()), this, SLOT(toggleSyncFlag()), Qt::UniqueConnection);
        connect(flagIgnoreFromWorldFromModel, SIGNAL(released()), this, SLOT(toggleIgnoreFromWorldFromModelFlag()), Qt::UniqueConnection);
        connect(flagIgnoreToWorldFromModel, SIGNAL(released()), this, SLOT(toggleIgnoreToWorldFromModelFlag()), Qt::UniqueConnection);
        connect(endMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndMode(int)), Qt::UniqueConnection);
        connect(blendCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(setBlendCurve(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(selfTransitionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelfTransitionMode(int)));
        disconnect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)));
        disconnect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()));
        disconnect(toGeneratorStartTimeFraction, SIGNAL(editingFinished()), this, SLOT(setToGeneratorStartTimeFraction()));
        disconnect(flagSync, SIGNAL(released()), this, SLOT(toggleSyncFlag()));
        disconnect(flagIgnoreFromWorldFromModel, SIGNAL(released()), this, SLOT(toggleIgnoreFromWorldFromModelFlag()));
        disconnect(flagIgnoreToWorldFromModel, SIGNAL(released()), this, SLOT(toggleIgnoreToWorldFromModelFlag()));
        disconnect(endMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndMode(int)));
        disconnect(blendCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(setBlendCurve(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void BlendingTransitionEffectUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data && data->getSignature() == HKB_BLENDING_TRANSITION_EFFECT){
        bsData = static_cast<hkbBlendingTransitionEffect *>(data);
        name->setText(bsData->getName());
        auto loadmethods = [&](ComboBox *combobox, const QString & method, const QStringList & methodlist){
            (!combobox->count()) ? combobox->insertItems(0, methodlist) : NULL;
            combobox->setCurrentIndex(methodlist.indexOf(method));
        };
        loadmethods(selfTransitionMode, bsData->getSelfTransitionMode(), bsData->SelfTransitionMode);
        loadmethods(eventMode, bsData->getEventMode(), bsData->EventMode);
        auto varBind = bsData->getVariableBindingSetData();
        UIHelper::loadBinding(DURATION_ROW, BINDING_COLUMN, varBind, "duration", table, bsData);
        UIHelper::loadBinding(TO_GENERATOR_START_TIME_FRACTION_ROW, BINDING_COLUMN, varBind, "toGeneratorStartTimeFraction", table, bsData);
        auto flagsstring = bsData->getFlags();
        auto flags = flagsstring.split("|");
        flagSync->setChecked(false);
        flagIgnoreFromWorldFromModel->setChecked(false);
        flagIgnoreToWorldFromModel->setChecked(false);
        if (flags.isEmpty()){
            if (flagsstring == "FLAG_SYNC"){
                flagSync->setChecked(true);
            }else if (flagsstring == "FLAG_IGNORE_FROM_WORLD_FROM_MODEL"){
                flagIgnoreFromWorldFromModel->setChecked(true);
            }else if (flagsstring == "FLAG_IGNORE_TO_WORLD_FROM_MODEL"){
                flagIgnoreToWorldFromModel->setChecked(true);
            }
        }else{
            for (auto i = 0; i < flags.size(); i++){
                if (flags.at(i) == "FLAG_SYNC"){
                    flagSync->setChecked(true);
                }else if (flags.at(i) == "FLAG_IGNORE_FROM_WORLD_FROM_MODEL"){
                    flagIgnoreFromWorldFromModel->setChecked(true);
                }else if (flags.at(i) == "FLAG_IGNORE_TO_WORLD_FROM_MODEL"){
                    flagIgnoreToWorldFromModel->setChecked(true);
                }
            }
        }
        duration->setValue(bsData->getDuration());
        toGeneratorStartTimeFraction->setValue(bsData->getToGeneratorStartTimeFraction());
        loadmethods(endMode, bsData->getEndMode(), bsData->EndMode);
        loadmethods(blendCurve, bsData->getBlendCurve(), bsData->BlendCurve);
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::loadData(): The data is nullptr or an incorrect type!!");
    }
    toggleSignals(true);
}

void BlendingTransitionEffectUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        emit transitionEffectRenamed(bsData->getName());
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::setName(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case DURATION_ROW:
            checkisproperty(DURATION_ROW, "duration", VARIABLE_TYPE_REAL); break;
        case TO_GENERATOR_START_TIME_FRACTION_ROW:
            checkisproperty(TO_GENERATOR_START_TIME_FRACTION_ROW, "toGeneratorStartTimeFraction", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void BlendingTransitionEffectUI::setSelfTransitionMode(int index){
    (bsData) ? bsData->setSelfTransitionMode(index) : LogFile::writeToLog("BlendingTransitionEffectUI::setSelfTransitionMode(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setEventMode(int index){
    (bsData) ? bsData->setEventMode(index) : LogFile::writeToLog("BlendingTransitionEffectUI::setEventMode(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setDuration(){
    (bsData) ? bsData->setDuration(duration->value()) : LogFile::writeToLog("BlendingTransitionEffectUI::setDuration(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setToGeneratorStartTimeFraction(){
    (bsData) ? bsData->setToGeneratorStartTimeFraction(toGeneratorStartTimeFraction->value()) : LogFile::writeToLog("BlendingTransitionEffectUI::setToGeneratorStartTimeFraction(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setEndMode(int index){
    (bsData) ? bsData->setEndMode(index) : LogFile::writeToLog("BlendingTransitionEffectUI::setEndMode(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setBlendCurve(int index){
    (bsData) ? bsData->setBlendCurve(index) : LogFile::writeToLog("BlendingTransitionEffectUI::setBlendCurve(): The data is nullptr!!");
}

void BlendingTransitionEffectUI::setFlag(CheckBox *flagcheckbox, QString &flags, const QString & flagtocheck){
    if (flagcheckbox->isChecked()){
        if (flags == flagtocheck){
            flags = flagtocheck;
        }else if (!flags.contains(flagtocheck)){
            flags.append("|"+flagtocheck);
        }
    }else{
        if (flags == flagtocheck){
            flags = flagtocheck;
        }else{
            flags.remove(flagtocheck);
            flags.replace("||", "|");
        }
    }
    bsData->setFlags(flags);
}

void BlendingTransitionEffectUI::toggleSyncFlag(){
    if (bsData){
        setFlag(flagSync, bsData->getFlags(), "FLAG_SYNC");
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::setFlagSync(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::toggleIgnoreFromWorldFromModelFlag(){
    if (bsData){
        setFlag(flagIgnoreFromWorldFromModel, bsData->getFlags(), "FLAG_IGNORE_FROM_WORLD_FROM_MODEL");
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::setFlagIgnoreFromWorldFromModel(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::toggleIgnoreToWorldFromModelFlag(){
    if (bsData){
        setFlag(flagIgnoreToWorldFromModel, bsData->getFlags(), "FLAG_IGNORE_TO_WORLD_FROM_MODEL");
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::setFlagIgnoreToWorldFromModel(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BlendingTransitionEffectUI::selectTableToView(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (column == BINDING_COLUMN){
            switch (row){
            case DURATION_ROW:
                checkisproperty(DURATION_ROW, "duration"); break;
            case TO_GENERATOR_START_TIME_FRACTION_ROW:
                checkisproperty(TO_GENERATOR_START_TIME_FRACTION_ROW, "toGeneratorStartTimeFraction"); break;
            }
        }
    }else{
        LogFile::writeToLog("StateMachineUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BlendingTransitionEffectUI::variableRenamed(const QString &name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("duration", DURATION_ROW);
            setname("toGeneratorStartTimeFraction", TO_GENERATOR_START_TIME_FRACTION_ROW);
        }
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::variableRenamed(): The data is nullptr!!");
    }
}
