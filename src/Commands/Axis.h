/*
 * Axis.h
 *
 *  Created on: 13 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_COMMANDS_AXIS_H_
#define SRC_COMMANDS_AXIS_H_

#include <QUndoStack>
#include "defs.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

struct AxisAppearance {
	QString label;
};

class AxisAppearanceCommand: public QUndoCommand {
public:
	AxisAppearanceCommand(QCustomPlot *plot, QCPAxis *axis, AxisAppearance &new_appearance, QUndoCommand *parent = 0);
	virtual ~AxisAppearanceCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;
	int id() const Q_DECL_OVERRIDE { return AXIS_APPEARANCE; }

private:
	QCustomPlot *_plot;
	QCPAxis *_axis;
	QMap<int, QString> _axis_types;
	AxisAppearance _new_appearance;
	AxisAppearance _old_appearance;
};

struct AxisRanges {
	QMap<QCPAxis *, QCPRange> ranges;
};

class AxisDraggingCommand: public QUndoCommand {
public:
	AxisDraggingCommand(QCustomPlot *plot, AxisRanges &old_ranges, QUndoCommand *parent = 0);
	virtual ~AxisDraggingCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	QCustomPlot *_plot;
	AxisRanges _new_ranges;
	AxisRanges _old_ranges;
};

} /* namespace dg */

#endif /* COMMANDS_AXIS_H_ */
