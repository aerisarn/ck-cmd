#ifndef KEYFRAMEINFOUI_H
#define KEYFRAMEINFOUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/modifiers/hkbkeyframebonesmodifier.h"

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

class KeyframeInfoUI final: public QGroupBox
{
    Q_OBJECT
    friend class KeyframeBonesModifierUI;
public:
    KeyframeInfoUI();
    KeyframeInfoUI& operator=(const KeyframeInfoUI&) = delete;
    KeyframeInfoUI(const KeyframeInfoUI &) = delete;
    ~KeyframeInfoUI() = default;
public:
    void loadData(BehaviorFile *parentFile, hkbKeyframeBonesModifier::hkKeyframeInfo *key, hkbKeyframeBonesModifier *par, int index);
signals:
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent();
private slots:
    void setKeyframedPosition();
    void setKeyframedRotation();
    void setBoneIndex(int index);
    void setIsValid();
    void viewSelectedChild(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
    void setBindingVariable(int index, const QString & name);
    void selectTableToView(bool viewproperties, const QString & path);
    void variableRenamed(const QString & name, int index);
private:
    static const QStringList headerLabels;
    BehaviorFile *file;
    hkbKeyframeBonesModifier::hkKeyframeInfo *bsData;
    hkbKeyframeBonesModifier *parent;
    int keyframeIndex;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    QuadVariableWidget *keyframedPosition;
    QuadVariableWidget *keyframedRotation;
    ComboBox *boneIndex;
    CheckBox *isValid;
};
}
#endif // KEYFRAMEINFOUI_H
