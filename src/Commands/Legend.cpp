/*
 * Legend.cpp
 *
 *  Created on: 10 Oct 2016
 *      Author: rovigattil
 */

#include "Legend.h"

namespace dg {

MoveLegendCommand::MoveLegendCommand(QCustomPlot *plot, QCPLayoutInset *inset_layout, const QPointF &old_pos, QUndoCommand *parent): QUndoCommand(parent) {
	_plot = plot;
	_inset_layout = inset_layout;
	_old_pos = old_pos;
	_new_pos = inset_layout->insetRect(0).topLeft();
}

MoveLegendCommand::~MoveLegendCommand() {

}

bool MoveLegendCommand::mergeWith(const QUndoCommand *command) {
	return false;

	const MoveLegendCommand *moveCommand = static_cast<const MoveLegendCommand *>(command);
	_new_pos = _inset_layout->insetRect(0).topLeft();

	setText(_get_formatted_text(_old_pos, _new_pos));

	return true;
}

void MoveLegendCommand::undo() {
	QRectF rect = _inset_layout->insetRect(0);
	rect.moveTopLeft(_old_pos);
	_inset_layout->setInsetRect(0, rect);
	_plot->replot();

	setText(_get_formatted_text(_new_pos, _old_pos));
}

void MoveLegendCommand::redo() {
	QRectF rect = _inset_layout->insetRect(0);
	rect.moveTopLeft(_new_pos);
	_inset_layout->setInsetRect(0, rect);
	_plot->replot();

	setText(_get_formatted_text(_old_pos, _new_pos));
}

QString MoveLegendCommand::_get_formatted_text(QPointF &old_pos, QPointF &new_pos) {
	return QObject::tr("Moving the legend from (%1, %2) to (%3, %4)").arg(
			QString::number(old_pos.x()), QString::number(old_pos.y()),
			QString::number(new_pos.x()), QString::number(new_pos.y())
	);
}

} /* namespace dg */
