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
}

MainWindow::~MainWindow() {
	delete _ui;
}
