/*
 * AgrDefaults.h
 *
 *  Created on: 26 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRDEFAULTS_H_
#define AGROBJECTS_AGRDEFAULTS_H_

#include "AgrSettings.h"

namespace dg {

class AgrDefaults {
public:
	static const AgrSettings &dataset() { return _dataset; }
	static const AgrSettings &graph() { return _graph; }
	static const AgrSettings &file() { return _file; }

	static void set_dataset(const AgrSettings &new_default) { _dataset = new_default; }
	static void set_graph(const AgrSettings &new_default) { _graph = new_default; }
	static void set_file(const AgrSettings &new_default) { _file = new_default; }

	static void init_defaults();

private:
	AgrDefaults();
	virtual ~AgrDefaults();

	static AgrSettings _dataset;
	static AgrSettings _graph;
	static AgrSettings _file;
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRDEFAULTS_H_ */
