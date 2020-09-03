#ifndef HANDIKDRIVERINFOHANDUI_H
#define HANDIKDRIVERINFOHANDUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"

class QuadVariableWidget;
class TableWidget;
class QVBoxLayout;
class DoubleSpinBox;
class CheckBox;
class QStackedLayout;
class ComboBox;
class QPushButton;
class LineEdit;

namespace UI {

class HkxObject;
class hkbHandIkDriverInfoHand;

class HandIkDriverInfoHandUI final: public QGroupBox
{
    Q_OBJECT
public:
    HandIkDriverInfoHandUI();
    HandIkDriverInfoHandUI& operator=(const HandIkDriverInfoHandUI&) = delete;
    HandIkDriverInfoHandUI(const HandIkDriverInfoHandUI &) = delete;
    ~HandIkDriverInfoHandUI() = default;
public:
    void loadData(hkbHandIkDriverInfoHand *data, hkbHandIkDriverInfo *par);
    void loadBoneList(QStringList &bones);
signals:
    void returnToParent();
private slots:
    void setElbowAxisLS();
    void setBackHandNormalLS();
    void setHandOffsetLS();
    void setHandOrienationOffsetLS();
    void setMaxElbowAngleDegrees();
    void setMinElbowAngleDegrees();
    void setShoulderIndex(int index);
    void setShoulderSiblingIndex(int index);
    void setElbowIndex(int index);
    void setElbowSiblingIndex(int index);
    void setWristIndex(int index);
    void setEnforceEndPosition();
    void setEnforceEndRotation();
    void setLocalFrameName(int index);
private:
    static const QStringList headerLabels1;
    hkbHandIkDriverInfo::hkbHandIkDriverInfoHand *bsData;
    hkbHandIkDriverInfo *parent;
    QVBoxLayout *lyt;
    QStackedLayout *stackLyt;
    QPushButton *returnPB;
    TableWidget *table;
    QuadVariableWidget *elbowAxisLS;
    QuadVariableWidget *backHandNormalLS;
    QuadVariableWidget *handOffsetLS;
    QuadVariableWidget *handOrienationOffsetLS;
    DoubleSpinBox *maxElbowAngleDegrees;
    DoubleSpinBox *minElbowAngleDegrees;
    ComboBox *shoulderIndex;
    ComboBox *shoulderSiblingIndex;
    ComboBox *elbowIndex;
    ComboBox *elbowSiblingIndex;
    ComboBox *wristIndex;
    CheckBox *enforceEndPosition;
    CheckBox *enforceEndRotation;
    ComboBox *localFrameName;
};
}
#endif // HANDIKDRIVERINFOHANDUI_H
