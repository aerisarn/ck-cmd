#include "bssynchronizedclipgeneratorui.h"

#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/bssynchronizedclipgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/eventui.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"

#include <QGridLayout>

#include "src/ui/genericdatawidgets.h"
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 10

#define NAME_ROW 0
#define CLIP_GENERATOR_ROW 1
#define SYNC_ANIM_PREFIX_ROW 2
#define SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW 3
#define GET_TO_MARK_TIME_ROW 4
#define MARK_ERROR_THRESHOLD_ROW 5
#define LEAD_CHARACTER_ROW 6
#define REORIENT_SUPPORT_CHAR_ROW 7
#define APPLY_MOTION_FROM_ROOT_ROW 8
#define ANIMATION_BINDING_INDEX_ROW 9

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSSynchronizedClipGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSSynchronizedClipGeneratorUI::BSSynchronizedClipGeneratorUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      syncAnimPrefix(new LineEdit),
      bSyncClipIgnoreMarkPlacement(new CheckBox),
      fGetToMarkTime(new DoubleSpinBox),
      fMarkErrorThreshold(new DoubleSpinBox),
      bLeadCharacter(new CheckBox),
      bReorientSupportChar(new CheckBox),
      bApplyMotionFromRoot(new CheckBox),
      sAnimationBindingIndex(new SpinBox)
{
    setTitle("BSSynchronizedClipGeneratorUI");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(CLIP_GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("pClipGenerator"));
    table->setItem(CLIP_GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbClipGenerator", Qt::AlignCenter));
    table->setItem(CLIP_GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(CLIP_GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    table->setItem(SYNC_ANIM_PREFIX_ROW, NAME_COLUMN, new TableWidgetItem("syncAnimPrefix"));
    table->setItem(SYNC_ANIM_PREFIX_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(SYNC_ANIM_PREFIX_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SYNC_ANIM_PREFIX_ROW, VALUE_COLUMN, syncAnimPrefix);
    table->setItem(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, NAME_COLUMN, new TableWidgetItem("bSyncClipIgnoreMarkPlacement"));
    table->setItem(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter));
    table->setCellWidget(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, VALUE_COLUMN, bSyncClipIgnoreMarkPlacement);
    table->setItem(GET_TO_MARK_TIME_ROW, NAME_COLUMN, new TableWidgetItem("fGetToMarkTime"));
    table->setItem(GET_TO_MARK_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GET_TO_MARK_TIME_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GET_TO_MARK_TIME_ROW, VALUE_COLUMN, fGetToMarkTime);
    table->setItem(MARK_ERROR_THRESHOLD_ROW, NAME_COLUMN, new TableWidgetItem("fMarkErrorThreshold"));
    table->setItem(MARK_ERROR_THRESHOLD_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MARK_ERROR_THRESHOLD_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MARK_ERROR_THRESHOLD_ROW, VALUE_COLUMN, fMarkErrorThreshold);
    table->setItem(LEAD_CHARACTER_ROW, NAME_COLUMN, new TableWidgetItem("bLeadCharacter"));
    table->setItem(LEAD_CHARACTER_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(LEAD_CHARACTER_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(LEAD_CHARACTER_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter));
    table->setCellWidget(LEAD_CHARACTER_ROW, VALUE_COLUMN, bLeadCharacter);
    table->setItem(REORIENT_SUPPORT_CHAR_ROW, NAME_COLUMN, new TableWidgetItem("bReorientSupportChar"));
    table->setItem(REORIENT_SUPPORT_CHAR_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(REORIENT_SUPPORT_CHAR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(REORIENT_SUPPORT_CHAR_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter));
    table->setCellWidget(REORIENT_SUPPORT_CHAR_ROW, VALUE_COLUMN, bReorientSupportChar);
    table->setItem(APPLY_MOTION_FROM_ROOT_ROW, NAME_COLUMN, new TableWidgetItem("bApplyMotionFromRoot"));
    table->setItem(APPLY_MOTION_FROM_ROOT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(APPLY_MOTION_FROM_ROOT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(APPLY_MOTION_FROM_ROOT_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter));
    table->setCellWidget(APPLY_MOTION_FROM_ROOT_ROW, VALUE_COLUMN, bApplyMotionFromRoot);
    table->setItem(ANIMATION_BINDING_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("sAnimationBindingIndex"));
    table->setItem(ANIMATION_BINDING_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ANIMATION_BINDING_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ANIMATION_BINDING_INDEX_ROW, VALUE_COLUMN, sAnimationBindingIndex);
    topLyt->addWidget(table, 1, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSSynchronizedClipGeneratorUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(syncAnimPrefix, SIGNAL(textEdited(QString)), this, SLOT(setSyncAnimPrefix(QString)), Qt::UniqueConnection);
        connect(bSyncClipIgnoreMarkPlacement, SIGNAL(released()), this, SLOT(setSyncClipIgnoreMarkPlacement()), Qt::UniqueConnection);
        connect(fGetToMarkTime, SIGNAL(editingFinished()), this, SLOT(setGetToMarkTime()), Qt::UniqueConnection);
        connect(fMarkErrorThreshold, SIGNAL(editingFinished()), this, SLOT(setMarkErrorThreshold()), Qt::UniqueConnection);
        connect(bLeadCharacter, SIGNAL(released()), this, SLOT(setLeadCharacter()), Qt::UniqueConnection);
        connect(bReorientSupportChar, SIGNAL(released()), this, SLOT(setReorientSupportChar()), Qt::UniqueConnection);
        connect(bApplyMotionFromRoot, SIGNAL(released()), this, SLOT(setApplyMotionFromRoot()), Qt::UniqueConnection);
        connect(sAnimationBindingIndex, SIGNAL(editingFinished()), this, SLOT(setAnimationBindingIndex()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(syncAnimPrefix, SIGNAL(textEdited(QString)), this, SLOT(setSyncAnimPrefix(QString)));
        disconnect(bSyncClipIgnoreMarkPlacement, SIGNAL(released()), this, SLOT(setSyncClipIgnoreMarkPlacement()));
        disconnect(fGetToMarkTime, SIGNAL(editingFinished()), this, SLOT(setGetToMarkTime()));
        disconnect(fMarkErrorThreshold, SIGNAL(editingFinished()), this, SLOT(setMarkErrorThreshold()));
        disconnect(bLeadCharacter, SIGNAL(released()), this, SLOT(setLeadCharacter()));
        disconnect(bReorientSupportChar, SIGNAL(released()), this, SLOT(setReorientSupportChar()));
        disconnect(bApplyMotionFromRoot, SIGNAL(released()), this, SLOT(setApplyMotionFromRoot()));
        disconnect(sAnimationBindingIndex, SIGNAL(editingFinished()), this, SLOT(setAnimationBindingIndex()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void BSSynchronizedClipGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_SYNCHRONIZED_CLIP_GENERATOR){
            bsData = static_cast<BSSynchronizedClipGenerator *>(data);
            name->setText(bsData->getName());
            syncAnimPrefix->setText(bsData->getSyncAnimPrefix());
            bSyncClipIgnoreMarkPlacement->setChecked(bsData->getBSyncClipIgnoreMarkPlacement());
            fGetToMarkTime->setValue(bsData->getFGetToMarkTime());
            fMarkErrorThreshold->setValue(bsData->getFMarkErrorThreshold());
            bLeadCharacter->setChecked(bsData->getBLeadCharacter());
            bReorientSupportChar->setChecked(bsData->getBReorientSupportChar());
            bApplyMotionFromRoot->setChecked(bsData->getBApplyMotionFromRoot());
            sAnimationBindingIndex->setValue(bsData->getSAnimationBindingIndex());
            table->item(CLIP_GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getClipGeneratorName());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, BINDING_COLUMN, varBind, "bSyncClipIgnoreMarkPlacement", table, bsData);
            UIHelper::loadBinding(GET_TO_MARK_TIME_ROW, BINDING_COLUMN, varBind, "fGetToMarkTime", table, bsData);
            UIHelper::loadBinding(MARK_ERROR_THRESHOLD_ROW, BINDING_COLUMN, varBind, "fMarkErrorThreshold", table, bsData);
            UIHelper::loadBinding(LEAD_CHARACTER_ROW, BINDING_COLUMN, varBind, "bLeadCharacter", table, bsData);
            UIHelper::loadBinding(REORIENT_SUPPORT_CHAR_ROW, BINDING_COLUMN, varBind, "bReorientSupportChar", table, bsData);
            UIHelper::loadBinding(APPLY_MOTION_FROM_ROOT_ROW, BINDING_COLUMN, varBind, "bApplyMotionFromRoot", table, bsData);
            UIHelper::loadBinding(ANIMATION_BINDING_INDEX_ROW, BINDING_COLUMN, varBind, "sAnimationBindingIndex", table, bsData);
        }else{
            LogFile::writeToLog(QString("BSSynchronizedClipGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::loadData(): Attempting to load a null pointer!!");
    }
    toggleSignals(true);
}

void BSSynchronizedClipGeneratorUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(bsData->getName(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::setSyncAnimPrefix(const QString &newname){
    (bsData) ? bsData->setSyncAnimPrefix(newname) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setSyncAnimPrefix(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setSyncClipIgnoreMarkPlacement(){
    (bsData) ? bsData->setBSyncClipIgnoreMarkPlacement(bSyncClipIgnoreMarkPlacement->isChecked()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setSyncClipIgnoreMarkPlacement(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setGetToMarkTime(){
    (bsData) ? bsData->setFGetToMarkTime(fGetToMarkTime->value()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setGetToMarkTime(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setMarkErrorThreshold(){
    (bsData) ? bsData->setFMarkErrorThreshold(fMarkErrorThreshold->value()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setMarkErrorThreshold(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setLeadCharacter(){
    (bsData) ? bsData->setBLeadCharacter(bLeadCharacter->isChecked()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setLeadCharacter(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setReorientSupportChar(){
    (bsData) ? bsData->setBReorientSupportChar(bReorientSupportChar->isChecked()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setReorientSupportChar(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setApplyMotionFromRoot(){
    (bsData) ? bsData->setBApplyMotionFromRoot(bApplyMotionFromRoot->isChecked()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setApplyMotionFromRoot(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::setAnimationBindingIndex(){
    (bsData) ? bsData->setSAnimationBindingIndex(sAnimationBindingIndex->value()) : LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setAnimationBindingIndex(): The data is nullptr!!");
}

void BSSynchronizedClipGeneratorUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("BSOffsetAnimationGeneratorUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW:
                checkisproperty(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, "bSyncClipIgnoreMarkPlacement"); break;
            case GET_TO_MARK_TIME_ROW:
                checkisproperty(GET_TO_MARK_TIME_ROW, "fGetToMarkTime"); break;
            case MARK_ERROR_THRESHOLD_ROW:
                checkisproperty(MARK_ERROR_THRESHOLD_ROW, "fMarkErrorThreshold"); break;
            case LEAD_CHARACTER_ROW:
                checkisproperty(LEAD_CHARACTER_ROW, "bLeadCharacter"); break;
            case REORIENT_SUPPORT_CHAR_ROW:
                checkisproperty(REORIENT_SUPPORT_CHAR_ROW, "bReorientSupportChar"); break;
            case APPLY_MOTION_FROM_ROOT_ROW:
                checkisproperty(APPLY_MOTION_FROM_ROOT_ROW, "bApplyMotionFromRoot"); break;
            case ANIMATION_BINDING_INDEX_ROW:
                checkisproperty(ANIMATION_BINDING_INDEX_ROW, "sAnimationBindingIndex"); break;
            }
        }else if (column == VALUE_COLUMN){
            if (row == CLIP_GENERATOR_ROW){
                emit viewGenerators(bsData->getIndexOfGenerator(bsData->pClipGenerator) + 1, hkbClipGenerator::getClassname(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::setClipGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->pClipGenerator.data()), HKB_CLIP_GENERATOR, HkxObject::TYPE_GENERATOR, table, behaviorView, CLIP_GENERATOR_ROW, VALUE_COLUMN);
}

void BSSynchronizedClipGeneratorUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW:
            checkisproperty(SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW, "bSyncClipIgnoreMarkPlacement", VARIABLE_TYPE_BOOL); break;
        case GET_TO_MARK_TIME_ROW:
            checkisproperty(GET_TO_MARK_TIME_ROW, "fGetToMarkTime", VARIABLE_TYPE_REAL); break;
        case MARK_ERROR_THRESHOLD_ROW:
            checkisproperty(MARK_ERROR_THRESHOLD_ROW, "fMarkErrorThreshold", VARIABLE_TYPE_REAL); break;
        case LEAD_CHARACTER_ROW:
            checkisproperty(LEAD_CHARACTER_ROW, "bLeadCharacter", VARIABLE_TYPE_BOOL); break;
        case REORIENT_SUPPORT_CHAR_ROW:
            checkisproperty(REORIENT_SUPPORT_CHAR_ROW, "bReorientSupportChar", VARIABLE_TYPE_BOOL); break;
        case APPLY_MOTION_FROM_ROOT_ROW:
            checkisproperty(APPLY_MOTION_FROM_ROOT_ROW, "bApplyMotionFromRoot", VARIABLE_TYPE_BOOL); break;
        case ANIMATION_BINDING_INDEX_ROW:
            checkisproperty(ANIMATION_BINDING_INDEX_ROW, "sAnimationBindingIndex", VARIABLE_TYPE_INT32); break;
        }
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::setBindingVariable(): The 'bsData' pointer is nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties){
    if (generators && variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(setClipGenerator(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("bSyncClipIgnoreMarkPlacement", SYNC_CLIP_IGNORE_MARK_PLACEMENT_ROW);
            setname("fGetToMarkTime", GET_TO_MARK_TIME_ROW);
            setname("fMarkErrorThreshold", MARK_ERROR_THRESHOLD_ROW);
            setname("bLeadCharacter", LEAD_CHARACTER_ROW);
            setname("bReorientSupportChar", REORIENT_SUPPORT_CHAR_ROW);
            setname("bApplyMotionFromRoot", APPLY_MOTION_FROM_ROOT_ROW);
            setname("sAnimationBindingIndex", ANIMATION_BINDING_INDEX_ROW);
        }
    }else{
        LogFile::writeToLog("BSSynchronizedClipGeneratorUI::variableRenamed(): The data is nullptr!!");
    }
}

void BSSynchronizedClipGeneratorUI::generatorRenamed(const QString &name, int index){
    index--;
    if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->pClipGenerator)){
        table->item(CLIP_GENERATOR_ROW, VALUE_COLUMN)->setText(name);
    }
}

void BSSynchronizedClipGeneratorUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

