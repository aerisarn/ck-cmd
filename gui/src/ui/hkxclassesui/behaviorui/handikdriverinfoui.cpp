#include "handikdriverinfoui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/handikdriverinfohandui.h"
#include "src/filetypes/behaviorfile.h"

#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QSpinBox>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 1

const QStringList HandIkDriverInfoUI::headerLabels1 = {
    "Name",
    "Type",
    "Value"
};

HandIkDriverInfoUI::HandIkDriverInfoUI()
    : bsData(nullptr),
      handDriverGB(new QGroupBox("hkbHandIkDriverInfo")),
      handUI(new HandIkDriverInfoHandUI),
      footDriverLyt(new QGridLayout),
      addHandPB(new QPushButton("Add hand")),
      removeHandPB(new QPushButton("Remove selected hand")),
      table(new TableWidget),
      fadeInOutCurve(new ComboBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels1);
    table->setItem(0, 0, new QTableWidgetItem("fadeInOutCurve"));
    table->setItem(0, 1, new QTableWidgetItem("BlendCurve"));
    table->setCellWidget(0, 2, fadeInOutCurve);
    footDriverLyt->addWidget(addHandPB, 0, 0, 1, 1);
    footDriverLyt->addWidget(removeHandPB, 0, 2, 1, 1);
    footDriverLyt->addWidget(table, 1, 0, 8, 3);
    handDriverGB->setLayout(footDriverLyt);
    addWidget(handDriverGB);
    addWidget(handUI);
    connect(fadeInOutCurve, SIGNAL(currentIndexChanged(int)), this, SLOT(setFadeInOutCurve(int)), Qt::UniqueConnection);
    connect(addHandPB, SIGNAL(released()), this, SLOT(addHand()), Qt::UniqueConnection);
    connect(removeHandPB, SIGNAL(released()), this, SLOT(removeSelectedHand()), Qt::UniqueConnection);
    connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewSelectedHand(int,int)), Qt::UniqueConnection);
    connect(handUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
}

void HandIkDriverInfoUI::loadData(HkxObject *data){
    if (data){
        if (data->getSignature() == HKB_HAND_IK_DRIVER_INFO){
            bsData = static_cast<hkbHandIkDriverInfo *>(data);
            (fadeInOutCurve->count() <= 0) ? fadeInOutCurve->addItems(bsData->BlendCurve) : NULL;
            fadeInOutCurve->setCurrentIndex(bsData->BlendCurve.indexOf(bsData->getFadeInOutCurve()));
            for (auto i = 0, k = 0; i < bsData->hands.size(); i++){
                k = i + BASE_NUMBER_OF_ROWS;
                if (k >= table->rowCount()){
                    table->setRowCount(table->rowCount() + 1);
                    table->setItem(k, 0, new QTableWidgetItem("Hand "+QString::number(i)));
                    table->setItem(k, 1, new QTableWidgetItem("hkbHandIkDriverInfoHand"));
                    table->setItem(k, 2, new QTableWidgetItem("Edit"));
                }else{
                    table->setRowHidden(k, false);
                    table->item(k, 0)->setText("Hand "+QString::number(i));
                    table->item(k, 1)->setText("hkbHandIkDriverInfoHand");
                }
            }
            for (auto j = bsData->getNumberOfHands() + BASE_NUMBER_OF_ROWS; j < table->rowCount(); j++){
                table->setRowHidden(j, true);
            }
        }else{
            LogFile::writeToLog("HandIkDriverInfoUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("HandIkDriverInfoUI::loadData(): The data is nullptr!!");
    }
}

void HandIkDriverInfoUI::setFadeInOutCurve(int index){
    (bsData) ? bsData->fadeInOutCurve = fadeInOutCurve->itemText(index), bsData->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoUI: fadeInOutCurve was not set!!");
}

void HandIkDriverInfoUI::addHand(){
    if (bsData){
        bsData->addHand();
        auto result = BASE_NUMBER_OF_ROWS + bsData->getNumberOfHands();
        if (result >= table->rowCount()){
            table->setRowCount(table->rowCount() + 1);
            table->setItem(--result, 0, new QTableWidgetItem("Hand "+QString::number(bsData->getNumberOfHands() - 1)));
            table->setItem(result, 1, new QTableWidgetItem("hkbHandIkDriverInfoHand"));
            table->setItem(result, 2, new QTableWidgetItem("Edit"));
        }else{
            table->setRowHidden(--result, false);
            table->item(result, 0)->setText("Hand "+QString::number(bsData->getNumberOfHands() - 1));
            table->item(result, 1)->setText("hkbHandIkDriverInfoHand");
        }
    }else{
        LogFile::writeToLog("HandIkDriverInfoUI::loadData(): The data is nullptr!!");
    }
}

void HandIkDriverInfoUI::removeSelectedHand(){
    if (bsData){
        auto result = table->currentRow() - BASE_NUMBER_OF_ROWS;
        if (result < bsData->hands.size() && result >= 0){
            bsData->removeHandAt(result);
            delete table->takeItem(table->currentRow(), 0);
            delete table->takeItem(table->currentRow(), 1);
            delete table->takeItem(table->currentRow(), 2);
            table->removeRow(table->currentRow());
        }
    }else{
        LogFile::writeToLog("HandIkDriverInfoUI::loadData(): The data is nullptr!!");
    }
}

void HandIkDriverInfoUI::viewSelectedHand(int row, int column){
    if (bsData){
        auto result = row - BASE_NUMBER_OF_ROWS;
        if (bsData->getNumberOfHands() > result && result >= 0 && column == 2){
            handUI->loadData((hkbHandIkDriverInfoHand *)bsData->getHandAt(result), bsData);
            setCurrentIndex(HAND_IK_DRIVER_INFO_HAND);
        }
    }else{
        LogFile::writeToLog("HandIkDriverInfoUI::loadData(): The data is nullptr!!");
    }
}

void HandIkDriverInfoUI::returnToWidget(){
    setCurrentIndex(HAND_IK_DRIVER_INFO);
}

void HandIkDriverInfoUI::loadBoneList(QStringList &bones){
    handUI->loadBoneList(bones);
}
