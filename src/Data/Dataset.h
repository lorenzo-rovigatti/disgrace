/*
 * Dataset.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <QVector>
#include <QString>

namespace dg {

class Dataset {
private:
	QString _name;
public:
	Dataset();
	virtual ~Dataset();

	QVector<double> x, y, z, dx, dy, dz;

	QString name() { return _name; }
	void set_name(QString name);
};

} /* namespace dg */

#endif /* SRC_DATASET_H_ */
