/*
 * AgrRegion.cpp
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#include "AgrRegion.h"

namespace dg {

AgrRegion::AgrRegion() {

}

AgrRegion::~AgrRegion() {

}

void AgrRegion::append(QString &line) {
	_lines.push_back(line);
}

} /* namespace dg */
