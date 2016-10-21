/*
 * AgrSet.h
 *
 *  Created on: 21 Oct 2016
 *      Author: rovigattil
 */

#ifndef AGROBJECTS_AGRSET_H_
#define AGROBJECTS_AGRSET_H_

#include <QString>
#include <QVector>

namespace dg {

/**
 * Represents the properties (i.e. not the actual data) of a dataset in an AgrGraph
 */
class AgrSet {
public:
	AgrSet();
	virtual ~AgrSet();

	void append(QString &line);

protected:
	QVector<QString> _lines;
};

} /* namespace dg */

#endif /* SRC_AGROBJECTS_AGRSET_H_ */
