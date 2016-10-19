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

	// connects the list widget to the mapper, so that the letter is notified when the user changes the selected set
	QObject::connect(_ui->list_curves->selectionModel(), &QItemSelectionModel::currentRowChanged, _mapper, &QDataWidgetMapper::setCurrentModelIndex);
	QObject::connect(dm, &DataManager::dataChanged, _mapper, &QDataWidgetMapper::setCurrentModelIndex);

	// set up the colour pickers
	QAction *line_colour_action = _ui->line_colour->addAction(QIcon(), QLineEdit::TrailingPosition);
	line_colour_action->setData(QVariant("pick_colour_action"));
	connect(line_colour_action, &QAction::triggered, this, &SetAppearance::pick_colour);
	connect(_ui->line_colour, &QLineEdit::textChanged, this, &SetAppearance::change_btn_colour_background);

	QAction *symbol_colour_action = _ui->symbol_colour->addAction(QIcon(), QLineEdit::TrailingPosition);
	symbol_colour_action->setData(QVariant("pick_colour_action"));
	connect(symbol_colour_action, &QAction::triggered, this, &SetAppearance::pick_colour);
	connect(_ui->symbol_colour, &QLineEdit::textChanged, this, &SetAppearance::change_btn_colour_background);

	// TODO: pressing apply and then Ok without changing anything should not push twice the same command
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
	QAction *action = static_cast<QAction *>(QObject::sender());
	QList<QWidget *> widgets = action->associatedWidgets();

	// we need to loop over the associated widgets to find the associated QLineEdit
	foreach(QWidget *w, widgets) {
		QLineEdit *le = dynamic_cast<QLineEdit *>(w);
		if(le) {
			qDebug() << w;
			const QColor colour = QColorDialog::getColor(QColor(le->text()), this, tr("Pick a colour"));
			if(colour.isValid()) le->setText(colour.name());
		}
	}
}

void SetAppearance::change_btn_colour_background(const QString &colour_name) {
	QLineEdit *le = dynamic_cast<QLineEdit *>(QObject::sender());
	if(!le) {
		qCritical() << "QObject::sender() cannot be cast to a QLineEdit *";
		exit(1);
	}

	QString ss_text = QString("background-color: %1").arg(colour_name);
	foreach(QAction *action, le->actions()) {
		if(action->data().toString() == QString("pick_colour_action")) {
			QPixmap pixmap(100, 100);
			pixmap.fill(QColor(colour_name));
			action->setIcon(QIcon(pixmap));
		}
	}
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

		_ui->cb_line_style->addItem(QIcon(pix), QString::number(pen_style), pen_style);
	}
}

} /* namespace dg */
