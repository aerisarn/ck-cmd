#ifndef GENERICDATAWIDGETS_H
#define GENERICDATAWIDGETS_H

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <limits>
#include <math.h>

#include <QSpinBox>
#include <QLineEdit>
#include <QGroupBox>

#include "src/utility.h"
#include "src/animData/skyrimanimdata.h"
#include "src/filetypes/behaviorfile.h"

#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QHeaderView>
#include <QSizePolicy>
#include <QLabel>
#include <QProgressDialog>
#include <QDropEvent>

#undef min
#undef max 

class BehaviorGraphView;
namespace UI {
	class hkbVariableBindingSet;
}

class CheckBox final: public QWidget
{
    Q_OBJECT
public:
    CheckBox& operator=(const CheckBox&) = delete;
    CheckBox(const CheckBox &) = delete;
    ~CheckBox() = default;
signals:
    void released();
    void clicked(bool b);
public:
    CheckBox(const QString & text = "", QWidget *parent = nullptr)
        : QWidget(parent),
          checkbox(new QCheckBox(text, parent))
    {
        auto lyt = new QHBoxLayout;
        lyt->addSpacing(1);
        lyt->addWidget(checkbox, Qt::AlignCenter);
        lyt->addSpacing(1);
        setLayout(lyt);
        connect(checkbox, SIGNAL(released()), this,  SIGNAL(released()), Qt::UniqueConnection);
        connect(checkbox, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)), Qt::UniqueConnection);
    }

    void setChecked(bool checked){
        checkbox->setChecked(checked);
    }

    bool isChecked() const{
        return checkbox->isChecked();
    }
private:
    QCheckBox *checkbox;
};

class ProgressDialog final: public QProgressDialog
{
public:
    ProgressDialog& operator=(const ProgressDialog&) = delete;
    ProgressDialog(const ProgressDialog &) = delete;
    ~ProgressDialog() = default;
public:
    ProgressDialog(const QString &labelText, const QString &cancelButtonText, int minimum, int maximum, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
        : QProgressDialog(labelText, cancelButtonText, minimum, maximum, parent, f)
    {
        setMinimumSize(600, 50);
        setMinimumDuration(0);
        setWindowModality(Qt::WindowModal);
    }

    void setProgress(const QString &labelText, int value){
        setLabelText(labelText);
        setValue(value);
    }
};

class Validator final: public QValidator
{
public:
    Validator() = default;
    Validator& operator=(const Validator&) = delete;
    Validator(const Validator &) = delete;
    ~Validator() = default;
public:
    QValidator::State validate(QString & input, int &) const Q_DECL_OVERRIDE{
        auto result = QValidator::Acceptable;
        (input == "") ? result = QValidator::Invalid : NULL;
        return result;
    }
};

class LineEdit final: public QLineEdit
{
    Q_OBJECT
public:
    LineEdit& operator=(const LineEdit&) = delete;
    LineEdit(const LineEdit &) = delete;
    ~LineEdit() = default;
public:
    LineEdit(const QString & text = "", QWidget * par = 0)
        : QLineEdit(text, par)
    {
        setValidator(new Validator());
    }
};

class CheckButtonCombo final: public QWidget
{
    Q_OBJECT
public:
    CheckButtonCombo& operator=(const CheckButtonCombo&) = delete;
    CheckButtonCombo(const CheckButtonCombo &) = delete;
    ~CheckButtonCombo() = default;
public:
    CheckButtonCombo(const QString & buttontip = "", const QString & boxtext = "Enable:", bool disablebutton = true, const QString & buttontext = "Edit", QWidget * par = 0)
        : QWidget(par),
          label(new QLabel(boxtext)),
          checkBox(new CheckBox),
          pushButton(new QPushButton(buttontext)),
          disableButton(disablebutton)
    {
        auto pal = palette();
        pal.setColor(QPalette::Base, Qt::lightGray);
        setPalette(pal);
        auto lyt = new QHBoxLayout;
        pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        pushButton->setDisabled(disablebutton);
        pushButton->setToolTip(buttontip);
        lyt->addWidget(label, 1);
        lyt->addWidget(checkBox, 1);
        lyt->addWidget(pushButton, 6);
        lyt->setContentsMargins(0,0,0,0);
        setLayout(lyt);
        connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(setChecked(bool)), Qt::UniqueConnection);
        connect(pushButton, SIGNAL(clicked(bool)), this, SIGNAL(pressed()), Qt::UniqueConnection);
    }

