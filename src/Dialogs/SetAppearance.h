#ifndef SETAPPEARANCE_H
#define SETAPPEARANCE_H

#include <QDialog>
#include "../qcustomplot/qcustomplot.h"

namespace Ui {
class SetAppearance;
}

namespace dg {

class SetAppearance: public QDialog {
Q_OBJECT

public:
	explicit SetAppearance(QWidget *parent = 0);
	~SetAppearance();

	void set_graphs(QList<QCPGraph *> &);

public slots:
	void begin_change_appearance();

signals:
	void change_appearance_ready();

private:
	Ui::SetAppearance *_ui;
	QList<QCPGraph *> _graphs;
};

} /* namespace dg */

#endif // SETAPPEARANCE_H
