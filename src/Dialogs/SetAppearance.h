#ifndef SETAPPEARANCE_H
#define SETAPPEARANCE_H

#include <QDialog>
#include "../qcustomplot/qcustomplot.h"

namespace Ui {
class SetAppearance;
}

namespace dg {

class DataManager;

class SetAppearance: public QDialog {
Q_OBJECT

public:
	explicit SetAppearance(DataManager *dm, QWidget *parent = 0);
	~SetAppearance();

public slots:
	void show() Q_DECL_OVERRIDE;

private:
	void _setup_widgets();

	Ui::SetAppearance *_ui;
	DataManager *_dm;
	QDataWidgetMapper *_mapper;
};

} /* namespace dg */

#endif // SETAPPEARANCE_H
