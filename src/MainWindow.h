/*
 * MainWindow.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QUndoStack>

#include "qcustomplot/qcustomplot.h"

#include "Data/DataManager.h"
#include "Dialogs/ImportDataset.h"
#include "Dialogs/SetAppearance.h"

namespace Ui {
class MainWindow;
}

namespace dg {

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QCommandLineParser *parser, QWidget *parent = 0);
	virtual ~MainWindow();

	void write_to_pdf(QString);

public slots:
	void toggle_range_dragging(bool);
	void toggle_range_zooming(bool);
	void toggle_legend(bool);
	void toggle_drag_legend(bool);
	void export_as_pdf();
	void import_datasets();

	void axis_double_click(QCPAxis *, QCPAxis::SelectablePart);
	void mouse_move(QMouseEvent *event);
	void mouse_press(QMouseEvent *event);
	void mouse_release(QMouseEvent *event);
	void before_replot();

private:
	void _initialise_axis(QCPAxis *);
	void _initialise_custom_plot();
	void _initialise_undo_stack();
	Ui::MainWindow *_ui;

	QUndoStack *_undo_stack;
	QUndoView *_undo_view;

	QCustomPlot *_plot;
	bool _toggle_drag_legend, _dragging_legend;
	QPointF _drag_legend_origin, _old_legend_pos;

	DataManager *_data_manager;
	ImportDataset *_import_dataset_dialog;
	SetAppearance *_set_appearance_dialog;
};

} /* namespace dg */

#endif /* MAINWINDOW_H_ */
