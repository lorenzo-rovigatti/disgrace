/*
 * defs.h
 *
 *  Created on: 15 ott 2016
 *      Author: lorenzo
 */

#ifndef COMMANDS_DEFS_H_
#define COMMANDS_DEFS_H_

#include <QPen>
#include "../Data/Dataset.h"

namespace dg {

enum COMMAND_IDS {
	MOVE_LEGEND = 1,
	AXIS_APPEARANCE = 2,
	SET_APPEARANCE = 3
};

struct SetAppearanceDetails {
	Dataset *dataset;
	QString legend;
	QPen line_pen;
	int symbol_type;
	int symbol_size;
	QPen symbol_pen;
};

} /* namespace dg */

#endif /* SRC_COMMANDS_DEFS_H_ */
