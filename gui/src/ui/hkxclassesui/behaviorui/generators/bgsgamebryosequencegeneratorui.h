#ifndef BGSGAMEBRYOSEQUENCEGENERATORUI_H
#define BGSGAMEBRYOSEQUENCEGENERATORUI_H

#include <QGroupBox>


class ComboBox;
class QGridLayout;
class TableWidget;
class LineEdit;
class DoubleSpinBox;

namespace UI {

class HkxObject;
class BGSGamebryoSequenceGenerator;

class BGSGamebryoSequenceGeneratorUI final: public QGroupBox
{
    Q_OBJECT
public:
    BGSGamebryoSequenceGeneratorUI();
    BGSGamebryoSequenceGeneratorUI& operator=(const BGSGamebryoSequenceGeneratorUI&) = delete;
    BGSGamebryoSequenceGeneratorUI(const BGSGamebryoSequenceGeneratorUI &) = delete;
    ~BGSGamebryoSequenceGeneratorUI() = default;
public:
    void loadData(HkxObject *data);
signals:
    void generatorNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setSequence(const QString &sequence);
    void setBlendModeFunction(int index);
    void setPercent();
private:
    void toggleSignals(bool toggleconnections);
private:
    static const QStringList headerLabels;
    BGSGamebryoSequenceGenerator *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    LineEdit *pSequence;
    ComboBox *eBlendModeFunction;
    DoubleSpinBox *fPercent;
};
}
#endif // BGSGAMEBRYOSEQUENCEGENERATORUI_H
