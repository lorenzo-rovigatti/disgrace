/*
 * AgrRegion.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRREGION_H_
#define AGROBJECTS_AGRREGION_H_

#include <QVector>
#include <QString>

namespace dg {

class AgrRegion {
public:
	AgrRegion();
	virtual ~AgrRegion();

	void append(QString &line);

protected:
	QVector<QString> _lines;
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRREGION_H_ */
