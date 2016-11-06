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
		_toggle_drag_legend(false), _dragging_legend(false),
		_is_unsaved(false), _intercept_dbl_click_on_plot(false) {
	_ui->setupUi(this);
	_ui->central_widget->layout()->setAlignment(_ui->custom_plot, Qt::AlignTop);
	_setup_icons();

	_import_dataset_dialog = new ImportDataset(this);
	_set_appearance_dialog = new SetAppearance(this);
	_autosave_timer = new QTimer(this);

	_plot = _ui->custom_plot;

	_initialise_undo_stack();

	_plot->setFocus();

	const QStringList args = parser->positionalArguments();
	qDebug() << "Passed in" << args.size() << "file(s)";

	_agr_file = NULL;
	AgrFile *new_file = new AgrFile(_plot);
	bool rescale = false;
	foreach(QString filename, args) {
		if(!new_file->parse_agr(filename)) {
			new_file->parse_text(filename);
			rescale = true;
		}
	}
	_use_agr_file(new_file);

	_plot->replot();
	if(rescale) _plot->rescaleAxes();

	connect(_ui->action_toggle_axis_dragging, &QAction::toggled, this, &MainWindow::toggle_axis_dragging);
	connect(_ui->action_toggle_legend, &QAction::toggled, this, &MainWindow::toggle_legend);
	connect(_ui->action_toggle_drag_legend, &QAction::triggered, this, &MainWindow::toggle_drag_legend);
	connect(_ui->action_export_as_PDF, &QAction::triggered, this, &MainWindow::export_as_pdf);
	connect(_ui->action_open, &QAction::triggered, this, &MainWindow::open);
	connect(_ui->action_save, &QAction::triggered, this, &MainWindow::save);
	connect(_ui->action_save_as, &QAction::triggered, this, &MainWindow::save_as);
	connect(_ui->action_data_import, &QAction::triggered, _import_dataset_dialog, &ImportDataset::show);
	connect(_ui->action_about, &QAction::triggered, this, &MainWindow::about);
	connect(_import_dataset_dialog, &ImportDataset::import_ready, this, &MainWindow::import_datasets);
	connect(_ui->action_set_appearance, &QAction::triggered, _set_appearance_dialog, &SetAppearance::show);

	connect(_plot, &QCustomPlot::mouseMove, this, &MainWindow::plot_mouse_move);
	connect(_plot, &QCustomPlot::mousePress, this, &MainWindow::plot_mouse_press);
	connect(_plot, &QCustomPlot::mouseRelease, this, &MainWindow::plot_mouse_release);
	connect(_plot, &QCustomPlot::beforeReplot, this, &MainWindow::before_replot);
	connect(_plot, &QCustomPlot::axisDoubleClick, this, &MainWindow::axis_double_click);
	connect(_plot, &QCustomPlot::mouseDoubleClick, this, &MainWindow::plot_double_click);
	connect(_plot, &QCustomPlot::plottableDoubleClick, this, &MainWindow::plottable_double_click);
	connect(_plot, &QCustomPlot::legendDoubleClick, this, &MainWindow::legend_double_click);

	connect(_autosave_timer, &QTimer::timeout, this, &MainWindow::_autosave);

	_load_settings();
}

MainWindow::~MainWindow() {
	delete _ui;
	delete _agr_file;
	delete _undo_view;
	delete _undo_stack;
	delete _import_dataset_dialog;
	delete _set_appearance_dialog;
}

void MainWindow::_load_settings() {
	QSettings settings;

	bool autosave_enabled = settings.value("autosave", true).toBool();
	if(autosave_enabled) _autosave_timer->start(settings.value("autosave/time", 60000).toInt());
}

void MainWindow::_on_execute_command() {
	_is_unsaved = true;
	_update_title();
}

void MainWindow::_update_title() {
	QString new_title("disgrace: ");
	if(_agr_file->filename().isNull()) {
		_is_unsaved = true;
		new_title += tr("[untitled]");
	}
	else {
		new_title += QString(": %1").arg(_agr_file->filename());
		if(_is_unsaved) new_title += tr(" [unsaved]");
	}

	setWindowTitle(new_title);
}

void MainWindow::_use_agr_file(AgrFile *new_file) {
	if(_agr_file != NULL) {
		foreach(AgrGraph *graph, _agr_file->graphs()) {
			// TODO: encapsulate this in a method that can be called when graphs are removed
			QObject::disconnect(graph, &AgrGraph::new_command, this, &MainWindow::push_command);
		}

		delete _agr_file;
	}
	_agr_file = new_file;
	_set_appearance_dialog->connect_to_file(_agr_file);

	foreach(AgrGraph *graph, _agr_file->graphs()) {
		// TODO: encapsulate this in a method that can be called when new graphs are added
		QObject::connect(graph, &AgrGraph::new_command, this, &MainWindow::push_command);
	}
	_update_title();
}

