#include <QFileDialog>
#include <QMessageBox>

#include "ImportDataset.h"
#include "ui_ImportDataset.h"

namespace dg {

ImportDataset::ImportDataset(QWidget *parent) :
		QDialog(parent), _ui(new Ui::ImportDataset), _is_error(false) {
	_ui->setupUi(this);

	QObject::connect(_ui->button_choose_file, &QPushButton::clicked, this, &ImportDataset::select_import_file);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ImportDataset::start_import);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ImportDataset::start_import);
}

ImportDataset::~ImportDataset() {
	delete _ui;
}

void ImportDataset::start_import() {
	_is_error = true;
	QString filename(_ui->line_chosen_file->text());

	if(filename.size() == 0) {
		QMessageBox::warning(this, "Warning", tr("No file name given"));
		return;
	}

	QFile f(filename);
	if(!f.exists()) {
		QMessageBox::warning(this, "Warning", tr("Can't stat file %1").arg(filename));
		return;
	}

	// validation has been completed, we save the choices
	_options.filename = filename;
	_options.load_as = _ui->cb_load_as->currentText();
	_options.autoscale = _ui->cb_autoscale->currentText();
	_is_error = false;

	emit import_ready();
}

void ImportDataset::select_import_file() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Import from..."));

	if(filename.size() > 0) _ui->line_chosen_file->setText(filename);
}

void ImportDataset::done(int r) {
	// OK was pressed, check that there are no errors
	if(r == QDialog::Accepted && _is_error) return;
	// cancel was pressed
	QDialog::done(r);
}

} /* namespace dg */
