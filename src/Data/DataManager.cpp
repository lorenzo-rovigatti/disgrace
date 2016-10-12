/*
 * DataManager.cpp
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#include "DataManager.h"

namespace dg {

DataManager::DataManager(QCustomPlot *plot): _plot(plot) {

}

void DataManager::add_datasets_from_file(QString filename, bool rescale) {
	Dataset new_dataset;
	new_dataset.set_name(filename);

	QFile input(filename);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text)) qCritical() << "File" << filename << "is not readable";

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
				new_dataset.x.push_back(n_lines);
				double val = spl[0].toDouble(&conv_ok);
				new_dataset.y.push_back(val);
				break;
			}
			case 2:
			default: {
				double val = spl[0].toDouble(&conv_ok);
				new_dataset.x.push_back(val);
				val = spl[1].toDouble(&conv_ok);
				new_dataset.y.push_back(val);
			}
			}

			n_lines++;
		}
	}

	QCPGraph *new_graph = _plot->addGraph();
	new_graph->setData(new_dataset.x, new_dataset.y);
	new_graph->setName(new_dataset.name());
	new_graph->addToLegend();

	if(rescale) _plot->rescaleAxes(true);
	_plot->replot();
}

DataManager::~DataManager() {
	QMap<Dataset *, QCPAbstractPlottable *>::iterator it = _datasets.begin();
	while(it != _datasets.end()) {
		delete it.key();
		++it;
	}
}

} /* namespace dg */