void MainWindow::toggle_axis_dragging(bool val) {
	if(!val) _undo_stack->push(new AxisDraggingCommand(_agr_file->current_graph()));

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

void MainWindow::plot_mouse_move(QMouseEvent *event) {
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

void MainWindow::plot_mouse_press(QMouseEvent *event) {
	if(_toggle_drag_legend && _plot->legend->selectTest(event->pos(), false) > 0) {
		_dragging_legend = true;
		_old_legend_pos = _plot->axisRect()->insetLayout()->insetRect(0).topLeft();
		// since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
		QPointF mousePoint((event->pos().x() - _plot->axisRect()->left()) / (double) _plot->axisRect()->width(), (event->pos().y() - _plot->axisRect()->top()) / (double) _plot->axisRect()->height());
		_drag_legend_origin = mousePoint - _old_legend_pos;
	}
}

void MainWindow::plot_mouse_release(QMouseEvent *event) {
	Q_UNUSED(event);

	if(_dragging_legend) {
		_dragging_legend = false;
		_undo_stack->push(new MoveLegendCommand(_plot, _plot->axisRect()->insetLayout(), _old_legend_pos));
	}

	if(_intercept_dbl_click_on_plot) {
		_set_appearance_dialog->show();
		_intercept_dbl_click_on_plot = false;
	}
}

void MainWindow::plot_double_click(QMouseEvent *event) {
	_intercept_dbl_click_on_plot = true;
}

void MainWindow::plottable_double_click(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event) {
	_set_appearance_dialog->select_set(_agr_file->current_graph()->model_index_from_plottable(plottable));
}

void MainWindow::item_double_click(QCPAbstractItem *item, QMouseEvent *event) {
	_intercept_dbl_click_on_plot = false;
}

void MainWindow::axis_double_click(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event) {
	_intercept_dbl_click_on_plot = false;
	bool ok;
	QString new_label = QInputDialog::getText(this, "disgrace", tr("New axis label:"), QLineEdit::Normal, axis->label(), &ok);
	if(ok) {
		AxisAppearanceDetails new_app;
		new_app.label = new_label;
		_undo_stack->push(new AxisAppearanceCommand(_agr_file->current_graph(), axis, new_app));
	}
}

void MainWindow::legend_double_click(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event) {
	_intercept_dbl_click_on_plot = false;
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

void MainWindow::open() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Agr files(*.agr)"));
	if(filename.size() > 0) {
		AgrFile *new_file = new AgrFile(_plot);
		new_file->parse_agr(filename);
		_use_agr_file(new_file);
	}
}

void MainWindow::save() {
	if(_agr_file->filename().isNull()) save_as();
	else write_to_agr(_agr_file->filename());
}

void MainWindow::save_as() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as..."), "", tr("Agr files(*.agr)"));
	if(filename.size() > 0) write_to_agr(filename);
}

void MainWindow::write_to_agr(QString filename) {
	_agr_file->write_to(filename);
	_is_unsaved = false;
	_update_title();
}

void MainWindow::_autosave() {
	if(!_agr_file->filename().isNull()) save();
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

void MainWindow::about() {
	QString text = tr("disgrace %1\n\nby Lorenzo Rovigatti (2016)").arg(DISGRACE_VERSION);
	QMessageBox::about(this, tr("About disgrace"), text);
}

void MainWindow::_setup_icons() {
	_ui->action_toggle_axis_dragging->setIcon(QIcon(":/images/icons/drag_range.png"));

	_ui->action_new->setIcon(QIcon::fromTheme("document-new"));
	_ui->action_open->setIcon(QIcon::fromTheme("document-open"));
	_ui->action_save->setIcon(QIcon::fromTheme("document-save"));
	_ui->action_save_as->setIcon(QIcon::fromTheme("document-save-as"));

	_ui->action_undo->setIcon(QIcon::fromTheme("edit-undo"));
	_ui->action_redo->setIcon(QIcon::fromTheme("edit-redo"));

	_ui->action_exit->setIcon(QIcon::fromTheme("application-exit"));
	_ui->action_about->setIcon(QIcon::fromTheme("help-about"));
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

	QObject::connect(_undo_stack, &QUndoStack::indexChanged, this, &MainWindow::_on_execute_command);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	event->accept();
	if(_is_unsaved) {
		QMessageBox::StandardButton answer = QMessageBox::question(this, tr("Confirmation"), tr("Exit losing unsaved changes?"));
		if(answer != QMessageBox::Yes) event->ignore();
	}
}

} /* namespace dg */
