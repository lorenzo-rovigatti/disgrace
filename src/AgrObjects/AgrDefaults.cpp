/*
 * AgrDefaults.cpp
 *
 *  Created on: 26 ott 2016
 *      Author: lorenzo
 */

#include "AgrDefaults.h"

namespace dg {

AgrSettings AgrDefaults::_dataset;
AgrSettings AgrDefaults::_graph;
AgrSettings AgrDefaults::_file;

void AgrDefaults::init_defaults() {
	_dataset.put("hidden", "false");
	_dataset.put("type", "xy");
	_dataset.put("symbol", "0");
	_dataset.put("symbol size", "1.000000");
	_dataset.put("symbol color", "1");
	_dataset.put("symbol pattern", "1");
	_dataset.put("symbol fill color", "1");
	_dataset.put("symbol fill pattern", "0");
	_dataset.put("symbol linewidth", "1.0");
	_dataset.put("symbol linestyle", " 1");
	_dataset.put("symbol char", "65");
	_dataset.put("symbol char font", "0");
	_dataset.put("symbol skip", "0");
	_dataset.put("line type", "1");
	_dataset.put("line linestyle", "1");
	_dataset.put("line linewidth", "2.0");
	_dataset.put("line color", "1");
	_dataset.put("line pattern", "1");
	_dataset.put("baseline ", "0");
	_dataset.put("baseline", "off");
	_dataset.put("dropline", "off");
	_dataset.put("fill type", "0");
	_dataset.put("fill rule ", "");
	_dataset.put("fill color", "1");
	_dataset.put("fill pattern", "1");
	_dataset.put("avalue", "off");
	_dataset.put("avalue type", "2");
	_dataset.put("avalue char size", "1.000000");
	_dataset.put("avalue font", "0");
	_dataset.put("avalue color", "1");
	_dataset.put("avalue rot", "0");
	_dataset.put("avalue format", "general");
	_dataset.put("avalue prec", "3");
	_dataset.put("avalue prepend", "\"\"");
	_dataset.put("avalue append", "\"\"");
	_dataset.put("avalue offset", "0.000000 , 0.000000");
	_dataset.put("errorbar", "on");
	_dataset.put("errorbar place", "both");
	_dataset.put("errorbar color", "1");
	_dataset.put("errorbar pattern", "1");
	_dataset.put("errorbar size", "1.000000");
	_dataset.put("errorbar linewidth", "1.0");
	_dataset.put("errorbar linestyle", "1");
	_dataset.put("errorbar riser linewidth", "1.0");
	_dataset.put("errorbar riser linestyle", "1");
	_dataset.put("errorbar riser clip ", "off");
	_dataset.put("errorbar riser clip length", "0.100000");
	_dataset.put("comment", "");
	_dataset.put("legend", "");
}

AgrDefaults::AgrDefaults() {

}

AgrDefaults::~AgrDefaults() {

}

} /* namespace dg */
