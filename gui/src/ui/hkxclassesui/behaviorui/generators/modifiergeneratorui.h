#ifndef MODIFIERGENERATORUI_H
#define MODIFIERGENERATORUI_H

#include <QGroupBox>


class BehaviorGraphView;
class QGridLayout;
class TableWidget;
class LineEdit;
class GenericTableWidget;

namespace UI {
	
class HkxObject;
class hkbModifierGenerator;

class ModifierGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    ModifierGeneratorUI();
    ModifierGeneratorUI& operator=(const ModifierGeneratorUI&) = delete;
    ModifierGeneratorUI(const ModifierGeneratorUI &) = delete;
    ~ModifierGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
    void generatorRenamed(const QString & name, int index);
    void modifierRenamed(const QString & name, int index);
    void connectToTables(GenericTableWidget *modifiers, GenericTableWidget *generators);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewModifiers(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setModifier(int index, const QString & name);
    void setGenerator(int index, const QString & name);
    void viewSelected(int row, int column);
private:
    void toggleSignals(bool toggleconnections);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbModifierGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
};
}
#endif // MODIFIERGENERATORUI_H
