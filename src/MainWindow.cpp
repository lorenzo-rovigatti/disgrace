/*
 * MainWindow.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
	_ui->setupUi(this);

	_initialise_custom_plot();

	QObject::connect(_ui->action_toggle_range_dragging, &QAction::toggled, this, &MainWindow::toggle_range_dragging);
	QObject::connect(_ui->action_toggle_range_zooming, &QAction::toggled, this, &MainWindow::toggle_range_zooming);
	QObject::connect(_ui->action_export_as_PDF, &QAction::triggered, this, &MainWindow::export_as_pdf);
}

MainWindow::~MainWindow() {
	delete _ui;
}

void MainWindow::add_plot(dg::Dataset &new_set, bool rescale) {
	QCPGraph *new_graph = _ui->custom_plot->addGraph();
	new_graph->setData(new_set.x, new_set.y);

	if(rescale) _ui->custom_plot->rescaleAxes(true);
}

void MainWindow::toggle_range_dragging(bool val) {
	qDebug() << "Toggling the range dragging to" << val;
	_ui->custom_plot->setInteraction(QCP::iRangeDrag, val);
}

void MainWindow::toggle_range_zooming(bool val) {
	qDebug() << "Toggling the range zooming to" << val;
	_ui->custom_plot->setInteraction(QCP::iRangeZoom, val);
}

void MainWindow::export_as_pdf() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to..."), "", tr("PDF files(*.pdf)"));

	if(filename.size() > 0) {
		_ui->custom_plot->savePdf(filename);
	}
}

void MainWindow::_initialise_axis(QCPAxis *axis) {
	axis->grid()->setPen(Qt::NoPen);
	axis->grid()->setZeroLinePen(Qt::NoPen);

	QFont font("Times New Roman", 18);

	axis->setBasePen(QPen(Qt::black, 2));
	axis->setLabelFont(font);
	axis->setTickLabelFont(font);

	// ticks
	axis->setTickLengthIn(14);
	axis->setTickPen(QPen(Qt::black, 2));

	// subticks
	axis->setSubTickLengthIn(7);
}

void MainWindow::_initialise_custom_plot() {
	// Creates a top and a right axes, set their visibility to true and then connects their ranges to the bottom and left axes, respectively
	_ui->custom_plot->axisRect()->setupFullAxesBox(true);
	foreach(QCPAxis *axis, _ui->custom_plot->axisRect()->axes()) {
		_initialise_axis(axis);
	}
}
