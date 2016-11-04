/*
 * Axis.h
 *
 *  Created on: 13 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_COMMANDS_AXIS_H_
#define SRC_COMMANDS_AXIS_H_

#include "defs.h"
#include "../AgrObjects/AgrGraph.h"
#include "../qcustomplot/qcustomplot.h"

#include <QUndoStack>

namespace dg {

class AxisAppearanceCommand: public QUndoCommand {
public:
	AxisAppearanceCommand(QCustomPlot *plot, QCPAxis *axis, AxisAppearanceDetails &new_appearance, QUndoCommand *parent = 0);
	virtual ~AxisAppearanceCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;
	int id() const Q_DECL_OVERRIDE { return AXIS_APPEARANCE; }

private:
	QCustomPlot *_plot;
	QCPAxis *_axis;
	QMap<int, QString> _axis_types;
	AxisAppearanceDetails _new_appearance;
	AxisAppearanceDetails _old_appearance;
};

class AxisDraggingCommand: public QUndoCommand {
public:
	AxisDraggingCommand(AgrGraph *graph, QUndoCommand *parent = 0);
	virtual ~AxisDraggingCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	AgrGraph *_graph;
	GraphRange _new_range;
	GraphRange _old_range;
};

} /* namespace dg */

#endif /* COMMANDS_AXIS_H_ */
