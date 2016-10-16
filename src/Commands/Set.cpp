/*
 * Set.cpp
 *
 *  Created on: 16 ott 2016
 *      Author: lorenzo
 */

#include "Set.h"

namespace dg {

SetAppearanceCommand::SetAppearanceCommand(QUndoCommand *parent):
	QUndoCommand(parent) {

}

SetAppearanceCommand::~SetAppearanceCommand() {

}

void SetAppearanceCommand::undo() {
	setText(QObject::tr("Restoring the appearance of set n. %1").arg(1));
}

void SetAppearanceCommand::redo() {
	setText(QObject::tr("Changing the appearance of set n. %1").arg(1));
}

} /* namespace dg */
