#ifndef SETAPPEARANCE_H
#define SETAPPEARANCE_H

#include <QDialog>
#include "../qcustomplot/qcustomplot.h"

namespace Ui {
class SetAppearance;
}

namespace dg {

class AgrFile;

class SetAppearance: public QDialog {
Q_OBJECT

public:
	explicit SetAppearance(QWidget *parent = 0);
	~SetAppearance();

	void connect_to_file(AgrFile *agr_file);

public slots:
	void show() Q_DECL_OVERRIDE;
	void pick_colour();

private slots:
	void change_btn_colour_background(const QString &colour_name);
	void update_list_model(int cb_idx);

private:
	void _setup_widgets();

	Ui::SetAppearance *_ui;
	AgrFile *_agr_file;
	QDataWidgetMapper *_mapper;
};

} /* namespace dg */

#endif // SETAPPEARANCE_H
