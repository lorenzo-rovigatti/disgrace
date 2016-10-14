/*
 * DataManager.cpp
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#include "DataManager.h"

namespace dg {

DataManager::DataManager(QCustomPlot *plot): _plot(plot) {
	_default_pen.setWidth(2);

	_default_colors.push_back(Qt::black);
	_default_colors.push_back(Qt::red);
	_default_colors.push_back(Qt::green);
	_default_colors.push_back(Qt::blue);
	_default_colors.push_back(QColor("orange"));
	_default_colors.push_back(QColor("brown"));
	_default_colors.push_back(QColor("magenta"));

	_curr_color = _default_colors.begin();
}

DataManager::~DataManager() {
	QMap<Dataset *, QCPAbstractPlottable *>::iterator it = _datasets.begin();
	while(it != _datasets.end()) {
		delete it.key();
		++it;
	}
}

Dataset *DataManager::_parse_next_dataset(QFile &input) {
	Dataset *new_ds = new Dataset();

	int n_columns = -1;
	int n_lines = 0;
	while(!input.atEnd()) {
		QString line = QString(input.readLine()).trimmed();
		if(line[0] != '#' && line.size() > 0) {
			// the regexp makes it possible to split the line in the presence of *any* whitespace
			QStringList spl = line.split(QRegExp("\\s"));
			if(n_columns == -1) n_columns = spl.size();
			bool conv_ok = true;

			switch(n_columns) {
			case 1: {
				new_ds->x.push_back(n_lines);
				double val = spl[0].toDouble(&conv_ok);
				new_ds->y.push_back(val);
				break;
			}
			case 2:
			default: {
				double val = spl[0].toDouble(&conv_ok);
				new_ds->x.push_back(val);
				val = spl[1].toDouble(&conv_ok);
				new_ds->y.push_back(val);
			}
			}

			n_lines++;
		}
		else if(!new_ds->empty()) return new_ds;
	}

	return new_ds;
}

void DataManager::add_datasets_from_file(QString filename, bool rescale_x, bool rescale_y) {
	QFile input(filename);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not readable";
		return;
	}

	while(!input.atEnd()) {
		Dataset *curr_dataset = _parse_next_dataset(input);
		curr_dataset->set_name(filename);

		QCPGraph *new_graph = _plot->addGraph();
		new_graph->setPen(_get_next_pen());
		new_graph->setData(curr_dataset->x, curr_dataset->y);
		new_graph->setName(curr_dataset->name());
		new_graph->addToLegend();

		QObject::connect(curr_dataset, &Dataset::data_changed, this, &DataManager::update_graph_data);
		_datasets[curr_dataset] = new_graph;
	}

	if(rescale_x) {
		_plot->xAxis->rescale();
		_plot->xAxis2->rescale();
	}
	if(rescale_y) {
		_plot->yAxis->rescale();
		_plot->yAxis2->rescale();
	}
	_plot->replot();
}

void DataManager::update_graph_data(Dataset *ds) {
	if(_datasets.contains(ds)) {
		QCPGraph *associated_graph = static_cast<QCPGraph *>(_datasets.value(ds));
		if(!associated_graph) {
			qCritical() << "The QCPAbstractPlottable pointer associated to a dataset does not point to a QCPGraph object";
			return;
		}

		qDebug() << ds->x[0];
		associated_graph->setData(ds->x, ds->y);
		_plot->replot();
	}
}

QPen DataManager::_get_next_pen() {
	QPen new_pen(_default_pen);
	new_pen.setColor(*_curr_color);
	_curr_color++;
	if(_curr_color == _default_colors.end()) _curr_color = _default_colors.begin();

	return new_pen;
}

} /* namespace dg */
