/*
 * MainWindow.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <qmainwindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:
	Ui::MainWindow *_ui;
};

#endif /* MAINWINDOW_H_ */
