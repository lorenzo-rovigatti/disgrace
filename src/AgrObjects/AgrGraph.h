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
#include "AgrSet.h"

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
	QVector<AgrSet> &sets() {
		return _sets;
	}

protected:
	QVector<QString> _lines;
	QVector<AgrSet> _sets;
	QString _state;
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRGRAPH_H_ */
