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

void Dataset::commit_data_changes() {
	emit data_changed(this);
}

void Dataset::set_name(QString name) {
	_name = name;
}

} /* namespace dg */
