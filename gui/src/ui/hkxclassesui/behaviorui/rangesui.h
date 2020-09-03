#ifndef RANGESUI_H
#define RANGESUI_H

#include <QGroupBox>

#include "src/hkxclasses/behavior/modifiers/hkbsensehandlemodifier.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;
class BehaviorFile;
class DoubleSpinBox;
class CheckBox;
class TableWidget;

namespace UI {
	
class hkbVariableBindingSet;

class RangesUI final: public QGroupBox
{
    Q_OBJECT
    friend class SenseHandleModifierUI;
public:
    RangesUI();
    RangesUI& operator=(const RangesUI&) = delete;
    RangesUI(const RangesUI &) = delete;
    ~RangesUI() = default;
public:
    void loadData(BehaviorFile *parentFile, hkbSenseHandleModifier::hkRanges *ranges, hkbSenseHandleModifier *par, int index);
signals:
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent();
private slots:
    void setEventId(int index, const QString &name);
    void setEventPayload();
    void setMinDistance();
    void setMaxDistance();
    void setIgnoreHandle();
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
    hkbSenseHandleModifier::hkRanges *bsData;
    hkbSenseHandleModifier *parent;
    int rangeIndex;
    QGridLayout *topLyt;
    QPushButton *returnPB;
    TableWidget *table;
    QLineEdit *payload;
    DoubleSpinBox *minDistance;
    DoubleSpinBox *maxDistance;
    CheckBox *ignoreHandle;
};
}
#endif // RANGESUI_H
