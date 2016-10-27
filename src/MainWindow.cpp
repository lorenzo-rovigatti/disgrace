/*
 * MainWindow.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Commands/Legend.h"

namespace dg {

MainWindow::MainWindow(QCommandLineParser *parser, QWidget *parent) :
		QMainWindow(parent), _ui(new Ui::MainWindow),
		_toggle_drag_legend(false), _dragging_legend(false) {
	_ui->setupUi(this);

	_plot = _ui->custom_plot;
	_agr_file = new AgrFile(_plot);

	_initialise_undo_stack();

	_plot->setFocus();

	const QStringList args = parser->positionalArguments();
	qDebug() << "Passed in" << args.size() << "file(s)";

	bool rescale = false;
	foreach(QString filename, args) {
		if(!_agr_file->parse_agr(filename)) {
			_agr_file->parse_text(filename);
			rescale = true;
		}
	}
	_plot->replot();
	if(rescale) _plot->rescaleAxes();

	_import_dataset_dialog = new ImportDataset(this);
	_set_appearance_dialog = new SetAppearance(_agr_file, this);

	foreach(AgrGraph *graph, _agr_file->graphs()) {
		// TODO: encapsulate this in a method that can be called when new graphs are added
		QObject::connect(graph, &AgrGraph::new_command, this, &MainWindow::push_command);
	}

	QObject::connect(_ui->action_toggle_axis_dragging, &QAction::toggled, this, &MainWindow::toggle_axis_dragging);
	QObject::connect(_ui->action_toggle_legend, &QAction::toggled, this, &MainWindow::toggle_legend);
	QObject::connect(_ui->action_toggle_drag_legend, &QAction::triggered, this, &MainWindow::toggle_drag_legend);
	QObject::connect(_ui->action_export_as_PDF, &QAction::triggered, this, &MainWindow::export_as_pdf);
	QObject::connect(_ui->action_save_as, &QAction::triggered, this, &MainWindow::save_as);
	QObject::connect(_ui->action_data_import, &QAction::triggered, _import_dataset_dialog, &ImportDataset::show);
	QObject::connect(_import_dataset_dialog, &ImportDataset::import_ready, this, &MainWindow::import_datasets);
	QObject::connect(_ui->action_set_appearance, &QAction::triggered, _set_appearance_dialog, &SetAppearance::show);

	QObject::connect(_plot, &QCustomPlot::mouseMove, this, &MainWindow::mouse_move);
	QObject::connect(_plot, &QCustomPlot::mousePress, this, &MainWindow::mouse_press);
	QObject::connect(_plot, &QCustomPlot::mouseRelease, this, &MainWindow::mouse_release);
	QObject::connect(_plot, &QCustomPlot::beforeReplot, this, &MainWindow::before_replot);
	QObject::connect(_plot, &QCustomPlot::axisDoubleClick, this, &MainWindow::axis_double_click);
}

MainWindow::~MainWindow() {
	delete _ui;
	delete _agr_file;
	delete _undo_view;
	delete _undo_stack;
	delete _import_dataset_dialog;
	delete _set_appearance_dialog;
}

void MainWindow::toggle_axis_dragging(bool val) {
	if(val) {
		_old_ranges.ranges.clear();
		foreach(QCPAxis *axis, _plot->axisRect()->axes()) {
			_old_ranges.ranges[axis] = axis->range();
		}
	}
	else _undo_stack->push(new AxisDraggingCommand(_plot, _old_ranges));

	qDebug() << "Toggling the axis dragging to" << val;
	_plot->setInteraction(QCP::iRangeDrag, val);
	_plot->setInteraction(QCP::iRangeZoom, val);
}

void MainWindow::toggle_legend(bool val) {
	qDebug() << "Toggling the legend to" << val;
	_plot->legend->setVisible(val);
	_plot->replot();
}

void MainWindow::toggle_drag_legend(bool val) {
	qDebug() << "Toggling dragging the legend to" << val;
	_toggle_drag_legend = val;
}

void MainWindow::mouse_move(QMouseEvent *event) {
	if(_dragging_legend) {
		QRectF rect = _plot->axisRect()->insetLayout()->insetRect(0);
		// since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
		QPointF mousePoint((event->pos().x() - _plot->axisRect()->left()) / (double) _plot->axisRect()->width(), (event->pos().y() - _plot->axisRect()->top()) / (double) _plot->axisRect()->height());
		rect.moveTopLeft(mousePoint - _drag_legend_origin);
		_plot->axisRect()->insetLayout()->setInsetRect(0, rect);
		_plot->replot();
	}

	// transform the mouse position to x,y coordinates and show them in the status bar
	double x = _plot->xAxis->pixelToCoord(event->pos().x());
	double y = _plot->yAxis->pixelToCoord(event->pos().y());
	QString msg = QString("X, Y = [%1 , %2]").arg(x).arg(y);
	_ui->statusbar->showMessage(msg);
}

void MainWindow::mouse_press(QMouseEvent *event) {
	if(_toggle_drag_legend && _plot->legend->selectTest(event->pos(), false) > 0) {
		_dragging_legend = true;
		_old_legend_pos = _plot->axisRect()->insetLayout()->insetRect(0).topLeft();
		// since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
		QPointF mousePoint((event->pos().x() - _plot->axisRect()->left()) / (double) _plot->axisRect()->width(), (event->pos().y() - _plot->axisRect()->top()) / (double) _plot->axisRect()->height());
		_drag_legend_origin = mousePoint - _old_legend_pos;
	}
}

void MainWindow::mouse_release(QMouseEvent *event) {
	Q_UNUSED(event);

	if(_dragging_legend) {
		_dragging_legend = false;
		_undo_stack->push(new MoveLegendCommand(_plot, _plot->axisRect()->insetLayout(), _old_legend_pos));
	}
}

void MainWindow::before_replot() {
	// this is to prevent the legend from stretching if the plot is stretched since we've set the inset placement to be ipFree, the width/height of the legend
	// is also defined in axisRect coordinates (0..1) and thus would stretch. This is due to change in a future release (still an issue in QCP 2.0 though).
	_plot->legend->setMaximumSize(_plot->legend->minimumSizeHint());
}

void MainWindow::push_command(QUndoCommand *nc) {
	_undo_stack->push(nc);
}

void MainWindow::export_as_pdf() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to..."), "", tr("PDF files(*.pdf)"));
	if(filename.size() > 0) write_to_pdf(filename);
}

void MainWindow::save_as() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as..."), "", tr("Agr files(*.agr)"));
	if(filename.size() > 0) _agr_file->write_to(filename);
}

void MainWindow::write_to_pdf(QString filename) {
	_plot->savePdf(filename);
}

void MainWindow::import_datasets(ImportDatasetResult &res) {
	bool rescale_x = res.autoscale.contains('X');
	bool rescale_y = res.autoscale.contains('Y');

	_agr_file->parse_text(res.filename);

	if(rescale_x) {
		_plot->xAxis->rescale();
		_plot->xAxis2->rescale();
	}
	if(rescale_y) {
		_plot->yAxis->rescale();
		_plot->yAxis2->rescale();
	}

	_plot->replot();
}

void MainWindow::axis_double_click(QCPAxis *axis, QCPAxis::SelectablePart part) {
	bool ok;
	QString new_label = QInputDialog::getText(this, "disgrace", tr("New axis label:"), QLineEdit::Normal, axis->label(), &ok);
	if(ok) {
		AxisAppearance new_app;
		new_app.label = new_label;
		_undo_stack->push(new AxisAppearanceCommand(_plot, axis, new_app));
	}
}

void MainWindow::_initialise_undo_stack() {
	_undo_stack = new QUndoStack();

	_undo_view = new QUndoView(_undo_stack, this);
	_undo_view->setWindowTitle(tr("Command List"));
	_undo_view->show();
	_undo_view->setAttribute(Qt::WA_QuitOnClose, false);
	_undo_view->setMaximumWidth(200);
	_ui->horizontalLayout->addWidget(_undo_view);

	_ui->action_undo->setShortcuts(QKeySequence::Undo);
	QObject::connect(_ui->action_undo, &QAction::triggered, _undo_stack, &QUndoStack::undo);

	_ui->action_redo->setShortcuts(QKeySequence::Redo);
	QObject::connect(_ui->action_redo, &QAction::triggered, _undo_stack, &QUndoStack::redo);
}

} /* namespace dg */
