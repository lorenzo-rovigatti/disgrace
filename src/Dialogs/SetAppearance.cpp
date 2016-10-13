#include "SetAppearance.h"
#include "ui_SetAppearance.h"

SetAppearance::SetAppearance(QWidget *parent) :
		QDialog(parent), ui(new Ui::SetAppearance) {
	ui->setupUi(this);
}

SetAppearance::~SetAppearance() {
	delete ui;
}
