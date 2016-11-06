/*
 * AgrFile.cpp
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#include "AgrFile.h"

#include "AgrDefaults.h"

#include <QSaveFile>
#include <QDebug>
#include <iostream>

namespace dg {

AgrFile::AgrFile(QCustomPlot *plot): _filename(), _plot(plot),
		_curr_graph(NULL), _curr_dataset(NULL) {
	// initialise the QCustomPlot instance
	_plot->plotLayout()->clear();
	_plot->setAutoAddPlottableToLegend(false);

	QStringList to_be_quoted;
	to_be_quoted << "default sformat"
			<< "timestamp def";
	_settings.set_paths_to_be_quoted(to_be_quoted);
	QStringList overlapping_keys("timestamp");
	_settings.set_overlapping_keys(overlapping_keys);
	_settings.overwrite_settings_from(AgrDefaults::file());
	_settings_map = AgrDefaults::settings_map();

	_add_graph(new AgrGraph(_plot), 0);

	_load_settings();
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

void AgrFile::write_to(QString filename) {
	_filename = filename;
	_settings.put("timestamp def", QDateTime::currentDateTime().toString("ddd MMM d hh:mm:ss yy"));

	QSaveFile fh(filename);
	if(!fh.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not writable";
		// TODO: to be removed;
		exit(1);
	}

	QTextStream out(&fh);
	out << "# disgrace project file" << '\n';
	out << "#" << '\n';

	QString prefix("@");
	foreach(QString line, _settings.as_string_list()) {
		out << prefix << line << '\n';
		if(line.startsWith("link page")) _settings_map.write_maps(out);
	}

	foreach(AgrGraph *graph, _graphs.values()) {
		graph->write_headers(out);
	}

	foreach(AgrGraph *graph, _graphs.values()) {
		graph->write_datasets(out);
	}

	fh.commit();
}

void AgrFile::_add_header_line(QString line) {
	if(line.startsWith("@")) {
		line.remove(0, 1);
		if(line.startsWith("map")) {
			_settings_map.add_line(line);
		}
		else {
			_settings.put(line);
		}
	}
	else _header_lines.push_back(line);
}

void AgrFile::_add_agr_graph(int graph_id, QString line) {
	if(_graphs.contains(graph_id)) {
		// the only time we overwrite the plot with id graph_id is at the very beginning, when there is only one
		// empty plot created by the constructor
		if(graph_id == 0 && _graphs[graph_id]->empty()) {
			_graphs[0]->remove();
		}
		else {
			QString error = QString("The AGR file contains a plot with id %1, but a plot with that id is already present").arg(graph_id);
			qCritical() << error;
			// TODO: to be removed
			exit(1);
		}
	}
	_add_graph(new AgrGraph(_plot, line), graph_id);
}

void AgrFile::_add_graph(AgrGraph *ng, int graph_id) {
	beginInsertRows(QModelIndex(), rowCount() - 1, rowCount());
	ng->set_id(graph_id);
	_graphs[graph_id] = ng;
	_sorted_graphs.push_back(graph_id);
	_curr_graph = ng;
	endInsertRows();

}

AgrGraph *AgrFile::graph(int graph_id) {
	if(!_graphs.contains(graph_id)) {
		qCritical() << "There is no graph" << graph_id;
		// TODO: to be removed
		exit(1);
	}

	return _graphs.value(graph_id);
}

AgrGraph *AgrFile::graph_by_sorted_idx(int idx) {
	if(!_sorted_graphs.contains(idx)) {
		qCritical() << "There is no graph with sorted index" << idx;
		// TODO: to be removed
		exit(1);
	}

	return graph(_sorted_graphs.at(idx));
}

bool AgrFile::parse_agr(QString filename) {
	QRegularExpression re_header_start("# (disgrace|Grace) project file");
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
			_add_header_line(line);

			if(_has_match(re_header_start, line)) state = "in_header";
		}
		else if(state == "in_header") {
			_add_header_line(line);

			if(_has_match(re_header_stop, line)) state = "in_drawing_objects";
		}
		else if(state == "in_drawing_objects") {
			// the drawing_options may be ended by the start of a region
			if(_has_match(re_region_start, line)) {
				state = "in_regions";
				_regions.push_back(AgrRegion());
				_regions.back().append(line);

				qDebug() << "line" << line_nr << "- parsing region n." << _regions.size() - 1;
			}
			// ... or by the start of a graph
			else if(_has_match(re_graph_start, line)) {
				state = "in_graphs";
				int graph_id = _last_match.captured(1).toInt();
				_add_agr_graph(graph_id, line);

				qDebug() << "line" << line_nr << "- parsing graph n." << _graphs.size() - 1;
			}
			// a proper drawing_objects line
			else {
				if(_has_match(re_object_start, line)) _drawing_objects.push_back(AgrDrawingObject());
				_drawing_objects.back().append(line);

				qDebug() << "line" << line_nr << "- found object n." << _drawing_objects.size() - 1;
			}
		}
		else if(state == "in_regions") {
			// the regions may be ended by the start of a graph
			if(_has_match(re_graph_start, line)) {
				state = "in_graphs";
				int graph_id = _last_match.captured(1).toInt();
				_add_agr_graph(graph_id, line);

				qDebug() << "line" << line_nr << "- parsing graph n." << _graphs.size();
			}
			// a proper region line
			else {
				if(_has_match(re_region_start, line)) _regions.push_back(AgrRegion());
				 _regions.back().append(line);
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

				qDebug() << "line" << line_nr << "- parsing dataset n." << set_id;
			}
			// a new graph
			else if(_has_match(re_graph_start, line)) {
				int graph_id = _last_match.captured(1).toInt();
				_add_agr_graph(graph_id, line);

				qDebug() << "line" << line_nr << "- parsing graph n." << _graphs.size() - 1;
			}
			// a proper graph line
			else _curr_graph->parse_agr_line(line);
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
				qDebug() << "line" << line_nr << "- parsing dataset n." << set_id;
			}
			_curr_dataset->append_agr_line(line);
		}
		else {
			QString msg = QString("Could not parse line %1 of %2").arg(line_nr).arg(filename);
			qWarning() << msg;
		}
	}
	qDebug() << "Parsing done";

	if(state == "opened") {
		qWarning() << "The file is missing the 'Grace project file' header.";
		_header_lines.clear();
		return false;
	}

	foreach(AgrGraph *graph, _graphs) {
		graph->load_agr_settings();
		graph->setup_new_datasets();
	}

	// TODO: add support for multiple plots
	if(_graphs.size() > 1) {
		qCritical() << "disgrace does not support multiple plots yet";
		// TODO: to be removed
		exit(1);
	}

	_filename = filename;
	_load_settings();
	_check_consistency();

	return true;
}

void AgrFile::parse_text(QString filename, int graph_id) {
	if(!_graphs.contains(graph_id)) {
		qCritical() << "There is no plot with id" << graph_id;
		// TODO: to be removed
		exit(1);
	}

	_graphs[graph_id]->add_datasets_from_file(filename);
}

bool AgrFile::_has_match(QRegularExpression &re, QString &str) {
	_last_match = re.match(str);
	return _last_match.hasMatch();
}

void AgrFile::_check_consistency() {
	// TODO: to be implemented
}

void AgrFile::_load_settings() {
	QList<QColor> colours = _settings_map.colours();
	for(int i = 0; i < QColorDialog::customCount() && i < colours.size(); i++) {
		QColorDialog::setCustomColor(i, colours[i]);
	}

	set_page_size(page_size());
}

void AgrFile::set_page_size(const QSize &new_size) {
	QString size = QString("%1, %2").arg(new_size.width()/PAGE_SIZE_FACTOR).arg(new_size.height()/PAGE_SIZE_FACTOR);
	_settings.put("page size", size);

	_plot->setMinimumSize(new_size);
	_plot->setMaximumSize(new_size);
}

QSize AgrFile::page_size() const {
	QVector<float> size_f = _settings.get<QVector<float> >("page size");
	return QSize(size_f[0]*PAGE_SIZE_FACTOR, size_f[1]*PAGE_SIZE_FACTOR);
}

int AgrFile::rowCount(const QModelIndex &parent) const {
	return _sorted_graphs.size();
}

int AgrFile::columnCount(const QModelIndex &parent) const {
	return 1;
}

QVariant AgrFile::data(const QModelIndex &index, int role) const {
	if(!index.isValid()) return QVariant();
	if(index.row() >= _graphs.size() || index.row() < 0) return QVariant();

	int graph_id = index.row();
	AgrGraph *graph = _graphs[_sorted_graphs[graph_id]];
	QVariant res;

	if(role == Qt::DisplayRole) {
		char plus_minus = (graph->visible()) ? '+' : '-';
		res = tr("(%1) Graph %2 [%3]").arg(plus_minus).arg(graph_id).arg(graph->rowCount());
	}

	return res;
}

} /* namespace dg */
