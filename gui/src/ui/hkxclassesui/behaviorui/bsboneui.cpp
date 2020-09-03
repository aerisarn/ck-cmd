#include "bsboneui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define INDEX_ROW 0
#define FWD_AXIS_LS_ROW 1
#define LIMIT_ANGLE_DEGREES_ROW 2
#define ON_GAIN_ROW 3
#define OFF_GAIN_ROW 4
#define ENABLED_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSBoneUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSBoneUI::BSBoneUI()
    : file(nullptr),
      bsBoneIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      index(new ComboBox),
      fwdAxisLS(new QuadVariableWidget),
      limitAngleDegrees(new DoubleSpinBox),
      onGain(new DoubleSpinBox),
      offGain(new DoubleSpinBox),
      enabled(new CheckBox)
{
    setTitle("BSBoneUI");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(INDEX_ROW, NAME_COLUMN, new TableWidgetItem("index"));
    table->setItem(INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(INDEX_ROW, VALUE_COLUMN, index);
    table->setItem(FWD_AXIS_LS_ROW, NAME_COLUMN, new TableWidgetItem("fwdAxisLS"));
    table->setItem(FWD_AXIS_LS_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(FWD_AXIS_LS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(FWD_AXIS_LS_ROW, VALUE_COLUMN, fwdAxisLS);
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, NAME_COLUMN, new TableWidgetItem("limitAngleDegrees"));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(LIMIT_ANGLE_DEGREES_ROW, VALUE_COLUMN, limitAngleDegrees);
    table->setItem(ON_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("onGain"));
    table->setItem(ON_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ON_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ON_GAIN_ROW, VALUE_COLUMN, onGain);
    table->setItem(OFF_GAIN_ROW, NAME_COLUMN, new TableWidgetItem("offGain"));
    table->setItem(OFF_GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(OFF_GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(OFF_GAIN_ROW, VALUE_COLUMN, offGain);
    table->setItem(ENABLED_ROW, NAME_COLUMN, new TableWidgetItem("enabled"));
    table->setItem(ENABLED_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLED_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLED_ROW, VALUE_COLUMN, enabled);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSBoneUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(index, SIGNAL(currentIndexChanged(int)), this, SLOT(setIndex(int)), Qt::UniqueConnection);
        connect(fwdAxisLS, SIGNAL(editingFinished()), this, SLOT(setFwdAxisLS()), Qt::UniqueConnection);
        connect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()), Qt::UniqueConnection);
        connect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()), Qt::UniqueConnection);
        connect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()), Qt::UniqueConnection);
        connect(enabled, SIGNAL(released()), this, SLOT(setEnabled()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(index, SIGNAL(currentIndexChanged(int)), this, SLOT(setIndex(int)));
        disconnect(fwdAxisLS, SIGNAL(editingFinished()), this, SLOT(setFwdAxisLS()));
        disconnect(limitAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setLimitAngleDegrees()));
        disconnect(onGain, SIGNAL(editingFinished()), this, SLOT(setOnGain()));
        disconnect(offGain, SIGNAL(editingFinished()), this, SLOT(setOffGain()));
        disconnect(enabled, SIGNAL(released()), this, SLOT(setEnabled()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void BSBoneUI::loadData(BehaviorFile *parentFile, BSLookAtModifier::BsBone *bon, BSLookAtModifier *par, int ind, bool isEyeBone){
    toggleSignals(false);
    if (parentFile && bon && par && ind > -1){
        parent = par;
        bsBoneIndex = ind;
        file = parentFile;
        bsData = bon;
        (isEyeBone) ? parameterName = "eyeBones:" : parameterName = "bones:";
        if (!index->count()){
            auto boneNames = QStringList("None") + file->getRigBoneNames();
            index->insertItems(0, boneNames);
        }
        index->setCurrentIndex(bsData->index + 1);
        limitAngleDegrees->setValue(bsData->limitAngleDegrees);
        fwdAxisLS->setValue(bsData->fwdAxisLS);
        onGain->setValue(bsData->onGain);
        offGain->setValue(bsData->offGain);
        enabled->setChecked(bsData->enabled);
        auto varBind = parent->getVariableBindingSetData();
        UIHelper::loadBinding(INDEX_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/index", table, parent);
        UIHelper::loadBinding(FWD_AXIS_LS_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/fwdAxisLS", table, parent);
        UIHelper::loadBinding(LIMIT_ANGLE_DEGREES_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/boneIndex", table, parent);
        UIHelper::loadBinding(ON_GAIN_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/onGain", table, parent);
        UIHelper::loadBinding(OFF_GAIN_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/offGain", table, parent);
        UIHelper::loadBinding(ENABLED_ROW, BINDING_COLUMN, varBind, parameterName+QString::number(bsBoneIndex)+"/enabled", table, parent);
    }else{
        LogFile::writeToLog("BSBoneUI::loadData(): Behavior file, bind or data is null!!!");
    }
    toggleSignals(true);
}

void BSBoneUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, parent);
        };
        switch (row){
        case INDEX_ROW:
            checkisproperty(INDEX_ROW, parameterName+QString::number(bsBoneIndex)+"/index", VARIABLE_TYPE_INT32); break;
        case FWD_AXIS_LS_ROW:
            checkisproperty(FWD_AXIS_LS_ROW, parameterName+QString::number(bsBoneIndex)+"/fwdAxisLS", VARIABLE_TYPE_VECTOR4); break;
        case LIMIT_ANGLE_DEGREES_ROW:
            checkisproperty(LIMIT_ANGLE_DEGREES_ROW, parameterName+QString::number(bsBoneIndex)+"/limitAngleDegrees", VARIABLE_TYPE_REAL); break;
        case ON_GAIN_ROW:
            checkisproperty(ON_GAIN_ROW, parameterName+QString::number(bsBoneIndex)+"/onGain", VARIABLE_TYPE_REAL); break;
        case OFF_GAIN_ROW:
            checkisproperty(OFF_GAIN_ROW, parameterName+QString::number(bsBoneIndex)+"/offGain", VARIABLE_TYPE_REAL); break;
        case ENABLED_ROW:
            checkisproperty(ENABLED_ROW, parameterName+QString::number(bsBoneIndex)+"/enabled", VARIABLE_TYPE_BOOL); break;
        default:
            return;
        }
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("BSBoneUI::setBindingVariable(): The data is nullptr!!");
    }
}

void BSBoneUI::setIndex(int index){
    (bsData && file) ? bsData->index = index - 1, file->setIsChanged(true) : LogFile::writeToLog("BSBoneUI::setindex(): Behavior file or  data is null!!!");
}

void BSBoneUI::setFwdAxisLS(){
    if (bsData && file){
        (bsData->fwdAxisLS != fwdAxisLS->value()) ? bsData->fwdAxisLS = fwdAxisLS->value(), file->setIsChanged(true) :  NULL;
    }else{
        LogFile::writeToLog("BSBoneUI::setfwdAxisLS(): Behavior file or  data is null!!!");
    }
}

void BSBoneUI::setLimitAngleDegrees(){
    if (bsData && file){
        (bsData->limitAngleDegrees != limitAngleDegrees->value()) ? bsData->limitAngleDegrees = limitAngleDegrees->value(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("BSBoneUI::setLimitAngleDegrees(): Behavior file or  data is null!!!");
    }
}

void BSBoneUI::setOnGain(){
    if (bsData && file){
        (bsData->onGain != onGain->value()) ? bsData->onGain = onGain->value(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("BSBoneUI::setOnGain(): Behavior file or  data is null!!!");
    }
}

void BSBoneUI::setOffGain(){
    if (bsData && file){
        (bsData->offGain != offGain->value()) ? bsData->offGain = offGain->value(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("BSBoneUI::setOffGain(): Behavior file or  data is null!!!");
    }
}

void BSBoneUI::setEnabled(){
    if (bsData && file){
        (bsData->enabled != enabled->isChecked()) ? bsData->enabled = enabled->isChecked(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("BSBoneUI::setEnabled(): Behavior file or  data is null!!!");
    }
}

void BSBoneUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case INDEX_ROW:
                checkisproperty(INDEX_ROW, parameterName+QString::number(bsBoneIndex)+"/index"); break;
            case FWD_AXIS_LS_ROW:
                checkisproperty(FWD_AXIS_LS_ROW, parameterName+QString::number(bsBoneIndex)+"/fwdAxisLS"); break;
            case LIMIT_ANGLE_DEGREES_ROW:
                checkisproperty(LIMIT_ANGLE_DEGREES_ROW, parameterName+QString::number(bsBoneIndex)+"/limitAngleDegrees"); break;
            case ON_GAIN_ROW:
                checkisproperty(ON_GAIN_ROW, parameterName+QString::number(bsBoneIndex)+"/onGain"); break;
            case OFF_GAIN_ROW:
                checkisproperty(OFF_GAIN_ROW, parameterName+QString::number(bsBoneIndex)+"/offGain"); break;
            case ENABLED_ROW:
                checkisproperty(ENABLED_ROW, parameterName+QString::number(bsBoneIndex)+"/enabled"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSBoneUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void BSBoneUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData && parent){
        if (viewproperties){
            if (parent->getVariableBindingSetData()){
                emit viewProperties(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (parent->getVariableBindingSetData()){
                emit viewVariables(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BSBoneUI::selectTableToView(): The data or parent is nullptr!!");
    }
}

void BSBoneUI::variableRenamed(const QString & name, int index){
    if (bsData && parent){
        index--;
        auto bind = parent->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname(parameterName+QString::number(bsBoneIndex)+"/index", INDEX_ROW);
            setname(parameterName+QString::number(bsBoneIndex)+"/fwdAxisLS", FWD_AXIS_LS_ROW);
            setname(parameterName+QString::number(bsBoneIndex)+"/limitAngleDegrees", LIMIT_ANGLE_DEGREES_ROW);
            setname(parameterName+QString::number(bsBoneIndex)+"/onGain", ON_GAIN_ROW);
            setname(parameterName+QString::number(bsBoneIndex)+"/offGain", OFF_GAIN_ROW);
            setname(parameterName+QString::number(bsBoneIndex)+"/enabled", ENABLED_ROW);
        }
    }else{
        LogFile::writeToLog("BSBoneUI::variableRenamed(): The data or parent is nullptr!!");
    }
}
