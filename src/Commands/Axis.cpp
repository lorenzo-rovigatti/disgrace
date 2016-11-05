/*
 * Axis.cpp
 *
 *  Created on: 13 Oct 2016
 *      Author: rovigattil
 */

#include "Axis.h"

namespace dg {

// BEGIN AxisAppearanceCommand
AxisAppearanceCommand::AxisAppearanceCommand(AgrGraph *graph, QCPAxis *axis, AxisAppearanceDetails &new_appearance, QUndoCommand *parent):
		QUndoCommand(parent), _graph(graph), _axis(axis) {
	_axis_types[QCPAxis::atLeft] = QString("left");
	_axis_types[QCPAxis::atRight] = QString("right");
	_axis_types[QCPAxis::atTop] = QString("top");
	_axis_types[QCPAxis::atBottom] = QString("bottom");

	_new_appearance = new_appearance;
	_old_appearance.label = _axis->label();
}

AxisAppearanceCommand::~AxisAppearanceCommand() {

}

void AxisAppearanceCommand::undo() {
	_graph->set_axis_label(_axis, _old_appearance.label);

	setText(QObject::tr("Restoring the appearance of the %1 axis").arg(_axis_types[_axis->axisType()]));
}

void AxisAppearanceCommand::redo() {
	_graph->set_axis_label(_axis, _new_appearance.label);

	setText(QObject::tr("Changing the appearance of the %1 axis").arg(_axis_types[_axis->axisType()]));
}
// END AxisAppearanceCommand

// BEGIN AxisDraggingCommand
AxisDraggingCommand::AxisDraggingCommand(AgrGraph *graph, QUndoCommand *parent):
	QUndoCommand(parent), _graph(graph) {

	_old_range = graph->graph_range();
	_new_range = graph->current_graph_range();
}

AxisDraggingCommand::~AxisDraggingCommand() {

}

void AxisDraggingCommand::undo() {
	_graph->set_graph_range(_old_range);

	setText(QObject::tr("Restoring the axes ranges"));
}

void AxisDraggingCommand::redo() {
	_graph->set_graph_range(_new_range);

	setText(QObject::tr("Changing the axes ranges"));
}
// END AxisDraggingCommand

} /* namespace dg */
