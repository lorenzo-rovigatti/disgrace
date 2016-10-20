/*
 * Set.cpp
 *
 *  Created on: 16 ott 2016
 *      Author: lorenzo
 */

#include "Set.h"
#include "../Data/DataManager.h"

namespace dg {

SetAppearanceCommand::SetAppearanceCommand(DataManager *dm, SetAppearanceDetails &old_appearance, SetAppearanceDetails &new_appearance, QUndoCommand *parent) :
		_dm(dm), _old_appearance(old_appearance), _new_appearance(new_appearance), QUndoCommand(parent) {
	if(old_appearance.dataset != new_appearance.dataset) {
		qCritical() << "The objects storing the old and new appearances point to different datasets";
		exit(1);
	}
}

SetAppearanceCommand::~SetAppearanceCommand() {

}

void SetAppearanceCommand::undo() {
	Dataset *dataset = _old_appearance.dataset;
	QCPCurve *graph = static_cast<QCPCurve *>(_dm->_datasets[dataset]);
	_set_graph_style(graph, _old_appearance);

	_dm->emit_dataChanged(dataset);

	// TODO change so that it displays the right set index
	setText(QObject::tr("Restoring the appearance of set n. %1").arg(1));
}

void SetAppearanceCommand::redo() {
	Dataset *dataset = _old_appearance.dataset;
	QCPCurve *graph = static_cast<QCPCurve *>(_dm->_datasets[dataset]);
	_set_graph_style(graph, _new_appearance);

	_dm->emit_dataChanged(dataset);

	// TODO change so that it displays the right set index
	setText(QObject::tr("Changing the appearance of set n. %1").arg(1));
}

void SetAppearanceCommand::_set_graph_style(QCPCurve *graph, SetAppearanceDetails &appearance) {
	graph->setName(appearance.legend);
	graph->setPen(appearance.line_pen);

	QCPScatterStyle ss((QCPScatterStyle::ScatterShape) appearance.symbol_type, appearance.symbol_pen.color(), appearance.symbol_size);
	graph->setScatterStyle(ss);
}

} /* namespace dg */
