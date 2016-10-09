/*
 * MainWindow.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <qmainwindow.h>
#include "qcustomplot/qcustomplot.h"

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
	void toggle_legend(bool);
	void toggle_drag_legend(bool);
	void export_as_pdf();

	void mouse_move_signal(QMouseEvent *event);
	void mouse_press_signal(QMouseEvent *event);
	void mouse_release_signal(QMouseEvent *event);
	void before_replot();

private:
	void _initialise_axis(QCPAxis *);
	void _initialise_custom_plot();
	Ui::MainWindow *_ui;

	QCustomPlot *_plot;
	bool _toggle_drag_legend, _dragging_legend;
	QPointF _drag_legend_origin;
};

#endif /* MAINWINDOW_H_ */
