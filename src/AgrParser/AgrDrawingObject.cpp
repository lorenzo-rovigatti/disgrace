/*
 * AgrDrawingObject.cpp
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#include "AgrDrawingObject.h"

namespace dg {

AgrDrawingObject::AgrDrawingObject() {

}

AgrDrawingObject::~AgrDrawingObject() {

}

void AgrDrawingObject::append(QString &line) {
	_lines.push_back(line);
}

} /* namespace dg */
