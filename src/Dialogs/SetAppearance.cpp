#include "SetAppearance.h"

#include <QMessageBox>
#include <QPushButton>

#include "../Data/DataManager.h"
#include "ui_SetAppearance.h"

namespace dg {

SetAppearance::SetAppearance(DataManager *dm, QWidget *parent) :
		QDialog(parent), _ui(new Ui::SetAppearance), _dm(dm) {
	_ui->setupUi(this);
	_ui->list_curves->setModel(dm);

	_setup_widgets();

	_mapper = new QDataWidgetMapper(this);
	_mapper->setModel(dm);
	_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	_mapper->addMapping(_ui->line_legend, 1);
	// TODO: we should set up a model for the combobox to make it possible to use currentData rather than currentText
	_mapper->addMapping(_ui->cb_line_style, 2, "currentText");
	_mapper->addMapping(_ui->sb_line_width, 3);
	_mapper->addMapping(_ui->line_colour, 4);

	QObject::connect(_ui->list_curves->selectionModel(), &QItemSelectionModel::currentRowChanged, _mapper, &QDataWidgetMapper::setCurrentModelIndex);

	connect(_ui->line_colour, &QLineEdit::textChanged, this, &SetAppearance::change_btn_colour_background);
	connect(_ui->btn_colour, &QAbstractButton::clicked, this, &SetAppearance::pick_colour);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Ok), &QPushButton::clicked, _mapper, &QDataWidgetMapper::submit);
	QObject::connect(_ui->button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked, _mapper, &QDataWidgetMapper::submit);
}

SetAppearance::~SetAppearance() {
	delete _mapper;
	delete _ui;
}

void SetAppearance::show() {
	QWidget::show();
	QModelIndex idx = _ui->list_curves->currentIndex();
	if(idx.row() == -1) {
		QModelIndex first_idx = _dm->index(0, 0);
		_ui->list_curves->setCurrentIndex(first_idx);
	}
}

void SetAppearance::pick_colour() {
	const QColor colour = QColorDialog::getColor(QColor(_ui->line_colour->text()), this, tr("Pick a colour"));

	if(colour.isValid()) {
		_ui->line_colour->setText(colour.name());
		change_btn_colour_background(colour.name());
	}
}

void SetAppearance::change_btn_colour_background(const QString &colour_name) {
	QString ss_text = QString("background-color: %1").arg(colour_name);
	_ui->btn_colour->setStyleSheet(ss_text);
}

void SetAppearance::_setup_widgets() {
	// Line style
	_ui->cb_line_style->setIconSize(QSize(80, 14));
	for(int pen_style = Qt::NoPen; pen_style < Qt::CustomDashLine; pen_style++) {
		QPixmap pix(80, 14);
		// make the option's background the same colour of the widget's background
		pix.fill(palette().color(QPalette::Background));

		QBrush brush(Qt::black);
		QPen pen(brush, 2.5, (Qt::PenStyle) pen_style);

		QPainter painter(&pix);
		painter.setPen(pen);
		painter.drawLine(2, 7, 78, 7);

		qDebug() << QString::number(pen_style);
		_ui->cb_line_style->addItem(QIcon(pix), QString::number(pen_style), pen_style);
	}
}

} /* namespace dg */
