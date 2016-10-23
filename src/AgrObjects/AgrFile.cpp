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

AgrFile::AgrFile(QCustomPlot *plot): _plot(plot), _curr_graph(NULL), _curr_dataset(NULL) {
	// initialise the QCustomPlot instance
	_plot->plotLayout()->clear();

	// xmgrace defaults (with some differences)
	_custom_colours.push_back(QColor(255, 255, 255));
	_custom_colours.push_back(QColor(0, 0, 0));
	_custom_colours.push_back(QColor(255, 0, 0));
	_custom_colours.push_back(QColor(0, 255, 0));
	_custom_colours.push_back(QColor(0, 0, 255));
	_custom_colours.push_back(QColor(255, 165, 0));
	_custom_colours.push_back(QColor(188, 143, 143));
	_custom_colours.push_back(QColor(200, 200, 200));
	_custom_colours.push_back(QColor(148, 0 , 211));
	_custom_colours.push_back(QColor(0, 255, 255));
	_custom_colours.push_back(QColor(255, 255, 0));
	_custom_colours.push_back(QColor(255, 0, 255));
	_custom_colours.push_back(QColor(114, 33, 188));
	_custom_colours.push_back(QColor(103, 7, 72));
	_custom_colours.push_back(QColor(64, 224, 208));
	_custom_colours.push_back(QColor(0, 139, 0));
}

AgrFile::~AgrFile() {

}

QList<Dataset *> AgrFile::datasets(int graph_id) {
	if(!_graphs.contains(graph_id)) {
		qCritical() << "Graph" << graph_id << "does not exist";
		// TODO: to be removed
		exit(1);
	}

	return _graphs[graph_id]->datasets();
}

void AgrFile::plot() {
	for(int i = 0; i < QColorDialog::customCount() && i < _custom_colours.size(); i++) {
		QColorDialog::setCustomColor(i, _custom_colours[i]);
	}

	foreach(AgrGraph *graph, _graphs.values()) {
		graph->plot();
	}

	_plot->rescaleAxes();
	_plot->replot();
}

bool AgrFile::parse_agr(QString filename) {
	_filename = filename;

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
				int graph_id = _last_match.captured(1).toInt();
				_curr_graph = new AgrGraph(_plot, line);
				_graphs[graph_id] = _curr_graph;

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
				int graph_id = _last_match.captured(1).toInt();
				_curr_graph = new AgrGraph(_plot, line);
				_graphs[graph_id] = _curr_graph;

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
				int graph_id = _last_match.captured(1).toInt();
				int set_id = _last_match.captured(2).toInt();

				if(!_graphs.contains(graph_id)) {
					qCritical() << "The agr file contains a dataset associated to a non-existent graph" << graph_id;
					// TODO: to be removed
					exit(1);
				}

				_curr_dataset = _graphs[graph_id]->dataset(set_id);
				_curr_dataset->append_agr_line(line);

				qDebug() << line_nr << "parsing dataset n." << set_id;
			}
			// a new graph
			else if(_has_match(re_graph_start, line)) {
				int graph_id = _last_match.captured(1).toInt();
				_curr_graph = new AgrGraph(_plot, line);
				_graphs[graph_id] = _curr_graph;

				qDebug() << line_nr << "parsing graph n." << _graphs.size() - 1;
			}
			// a proper graph line
			else {
				_curr_graph->parse_line(line);

				qDebug() << line_nr << "adding line to graph";
			}
		}
		else if(state == "in_datasets") {
			if(_has_match(re_dataset_start, line)) {
				int graph_id = _last_match.captured(1).toInt();
				int set_id = _last_match.captured(2).toInt();

				if(!_graphs.contains(graph_id)) {
					qCritical() << "The agr file contains a dataset associated to a non-existent graph" << graph_id;
					// TODO: to be removed
					exit(1);
				}

				_curr_dataset = _graphs[graph_id]->dataset(set_id);
				qDebug() << line_nr << "parsing dataset n." << set_id;
			}
			_curr_dataset->append_agr_line(line);

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

void AgrFile::parse_text(QString filename, int graph_id) {
	AgrGraph *graph;
	if(_graphs.empty()) {
		graph = new AgrGraph(_plot);
		graph->set_id(0);
		_graphs[0] = graph;
	}
	else graph = _graphs[0];

	graph->add_datasets_from_file(filename);
}

bool AgrFile::_has_match(QRegularExpression &re, QString &str) {
	_last_match = re.match(str);
	return _last_match.hasMatch();
}

void AgrFile::_check_consistency() {
	// TODO: to be implemented
}

} /* namespace dg */
