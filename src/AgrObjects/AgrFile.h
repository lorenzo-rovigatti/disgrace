/*
 * AgrFile.h
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRFILE_H_
#define AGROBJECTS_AGRFILE_H_

#include <QString>
#include <QVector>
#include <QRegularExpression>

#include "../AgrObjects/AgrDrawingObject.h"
#include "../AgrObjects/AgrGraph.h"
#include "../AgrObjects/AgrRegion.h"
#include "../Data/Dataset.h"

namespace dg {

class AgrFile {
public:
	AgrFile();
	virtual ~AgrFile();

	bool parse(QString filename);
	QVector<Dataset *> &datasets() {
		return _datasets;
	}

private:
	QVector<QString> _header_lines;
	QVector<Dataset *> _datasets;
	QVector<AgrGraph> _graphs;
	QVector<AgrRegion> _regions;
	QVector<AgrDrawingObject> _drawing_objects;
	QString _filename;

	bool _has_match(QRegularExpression &re, QString &str);
	void _check_consistency();
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRFILE_H_ */
