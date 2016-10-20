/*
 * AgrGraph.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGRPARSER_AGRGRAPH_H_
#define AGRPARSER_AGRGRAPH_H_

#include <QString>

namespace dg {

class AgrGraph {
public:
	AgrGraph();
	AgrGraph(QString &line);
	virtual ~AgrGraph();

	void parse_line(QString &line);

protected:
	QString _line;
};

} /* namespace dg */

#endif /* AGRPARSER_AGRGRAPH_H_ */
