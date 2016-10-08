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

	QObject::connect(_ui->actionToggle_range_dragging, &QAction::toggled, this, &MainWindow::toggle_range_dragging);
	QObject::connect(_ui->actionToggle_range_zooming, &QAction::toggled, this, &MainWindow::toggle_range_zooming);
}

MainWindow::~MainWindow() {
	delete _ui;
}

void MainWindow::toggle_range_dragging(bool val) {
	qDebug() << "Toggling the range dragging to" << val;
	_ui->custom_plot->setInteraction(QCP::iRangeDrag, val);
}

void MainWindow::toggle_range_zooming(bool val) {
	qDebug() << "Toggling the range zooming to" << val;
	_ui->custom_plot->setInteraction(QCP::iRangeZoom, val);
}

void MainWindow::_initialise_custom_plot() {

}
