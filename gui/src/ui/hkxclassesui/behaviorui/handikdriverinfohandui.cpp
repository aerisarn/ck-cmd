#include "handikdriverinfohandui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "src/ui/genericdatawidgets.h"
#include <QStackedLayout>
#include <QHeaderView>
#include <QSpinBox>
#include <QLineEdit>

using namespace UI;

const QStringList HandIkDriverInfoHandUI::headerLabels1 = {
    "Name",
    "Type",
    "Value"
};

HandIkDriverInfoHandUI::HandIkDriverInfoHandUI()
    : bsData(nullptr),
      lyt(new QVBoxLayout),
      stackLyt(new QStackedLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      elbowAxisLS(new QuadVariableWidget),
      backHandNormalLS(new QuadVariableWidget),
      handOffsetLS(new QuadVariableWidget),
      handOrienationOffsetLS(new QuadVariableWidget),
      maxElbowAngleDegrees(new DoubleSpinBox),
      minElbowAngleDegrees(new DoubleSpinBox),
      shoulderIndex(new ComboBox),
      shoulderSiblingIndex(new ComboBox),
      elbowIndex(new ComboBox),
      elbowSiblingIndex(new ComboBox),
      wristIndex(new ComboBox),
      enforceEndPosition(new CheckBox),
      enforceEndRotation(new CheckBox),
      localFrameName(new ComboBox)
{
    setTitle("hkbHandIkDriverInfoHand");
    table->setRowCount(15);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels1);
    table->setCellWidget(0, 1, returnPB);
    table->setItem(1, 0, new QTableWidgetItem("elbowAxisLS"));
    table->setItem(1, 1, new QTableWidgetItem("hkVector4"));
    table->setCellWidget(1, 2, elbowAxisLS);
    table->setItem(2, 0, new QTableWidgetItem("backHandNormalLS"));
    table->setItem(2, 1, new QTableWidgetItem("hkVector4"));
    table->setCellWidget(2, 2, backHandNormalLS);
    table->setItem(3, 0, new QTableWidgetItem("handOffsetLS"));
    table->setItem(3, 1, new QTableWidgetItem("hkVector4"));
    table->setCellWidget(3, 2, handOffsetLS);
    table->setItem(4, 0, new QTableWidgetItem("handOrienationOffsetLS"));
    table->setItem(4, 1, new QTableWidgetItem("hkQuaternion"));
    table->setCellWidget(4, 2, handOrienationOffsetLS);
    table->setItem(5, 0, new QTableWidgetItem("maxElbowAngleDegrees"));
    table->setItem(5, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(5, 2, maxElbowAngleDegrees);
    table->setItem(6, 0, new QTableWidgetItem("minElbowAngleDegrees"));
    table->setItem(6, 1, new QTableWidgetItem("hkReal"));
    table->setCellWidget(6, 2, minElbowAngleDegrees);
    table->setItem(7, 0, new QTableWidgetItem("shoulderIndex"));
    table->setItem(7, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(7, 2, shoulderIndex);
    table->setItem(8, 0, new QTableWidgetItem("shoulderSiblingIndex"));
    table->setItem(8, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(8, 2, shoulderSiblingIndex);
    table->setItem(9, 0, new QTableWidgetItem("elbowIndex"));
    table->setItem(9, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(9, 2, elbowIndex);
    table->setItem(10, 0, new QTableWidgetItem("elbowSiblingIndex"));
    table->setItem(10, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(10, 2, elbowSiblingIndex);
    table->setItem(11, 0, new QTableWidgetItem("wristIndex"));
    table->setItem(11, 1, new QTableWidgetItem("hkInt32"));
    table->setCellWidget(11, 2, wristIndex);
    table->setItem(12, 0, new QTableWidgetItem("enforceEndPosition"));
    table->setItem(12, 1, new QTableWidgetItem("hkBool"));
    table->setCellWidget(12, 2, enforceEndPosition);
    table->setItem(13, 0, new QTableWidgetItem("enforceEndRotation"));
    table->setItem(13, 1, new QTableWidgetItem("hkBool"));
    table->setCellWidget(13, 2, enforceEndRotation);
    table->setItem(14, 0, new QTableWidgetItem("localFrameName"));
    table->setItem(14, 1, new QTableWidgetItem("hkStringPtr"));
    table->setCellWidget(14, 2, localFrameName);
    lyt->addWidget(table);
    setLayout(lyt);
    connect(elbowAxisLS, SIGNAL(editingFinished()), this, SLOT(setElbowAxisLS()), Qt::UniqueConnection);
    connect(backHandNormalLS, SIGNAL(editingFinished()), this, SLOT(setBackHandNormalLS()), Qt::UniqueConnection);
    connect(handOffsetLS, SIGNAL(editingFinished()), this, SLOT(setHandOffsetLS()), Qt::UniqueConnection);
    connect(handOrienationOffsetLS, SIGNAL(editingFinished()), this, SLOT(setHandOrienationOffsetLS()), Qt::UniqueConnection);
    connect(maxElbowAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setMaxElbowAngleDegrees()), Qt::UniqueConnection);
    connect(minElbowAngleDegrees, SIGNAL(editingFinished()), this, SLOT(setMinElbowAngleDegrees()), Qt::UniqueConnection);
    connect(shoulderIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setShoulderIndex(int)), Qt::UniqueConnection);
    connect(shoulderSiblingIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setShoulderSiblingIndex(int)), Qt::UniqueConnection);
    connect(elbowIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setElbowIndex(int)), Qt::UniqueConnection);
    connect(elbowSiblingIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setElbowSiblingIndex(int)), Qt::UniqueConnection);
    connect(wristIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(setWristIndex(int)), Qt::UniqueConnection);
    connect(enforceEndPosition, SIGNAL(released()), this, SLOT(setEnforceEndPosition()), Qt::UniqueConnection);
    connect(enforceEndRotation, SIGNAL(released()), this, SLOT(setEnforceEndRotation()), Qt::UniqueConnection);
    connect(localFrameName, SIGNAL(currentIndexChanged(int)), this, SLOT(setLocalFrameName(int)), Qt::UniqueConnection);
    connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
}

