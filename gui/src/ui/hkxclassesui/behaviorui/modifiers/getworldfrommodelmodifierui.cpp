#include "getworldfrommodelmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbgetworldfrommodelmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define ENABLE_ROW 1
#define TRANSLATION_OUT_ROW 2
#define ROTATION_OUT_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList GetWorldFromModelModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

GetWorldFromModelModifierUI::GetWorldFromModelModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      translationOut(new QuadVariableWidget),
      rotationOut(new QuadVariableWidget)
{
    setTitle("hkbGetWorldFromModelModifier");
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
    table->setItem(TRANSLATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("translationOut"));
    table->setItem(TRANSLATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TRANSLATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TRANSLATION_OUT_ROW, VALUE_COLUMN, translationOut);
    table->setItem(ROTATION_OUT_ROW, NAME_COLUMN, new TableWidgetItem("rotationOut"));
    table->setItem(ROTATION_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkQuaternion", Qt::AlignCenter));
    table->setItem(ROTATION_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ROTATION_OUT_ROW, VALUE_COLUMN, rotationOut);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void GetWorldFromModelModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(translationOut, SIGNAL(editingFinished()), this, SLOT(setTranslationOut()), Qt::UniqueConnection);
        connect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(translationOut, SIGNAL(editingFinished()), this, SLOT(setTranslationOut()));
        disconnect(rotationOut, SIGNAL(editingFinished()), this, SLOT(setRotationOut()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void GetWorldFromModelModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void GetWorldFromModelModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_GET_WORLD_FROM_MODEL_MODIFIER){
            bsData = static_cast<hkbGetWorldFromModelModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            translationOut->setValue(bsData->getTranslationOut());
            rotationOut->setValue(bsData->getRotationOut());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(TRANSLATION_OUT_ROW, BINDING_COLUMN, varBind, "translationOut", table, bsData);
            UIHelper::loadBinding(ROTATION_OUT_ROW, BINDING_COLUMN, varBind, "rotationOut", table, bsData);
        }else{
            LogFile::writeToLog("GetWorldFromModelModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void GetWorldFromModelModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::setName(): The data is nullptr!!");
    }
}

void GetWorldFromModelModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("GetWorldFromModelModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void GetWorldFromModelModifierUI::setTranslationOut(){
    (bsData) ? bsData->setTranslationOut(translationOut->value()) : LogFile::writeToLog("GetWorldFromModelModifierUI::setTranslationOut(): The data is nullptr!!");
}

void GetWorldFromModelModifierUI::setRotationOut(){
    (bsData) ? bsData->setRotationOut(rotationOut->value()) : LogFile::writeToLog("GetWorldFromModelModifierUI::setRotationOut(): The data is nullptr!!");
}

void GetWorldFromModelModifierUI::viewSelected(int row, int column){
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
            case TRANSLATION_OUT_ROW:
                checkisproperty(TRANSLATION_OUT_ROW, "translationOut"); break;
            case ROTATION_OUT_ROW:
                checkisproperty(ROTATION_OUT_ROW, "rotationOut"); break;
            }
        }
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void GetWorldFromModelModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("GetWorldFromModelModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void GetWorldFromModelModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("translationOut", TRANSLATION_OUT_ROW);
            setname("rotationOut", ROTATION_OUT_ROW);
        }
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void GetWorldFromModelModifierUI::setBindingVariable(int index, const QString &name){
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
        case TRANSLATION_OUT_ROW:
            checkisproperty(TRANSLATION_OUT_ROW, "translationOut", VARIABLE_TYPE_VECTOR4); break;
        case ROTATION_OUT_ROW:
            checkisproperty(ROTATION_OUT_ROW, "rotationOut", VARIABLE_TYPE_QUATERNION); break;
        }
    }else{
        LogFile::writeToLog("GetWorldFromModelModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
