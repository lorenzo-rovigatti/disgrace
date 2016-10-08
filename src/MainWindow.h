/*
 * MainWindow.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <qmainwindow.h>
#include "Data/Dataset.h"

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

	void add_plot(dg::Dataset &, bool rescale=true);

	// Slots
	void toggle_range_dragging(bool);
	void toggle_range_zooming(bool);

private:
	void _initialise_custom_plot();
	Ui::MainWindow *_ui;
};

#endif /* MAINWINDOW_H_ */
