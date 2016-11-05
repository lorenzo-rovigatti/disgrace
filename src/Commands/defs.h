/*
 * defs.h
 *
 *  Created on: 15 ott 2016
 *      Author: lorenzo
 */

#ifndef COMMANDS_DEFS_H_
#define COMMANDS_DEFS_H_

// TODO: encapsulate these in some other way
#define SYMBOL_FACTOR 0.1
#define PAGE_SIZE_FACTOR 1.3

#include <QPen>
#include "../AgrObjects/Dataset.h"

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

struct AxisAppearanceDetails {
	QCPRange range;
	QString label;
};

struct LegendAppearanceDetails {
	bool visible;
	QPointF position;
};

struct GraphRange {
	QCPRange x_range;
	QCPRange y_range;
};

struct GraphAppearanceDetails {
	bool visible;
	QList<AxisAppearanceDetails> axes;
	LegendAppearanceDetails legend;
};

} /* namespace dg */

#endif /* SRC_COMMANDS_DEFS_H_ */
