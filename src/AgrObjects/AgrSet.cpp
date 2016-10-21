/*
 * AgrSet.cpp
 *
 *  Created on: 21 Oct 2016
 *      Author: rovigattil
 */

#include "AgrSet.h"

namespace dg {

AgrSet::AgrSet() {

}

AgrSet::~AgrSet() {

}

void AgrSet::append(QString &line) {
	_lines.push_back(line);
}

} /* namespace dg */
