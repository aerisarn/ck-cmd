#ifndef BSBONEUI_H
#define BSBONEUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/modifiers/bslookatmodifier.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class DoubleSpinBox;
class CheckBox;
class TableWidget;
class QuadVariableWidget;
class ComboBox;


class BehaviorFile;

namespace UI {

class hkbVariableBindingSet;

class BSBoneUI final: public QGroupBox
{
    Q_OBJECT
    friend class BSLookAtModifierUI;
public:
    BSBoneUI();
    BSBoneUI& operator=(const BSBoneUI&) = delete;
    BSBoneUI(const BSBoneUI &) = delete;
    ~BSBoneUI() = default;
public:
    void loadData(BehaviorFile *parentFile, UI::BSLookAtModifier::BsBone *bon, BSLookAtModifier *par, int ind, bool isEyeBone);
    void variableRenamed(const QString & name, int index);
signals:
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent();
private slots:
    void setIndex(int index);
    void setFwdAxisLS();
    void setLimitAngleDegrees();
    void setOnGain();
    void setOffGain();
    void setEnabled();
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorFile *file;
    UI::BSLookAtModifier::BsBone *bsData;
    UI::BSLookAtModifier *parent;
    int bsBoneIndex;
    QString parameterName;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    ComboBox *index;
    QuadVariableWidget *fwdAxisLS;
    DoubleSpinBox *limitAngleDegrees;
    DoubleSpinBox *onGain;
    DoubleSpinBox *offGain;
    CheckBox *enabled;
};
}
#endif // BSBONEUI_H
