/*
 * DataManager.h
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_DATA_DATAMANAGER_H_
#define SRC_DATA_DATAMANAGER_H_

#include <QObject>
#include <QMap>

#include "Dataset.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

class DataManager: public QObject {
Q_OBJECT;

public:
	DataManager(QCustomPlot *plot);
	virtual ~DataManager();

	void add_datasets_from_file(QString filename, bool rescale);

public slots:
	void update_graph_data(Dataset *);

private:
	Dataset *_parse_next_dataset(QFile &input);
	QPen _get_next_pen();

	QCustomPlot *_plot;
	QMap<Dataset *, QCPAbstractPlottable *> _datasets;

	QPen _default_pen;
	QList<QColor> _default_colors;
	QList<QColor>::iterator _curr_color;
};

} /* namespace dg */

#endif /* SRC_DATA_DATAMANAGER_H_ */
