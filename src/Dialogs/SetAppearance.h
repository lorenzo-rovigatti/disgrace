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
	void begin_change_appearance();

signals:
	void change_appearance_ready();

private:
	Ui::SetAppearance *_ui;
	QDataWidgetMapper *_mapper;
};

} /* namespace dg */

#endif // SETAPPEARANCE_H