    void setText(const QString & buttontext){
        pushButton->setText(buttontext);
    }

    bool isChecked() const{
        return checkBox->isChecked();
    }
signals:
    void pressed();
    void enabled(bool enabled);
public slots:
    void setChecked(bool checked){
        if (disableButton){
            pushButton->setEnabled(checked);
        }
        disconnect(checkBox, SIGNAL(clicked(bool)), this, SLOT(setChecked(bool)));
        checkBox->setChecked(checked);
        connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(setChecked(bool)), Qt::UniqueConnection);
        emit enabled(checked);
    }
private:
    QLabel *label;
    CheckBox *checkBox;
    QPushButton *pushButton;
    bool disableButton;
};

class ConditionValidator final: public QValidator
{
public:
    ConditionValidator() = default;
    ConditionValidator& operator=(const ConditionValidator&) = delete;
    ConditionValidator(const ConditionValidator &) = delete;
    ~ConditionValidator() = default;
public:
    QValidator::State validate(QString & input, int &) const Q_DECL_OVERRIDE{
        auto result = QValidator::Acceptable;
        (input.contains(">") || input.contains("<")) ? result = QValidator::Invalid : NULL;
        return result;
    }

    void fixup(QString & input) const{
        input.replace(">", "gt;");
        input.replace("<", "lt;");
    }
};

class ConditionLineEdit final: public QLineEdit{
    Q_OBJECT
public:
    ConditionLineEdit& operator=(const ConditionLineEdit&) = delete;
    ConditionLineEdit(const ConditionLineEdit &) = delete;
    ~ConditionLineEdit() = default;
public:
    ConditionLineEdit(const QString & text = "", QWidget * par = 0)
        : QLineEdit(text, par)
    {
        setValidator(new ConditionValidator());
    }
};

class TableWidgetItem final: public QTableWidgetItem
{
public:
    TableWidgetItem& operator=(const TableWidgetItem&) = delete;
    TableWidgetItem(const TableWidgetItem &) = delete;
    ~TableWidgetItem() = default;
public:
    TableWidgetItem(const QString & text, int align = Qt::AlignLeft | Qt::AlignVCenter, const QColor & backgroundColor = QColor(Qt::white), const QBrush & textColor = QBrush(Qt::black), const QString & tip = "", bool checkable = false)
        : QTableWidgetItem(text)
    {
        (checkable) ? setCheckState(Qt::Unchecked) : NULL;
        setTextAlignment(align);
        setBackgroundColor(backgroundColor);
        setForeground(textColor);
        setToolTip(tip);
    }
};

