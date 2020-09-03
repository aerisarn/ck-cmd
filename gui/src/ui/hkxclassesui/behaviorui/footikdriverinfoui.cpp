#include "footikdriverinfoui.h"
#include "footikdriverinfolegui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbfootikdriverinfo.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"

#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QSpinBox>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 11

const QStringList FootIkDriverInfoUI::headerLabels1 = {
    "Name",
    "Type",
    "Value"
};

FootIkDriverInfoUI::FootIkDriverInfoUI()
    : bsData(nullptr),
      footDriverGB(new QGroupBox("hkbFootIkDriverInfo")),
      legUI(new FootIkDriverInfoLegUI),
      footDriverLyt(new QGridLayout),
      addLegPB(new QPushButton("Add leg")),
      removeLegPB(new QPushButton("Remove selected leg")),
      table(new TableWidget),
      raycastDistanceUp(new DoubleSpinBox),
      raycastDistanceDown(new DoubleSpinBox),
      originalGroundHeightMS(new DoubleSpinBox),
      verticalOffset(new DoubleSpinBox),
      collisionFilterInfo(new SpinBox),
      forwardAlignFraction(new DoubleSpinBox),
      sidewaysAlignFraction(new DoubleSpinBox),
      sidewaysSampleWidth(new DoubleSpinBox),
      lockFeetWhenPlanted(new CheckBox),
      useCharacterUpVector(new CheckBox),
      isQuadrupedNarrow(new CheckBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels1);
    table->setItem(0, 0, new QTableWidgetItem("raycastDistanceUp"));
    table->setItem(0, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(0, 2, raycastDistanceUp);
    table->setItem(1, 0, new QTableWidgetItem("raycastDistanceDown"));
    table->setItem(1, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(1, 2, raycastDistanceDown);
    table->setItem(2, 0, new QTableWidgetItem("originalGroundHeightMS"));
    table->setItem(2, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(2, 2, originalGroundHeightMS);
    table->setItem(3, 0, new QTableWidgetItem("verticalOffset"));
    table->setItem(3, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(3, 2, verticalOffset);
    table->setItem(4, 0, new QTableWidgetItem("collisionFilterInfo"));
    table->setItem(4, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(4, 2, collisionFilterInfo);
    table->setItem(5, 0, new QTableWidgetItem("forwardAlignFraction"));
    table->setItem(5, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(5, 2, forwardAlignFraction);
    table->setItem(6, 0, new QTableWidgetItem("sidewaysAlignFraction"));
    table->setItem(6, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(6, 2, sidewaysAlignFraction);
    table->setItem(7, 0, new QTableWidgetItem("sidewaysSampleWidth"));
    table->setItem(7, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(7, 2, sidewaysSampleWidth);
    table->setItem(8, 0, new QTableWidgetItem("lockFeetWhenPlanted"));
    table->setItem(8, 1, new QTableWidgetItem("hkBool"));
    table->setCellWidget(8, 2, lockFeetWhenPlanted);
    table->setItem(9, 0, new QTableWidgetItem("useCharacterUpVector"));
    table->setItem(9, 1, new QTableWidgetItem("hkBool"));
    table->setCellWidget(9, 2, useCharacterUpVector);
    table->setItem(10, 0, new QTableWidgetItem("isQuadrupedNarrow"));
    table->setItem(10, 1, new QTableWidgetItem("hkBool"));
    table->setCellWidget(10, 2, isQuadrupedNarrow);
    footDriverLyt->addWidget(addLegPB, 0, 0, 1, 1);
    footDriverLyt->addWidget(removeLegPB, 0, 2, 1, 1);
    footDriverLyt->addWidget(table, 1, 0, 8, 3);
    footDriverGB->setLayout(footDriverLyt);
    addWidget(footDriverGB);
    addWidget(legUI);
    connect(raycastDistanceUp, SIGNAL(editingFinished()), this, SLOT(setRaycastDistanceUp()), Qt::UniqueConnection);
    connect(raycastDistanceDown, SIGNAL(editingFinished()), this, SLOT(setRaycastDistanceDown()), Qt::UniqueConnection);
    connect(originalGroundHeightMS, SIGNAL(editingFinished()), this, SLOT(setOriginalGroundHeightMS()), Qt::UniqueConnection);
    connect(verticalOffset, SIGNAL(editingFinished()), this, SLOT(setVerticalOffset()), Qt::UniqueConnection);
    connect(collisionFilterInfo, SIGNAL(editingFinished()), this, SLOT(setCollisionFilterInfo()), Qt::UniqueConnection);
    connect(forwardAlignFraction, SIGNAL(editingFinished()), this, SLOT(setForwardAlignFraction()), Qt::UniqueConnection);
    connect(sidewaysAlignFraction, SIGNAL(editingFinished()), this, SLOT(setSidewaysAlignFraction()), Qt::UniqueConnection);
    connect(sidewaysSampleWidth, SIGNAL(editingFinished()), this, SLOT(setSidewaysSampleWidth()), Qt::UniqueConnection);
    connect(lockFeetWhenPlanted, SIGNAL(released()), this, SLOT(setLockFeetWhenPlanted()), Qt::UniqueConnection);
    connect(useCharacterUpVector, SIGNAL(released()), this, SLOT(setUseCharacterUpVector()), Qt::UniqueConnection);
    connect(isQuadrupedNarrow, SIGNAL(released()), this, SLOT(setIsQuadrupedNarrow()), Qt::UniqueConnection);
    connect(addLegPB, SIGNAL(released()), this, SLOT(addLeg()), Qt::UniqueConnection);
    connect(removeLegPB, SIGNAL(released()), this, SLOT(removeSelectedLeg()), Qt::UniqueConnection);
    connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewSelectedLeg(int,int)), Qt::UniqueConnection);
    connect(legUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
}

void FootIkDriverInfoUI::loadData(HkxObject *data){
    if (data){
        if (data->getSignature() == HKB_FOOT_IK_DRIVER_INFO){
            bsData = static_cast<hkbFootIkDriverInfo *>(data);
            raycastDistanceUp->setValue(bsData->getRaycastDistanceUp());
            raycastDistanceDown->setValue(bsData->getRaycastDistanceDown());
            originalGroundHeightMS->setValue(bsData->getOriginalGroundHeightMS());
            verticalOffset->setValue(bsData->getVerticalOffset());
            collisionFilterInfo->setValue(bsData->getCollisionFilterInfo());
            forwardAlignFraction->setValue(bsData->getForwardAlignFraction());
            sidewaysAlignFraction->setValue(bsData->getSidewaysAlignFraction());
            sidewaysSampleWidth->setValue(bsData->getSidewaysSampleWidth());
            lockFeetWhenPlanted->setChecked(bsData->getLockFeetWhenPlanted());
            useCharacterUpVector->setChecked(bsData->getUseCharacterUpVector());
            isQuadrupedNarrow->setChecked(bsData->getIsQuadrupedNarrow());
            auto numlegs = bsData->getNumberOfLegs();
            for (auto i = 0, k = 0; i < numlegs; i++){
                k = i + BASE_NUMBER_OF_ROWS;
                if (k >= table->rowCount()){
                    table->setRowCount(table->rowCount() + 1);
                    table->setItem(k, 0, new QTableWidgetItem("Leg "+QString::number(i)));
                    table->setItem(k, 1, new QTableWidgetItem("hkbFootIkDriverInfoLeg"));
                    table->setItem(k, 2, new QTableWidgetItem("Edit"));
                }else{
                    table->setRowHidden(k, false);
                    table->item(k, 0)->setText("Leg "+QString::number(i));
                    table->item(k, 1)->setText("hkbFootIkDriverInfoLeg");
                }
            }
            for (auto j = numlegs + BASE_NUMBER_OF_ROWS; j < table->rowCount(); j++){
                table->setRowHidden(j, true);
            }
        }
    }else{
        LogFile::writeToLog("FootIkDriverInfoUI: The data is nullptr!!");
    }
}

void FootIkDriverInfoUI::setRaycastDistanceUp(){
    (bsData) ? bsData->setRaycastDistanceUp(raycastDistanceUp->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setRaycastDistanceUp(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setRaycastDistanceDown(){
    (bsData) ? bsData->setRaycastDistanceDown(raycastDistanceDown->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setRaycastDistanceDown(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setOriginalGroundHeightMS(){
    (bsData) ? bsData->setOriginalGroundHeightMS(originalGroundHeightMS->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setOriginalGroundHeightMS(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setVerticalOffset(){
    (bsData) ? bsData->setVerticalOffset(verticalOffset->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setVerticalOffset(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setCollisionFilterInfo(){
    (bsData) ? bsData->setCollisionFilterInfo(collisionFilterInfo->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setCollisionFilterInfo(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setForwardAlignFraction(){
    (bsData) ? bsData->setForwardAlignFraction(forwardAlignFraction->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setForwardAlignFraction(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setSidewaysAlignFraction(){
    (bsData) ? bsData->setSidewaysAlignFraction(sidewaysAlignFraction->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setSidewaysAlignFraction(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setSidewaysSampleWidth(){
    (bsData) ? bsData->setSidewaysSampleWidth(sidewaysSampleWidth->value()) : LogFile::writeToLog("FootIkDriverInfoUI::setSidewaysSampleWidth(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setLockFeetWhenPlanted(){
    (bsData) ? bsData->setLockFeetWhenPlanted(lockFeetWhenPlanted->isChecked()) : LogFile::writeToLog("FootIkDriverInfoUI::setLockFeetWhenPlanted(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setUseCharacterUpVector(){
    (bsData) ? bsData->setUseCharacterUpVector(useCharacterUpVector->isChecked()) : LogFile::writeToLog("FootIkDriverInfoUI::setUseCharacterUpVector(): The data is nullptr!!");
}

void FootIkDriverInfoUI::setIsQuadrupedNarrow(){
    (bsData) ? bsData->setIsQuadrupedNarrow(isQuadrupedNarrow->isChecked()) : LogFile::writeToLog("FootIkDriverInfoUI::setIsQuadrupedNarrow(): The data is nullptr!!");
}

void FootIkDriverInfoUI::addLeg(){
    if (bsData){
        bsData->addLeg();
        auto result = BASE_NUMBER_OF_ROWS + bsData->legs.size();
        auto numlegs = bsData->getNumberOfLegs();
        if (result-- >= table->rowCount()){
            table->setRowCount(table->rowCount() + 1);
            table->setItem(result, 0, new QTableWidgetItem("Leg "+QString::number(numlegs - 1)));
            table->setItem(result, 1, new QTableWidgetItem("hkbFootIkDriverInfoLeg"));
            table->setItem(result, 2, new QTableWidgetItem("Edit"));
        }else{
            table->setRowHidden(--result, false);
            table->item(result, 0)->setText("Leg "+QString::number(numlegs - 1));
            table->item(result, 1)->setText("hkbFootIkDriverInfoLeg");
        }
    }else{
        LogFile::writeToLog("FootIkDriverInfoUI: The data is nullptr!!");
    }
}

void FootIkDriverInfoUI::removeSelectedLeg(){
    int result = table->currentRow() - BASE_NUMBER_OF_ROWS;
    if (bsData){
        if (result < bsData->getNumberOfLegs()){
            bsData->removeLegAt(result);
            delete table->takeItem(table->currentRow(), 0);
            delete table->takeItem(table->currentRow(), 1);
            delete table->takeItem(table->currentRow(), 2);
            table->removeRow(table->currentRow());
        }
    }else{
        LogFile::writeToLog("FootIkDriverInfoUI: The data is nullptr!!");
    }
}

void FootIkDriverInfoUI::viewSelectedLeg(int row, int column){
    auto result = row - BASE_NUMBER_OF_ROWS;
    if (bsData){
        if (column == 2 && bsData->getNumberOfLegs() > result){
            legUI->loadData((hkbFootIkDriverInfoLeg *)bsData->getLegAt(result));
            setCurrentIndex(FOOT_IK_DRIVER_INFO_LEG);
        }
    }else{
        LogFile::writeToLog("FootIkDriverInfoUI: The data is nullptr!!");
    }
}

void FootIkDriverInfoUI::returnToWidget(){
    setCurrentIndex(FOOT_IK_DRIVER_INFO);
}

void FootIkDriverInfoUI::loadBoneList(QStringList &bones){
    legUI->loadBoneList(bones);
}
