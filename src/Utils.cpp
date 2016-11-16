/*
 * Utils.cpp
 *
 *  Created on: 16 nov 2016
 *      Author: lorenzo
 */

#include "Utils.h"

#include <cmath>

namespace dg {

Utils::Utils() {

}

Utils::~Utils() {

}

QSize Utils::page_size_to_agr(const QSize &qs) {
	return QSize(qs.width()/_page_size_factor, qs.height()/_page_size_factor);
}

QSize Utils::page_size_to_disgrace(const QSize &qs) {
	return QSize(qs.width()*_page_size_factor, qs.height()*_page_size_factor);
}

double Utils::symbol_size_to_agr(int size) {
	return size/_symbol_size_factor;
}

int Utils::symbol_size_to_disgrace(double size) {
	return (int) round(size*_symbol_size_factor);
}

double Utils::line_width_to_agr(int size) {
	return size/_line_width_factor;
}

int Utils::line_width_to_disgrace(double size) {
	return (int) round(size*_line_width_factor);
}

} /* namespace dg */