class TableWidget final: public QTableWidget
{
    Q_OBJECT
public:
    TableWidget& operator=(const TableWidget&) = delete;
    TableWidget(const TableWidget &) = delete;
    ~TableWidget() = default;
signals:
    void itemDropped(int row1, int row2);
public:
    TableWidget(const QColor & background = QColor(Qt::white), QWidget *parent = 0)
        : QTableWidget(parent)
    {
        auto pal = palette();
        pal.setColor(QPalette::Base, background);
        setPalette(pal);
        setMouseTracking(true);
        //setStyleSheet("QTableWidget { background:cyan }");
        setStyleSheet("QHeaderView::section { background-color:grey }");
        //verticalHeader()->setVisible(false);
        setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        horizontalHeader()->setSectionsClickable(false);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE{
        auto item = itemAt(event->pos());
        auto oldRow = currentRow();
        auto newRow = row(item);
        if ((oldRow >= range.min && newRow >= range.min) && (oldRow <= range.max && newRow <= range.max)){
            if (swapRowItems(oldRow, newRow)){
                emit itemDropped(oldRow, newRow);
            }
        }else{
            emit itemDropped(-1, -1);
        }
    }

    bool swapRowItems(int row1, int row2){
        if (rowCount() > row1 && rowCount() > row2 && row1 != row2 && row1 >= 0 && row2 >= 0){
            for (auto i = 0; i < columnCount(); i++){
                auto item1 = takeItem(row1, i);
                auto item2 = takeItem(row2, i);
                setItem(row2, i, item1);
                setItem(row1, i, item2);
            }
            return true;
        }
        return false;
    }

    QSize sizeHint() const{
        return QSize(100, 400);
    }

#undef max

    void setRowSwapRange(int min, int max = std::numeric_limits<int>::max()){
        range.min = min;
        range.max = max;
    }

private:
    struct RowSwapRange{
        RowSwapRange(): min(0), max(std::numeric_limits<int>::max()){}
        int min;
        int max;
    };

    RowSwapRange range;
};

class SpinBox final: public QSpinBox
{
    Q_OBJECT
public:
    SpinBox& operator=(const SpinBox&) = delete;
    SpinBox(const SpinBox &) = delete;
    ~SpinBox() = default;
public:
    SpinBox(QWidget* parent = 0)
        : QSpinBox(parent)
    {
        setMaximum(std::numeric_limits<int>::max());
        setMinimum(std::numeric_limits<int>::min());
        setFocusPolicy(Qt::StrongFocus);
    }
protected:
    void wheelEvent(QWheelEvent *e){
        (hasFocus()) ? QSpinBox::wheelEvent(e) : NULL;
    }
};

class DoubleSpinBox final: public QDoubleSpinBox
{
    Q_OBJECT
public:
    DoubleSpinBox& operator=(const DoubleSpinBox&) = delete;
    DoubleSpinBox(const DoubleSpinBox &) = delete;
    ~DoubleSpinBox() = default;
public:
    DoubleSpinBox(QWidget* parent = 0, qreal value = 0, int precision = 6, qreal step = 1.0, qreal max = std::numeric_limits<double>::max(), qreal min = -std::numeric_limits<double>::max())
        : QDoubleSpinBox(parent)
    {
        setMaximum(max);
        setMinimum(min);
        setDecimals(precision);
        setSingleStep(step);
        setValue(value);
        setFocusPolicy(Qt::StrongFocus);
    }
protected:
    void wheelEvent(QWheelEvent *e){
        (hasFocus()) ? QDoubleSpinBox::wheelEvent(e) : NULL;
    }
};

class ComboBox final: public QComboBox
{
    Q_OBJECT
public:
    ComboBox& operator=(const ComboBox&) = delete;
    ComboBox(const ComboBox &) = delete;
    ~ComboBox() = default;
public:
    ComboBox(QWidget* parent = 0)
        : QComboBox(parent)
    {
        //setStyleSheet("QComboBox:hover { background-color:red }");
        setFocusPolicy(Qt::StrongFocus);
    }
protected:
    void wheelEvent(QWheelEvent *e){
        (hasFocus()) ? QComboBox::wheelEvent(e) : NULL;
    }
};

class QuadVariableWidget final: public QWidget
{
    Q_OBJECT
public:
    QuadVariableWidget& operator=(const QuadVariableWidget&) = delete;
    QuadVariableWidget(const QuadVariableWidget &) = delete;
    ~QuadVariableWidget() = default;
public:
    QuadVariableWidget()
        : lyt(new QHBoxLayout),
          spinBoxX(new DoubleSpinBox()),
          spinBoxY(new DoubleSpinBox),
          spinBoxZ(new DoubleSpinBox),
          spinBoxW(new DoubleSpinBox)
    {
        setStyleSheet("background-color: white;");
        spinBoxX->setPrefix("X: ");
        spinBoxY->setPrefix("Y: ");
        spinBoxZ->setPrefix("Z: ");
        spinBoxW->setPrefix("W: ");
        spinBoxX->setMaximum(std::numeric_limits<double>::max());
        spinBoxX->setMinimum(-std::numeric_limits<double>::max());
        spinBoxY->setMaximum(std::numeric_limits<double>::max());
        spinBoxY->setMinimum(-std::numeric_limits<double>::max());
        spinBoxZ->setMaximum(std::numeric_limits<double>::max());
        spinBoxZ->setMinimum(-std::numeric_limits<double>::max());
        spinBoxW->setMaximum(std::numeric_limits<double>::max());
        spinBoxW->setMinimum(-std::numeric_limits<double>::max());
        lyt->addWidget(spinBoxX);
        lyt->addWidget(spinBoxY);
        lyt->addWidget(spinBoxZ);
        lyt->addWidget(spinBoxW);
        setLayout(lyt);
        lyt->setMargin(0);
        spinBoxX->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spinBoxY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spinBoxZ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spinBoxW->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(spinBoxX, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()), Qt::UniqueConnection);
        connect(spinBoxY, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()), Qt::UniqueConnection);
        connect(spinBoxZ, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()), Qt::UniqueConnection);
        connect(spinBoxW, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()), Qt::UniqueConnection);
    }

    void setVector4(){
        spinBoxW->setVisible(true);
    }

    void setVector3(){
        spinBoxW->setVisible(false);
    }

	UI::hkQuadVariable value() const{
        return UI::hkQuadVariable(spinBoxX->value(), spinBoxY->value(), spinBoxZ->value(), spinBoxW->value());
    }

	UI::hkVector3 valueAsVector3() const{
        return UI::hkVector3(spinBoxX->value(), spinBoxY->value(), spinBoxZ->value());
    }

    void setValue(const UI::hkQuadVariable & value){
        spinBoxX->setValue(value.x);
        spinBoxY->setValue(value.y);
        spinBoxZ->setValue(value.z);
        spinBoxW->setValue(value.w);
    }

    void setValue(const UI::hkVector3 & value){
        spinBoxX->setValue(value.x);
        spinBoxY->setValue(value.y);
        spinBoxZ->setValue(value.z);
    }
