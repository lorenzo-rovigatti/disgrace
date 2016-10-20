/*
 * AgrDrawingObject.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGRPARSER_AGRDRAWINGOBJECT_H_
#define AGRPARSER_AGRDRAWINGOBJECT_H_

#include <QVector>
#include <QString>

namespace dg {

class AgrDrawingObject {
public:
	AgrDrawingObject();
	virtual ~AgrDrawingObject();

	void append(QString &line);

protected:
	QVector<QString> _lines;
};

} /* namespace dg */

#endif /* AGRPARSER_AGRDRAWINGOBJECT_H_ */