void HandIkDriverInfoHandUI::loadData(hkbHandIkDriverInfoHand *data, hkbHandIkDriverInfo *par){
    if (data && par){
        bsData = (hkbHandIkDriverInfo::hkbHandIkDriverInfoHand *)data;
        parent = par;
        elbowAxisLS->setValue(bsData->elbowAxisLS);
        backHandNormalLS->setValue(bsData->backHandNormalLS);
        handOffsetLS->setValue(bsData->handOffsetLS);
        handOrienationOffsetLS->setValue(bsData->handOrienationOffsetLS);
        maxElbowAngleDegrees->setValue(bsData->maxElbowAngleDegrees);
        minElbowAngleDegrees->setValue(bsData->minElbowAngleDegrees);
        shoulderIndex->setCurrentIndex(bsData->shoulderIndex + 1);
        shoulderSiblingIndex->setCurrentIndex(bsData->shoulderSiblingIndex + 1);
        elbowIndex->setCurrentIndex(bsData->elbowIndex + 1);
        elbowSiblingIndex->setCurrentIndex(bsData->elbowSiblingIndex + 1);
        wristIndex->setCurrentIndex(bsData->wristIndex + 1);
        enforceEndPosition->setChecked(bsData->enforceEndPosition);
        enforceEndRotation->setChecked(bsData->enforceEndRotation);
        if (!localFrameName->count()){
            auto localFrames = QStringList("None") + static_cast<BehaviorFile *>(parent->getParentFile())->getLocalFrameNames();
            localFrameName->insertItems(0, localFrames);
        }
        auto index = localFrameName->findText(bsData->localFrameName);
        if (index < 0 || index >= localFrameName->count()){
            LogFile::writeToLog("The local frame name was not loaded correctly!!!");
        }else{
            localFrameName->setCurrentIndex(index);
        }
    }else{
        LogFile::writeToLog("HandIkDriverInfoHandUI::loadData(): The data or parent object is nullptr!!");
    }
}

void HandIkDriverInfoHandUI::setElbowAxisLS(){
    (bsData && parent && bsData->elbowAxisLS != elbowAxisLS->value()) ? bsData->elbowAxisLS = elbowAxisLS->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: elbowAxisLS was not set!!");
}

