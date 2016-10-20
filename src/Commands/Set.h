/*
 * Set.h
 *
 *  Created on: 16 ott 2016
 *      Author: lorenzo
 */

#ifndef COMMANDS_SET_H_
#define COMMANDS_SET_H_

#include <QUndoStack>
#include "defs.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

class DataManager;

class SetAppearanceCommand: public QUndoCommand {
public:
	SetAppearanceCommand(DataManager *dm, SetAppearanceDetails &old_appearance, SetAppearanceDetails &new_appearance, QUndoCommand *parent = 0);
	virtual ~SetAppearanceCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;
	int id() const Q_DECL_OVERRIDE { return SET_APPEARANCE; }

private:
	void _set_graph_style(QCPCurve *graph, SetAppearanceDetails &appearance);

	DataManager *_dm;
	SetAppearanceDetails _old_appearance;
	SetAppearanceDetails _new_appearance;
};

} /* namespace dg */

#endif /* COMMANDS_SET_H_ */
