/*
 * Axis.h
 *
 *  Created on: 13 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_COMMANDS_AXIS_H_
#define SRC_COMMANDS_AXIS_H_

#include <QUndoStack>
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

private:
	QCustomPlot *_plot;
	QCPAxis *_axis;
	QMap<int, QString> _axis_types;
	AxisAppearance _new_appearance;
	AxisAppearance _old_appearance;
};

} /* namespace dg */

#endif /* COMMANDS_AXIS_H_ */
