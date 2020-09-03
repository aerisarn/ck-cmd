#include "bsinterpvaluemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/bsinterpvaluemodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define SOURCE_ROW 2
#define TARGET_ROW 3
#define RESULT_ROW 4
#define GAIN_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSInterpValueModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSInterpValueModifierUI::BSInterpValueModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      source(new DoubleSpinBox),
      target(new DoubleSpinBox),
      result(new DoubleSpinBox),
      gain(new DoubleSpinBox)
{
    setTitle("BSInterpValueModifier");
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
    table->setItem(SOURCE_ROW, NAME_COLUMN, new TableWidgetItem("source"));
    table->setItem(SOURCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(SOURCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(SOURCE_ROW, VALUE_COLUMN, source);
    table->setItem(TARGET_ROW, NAME_COLUMN, new TableWidgetItem("target"));
    table->setItem(TARGET_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TARGET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_ROW, VALUE_COLUMN, target);
    table->setItem(RESULT_ROW, NAME_COLUMN, new TableWidgetItem("result"));
    table->setItem(RESULT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(RESULT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RESULT_ROW, VALUE_COLUMN, result);
    table->setItem(GAIN_ROW, NAME_COLUMN, new TableWidgetItem("gain"));
    table->setItem(GAIN_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GAIN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GAIN_ROW, VALUE_COLUMN, gain);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSInterpValueModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(source, SIGNAL(editingFinished()), this, SLOT(setSource()), Qt::UniqueConnection);
        connect(target, SIGNAL(editingFinished()), this, SLOT(setTarget()), Qt::UniqueConnection);
        connect(result, SIGNAL(editingFinished()), this, SLOT(setResult()), Qt::UniqueConnection);
        connect(gain, SIGNAL(editingFinished()), this, SLOT(setGain()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(source, SIGNAL(editingFinished()), this, SLOT(setSource()));
        disconnect(target, SIGNAL(editingFinished()), this, SLOT(setTarget()));
        disconnect(result, SIGNAL(editingFinished()), this, SLOT(setResult()));
        disconnect(gain, SIGNAL(editingFinished()), this, SLOT(setGain()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSInterpValueModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSInterpValueModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_INTERP_VALUE_MODIFIER){
            bsData = static_cast<BSInterpValueModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            source->setValue(bsData->getSource());
            target->setValue(bsData->getTarget());
            result->setValue(bsData->getResult());
            gain->setValue(bsData->getGain());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(SOURCE_ROW, BINDING_COLUMN, varBind, "source", table, bsData);
            UIHelper::loadBinding(TARGET_ROW, BINDING_COLUMN, varBind, "target", table, bsData);
            UIHelper::loadBinding(RESULT_ROW, BINDING_COLUMN, varBind, "result", table, bsData);
            UIHelper::loadBinding(GAIN_ROW, BINDING_COLUMN, varBind, "gain", table, bsData);
        }else{
            LogFile::writeToLog("BSInterpValueModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSInterpValueModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::setName(): The data is nullptr!!");
    }
}

void BSInterpValueModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSInterpValueModifierUI::setEnable(): The data is nullptr!!");
}

void BSInterpValueModifierUI::setSource(){
    (bsData) ? bsData->setSource(source->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setSource(): The data is nullptr!!");
}

void BSInterpValueModifierUI::setTarget(){
    (bsData) ? bsData->setTarget(target->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setTarget(): The data is nullptr!!");
}

void BSInterpValueModifierUI::setResult(){
    (bsData) ? bsData->setResult(result->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setResult(): The data is nullptr!!");
}

void BSInterpValueModifierUI::setGain(){
    (bsData) ? bsData->setGain(gain->value()) : LogFile::writeToLog("BSInterpValueModifierUI::setGain(): The data is nullptr!!");
}

void BSInterpValueModifierUI::viewSelected(int row, int column){
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
            case SOURCE_ROW:
                checkisproperty(SOURCE_ROW, "source"); break;
            case TARGET_ROW:
                checkisproperty(TARGET_ROW, "target"); break;
            case RESULT_ROW:
                checkisproperty(RESULT_ROW, "result"); break;
            case GAIN_ROW:
                checkisproperty(GAIN_ROW, "gain"); break;
            }
        }
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSInterpValueModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSInterpValueModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSInterpValueModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("source", SOURCE_ROW);
            setname("target", TARGET_ROW);
            setname("result", RESULT_ROW);
            setname("gain", GAIN_ROW);
        }
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSInterpValueModifierUI::setBindingVariable(int index, const QString &name){
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
        case SOURCE_ROW:
            checkisproperty(SOURCE_ROW, "source", VARIABLE_TYPE_REAL); break;
        case TARGET_ROW:
            checkisproperty(TARGET_ROW, "target", VARIABLE_TYPE_REAL); break;
        case RESULT_ROW:
            checkisproperty(RESULT_ROW, "result", VARIABLE_TYPE_REAL); break;
        case GAIN_ROW:
            checkisproperty(GAIN_ROW, "gain", VARIABLE_TYPE_REAL); break;
        }
    }else{
        LogFile::writeToLog("BSInterpValueModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
