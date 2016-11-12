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

#include "AgrObjects/AgrFile.h"
#include "Dialogs/ImportDataset.h"
#include "Dialogs/SetAppearance.h"

#include "Commands/Axis.h"

namespace Ui {
class MainWindow;
}

namespace dg {

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QCommandLineParser *parser, QWidget *parent = 0);
	virtual ~MainWindow();

	void write_to_agr(QString filename);
	void write_to_pdf(QString);

public slots:
	void toggle_axis_dragging(bool);
	void toggle_legend(bool);
	void toggle_drag_legend(bool);
	void export_as_pdf();
	void open();
	void save();
	void save_as();
	void import_datasets(ImportDatasetResult &);
	void about();
	void autoscale(bool x = true, bool y = true, bool add_to_stack = true);

	void plot_double_click(QMouseEvent *event);
	void plot_mouse_move(QMouseEvent *event);
	void plot_mouse_press(QMouseEvent *event);
	void plot_mouse_release(QMouseEvent *event);
	void plottable_double_click(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
	void item_double_click(QCPAbstractItem *item, QMouseEvent *event);
	void axis_double_click(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
	void legend_double_click(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
	void before_replot();
	/** Adds the passed QUndoCommand to the undo stack
	 *
	 * @param nc the new command to be added to the undo stack
	 */
	void push_command(QUndoCommand *nc);

private slots:
	void _on_execute_command();
	void _autosave();

private:
	Ui::MainWindow *_ui;

	QUndoStack *_undo_stack;
	QUndoView *_undo_view;

	QTimer *_autosave_timer;

	QCustomPlot *_plot;
	bool _toggle_drag_legend, _dragging_legend;
	QPointF _drag_legend_origin, _old_legend_pos;

	AgrFile *_agr_file;
	ImportDataset *_import_dataset_dialog;
	SetAppearance *_set_appearance_dialog;

	bool _is_unsaved;
	bool _intercept_dbl_click_on_plot;

	void _initialise_undo_stack();
	void _setup_icons();
	void _use_agr_file(AgrFile *new_file);
	void _update_title();
	void _load_settings();

	void closeEvent(QCloseEvent *event);
};

} /* namespace dg */

#endif /* MAINWINDOW_H_ */
