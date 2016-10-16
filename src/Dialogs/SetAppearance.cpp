#include <QMessageBox>
#include <QPushButton>

#include "SetAppearance.h"
#include "ui_SetAppearance.h"

namespace dg {

SetAppearance::SetAppearance(QWidget *parent) :
		QDialog(parent), _ui(new Ui::SetAppearance) {
	_ui->setupUi(this);

	QObject::connect(_ui->button_box->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SetAppearance::begin_change_appearance);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SetAppearance::begin_change_appearance);
}

SetAppearance::~SetAppearance() {
	delete _ui;
}

void SetAppearance::set_graphs(QList<QCPGraph *> &graphs) {
	_graphs = graphs;
}

void SetAppearance::begin_change_appearance() {
	emit change_appearance_ready();
}

} /* namespace dg */
