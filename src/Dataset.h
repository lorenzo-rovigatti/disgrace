/*
 * Dataset.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <vector>

namespace dg {

class Dataset {
public:
	Dataset();
	virtual ~Dataset();

	std::vector<double> x, y, z, dx, dy, dz;
};

} /* namespace dg */

#endif /* SRC_DATASET_H_ */
