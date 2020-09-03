#ifndef BEHAVIORREFERENCEGENERATORUI_H
#define BEHAVIORREFERENCEGENERATORUI_H

#include <QGroupBox>

class ComboBox;
class QGridLayout;
class TableWidget;
class LineEdit;

namespace UI {

class HkxObject;
class hkbBehaviorReferenceGenerator;

class BehaviorReferenceGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    BehaviorReferenceGeneratorUI();
    BehaviorReferenceGeneratorUI& operator=(const BehaviorReferenceGeneratorUI&) = delete;
    BehaviorReferenceGeneratorUI(const BehaviorReferenceGeneratorUI &) = delete;
    ~BehaviorReferenceGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
signals:
    void generatorNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setBehaviorName(const QString &text);
private:
    void toggleSignals(bool toggleconnections);
    void behaviorRenamed(const QString & name, int index);
private:
    static const QStringList headerLabels;
    hkbBehaviorReferenceGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    ComboBox *behaviorName;
};
}
#endif // BEHAVIORREFERENCEGENERATORUI_H
