#include "SetAppearance.h"

#include <QMessageBox>
#include <QPushButton>

#include "../Data/DataManager.h"
#include "ui_SetAppearance.h"

namespace dg {

SetAppearance::SetAppearance(DataManager *dm, QWidget *parent) :
		QDialog(parent), _ui(new Ui::SetAppearance) {
	_ui->setupUi(this);
	_ui->list_curves->setModel(dm);

	_mapper = new QDataWidgetMapper(this);
	_mapper->setModel(dm);
	_mapper->addMapping(_ui->line_legend, 1);

	QObject::connect(_ui->list_curves, &QListView::clicked, _mapper, &QDataWidgetMapper::setCurrentModelIndex);

	QObject::connect(_ui->button_box->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SetAppearance::begin_change_appearance);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SetAppearance::begin_change_appearance);
}

SetAppearance::~SetAppearance() {
	delete _ui;
}

void SetAppearance::begin_change_appearance() {
	emit change_appearance_ready();
}

} /* namespace dg */
