#ifndef BSSYNCHRONIZEDCLIPGENERATORUI_H
#define BSSYNCHRONIZEDCLIPGENERATORUI_H

#include <QGroupBox>

#include "src/utility.h"

class TableWidget;

class QGridLayout;
class DoubleSpinBox;
class LineEdit;
class BehaviorGraphView;
class GenericTableWidget;
class CheckBox;
class SpinBox;

namespace UI {

class HkxObject;
class BSSynchronizedClipGenerator;
class hkbVariableBindingSet;

class BSSynchronizedClipGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSSynchronizedClipGeneratorUI();
    BSSynchronizedClipGeneratorUI& operator=(const BSSynchronizedClipGeneratorUI&) = delete;
    BSSynchronizedClipGeneratorUI(const BSSynchronizedClipGeneratorUI &) = delete;
    ~BSSynchronizedClipGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setClipGenerator(int index, const QString & name);
    void setSyncAnimPrefix(const QString &newname);
    void setSyncClipIgnoreMarkPlacement();
    void setGetToMarkTime();
    void setMarkErrorThreshold();
    void setLeadCharacter();
    void setReorientSupportChar();
    void setApplyMotionFromRoot();
    void setAnimationBindingIndex();
    void viewSelectedChild(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    BSSynchronizedClipGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    LineEdit *syncAnimPrefix;
    CheckBox *bSyncClipIgnoreMarkPlacement;
    DoubleSpinBox *fGetToMarkTime;
    DoubleSpinBox *fMarkErrorThreshold;
    CheckBox *bLeadCharacter;
    CheckBox *bReorientSupportChar;
    CheckBox *bApplyMotionFromRoot;
    SpinBox *sAnimationBindingIndex;
};
}
#endif // BSSYNCHRONIZEDCLIPGENERATORUI_H
