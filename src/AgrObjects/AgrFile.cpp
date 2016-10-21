/*
 * AgrFile.cpp
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#include "AgrFile.h"

#include <QFile>
#include <QDebug>

namespace dg {

AgrFile::AgrFile() {

}

AgrFile::~AgrFile() {

}

bool AgrFile::parse(QString filename) {
	_filename = filename;

	_header_lines.clear();
	_datasets.clear();
	_graphs.clear();
	_regions.clear();
	_drawing_objects.clear();

	QRegularExpression re_header_start("# Grace project file");
	QRegularExpression re_header_stop("@timestamp def");
	QRegularExpression re_region_start("@r(\\d+) (on|off)");
	QRegularExpression re_object_start("@with (\\w+)");
	QRegularExpression re_graph_start("@g(\\d+) (on|off)");
	QRegularExpression re_dataset_start("@target G(\\d+).S(\\d+)");
	QRegularExpression re_set_label("G(\\d+)S(\\d+)", QRegularExpression::CaseInsensitiveOption);
	QRegularExpression re_page_size("@page\\s+size\\s+(\\d+),\\s*(\\d+)");
	QRegularExpression re_font_size("(^@ \\s* default \\s+ char \\s+ size \\s+)(?P<val>.*)");

	QString state("opened");
	qDebug() << "Parsing" << filename;

	QFile fh(filename);
	if(!fh.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not readable";
		return false;
	}

	int line_nr = 0;
	while(!fh.atEnd()) {
		QString line = QString(fh.readLine()).trimmed();
		line_nr += 1;

		if(line.size() == 0) continue;
		if(state == "opened") {
			_header_lines.push_back(line);
			qDebug() << line_nr << "adding line to header";

			if(_has_match(re_header_start, line)) state = "in_header";
		}
		else if(state == "in_header") {
			_header_lines.push_back(line);
			qDebug() << line_nr << "adding line to header";

			if(_has_match(re_header_stop, line)) state = "in_drawing_objects";
		}
		else if(state == "in_drawing_objects") {
			// the drawing_options may be ended by the start of a region
			if(_has_match(re_region_start, line)) {
				state = "in_regions";
				_regions.push_back(AgrRegion());
				_regions.back().append(line);

				qDebug() << line_nr << "parsing region n." << _regions.size() - 1;
			}
			// ... or by the start of a graph
			else if(_has_match(re_graph_start, line)) {
				state = "in_graphs";
				_graphs.push_back(AgrGraph(line));

				qDebug() << line_nr << "parsing graph n." << _graphs.size() - 1;
			}
			// a proper drawing_objects line
			else {
				if(_has_match(re_object_start, line)) _drawing_objects.push_back(AgrDrawingObject());
				_drawing_objects.back().append(line);

				qDebug() << line_nr << "found object n." << _drawing_objects.size() - 1;
			}
		}
		else if(state == "in_regions") {
			// the regions may be ended by the start of a graph
			if(_has_match(re_graph_start, line)) {
				state = "in_graphs";
				_graphs.push_back(AgrGraph(line));

				qDebug() << line_nr << "parsing graph n." << _graphs.size();
			}
			// a proper region line
			else {
				if(_has_match(re_region_start, line)) _regions.push_back(AgrRegion());
				 _regions.back().append(line);

				qDebug() << line_nr << "adding line to region";
			}
		}
		else if(state == "in_graphs") {
			// the graphs may be ended by the start of a dataset
			if(_has_match(re_dataset_start, line)) {
				state = "in_datasets";
				_datasets.push_back(new Dataset());
				_datasets.back()->append_agr_line(line);

				qDebug() << line_nr << "parsing dataset n." << _datasets.size() - 1;
			}
			// a new graph
			else if(_has_match(re_graph_start, line)) {
				_graphs.push_back(AgrGraph(line));

				qDebug() << line_nr << "parsing graph n." << _graphs.size() - 1;
			}
			// a proper graph line
			else {
				_graphs.back().parse_line(line);

				qDebug() << line_nr << "adding line to graph";
			}
		}
		else if(state == "in_datasets") {
			if(_has_match(re_dataset_start, line)) {
				_datasets.push_back(new Dataset());
				qDebug() << line_nr << "parsing dataset n." << _datasets.size() - 1;
			}
			_datasets.back()->append_agr_line(line);

			qDebug() << line_nr << "adding line to dataset";
		}
		else {
			QString msg = QString("Could not parse line %1 of %2").arg(line_nr).arg(filename);
			qWarning() << msg;
		}
	}
	qDebug() << "Parsing done";

	if(state == "opened") {
		qWarning() << "The file is missing the 'Grace project file' header.";
		return false;
	}

	// TODO: add support for multiple plots
	if(_graphs.size() > 1) {
		qCritical() << "disgrace does not support multiple plots yet";
		// TODO: to be removed
		exit(1);
	}

	_check_consistency();

	return true;
}

bool AgrFile::_has_match(QRegularExpression &re, QString &str) {
	QRegularExpressionMatch match = re.match(str);
	return match.hasMatch();
}

void AgrFile::_check_consistency() {
	// TODO: to be implemented
}

} /* namespace dg */
