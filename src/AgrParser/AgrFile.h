/*
 * AgrFile.h
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#ifndef AGRPARSER_AGRFILE_H_
#define AGRPARSER_AGRFILE_H_

#include "../Data/Dataset.h"

namespace dg {

class AgrFile {
private:
	std::vector<Dataset> _datasets;

public:
	AgrFile();
	virtual ~AgrFile();
};

} /* namespace dg */

#endif /* AGRPARSER_AGRFILE_H_ */
