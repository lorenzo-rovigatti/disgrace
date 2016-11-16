/*
 * Utils.h
 *
 *  Created on: 16 nov 2016
 *      Author: lorenzo
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <QSize>

namespace dg {

class Utils {
public:
	static QSize page_size_to_agr(const QSize &qs);
	static QSize page_size_to_disgrace(const QSize &qs);

	static double symbol_size_to_agr(int size);
	static int symbol_size_to_disgrace(double size);

	static double line_width_to_agr(int size);
	static int line_width_to_disgrace(double size);

private:
	Utils();
	virtual ~Utils();

	static const double _page_size_factor = 1.3;
	static const double _symbol_size_factor = 10;
	static const double _line_width_factor = 2;
};

} /* namespace dg */

#endif /* SRC_UTILS_H_ */
