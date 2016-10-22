/*
 * Set.cpp
 *
 *  Created on: 16 ott 2016
 *      Author: lorenzo
 */

#include "Set.h"

namespace dg {

SetAppearanceCommand::SetAppearanceCommand(SetAppearanceDetails &old_appearance, SetAppearanceDetails &new_appearance, QUndoCommand *parent) :
		QUndoCommand(parent), _old_appearance(old_appearance), _new_appearance(new_appearance) {
	if(old_appearance.dataset != new_appearance.dataset) {
		qCritical() << "The objects storing the old and new appearances point to different datasets";
		exit(1);
	}
}

SetAppearanceCommand::~SetAppearanceCommand() {

}

void SetAppearanceCommand::undo() {
	Dataset *dataset = _old_appearance.dataset;
	dataset->set_appearance(_old_appearance);

	setText(QObject::tr("Restoring the appearance of set n. %1").arg(dataset->id()));
}

void SetAppearanceCommand::redo() {
	Dataset *dataset = _old_appearance.dataset;
	dataset->set_appearance(_new_appearance);

	setText(QObject::tr("Changing the appearance of set n. %1").arg(dataset->id()));
}

} /* namespace dg */
