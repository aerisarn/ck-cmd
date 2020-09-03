#include "skyrimanimationdataui.h"

#include "src/ui/genericdatawidgets.h"
#include "src/ui/animationrotationui.h"
#include "src/ui/animationtranslationui.h"
#include "src/ui/mainwindow.h"

#include "src/animData/skyrimanimationmotiondata.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define DURATION_ROW 1
#define ADD_TRANSLATION_ROW 2
#define INITIAL_ROTATION_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList SkyrimAnimationDataUI::headerLabels = {
    "Name",
    "Type",
    "",
    "Value"
};

SkyrimAnimationDataUI::SkyrimAnimationDataUI()
    : bsData(nullptr),
      rotationsButtonRow(INITIAL_ROTATION_ROW),
      groupBox(new QGroupBox("AnimationData")),
      topLyt(new QGridLayout),
      rotationUI(new AnimationRotationUI()),
      translationUI(new AnimationTranslationUI()),
      table(new TableWidget(QColor(Qt::white))),
      returnPB(new QPushButton("Return")),
      duration(new DoubleSpinBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(NAME_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter));
    table->setItem(DURATION_ROW, NAME_COLUMN, new TableWidgetItem("duration"));
    table->setItem(DURATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(DURATION_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(DURATION_ROW, VALUE_COLUMN, duration);
    table->setItem(ADD_TRANSLATION_ROW, NAME_COLUMN, new TableWidgetItem("Add Translation", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a Translation"));
    table->setItem(ADD_TRANSLATION_ROW, TYPE_COLUMN, new TableWidgetItem("Translation", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(ADD_TRANSLATION_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Translation", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(ADD_TRANSLATION_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Translation", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_ROTATION_ROW, NAME_COLUMN, new TableWidgetItem("Add Rotation", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a Rotation"));
    table->setItem(INITIAL_ROTATION_ROW, TYPE_COLUMN, new TableWidgetItem("Rotation", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(INITIAL_ROTATION_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Rotation", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_ROTATION_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Rotation", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(translationUI);
    addWidget(rotationUI);
    toggleSignals(true);
}

void SkyrimAnimationDataUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(translationUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(rotationUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(duration, SIGNAL(editingFinished()), this, SLOT(setDuration()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(translationUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(rotationUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
    }
}

void SkyrimAnimationDataUI::loadData(SkyrimAnimationMotionData *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data){
        bsData = data;
        duration->setValue(bsData->duration);
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("SkyrimAnimationDataUI::loadData(): The data is nullptr or an incorrect type!!");
    }
    toggleSignals(true);
}

void SkyrimAnimationDataUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_TRANSLATION_ROW + bsData->translations.size() + 1 - rotationsButtonRow;
        if (temp > 0){
            for (auto i = 0; i < temp; i++){
                table->insertRow(rotationsButtonRow);
                rotationsButtonRow++;
            }
        }else if (temp < 0){
            for (auto i = temp; i < 0; i++){
                table->removeRow(rotationsButtonRow - 1);
                rotationsButtonRow--;
            }
        }
        rotationsButtonRow = ADD_TRANSLATION_ROW + bsData->translations.size() + 1;
        for (auto i = INITIAL_ROTATION_ROW, j = 0; i < rotationsButtonRow, j < bsData->translations.size(); i++, j++){
            UIHelper::setRowItems(i, "Translation "+QString::number(j), "hkVector3", "Remove", "Edit", "Double click to remove this translation", "Double click to edit this translation", table);
        }
        table->setRowCount(rotationsButtonRow + bsData->rotations.size() + 1);
        for (auto i = rotationsButtonRow + 1, j = 0; i < table->rowCount(), j < bsData->rotations.size(); i++, j++){
            UIHelper::setRowItems(i, "Rotation "+QString::number(j), "hkQuaternion", "Remove", "Edit", "Double click to remove this quaternion", "Double click to edit this quaternion", table);
        }
    }else{
        LogFile::writeToLog("SkyrimAnimationDataUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void SkyrimAnimationDataUI::setDuration(){
    (bsData) ? bsData->duration = duration->value() : LogFile::writeToLog("SkyrimAnimationDataUI::setduration(): The data is nullptr!!");
}

void SkyrimAnimationDataUI::addTranslation(){
    (bsData) ? bsData->addTranslation(), loadDynamicTableRows() : LogFile::writeToLog("SkyrimAnimationDataUI::addEnterEvent(): The data is nullptr!!");
}

void SkyrimAnimationDataUI::removeTranslation(int index){
    (bsData) ? bsData->removeTranslation(index), loadDynamicTableRows() : LogFile::writeToLog("SkyrimAnimationDataUI::removeEnterEvent(): The data is nullptr!!");
}

void SkyrimAnimationDataUI::addRotation(){
    (bsData) ? bsData->addRotation(), loadDynamicTableRows() : LogFile::writeToLog("SkyrimAnimationDataUI::addExitEvent(): The data is nullptr!!");
}

void SkyrimAnimationDataUI::removeRotation(int index){
    (bsData) ? bsData->removeRotation(index), loadDynamicTableRows() : LogFile::writeToLog("SkyrimAnimationDataUI::removeExitEvent(): The data is nullptr!!");
}

void SkyrimAnimationDataUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == ADD_TRANSLATION_ROW && column == NAME_COLUMN){
            addTranslation();
        }else if (row == rotationsButtonRow && column == NAME_COLUMN){
            addRotation();
        }else if (row > ADD_TRANSLATION_ROW && row < rotationsButtonRow){
            auto result = row - ADD_TRANSLATION_ROW - 1;
            if (bsData->translations.size() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    translationUI->loadData(&bsData->translations[result], bsData->duration);
                    setCurrentIndex(TRANSLATION_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the enter translation \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeTranslation(result);
                    }
                }
            }else{
                LogFile::writeToLog("SkyrimAnimationDataUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }else if (row > rotationsButtonRow && row < table->rowCount()){
            auto result = row - BASE_NUMBER_OF_ROWS - bsData->translations.size();
            if (result < bsData->rotations.size() && result >= 0){
                if (column == VALUE_COLUMN){
                    rotationUI->loadData(&bsData->rotations[result], bsData->duration);
                    setCurrentIndex(ROTATION_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the rotation \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeRotation(result);
                    }
                }
            }else{
                LogFile::writeToLog("SkyrimAnimationDataUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("SkyrimAnimationDataUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void SkyrimAnimationDataUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}
