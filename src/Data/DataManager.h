/*
 * DataManager.h
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_DATA_DATAMANAGER_H_
#define SRC_DATA_DATAMANAGER_H_

#include <QMap>

#include "Dataset.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

class DataManager {
public:
	DataManager(QCustomPlot *plot);
	virtual ~DataManager();

	void add_datasets_from_file(QString filename, bool rescale);

private:
	QCustomPlot *_plot;
	QMap<Dataset *, QCPAbstractPlottable *> _datasets;
};

} /* namespace dg */

#endif /* SRC_DATA_DATAMANAGER_H_ */
