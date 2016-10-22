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

AgrGraph::AgrGraph() {
	_initialise();
}

AgrGraph::AgrGraph(QString &line) {
	_initialise();
	_lines.push_back(line);

	QRegularExpression re_graph_start("@g(\\d+) (on|off)");
	QRegularExpressionMatch match = re_graph_start.match(line);
	if(match.hasMatch()) _id = match.captured(1).toInt();
	else {
		qCritical() << "The AgrGraph header line" << line << "does not contain a valid graph id";
		// TODO: to be removed
		exit(1);
	}
}

void AgrGraph::_initialise() {
	_state = "in_properties";
	_curr_dataset = NULL;
	_id = -1;
}

AgrGraph::~AgrGraph() {

}

void AgrGraph::parse_line(QString &line) {
	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");

	QRegularExpressionMatch match = re_set_start.match(line);
	if(_state == "in_properties") {
		if(match.hasMatch()) {
			int set_id = match.captured(1).toInt();
			_curr_dataset = new Dataset();
			_curr_dataset->append_header_line(line);
			_datasets[set_id] = _curr_dataset;

			_state = "in_sets";
			qDebug() << "Parsing a set line";
		}
		else {
			_lines.push_back(line);

			qDebug() << "Parsing a graph line";
		}
	}
	else if(_state == "in_sets") {
		if(match.hasMatch()) {
			int set_id = match.captured(1).toInt();
			_curr_dataset = new Dataset();
			_curr_dataset->append_header_line(line);
			_datasets[set_id] = _curr_dataset;
		}
		_curr_dataset->append_header_line(line);

		qDebug() << "Parsing a set line";
	}
	else {
		qCritical() << "Could not parse graph line";
		// TODO: to be removed
		exit(1);
	}
}

Dataset *AgrGraph::dataset(int d_id) {
	if(!_datasets.contains(d_id)) {
		qCritical() << "Graph" << _id;
		// TODO: to be removed
		exit(1);
	}

	return _datasets[d_id];
}

} /* namespace dg */
