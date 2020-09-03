#ifndef SKYRIMANIMATIONDATAUI_H
#define SKYRIMANIMATIONDATAUI_H

#include <QStackedWidget>
#include <QVBoxLayout>

#include "src/utility.h"
#include "src/animData/skyrimanimdata.h"

class QGridLayout;
class TableWidget;
class SpinBox;
class LineEdit;
class ComboBox;
class QPushButton;
class DoubleSpinBox;
class CheckBox;
class QGroupBox;


namespace UI {

	class AnimationRotationUI;
	class AnimationTranslationUI;

	class SkyrimAnimationDataUI final : public QStackedWidget
	{
		Q_OBJECT
	public:
		SkyrimAnimationDataUI();
		SkyrimAnimationDataUI& operator=(const SkyrimAnimationDataUI&) = delete;
		SkyrimAnimationDataUI(const SkyrimAnimationDataUI &) = delete;
		~SkyrimAnimationDataUI() = default;
	public:
		void loadData(SkyrimAnimationMotionData * data);
	signals:
		void returnToParent();
	private slots:
		void setDuration();
		void viewSelectedChild(int row, int column);
		void returnToWidget();
	private:
		void toggleSignals(bool toggleconnections);
		void addTranslation();
		void removeTranslation(int index);
		void addRotation();
		void removeRotation(int index);
		void loadDynamicTableRows();
	private:
		enum ACTIVE_WIDGET {
			MAIN_WIDGET,
			TRANSLATION_WIDGET,
			ROTATION_WIDGET
		};
	private:
		static const QStringList headerLabels;
		int rotationsButtonRow;
		SkyrimAnimationMotionData *bsData;
		QGroupBox *groupBox;
		AnimationTranslationUI *translationUI;
		AnimationRotationUI *rotationUI;
		QGridLayout *topLyt;
		TableWidget *table;
		QPushButton *returnPB;
		DoubleSpinBox *duration;
	};
}
#endif // SKYRIMANIMATIONDATAUI_H