signals:
    void editingFinished();
private:
    QHBoxLayout *lyt;
    DoubleSpinBox *spinBoxX;
    DoubleSpinBox *spinBoxY;
    DoubleSpinBox *spinBoxZ;
    DoubleSpinBox *spinBoxW;
};

namespace UI {
	class HkDataUI;
}

class GenericTableWidget final: public QWidget
{
    Q_OBJECT
    friend class UI::HkDataUI;
public:
    GenericTableWidget(const QString & title);
    GenericTableWidget& operator=(const GenericTableWidget&) = delete;
    GenericTableWidget(const GenericTableWidget &) = delete;
    ~GenericTableWidget() = default;
public:
    void loadTable(const QStringList & names, const QStringList & types, const QString & firstElement = "");
    void loadTable(const QStringList & names, const QString & type, const QString & firstElement = "");
    void addItem(const QString & name, const QString & type);
    void renameItem(int index, const QString & newname);
    void removeItem(int index);
    int getNumRows() const;
signals:
    void elementSelected(int index, const QString & name);
    void elementAdded(int index, const QString & type);
private slots:
    void itemSelected();
    void itemSelectedAt(int row, int );
    void showTable(int index, const QString &typeallowed = "", const QStringList &typesdisallowed = QStringList());
    void showTable(const QString & name, const QString &typeallowed = "", const QStringList &typesdisallowed = QStringList());
    void filterItems();
    void resetFilter();
    void setTypeFilter(const QString & typeallowed, const QStringList &typesdisallowed = QStringList());
private:
    void setitem(int row, const QString & column0, const QString & column1);
private:
    QString onlyTypeAllowed;
    QStringList typesDisallowed;
    QGridLayout *lyt;
    QTableWidget *table;
    QPushButton *resetFilterPB;
    QPushButton *filterPB;
    QLineEdit *filterLE;
    QPushButton *selectPB;
    QPushButton *cancelPB;
    int lastSelectedRow;
};

namespace UIHelper{

void setRowItems(int row, const QString & name, const QString & classname, const QString & bind, const QString & value, const QString & tip1, const QString & tip2, TableWidget *table);
void setBinding(int index, int row, int column, const QString & variableName, const QString & path, UI::hkVariableType type, bool isProperty, TableWidget *table, UI::HkDynamicObject *bsData);
void loadBinding(int row, int column, UI::hkbVariableBindingSet *varBind, const QString &path, TableWidget *table, UI::HkxObject *bsData);
void setGenerator(int index, const QString &name, DataIconManager *dynobj, UI::hkbGenerator *child, UI::HkxSignature sig, UI::HkxObject::HkxType type, TableWidget *table, BehaviorGraphView *behaviorView, int row, int column);
void setModifier(int index, const QString &name, DataIconManager *dynobj, UI::hkbModifier *child, UI::HkxSignature sig, UI::HkxObject::HkxType type, TableWidget *table, BehaviorGraphView *behaviorView, int row, int column);

}

#endif // GENERICDATAWIDGETS_H
