/*
 * AgrFile.h
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#ifndef AGRPARSER_AGRFILE_H_
#define AGRPARSER_AGRFILE_H_

#include <QString>
#include <QVector>

#include "AgrDrawingObject.h"
#include "AgrGraph.h"
#include "AgrRegion.h"
#include "../Data/Dataset.h"

namespace dg {

class AgrFile {
private:
	QVector<Dataset> _datasets;
	QVector<AgrGraph> _graphs;
	QVector<AgrRegion> _regions;
	QVector<AgrDrawingObject> _drawing_objects;
	QString _filename;

public:
	AgrFile();
	virtual ~AgrFile();

	bool parse(QString filename);
};

} /* namespace dg */

#endif /* AGRPARSER_AGRFILE_H_ */
