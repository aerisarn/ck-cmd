#include "computedirectionmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/modifiers/hkbcomputedirectionmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 13

#define NAME_ROW 0
#define ENABLE_ROW 1
#define POINT_IN_ROW 2
#define POINT_OUT_ROW 3
#define GROUND_ANGLE_OUT_ROW 4
#define UP_ANGLE_OUT_ROW 5
#define VERTICAL_OFFSET_ROW 6
#define REVERSE_GROUND_ANGLE_ROW 7
#define REVERSE_UP_ANGLE_ROW 8
#define PROJECT_POINT_ROW 9
#define NORMALIZE_POINT_ROW 10
#define COMPUTE_ONLY_ONCE_ROW 11
#define COMPUTED_OUTPUT_ROW 12

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList ComputeDirectionModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

ComputeDirectionModifierUI::ComputeDirectionModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      pointIn(new QuadVariableWidget),
      pointOut(new QuadVariableWidget),
      groundAngleOut(new DoubleSpinBox),
      upAngleOut(new DoubleSpinBox),
      verticalOffset(new DoubleSpinBox),
      reverseGroundAngle(new CheckBox),
      reverseUpAngle(new CheckBox),
      projectPoint(new CheckBox),
      normalizePoint(new CheckBox),
      computeOnlyOnce(new CheckBox),
      computedOutput(new CheckBox)
{
    setTitle("hkbComputeDirectionModifier");
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
    table->setItem(POINT_IN_ROW, NAME_COLUMN, new TableWidgetItem("pointIn"));
    table->setItem(POINT_IN_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(POINT_IN_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POINT_IN_ROW, VALUE_COLUMN, pointIn);
    table->setItem(POINT_OUT_ROW, NAME_COLUMN, new TableWidgetItem("pointOut"));
    table->setItem(POINT_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(POINT_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(POINT_OUT_ROW, VALUE_COLUMN, pointOut);
    table->setItem(GROUND_ANGLE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("groundAngleOut"));
    table->setItem(GROUND_ANGLE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(GROUND_ANGLE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GROUND_ANGLE_OUT_ROW, VALUE_COLUMN, groundAngleOut);
    table->setItem(UP_ANGLE_OUT_ROW, NAME_COLUMN, new TableWidgetItem("upAngleOut"));
    table->setItem(UP_ANGLE_OUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(UP_ANGLE_OUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(UP_ANGLE_OUT_ROW, VALUE_COLUMN, upAngleOut);
    table->setItem(VERTICAL_OFFSET_ROW, NAME_COLUMN, new TableWidgetItem("verticalOffset"));
    table->setItem(VERTICAL_OFFSET_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(VERTICAL_OFFSET_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VERTICAL_OFFSET_ROW, VALUE_COLUMN, verticalOffset);
    table->setItem(REVERSE_GROUND_ANGLE_ROW, NAME_COLUMN, new TableWidgetItem("reverseGroundAngle"));
    table->setItem(REVERSE_GROUND_ANGLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(REVERSE_GROUND_ANGLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(REVERSE_GROUND_ANGLE_ROW, VALUE_COLUMN, reverseGroundAngle);
    table->setItem(REVERSE_UP_ANGLE_ROW, NAME_COLUMN, new TableWidgetItem("reverseUpAngle"));
    table->setItem(REVERSE_UP_ANGLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(REVERSE_UP_ANGLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(REVERSE_UP_ANGLE_ROW, VALUE_COLUMN, reverseUpAngle);
    table->setItem(PROJECT_POINT_ROW, NAME_COLUMN, new TableWidgetItem("projectPoint"));
    table->setItem(PROJECT_POINT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(PROJECT_POINT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(PROJECT_POINT_ROW, VALUE_COLUMN, projectPoint);
    table->setItem(NORMALIZE_POINT_ROW, NAME_COLUMN, new TableWidgetItem("normalizePoint"));
    table->setItem(NORMALIZE_POINT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(NORMALIZE_POINT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NORMALIZE_POINT_ROW, VALUE_COLUMN, normalizePoint);
    table->setItem(COMPUTE_ONLY_ONCE_ROW, NAME_COLUMN, new TableWidgetItem("computeOnlyOnce"));
    table->setItem(COMPUTE_ONLY_ONCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(COMPUTE_ONLY_ONCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COMPUTE_ONLY_ONCE_ROW, VALUE_COLUMN, computeOnlyOnce);
    table->setItem(COMPUTED_OUTPUT_ROW, NAME_COLUMN, new TableWidgetItem("computedOutput"));
    table->setItem(COMPUTED_OUTPUT_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(COMPUTED_OUTPUT_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(COMPUTED_OUTPUT_ROW, VALUE_COLUMN, computedOutput);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ComputeDirectionModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(pointIn, SIGNAL(editingFinished()), this, SLOT(setPointIn()), Qt::UniqueConnection);
        connect(pointOut, SIGNAL(editingFinished()), this, SLOT(setPointOut()), Qt::UniqueConnection);
        connect(groundAngleOut, SIGNAL(editingFinished()), this, SLOT(setGroundAngleOut()), Qt::UniqueConnection);
        connect(upAngleOut, SIGNAL(editingFinished()), this, SLOT(setUpAngleOut()), Qt::UniqueConnection);
        connect(verticalOffset, SIGNAL(editingFinished()), this, SLOT(setVerticalOffset()), Qt::UniqueConnection);
        connect(reverseGroundAngle, SIGNAL(released()), this, SLOT(setReverseGroundAngle()), Qt::UniqueConnection);
        connect(reverseUpAngle, SIGNAL(released()), this, SLOT(setReverseUpAngle()), Qt::UniqueConnection);
        connect(projectPoint, SIGNAL(released()), this, SLOT(setProjectPoint()), Qt::UniqueConnection);
        connect(normalizePoint, SIGNAL(released()), this, SLOT(setNormalizePoint()), Qt::UniqueConnection);
        connect(computeOnlyOnce, SIGNAL(released()), this, SLOT(setComputeOnlyOnce()), Qt::UniqueConnection);
        connect(computedOutput, SIGNAL(released()), this, SLOT(setComputedOutput()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(pointIn, SIGNAL(editingFinished()), this, SLOT(setPointIn()));
        disconnect(pointOut, SIGNAL(editingFinished()), this, SLOT(setPointOut()));
        disconnect(groundAngleOut, SIGNAL(editingFinished()), this, SLOT(setGroundAngleOut()));
        disconnect(upAngleOut, SIGNAL(editingFinished()), this, SLOT(setUpAngleOut()));
        disconnect(verticalOffset, SIGNAL(editingFinished()), this, SLOT(setVerticalOffset()));
        disconnect(reverseGroundAngle, SIGNAL(released()), this, SLOT(setReverseGroundAngle()));
        disconnect(reverseUpAngle, SIGNAL(released()), this, SLOT(setReverseUpAngle()));
        disconnect(projectPoint, SIGNAL(released()), this, SLOT(setProjectPoint()));
        disconnect(normalizePoint, SIGNAL(released()), this, SLOT(setNormalizePoint()));
        disconnect(computeOnlyOnce, SIGNAL(released()), this, SLOT(setComputeOnlyOnce()));
        disconnect(computedOutput, SIGNAL(released()), this, SLOT(setComputedOutput()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void ComputeDirectionModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties){
    if (variables && properties){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void ComputeDirectionModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == HKB_COMPUTE_DIRECTION_MODIFIER){
            bsData = static_cast<hkbComputeDirectionModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            pointIn->setValue(bsData->getPointIn());
            pointOut->setValue(bsData->getPointOut());
            groundAngleOut->setValue(bsData->getGroundAngleOut());
            upAngleOut->setValue(bsData->getUpAngleOut());
            verticalOffset->setValue(bsData->getVerticalOffset());
            reverseGroundAngle->setChecked(bsData->getReverseGroundAngle());
            reverseUpAngle->setChecked(bsData->getReverseUpAngle());
            projectPoint->setChecked(bsData->getProjectPoint());
            normalizePoint->setChecked(bsData->getNormalizePoint());
            computeOnlyOnce->setChecked(bsData->getComputeOnlyOnce());
            computedOutput->setChecked(bsData->getComputedOutput());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(POINT_IN_ROW, BINDING_COLUMN, varBind, "pointIn", table, bsData);
            UIHelper::loadBinding(POINT_OUT_ROW, BINDING_COLUMN, varBind, "pointOut", table, bsData);
            UIHelper::loadBinding(GROUND_ANGLE_OUT_ROW, BINDING_COLUMN, varBind, "groundAngleOut", table, bsData);
            UIHelper::loadBinding(UP_ANGLE_OUT_ROW, BINDING_COLUMN, varBind, "upAngleOut", table, bsData);
            UIHelper::loadBinding(VERTICAL_OFFSET_ROW, BINDING_COLUMN, varBind, "verticalOffset", table, bsData);
            UIHelper::loadBinding(REVERSE_GROUND_ANGLE_ROW, BINDING_COLUMN, varBind, "reverseGroundAngle", table, bsData);
            UIHelper::loadBinding(REVERSE_UP_ANGLE_ROW, BINDING_COLUMN, varBind, "reverseUpAngle", table, bsData);
            UIHelper::loadBinding(PROJECT_POINT_ROW, BINDING_COLUMN, varBind, "projectPoint", table, bsData);
            UIHelper::loadBinding(NORMALIZE_POINT_ROW, BINDING_COLUMN, varBind, "normalizePoint", table, bsData);
            UIHelper::loadBinding(COMPUTE_ONLY_ONCE_ROW, BINDING_COLUMN, varBind, "computeOnlyOnce", table, bsData);
            UIHelper::loadBinding(COMPUTED_OUTPUT_ROW, BINDING_COLUMN, varBind, "computedOutput", table, bsData);
        }else{
            LogFile::writeToLog("ComputeDirectionModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void ComputeDirectionModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::setName(): The data is nullptr!!");
    }
}

void ComputeDirectionModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setEnable(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setPointIn(){
    (bsData) ? bsData->setPointIn(pointIn->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setPointIn(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setPointOut(){
    (bsData) ? bsData->setPointOut(pointOut->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setPointOut(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setGroundAngleOut(){
    (bsData) ? bsData->setGroundAngleOut(groundAngleOut->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setGroundAngleOut(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setUpAngleOut(){
    (bsData) ? bsData->setUpAngleOut(upAngleOut->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setUpAngleOut(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setVerticalOffset(){
    (bsData) ? bsData->setVerticalOffset(verticalOffset->value()) : LogFile::writeToLog("ComputeDirectionModifierUI::setVerticalOffset(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setReverseGroundAngle(){
    (bsData) ? bsData->setReverseGroundAngle(reverseGroundAngle->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setReverseGroundAngle(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setReverseUpAngle(){
    (bsData) ? bsData->setReverseUpAngle(reverseUpAngle->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setReverseUpAngle(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setProjectPoint(){
    (bsData) ? bsData->setProjectPoint(projectPoint->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setProjectPoint(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setNormalizePoint(){
    (bsData) ? bsData->setNormalizePoint(normalizePoint->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setNormalizePoint(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setComputeOnlyOnce(){
    (bsData) ? bsData->setComputeOnlyOnce(computeOnlyOnce->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setComputeOnlyOnce(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::setComputedOutput(){
    (bsData) ? bsData->setComputedOutput(computedOutput->isChecked()) : LogFile::writeToLog("ComputeDirectionModifierUI::setComputedOutput(): The data is nullptr!!");
}

void ComputeDirectionModifierUI::viewSelected(int row, int column){
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
            case POINT_IN_ROW:
                checkisproperty(POINT_IN_ROW, "pointIn"); break;
            case POINT_OUT_ROW:
                checkisproperty(POINT_OUT_ROW, "pointOut"); break;
            case GROUND_ANGLE_OUT_ROW:
                checkisproperty(GROUND_ANGLE_OUT_ROW, "groundAngleOut"); break;
            case UP_ANGLE_OUT_ROW:
                checkisproperty(UP_ANGLE_OUT_ROW, "upAngleOut"); break;
            case VERTICAL_OFFSET_ROW:
                checkisproperty(VERTICAL_OFFSET_ROW, "verticalOffset"); break;
            case REVERSE_GROUND_ANGLE_ROW:
                checkisproperty(REVERSE_GROUND_ANGLE_ROW, "reverseGroundAngle"); break;
            case REVERSE_UP_ANGLE_ROW:
                checkisproperty(REVERSE_UP_ANGLE_ROW, "reverseUpAngle"); break;
            case PROJECT_POINT_ROW:
                checkisproperty(PROJECT_POINT_ROW, "projectPoint"); break;
            case NORMALIZE_POINT_ROW:
                checkisproperty(NORMALIZE_POINT_ROW, "normalizePoint"); break;
            case COMPUTE_ONLY_ONCE_ROW:
                checkisproperty(COMPUTE_ONLY_ONCE_ROW, "computeOnlyOnce"); break;
            case COMPUTED_OUTPUT_ROW:
                checkisproperty(COMPUTED_OUTPUT_ROW, "computedOutput"); break;
            }
        }
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeDirectionModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("ComputeDirectionModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void ComputeDirectionModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("pointIn", POINT_IN_ROW);
            setname("pointOut", POINT_OUT_ROW);
            setname("groundAngleOut", GROUND_ANGLE_OUT_ROW);
            setname("upAngleOut", UP_ANGLE_OUT_ROW);
            setname("verticalOffset", VERTICAL_OFFSET_ROW);
            setname("reverseGroundAngle", REVERSE_GROUND_ANGLE_ROW);
            setname("reverseUpAngle", REVERSE_UP_ANGLE_ROW);
            setname("projectPoint", PROJECT_POINT_ROW);
            setname("normalizePoint", NORMALIZE_POINT_ROW);
            setname("computeOnlyOnce", COMPUTE_ONLY_ONCE_ROW);
            setname("computedOutput", COMPUTED_OUTPUT_ROW);
        }
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void ComputeDirectionModifierUI::setBindingVariable(int index, const QString &name){
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
        case POINT_IN_ROW:
            checkisproperty(POINT_IN_ROW, "pointIn", VARIABLE_TYPE_VECTOR4); break;
        case POINT_OUT_ROW:
            checkisproperty(POINT_OUT_ROW, "pointOut", VARIABLE_TYPE_VECTOR4); break;
        case GROUND_ANGLE_OUT_ROW:
            checkisproperty(GROUND_ANGLE_OUT_ROW, "groundAngleOut", VARIABLE_TYPE_REAL); break;
        case UP_ANGLE_OUT_ROW:
            checkisproperty(UP_ANGLE_OUT_ROW, "upAngleOut", VARIABLE_TYPE_REAL); break;
        case VERTICAL_OFFSET_ROW:
            checkisproperty(VERTICAL_OFFSET_ROW, "verticalOffset", VARIABLE_TYPE_REAL); break;
        case REVERSE_GROUND_ANGLE_ROW:
            checkisproperty(REVERSE_GROUND_ANGLE_ROW, "reverseGroundAngle", VARIABLE_TYPE_BOOL); break;
        case REVERSE_UP_ANGLE_ROW:
            checkisproperty(REVERSE_UP_ANGLE_ROW, "reverseUpAngle", VARIABLE_TYPE_BOOL); break;
        case PROJECT_POINT_ROW:
            checkisproperty(PROJECT_POINT_ROW, "projectPoint", VARIABLE_TYPE_BOOL); break;
        case NORMALIZE_POINT_ROW:
            checkisproperty(NORMALIZE_POINT_ROW, "normalizePoint", VARIABLE_TYPE_BOOL); break;
        case COMPUTE_ONLY_ONCE_ROW:
            checkisproperty(COMPUTE_ONLY_ONCE_ROW, "computeOnlyOnce", VARIABLE_TYPE_BOOL); break;
        case COMPUTED_OUTPUT_ROW:
            checkisproperty(COMPUTED_OUTPUT_ROW, "computedOutput", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("ComputeDirectionModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
