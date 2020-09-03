#include "twistmodifierui.h"

#include "src/hkxclasses/behavior/modifiers/hkbTwistModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 9

#define NAME_ROW 0
#define ENABLE_ROW 1
#define AXIS_OF_ROTATION_ROW 2
#define TWIST_ANGLE_ROW 3
#define START_BONE_INDEX_ROW 4
#define END_BONE_INDEX_ROW 5
#define SET_ANGLE_METHOD_ROW 6
#define ROTATION_AXIS_COORDINATE_ROW 7
#define IS_ADDITIVE_ROW 8

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList TwistModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

TwistModifierUI::TwistModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      axisOfRotation(new QuadVariableWidget),
      twistAngle(new DoubleSpinBox),
      startBoneIndex(new ComboBox),
      endBoneIndex(new ComboBox),
      setAngleMethod(new ComboBox),
      rotationAxisCoordinates(new ComboBox),
      isAdditive(new CheckBox)
{
    setTitle("hkbTwistModifier");
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
    table->setItem(AXIS_OF_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("axisOfRotation"));
    table->setItem(AXIS_OF_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(AXIS_OF_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(AXIS_OF_ROTATION_ROW, VALUE_COLUMN, axisOfRotation);
    table->setItem(TWIST_ANGLE_ROW, NAME_COLUMN, new TableWidgetItem("twistAngle"));
    table->setItem(TWIST_ANGLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(TWIST_ANGLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TWIST_ANGLE_ROW, VALUE_COLUMN, twistAngle);
    table->setItem(START_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("startBoneIndex"));
    table->setItem(START_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(START_BONE_INDEX_ROW, VALUE_COLUMN, startBoneIndex);
    table->setItem(END_BONE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("endBoneIndex"));
    table->setItem(END_BONE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(END_BONE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(END_BONE_INDEX_ROW, VALUE_COLUMN, endBoneIndex);
    table->setItem(SET_ANGLE_METHOD_ROW, NAME_COLUMN, new TableWidgetItem("setAngleMethod"));
    table->setItem(SET_ANGLE_METHOD_ROW, TYPE_COLUMN, new TableWidgetItem("SetAngleMethod", Qt::AlignCenter));
    table->setItem(SET_ANGLE_METHOD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SET_ANGLE_METHOD_ROW, VALUE_COLUMN, setAngleMethod);
    table->setItem(ROTATION_AXIS_COORDINATE_ROW, NAME_COLUMN, new TableWidgetItem("rotationAxisCoordinates"));
    table->setItem(ROTATION_AXIS_COORDINATE_ROW, TYPE_COLUMN, new TableWidgetItem("RotationAxisCoordinates", Qt::AlignCenter));
    table->setItem(ROTATION_AXIS_COORDINATE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(ROTATION_AXIS_COORDINATE_ROW, VALUE_COLUMN, rotationAxisCoordinates);
    table->setItem(IS_ADDITIVE_ROW, NAME_COLUMN, new TableWidgetItem("isAdditive"));
    table->setItem(IS_ADDITIVE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_ADDITIVE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_ADDITIVE_ROW, VALUE_COLUMN, isAdditive);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void TwistModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(axisOfRotation, SIGNAL(editingFinished()), this, SLOT(setAxisOfRotation()), Qt::UniqueConnection);
        connect(twistAngle, SIGNAL(editingFinished()), this, SLOT(setTwistAngle()), Qt::UniqueConnection);
        connect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)), Qt::UniqueConnection);
        connect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)), Qt::UniqueConnection);
        connect(setAngleMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(setSetAngleMethod(int)), Qt::UniqueConnection);
        connect(rotationAxisCoordinates, SIGNAL(currentIndexChanged(int)), this, SLOT(setRotationAxisCoordinates(int)), Qt::UniqueConnection);
        connect(isAdditive, SIGNAL(released()), this, SLOT(setIsAdditive()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(axisOfRotation, SIGNAL(editingFinished()), this, SLOT(setAxisOfRotation()));
        disconnect(twistAngle, SIGNAL(editingFinished()), this, SLOT(setTwistAngle()));
        disconnect(startBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartBoneIndex(int)));
        disconnect(endBoneIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEndBoneIndex(int)));
        disconnect(setAngleMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(setSetAngleMethod(int)));
        disconnect(rotationAxisCoordinates, SIGNAL(currentIndexChanged(int)), this, SLOT(setRotationAxisCoordinates(int)));
        disconnect(isAdditive, SIGNAL(released()), this, SLOT(setIsAdditive()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void TwistModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("TwistModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void TwistModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_TWIST_MODIFIER){
            bsData = static_cast<hkbTwistModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            axisOfRotation->setValue(bsData->getAxisOfRotation());
            twistAngle->setValue(bsData->getTwistAngle());
            auto loadbones = [&](ComboBox *combobox, int indextoset){
                if (!combobox->count()){
                    auto boneNames = QStringList("None") + static_cast<BehaviorFile *>(bsData->getParentFile())->getRigBoneNames();
                    combobox->insertItems(0, boneNames);
                }
                combobox->setCurrentIndex(indextoset);
            };
            loadbones(startBoneIndex, bsData->getStartBoneIndex() + 1);
            loadbones(endBoneIndex, bsData->getEndBoneIndex() + 1);
            auto loadmethods = [&](ComboBox *combobox, const QString & method, const QStringList & methodlist){
                (!combobox->count()) ? combobox->insertItems(0, methodlist) : NULL;
                combobox->setCurrentIndex(methodlist.indexOf(method));
            };
            loadmethods(setAngleMethod, bsData->getSetAngleMethod(), bsData->SetAngleMethod);
            loadmethods(rotationAxisCoordinates, bsData->getRotationAxisCoordinates(), bsData->RotationAxisCoordinates);
            isAdditive->setChecked(bsData->getIsAdditive());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(AXIS_OF_ROTATION_ROW, BINDING_COLUMN, varBind, "axisOfRotation", table, bsData);
            UIHelper::loadBinding(TWIST_ANGLE_ROW, BINDING_COLUMN, varBind, "twistAngle", table, bsData);
            UIHelper::loadBinding(START_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "startBoneIndex", table, bsData);
            UIHelper::loadBinding(END_BONE_INDEX_ROW, BINDING_COLUMN, varBind, "endBoneIndex", table, bsData);
            UIHelper::loadBinding(IS_ADDITIVE_ROW, BINDING_COLUMN, varBind, "isAdditive", table, bsData);
        }else{
            LogFile::writeToLog("TwistModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("TwistModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void TwistModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("TwistModifierUI::setName(): The data is nullptr!!");
    }
}

void TwistModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("TwistModifierUI::setEnable(): The 'bsData' pointer is nullptr!!");
}

void TwistModifierUI::setAxisOfRotation(){
    (bsData) ? bsData->setAxisOfRotation(axisOfRotation->value()) : LogFile::writeToLog("TwistModifierUI::setAxisOfRotation(): The 'bsData' pointer is nullptr!!");
}

void TwistModifierUI::setStartBoneIndex(int index){
    (bsData) ? bsData->setStartBoneIndex(index - 1) : LogFile::writeToLog("TwistModifierUI::setStartBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void TwistModifierUI::setEndBoneIndex(int index){
    (bsData) ? bsData->setEndBoneIndex(index - 1) : LogFile::writeToLog("TwistModifierUI::setEndBoneIndex(): The 'bsData' pointer is nullptr!!");
}

void TwistModifierUI::setTwistAngle(){
    (bsData) ? bsData->setTwistAngle(twistAngle->value()) : LogFile::writeToLog("TwistModifierUI::setTwistAngle(): The data is nullptr!!");
}

void TwistModifierUI::setSetAngleMethod(int index){
    (bsData) ? bsData->setSetAngleMethod(index) : LogFile::writeToLog("TwistModifierUI::setSetAngleMethod(): The data is nullptr!!");
}

void TwistModifierUI::setRotationAxisCoordinates(int index){
    (bsData) ? bsData->setRotationAxisCoordinates(index) : LogFile::writeToLog("TwistModifierUI::setRotationAxisCoordinates(): The data is nullptr!!");
}

void TwistModifierUI::setIsAdditive(){
    (bsData) ? bsData->setIsAdditive(isAdditive->isChecked()) : LogFile::writeToLog("TwistModifierUI::setIsAdditive(): The 'bsData' pointer is nullptr!!");
}

void TwistModifierUI::viewSelected(int row, int column){
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
            case AXIS_OF_ROTATION_ROW:
                checkisproperty(AXIS_OF_ROTATION_ROW, "axisOfRotation"); break;
            case TWIST_ANGLE_ROW:
                checkisproperty(TWIST_ANGLE_ROW, "twistAngle"); break;
            case START_BONE_INDEX_ROW:
                checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex"); break;
            case END_BONE_INDEX_ROW:
                checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex"); break;
            case IS_ADDITIVE_ROW:
                checkisproperty(IS_ADDITIVE_ROW, "isAdditive"); break;
            }
        }
    }else{
        LogFile::writeToLog("TwistModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void TwistModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("TwistModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void TwistModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("axisOfRotation", AXIS_OF_ROTATION_ROW);
            setname("twistAngle", TWIST_ANGLE_ROW);
            setname("startBoneIndex", START_BONE_INDEX_ROW);
            setname("endBoneIndex", END_BONE_INDEX_ROW);
            setname("isAdditive", IS_ADDITIVE_ROW);
        }
    }else{
        LogFile::writeToLog("TwistModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void TwistModifierUI::setBindingVariable(int index, const QString &name){
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
        case AXIS_OF_ROTATION_ROW:
            checkisproperty(AXIS_OF_ROTATION_ROW, "axisOfRotation", VARIABLE_TYPE_VECTOR4); break;
        case TWIST_ANGLE_ROW:
            checkisproperty(TWIST_ANGLE_ROW, "twistAngle", VARIABLE_TYPE_REAL); break;
        case START_BONE_INDEX_ROW:
            checkisproperty(START_BONE_INDEX_ROW, "startBoneIndex", VARIABLE_TYPE_INT32); break;
        case END_BONE_INDEX_ROW:
            checkisproperty(END_BONE_INDEX_ROW, "endBoneIndex", VARIABLE_TYPE_INT32); break;
        case IS_ADDITIVE_ROW:
            checkisproperty(IS_ADDITIVE_ROW, "isAdditive", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("TwistModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
