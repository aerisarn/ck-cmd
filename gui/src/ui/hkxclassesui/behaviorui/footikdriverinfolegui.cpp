#include "footikdriverinfolegui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbfootikdriverinfo.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QSpinBox>

using namespace UI;

const QStringList FootIkDriverInfoLegUI::headerLabels1 = {
    "Name",
    "Type",
    "Value"
};

FootIkDriverInfoLegUI::FootIkDriverInfoLegUI()
    : bsData(nullptr),
      lyt(new QVBoxLayout),
      stackLyt(new QStackedLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      kneeAxisLS(new QuadVariableWidget),
      footEndLS(new QuadVariableWidget),
      footPlantedAnkleHeightMS(new DoubleSpinBox),
      footRaisedAnkleHeightMS(new DoubleSpinBox),
      maxAnkleHeightMS(new DoubleSpinBox),
      minAnkleHeightMS(new DoubleSpinBox),
      maxKneeAngleDegrees(new DoubleSpinBox),
      minKneeAngleDegrees(new DoubleSpinBox),
      maxAnkleAngleDegrees(new DoubleSpinBox),
      hipIndex(new ComboBox),
      kneeIndex(new ComboBox),
      ankleIndex(new ComboBox)
{
    setTitle("hkbFootIkDriverInfoLeg");
    table->setRowCount(13);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels1);
    table->setCellWidget(0, 1, returnPB);
    table->setItem(1, 0, new QTableWidgetItem("kneeAxisLS"));
    table->setItem(2, 1, new QTableWidgetItem("hkVector4"));
    table->setCellWidget(1, 2, kneeAxisLS);
    table->setItem(2, 0, new QTableWidgetItem("footEndLS"));
    table->setItem(2, 1, new QTableWidgetItem("hkVector4"));
    table->setCellWidget(2, 2, footEndLS);
    table->setItem(3, 0, new QTableWidgetItem("footPlantedAnkleHeightMS"));
    table->setItem(3, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(3, 2, footPlantedAnkleHeightMS);
    table->setItem(4, 0, new QTableWidgetItem("footRaisedAnkleHeightMS"));
    table->setItem(4, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(4, 2, footRaisedAnkleHeightMS);
    table->setItem(5, 0, new QTableWidgetItem("maxAnkleHeightMS"));
    table->setItem(5, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(5, 2, maxAnkleHeightMS);
    table->setItem(6, 0, new QTableWidgetItem("minAnkleHeightMS"));
    table->setItem(6, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(6, 2, minAnkleHeightMS);
    table->setItem(7, 0, new QTableWidgetItem("maxKneeAngleDegrees"));
    table->setItem(7, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(7, 2, maxKneeAngleDegrees);
    table->setItem(8, 0, new QTableWidgetItem("minKneeAngleDegrees"));
    table->setItem(8, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(8, 2, minKneeAngleDegrees);
    table->setItem(9, 0, new QTableWidgetItem("maxAnkleAngleDegrees"));
    table->setItem(9, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(9, 2, maxAnkleAngleDegrees);
    table->setItem(10, 0, new QTableWidgetItem("hipIndex"));
    table->setItem(10, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(10, 2, hipIndex);
    table->setItem(11, 0, new QTableWidgetItem("kneeIndex"));
    table->setItem(11, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(11, 2, kneeIndex);
    table->setItem(12, 0, new QTableWidgetItem("ankleIndex"));
    table->setItem(12, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(12, 2, ankleIndex);
    lyt->addWidget(table);
    setLayout(lyt);
    connect(kneeAxisLS, SIGNAL(editingFinished()), this, SLOT(setKneeAxisLS()), Qt::UniqueConnection);
    connect(footEndLS, SIGNAL(editingFinished()), this, SLOT(setFootEndLS()), Qt::UniqueConnection);
    connect(footPlantedAnkleHeightMS, SIGNAL(editingFinished()), this, SLOT(setFootPlantedAnkleHeightMS()), Qt::UniqueConnection);
    connect(footRaisedAnkleHeightMS, SIGNAL(editingFinished()), this, SLOT(setFootRaisedAnkleHeightMS()), Qt::UniqueConnection);
    connect(maxAnkleHeightMS, SIGNAL(editingFinished()), this, SLOT(setMaxAnkleHeightMS()), Qt::UniqueConnection);
    connect(minAnkleHeightMS, SIGNAL(editingFinished()), this, SLOT(setMinAnkleHeightMS()), Qt::UniqueConnection);
    connect(maxKneeAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setMaxKneeAngleDegrees()), Qt::UniqueConnection);
    connect(minKneeAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setMinKneeAngleDegrees()), Qt::UniqueConnection);
    connect(maxAnkleAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setMaxAnkleAngleDegrees()), Qt::UniqueConnection);
    connect(hipIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setHipIndex(int)), Qt::UniqueConnection);
    connect(kneeIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setKneeIndex(int)), Qt::UniqueConnection);
    connect(ankleIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnkleIndex(int)), Qt::UniqueConnection);
    connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
}

void FootIkDriverInfoLegUI::loadData(hkbFootIkDriverInfoLeg *data){
    if (data){
        bsData = (hkbFootIkDriverInfo::hkbFootIkDriverInfoLeg *)data;
        kneeAxisLS->setValue(bsData->kneeAxisLS);
        footEndLS->setValue(bsData->footEndLS);
        footPlantedAnkleHeightMS->setValue(bsData->footPlantedAnkleHeightMS);
        footRaisedAnkleHeightMS->setValue(bsData->footRaisedAnkleHeightMS);
        maxAnkleHeightMS->setValue(bsData->maxAnkleHeightMS);
        minAnkleHeightMS->setValue(bsData->minAnkleHeightMS);
        maxKneeAngleDegrees->setValue(bsData->maxKneeAngleDegrees);
        minKneeAngleDegrees->setValue(bsData->minKneeAngleDegrees);
        maxAnkleAngleDegrees->setValue(bsData->maxAnkleAngleDegrees);
        hipIndex->setCurrentIndex(bsData->hipIndex + 1);
        kneeIndex->setCurrentIndex(bsData->kneeIndex + 1);
        ankleIndex->setCurrentIndex(bsData->ankleIndex + 1);
    }else{
        LogFile::writeToLog("FootIkDriverInfoLegUI: The data is nullptr!!");
    }
}

void FootIkDriverInfoLegUI::setKneeAxisLS(){
    (bsData && bsData->parent && bsData->kneeAxisLS != kneeAxisLS->value()) ? bsData->kneeAxisLS = kneeAxisLS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: kneeAxisLS was not set!!");
}

void FootIkDriverInfoLegUI::setFootEndLS(){
    (bsData && bsData->parent && bsData->footEndLS != footEndLS->value()) ? bsData->footEndLS = footEndLS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: footEndLS was not set!!");
}

void FootIkDriverInfoLegUI::setFootPlantedAnkleHeightMS(){
    (bsData && bsData->parent && bsData->footPlantedAnkleHeightMS != footPlantedAnkleHeightMS->value()) ? bsData->footPlantedAnkleHeightMS = footPlantedAnkleHeightMS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: footPlantedAnkleHeightMS was not set!!");
}

void FootIkDriverInfoLegUI::setFootRaisedAnkleHeightMS(){
    (bsData && bsData->parent && bsData->footRaisedAnkleHeightMS != footRaisedAnkleHeightMS->value()) ? bsData->footRaisedAnkleHeightMS = footRaisedAnkleHeightMS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: footRaisedAnkleHeightMS was not set!!");
}

void FootIkDriverInfoLegUI::setMaxAnkleHeightMS(){
    (bsData && bsData->parent && bsData->maxAnkleHeightMS != maxAnkleHeightMS->value()) ? bsData->maxAnkleHeightMS = maxAnkleHeightMS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: maxAnkleHeightMS was not set!!");
}

void FootIkDriverInfoLegUI::setMinAnkleHeightMS(){
    (bsData && bsData->parent && bsData->minAnkleHeightMS != minAnkleHeightMS->value()) ? bsData->minAnkleHeightMS = minAnkleHeightMS->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: minAnkleHeightMS was not set!!");
}

void FootIkDriverInfoLegUI::setMaxKneeAngleDegrees(){
    (bsData && bsData->parent && bsData->maxKneeAngleDegrees != maxKneeAngleDegrees->value()) ? bsData->maxKneeAngleDegrees = maxKneeAngleDegrees->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: maxKneeAngleDegrees was not set!!");
}

void FootIkDriverInfoLegUI::setMinKneeAngleDegrees(){
    (bsData && bsData->parent && bsData->minKneeAngleDegrees != minKneeAngleDegrees->value()) ? bsData->minKneeAngleDegrees = minKneeAngleDegrees->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: minKneeAngleDegrees was not set!!");
}

void FootIkDriverInfoLegUI::setMaxAnkleAngleDegrees(){
    (bsData && bsData->parent && bsData->maxAnkleAngleDegrees != maxAnkleAngleDegrees->value()) ? bsData->maxAnkleAngleDegrees = maxAnkleAngleDegrees->value(), bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: maxAnkleAngleDegrees was not set!!");
}

void FootIkDriverInfoLegUI::setHipIndex(int index){
    (bsData && bsData->parent) ? bsData->hipIndex = index - 1, bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: The data is nullptr!!");
}

void FootIkDriverInfoLegUI::setKneeIndex(int index){
    (bsData && bsData->parent) ? bsData->kneeIndex = index - 1, bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: The data is nullptr!!");
}

void FootIkDriverInfoLegUI::setAnkleIndex(int index){
    (bsData && bsData->parent) ? bsData->ankleIndex = index - 1, bsData->parent->setIsFileChanged(true) : LogFile::writeToLog("FootIkDriverInfoLegUI: The data is nullptr!!");
}

void FootIkDriverInfoLegUI::loadBoneList(QStringList & bones){
    bones.prepend("None");
    hipIndex->clear();
    hipIndex->addItems(bones);
    kneeIndex->clear();
    kneeIndex->addItems(bones);
    ankleIndex->clear();
    ankleIndex->addItems(bones);
}
