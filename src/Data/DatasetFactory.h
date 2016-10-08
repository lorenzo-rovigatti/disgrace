/*
 * DatasetFactory.h
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#ifndef DATA_DATASETFACTORY_H_
#define DATA_DATASETFACTORY_H_

#include "Dataset.h"

namespace dg {

class DatasetFactory {
public:
	static Dataset build_dataset(QString filename);

private:
	DatasetFactory();
	virtual ~DatasetFactory();
};

} /* namespace dg */

#endif /* DATA_DATASETFACTORY_H_ */