void HandIkDriverInfoHandUI::setBackHandNormalLS(){
    (bsData && parent && bsData->backHandNormalLS != backHandNormalLS->value()) ? bsData->backHandNormalLS = backHandNormalLS->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: backHandNormalLS was not set!!");
}

void HandIkDriverInfoHandUI::setHandOffsetLS(){
    (bsData && parent && bsData->handOffsetLS != handOffsetLS->value()) ? bsData->handOffsetLS = handOffsetLS->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: handOffsetLS was not set!!");
}

void HandIkDriverInfoHandUI::setHandOrienationOffsetLS(){
    (bsData && parent && bsData->handOrienationOffsetLS != handOrienationOffsetLS->value()) ? bsData->handOrienationOffsetLS = handOrienationOffsetLS->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: handOrienationOffsetLS was not set!!");
}

void HandIkDriverInfoHandUI::setMaxElbowAngleDegrees(){
    (bsData && parent && bsData->maxElbowAngleDegrees != maxElbowAngleDegrees->value()) ? bsData->maxElbowAngleDegrees = maxElbowAngleDegrees->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: maxElbowAngleDegrees was not set!!");
}

void HandIkDriverInfoHandUI::setMinElbowAngleDegrees(){
    (bsData && parent && bsData->minElbowAngleDegrees != minElbowAngleDegrees->value()) ? bsData->minElbowAngleDegrees = minElbowAngleDegrees->value(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: minElbowAngleDegrees was not set!!");
}

void HandIkDriverInfoHandUI::setShoulderIndex(int index){
    (bsData && parent) ? bsData->shoulderIndex = index - 1, parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: shoulderIndex was not set!!");
}

void HandIkDriverInfoHandUI::setShoulderSiblingIndex(int index){
    (bsData && parent) ? bsData->shoulderSiblingIndex = index - 1, parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: shoulderSiblingIndex was not set!!");
}

void HandIkDriverInfoHandUI::setElbowIndex(int index){
    (bsData && parent) ? bsData->elbowIndex = index - 1, parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: elbowIndex was not set!!");
}

void HandIkDriverInfoHandUI::setElbowSiblingIndex(int index){
    (bsData && parent) ? bsData->elbowSiblingIndex = index - 1, parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: elbowSiblingIndex was not set!!");
}

void HandIkDriverInfoHandUI::setWristIndex(int index){
    (bsData && parent) ? bsData->wristIndex = index - 1, parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: wristIndex was not set!!");
}

void HandIkDriverInfoHandUI::setEnforceEndPosition(){
    (bsData && parent && bsData->enforceEndPosition != enforceEndPosition->isChecked()) ? bsData->enforceEndPosition = enforceEndPosition->isChecked(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: enforceEndPosition was not set!!");
}

void HandIkDriverInfoHandUI::setEnforceEndRotation(){
    (bsData && parent && bsData->enforceEndRotation != enforceEndRotation->isChecked()) ? bsData->enforceEndRotation = enforceEndRotation->isChecked(), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: enforceEndRotation was not set!!");
}

void HandIkDriverInfoHandUI::setLocalFrameName(int index){
    if (bsData && parent){
        auto localframes = static_cast<BehaviorFile *>(parent->getParentFile())->getLocalFrameNames();
        (index >= 0 && index < localframes.size() && localframes.at(index) != bsData->localFrameName) ? bsData->localFrameName = localframes.at(index), parent->setIsFileChanged(true) : LogFile::writeToLog("HandIkDriverInfoHandUI: 'localFrameName' was not set!");
    }else{
        LogFile::writeToLog("HandIkDriverInfoHandUI: 'localFrameName' was not set!");
    }
}

void HandIkDriverInfoHandUI::loadBoneList(QStringList & bones){
    bones.prepend("None");
    shoulderIndex->clear();
    shoulderIndex->addItems(bones);
    shoulderSiblingIndex->clear();
    shoulderSiblingIndex->addItems(bones);
    elbowIndex->clear();
    elbowIndex->addItems(bones);
    elbowSiblingIndex->clear();
    elbowSiblingIndex->addItems(bones);
    wristIndex->clear();
    wristIndex->addItems(bones);
}
