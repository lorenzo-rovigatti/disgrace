/*
 * AgrGraph.cpp
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#include "AgrGraph.h"

#include <QRegularExpression>
#include <QDebug>

namespace dg {

AgrGraph::AgrGraph(): _state("in_properties") {

}

AgrGraph::AgrGraph(QString &line): _state("in_properties") {
	_lines.push_back(line);
}

AgrGraph::~AgrGraph() {

}

void AgrGraph::parse_line(QString &line) {
	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");

	QRegularExpressionMatch match = re_set_start.match(line);
	qDebug() << _state;
	if(_state == "in_properties") {
		if(match.hasMatch()) {
			_sets.push_back(AgrSet());
			_sets.back().append(line);

			_state = "in_sets";
			qDebug() << "Parsing a set line";
		}
		else {
			_lines.push_back(line);

			qDebug() << "Parsing a graph line";
		}
	}
	else if(_state == "in_sets") {
		if(match.hasMatch()) _sets.push_back(AgrSet());
		_sets.back().append(line);

		qDebug() << "Parsing a set line";
	}
	else {
		qCritical() << "Could not parse graph line";
		// TODO: to be removed
		exit(1);
	}
}

} /* namespace dg */
