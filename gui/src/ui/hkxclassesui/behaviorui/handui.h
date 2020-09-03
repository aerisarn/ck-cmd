#ifndef HANDUI_H
#define HANDUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/modifiers/hkbhandikcontrolsmodifier.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class BehaviorFile;
class DoubleSpinBox;
class CheckBox;
class TableWidget;
class QuadVariableWidget;
class ComboBox;

namespace UI {

class hkbVariableBindingSet;

class HandUI final: public QGroupBox
{
    Q_OBJECT
    friend class hkbHandIkControlsModifierUI;
    friend class HandIkControlsModifierUI;
public:
    HandUI();
    HandUI& operator=(const HandUI&) = delete;
    HandUI(const HandUI &) = delete;
    ~HandUI() = default;
public:
    void loadData(BehaviorFile *parentFile, hkbHandIkControlsModifier::hkHand *bon, hkbHandIkControlsModifier *par, int ind);
signals:
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent();
private slots:
    void setTargetPosition();
    void setTargetRotation();
    void setTargetNormal();
    void setTransformOnFraction();
    void setNormalOnFraction();
    void setFadeInDuration();
    void setFadeOutDuration();
    void setExtrapolationTimeStep();
    void setHandleChangeSpeed();
    void setHandleChangeMode(const QString & mode);
    void setFixUp();
    void setHandIndex(int index);
    void setEnable();
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
    void variableRenamed(const QString & name, int index);
private:
    static const QStringList headerLabels;
    BehaviorFile *file;
    hkbHandIkControlsModifier::hkHand *bsData;
    hkbHandIkControlsModifier *parent;
    int bsBoneIndex;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    QuadVariableWidget *targetPosition;
    QuadVariableWidget *targetRotation;
    QuadVariableWidget *targetNormal;
    DoubleSpinBox *transformOnFraction;
    DoubleSpinBox *normalOnFraction;
    DoubleSpinBox *fadeInDuration;
    DoubleSpinBox *fadeOutDuration;
    DoubleSpinBox *extrapolationTimeStep;
    DoubleSpinBox *handleChangeSpeed;
    ComboBox *handleChangeMode;
    CheckBox *fixUp;
    ComboBox *handIndex;
    CheckBox *enable;
};
}
#endif // HANDUI_H
