#ifndef BLENDINGTRANSITIONEFFECTUI_H
#define BLENDINGTRANSITIONEFFECTUI_H

#include <QGroupBox>

#include "src/utility.h"

class QGroupBox;
class TableWidget;
class LineEdit;
class ComboBox;
class QGridLayout;

class SpinBox;
class DoubleSpinBox;
class CheckBox;
class QPushButton;


namespace UI {

class HkxObject;
class hkbVariableBindingSet;
class hkbBlendingTransitionEffect;

class BlendingTransitionEffectUI final: public QGroupBox
{
    Q_OBJECT
    friend class TransitionsUI;
public:
    BlendingTransitionEffectUI();
    BlendingTransitionEffectUI& operator=(const BlendingTransitionEffectUI&) = delete;
    BlendingTransitionEffectUI(const BlendingTransitionEffectUI &) = delete;
    ~BlendingTransitionEffectUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void transitionEffectRenamed(const QString & name);
    void returnToParent();
private slots:
    void setName(const QString &newname);
    void setBindingVariable(int index, const QString & name);
    void setSelfTransitionMode(int index);
    void setEventMode(int index);
    void setDuration();
    void setToGeneratorStartTimeFraction();
    void setEndMode(int index);
    void setBlendCurve(int index);
    void toggleSyncFlag();
    void toggleIgnoreFromWorldFromModelFlag();
    void toggleIgnoreToWorldFromModelFlag();
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
    void setFlag(CheckBox *flagcheckbox, QString &flags, const QString &flagtocheck);
private:
    static const QStringList headerLabels;
    hkbBlendingTransitionEffect *bsData;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    LineEdit *name;
    ComboBox *selfTransitionMode;
    ComboBox *eventMode;
    DoubleSpinBox *duration;
    DoubleSpinBox *toGeneratorStartTimeFraction;
    CheckBox *flagSync;
    CheckBox *flagIgnoreFromWorldFromModel;
    CheckBox *flagIgnoreToWorldFromModel;
    ComboBox *endMode;
    ComboBox *blendCurve;
};
}
#endif // BLENDINGTRANSITIONEFFECTUI_H
