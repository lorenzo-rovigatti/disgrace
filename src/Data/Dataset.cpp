/*
 * Dataset.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "Dataset.h"

namespace dg {

Dataset::Dataset(): _name("") {

}

Dataset::~Dataset() {

}

void Dataset::set_name(QString name) {
	_name = name;
}

} /* namespace dg */
