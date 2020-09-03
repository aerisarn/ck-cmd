#ifndef MANUALSELECTORGENERATORUI_H
#define MANUALSELECTORGENERATORUI_H

#include <QGroupBox>

#include "src/utility.h"


class BehaviorGraphView;
class GenericTableWidget;
class TableWidget;
class SpinBox;
class LineEdit;
class ComboBox;
class QGridLayout;

namespace UI {
	
class HkxObject;
class hkbManualSelectorGenerator;
class hkbVariableBindingSet;

class ManualSelectorGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    ManualSelectorGeneratorUI();
    ManualSelectorGeneratorUI& operator=(const ManualSelectorGeneratorUI&) = delete;
    ManualSelectorGeneratorUI(const ManualSelectorGeneratorUI &) = delete;
    ~ManualSelectorGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setSelectedGeneratorIndex();
    void setCurrentGeneratorIndex();
    void setBindingVariable(int index, const QString & name);
    void setGenerator(int index, const QString & name);
    void viewSelectedChild(int row, int column);
    void swapGeneratorIndices(int index1, int index2);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
    void removeGenerator(int index);
    void addGenerator();
    void loadDynamicTableRows();
private:
    enum Generator_Type {
        NONE,
        STATE_MACHINE,
        MANUAL_SELECTOR_GENERATOR,
        BLENDER_GENERATOR,
        I_STATE_TAGGING_GENERATOR,
        BONE_SWITCH_GENERATOR,
        CYCLIC_BLEND_TRANSITION_GENERATOR,
        SYNCHRONIZED_CLIP_GENERATOR,
        MODIFIER_GENERATOR,
        OFFSET_ANIMATION_GENERATOR,
        POSE_MATCHING_GENERATOR,
        CLIP_GENERATOR,
        BEHAVIOR_REFERENCE_GENERATOR,
        GAMEBYRO_SEQUENCE_GENERATOR
    };
private:
    static const QStringList types;
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbManualSelectorGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    SpinBox *selectedGeneratorIndex;
    SpinBox *currentGeneratorIndex;
    ComboBox *typeSelectorCB;
};
}
#endif // MANUALSELECTORGENERATORUI_H
