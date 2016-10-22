/*
 * AgrGraph.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRGRAPH_H_
#define AGROBJECTS_AGRGRAPH_H_

#include <QString>
#include <QVector>
#include <QMap>
#include "../Data/Dataset.h"

namespace dg {

/**
 * Represents a plot in the xmgrace language.
 */
class AgrGraph {
public:
	AgrGraph();
	AgrGraph(QString &line);
	virtual ~AgrGraph();

	void parse_line(QString &line);
	QList<Dataset *> datasets() {
		return _datasets.values();
	}

	Dataset *dataset(int d_id);

protected:
	Dataset *_curr_dataset;

	QVector<QString> _lines;
	QMap<int, Dataset *> _datasets;
	QString _state;
	int _id;

	void _initialise();
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRGRAPH_H_ */
