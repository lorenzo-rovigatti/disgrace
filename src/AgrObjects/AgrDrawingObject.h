/*
 * AgrDrawingObject.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRDRAWINGOBJECT_H_
#define AGROBJECTS_AGRDRAWINGOBJECT_H_

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

#endif /* AGROBJECTS_AGRDRAWINGOBJECT_H_ */
