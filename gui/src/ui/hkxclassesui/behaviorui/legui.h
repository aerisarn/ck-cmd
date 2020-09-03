#ifndef LEGUI_H
#define LEGUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/modifiers/hkbfootikcontrolsmodifier.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class BehaviorFile;
class DoubleSpinBox;
class CheckBox;
class TableWidget;
class QuadVariableWidget;

namespace UI {

class hkbVariableBindingSet;

class LegUI final: public QGroupBox
{
    Q_OBJECT
    friend class FootIkControlsModifierUI;
public:
    LegUI();
    LegUI& operator=(const LegUI&) = delete;
    LegUI(const LegUI &) = delete;
    ~LegUI() = default;
public:
    void loadData(BehaviorFile *parentFile, hkbFootIkControlsModifier::hkLeg *leg, hkbFootIkControlsModifier *par, int index);
signals:
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent();
private slots:
    void setEventId(int index, const QString &name);
    void setEventPayload();
    void setGroundPosition();
    void setVerticalError();
    void setHitSomething();
    void setIsPlantedMS();
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
    void eventRenamed(const QString & name, int index);
    void variableRenamed(const QString & name, int index);
private:
    static const QStringList headerLabels;
    BehaviorFile *file;
    hkbFootIkControlsModifier::hkLeg *bsData;
    hkbFootIkControlsModifier *parent;
    int legIndex;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    QuadVariableWidget *groundPosition;
    QLineEdit *payload;
    DoubleSpinBox *verticalError;
    CheckBox *hitSomething;
    CheckBox *isPlantedMS;
};
}
#endif // LEGUI_H
