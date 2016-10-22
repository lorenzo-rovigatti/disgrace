/*
 * Dataset.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "Dataset.h"

#include <QRegularExpression>
#include <QDebug>

namespace dg {

Dataset::Dataset(): _name(""), _type(""), _id_dataset(-1), _id_header(-1) {
	_type_to_n_column["xy"] = 2; // An X-Y scatter and/or line plot, plus (optionally) an annotated value
	_type_to_n_column["xydx"] = 3; // Same as XY, but with error bars (either one- or two-sided) along X axis
	_type_to_n_column["xydy"] = 3; // Same as XYDX, but error bars are along Y axis
	_type_to_n_column["xydxdx"] = 4; // Same as XYDX, but left and right error bars are defined separately
	_type_to_n_column["xydydy"] = 4; // Same as XYDXDX, but error bars are along Y axis
	_type_to_n_column["xydxdy"] = 4; // Same as XY, but with X and Y error bars (either one- or two-sided)
	_type_to_n_column["xydxdxdydy"] = 6; // Same as XYDXDY, but left/right and upper/lower error bars are defined separately
	_type_to_n_column["bar"] = 2; // Same as XY, but vertical bars are used instead of symbols
	_type_to_n_column["bardy"] = 3; // Same as BAR, but with error bars (either one- or two-sided) along Y axis
	_type_to_n_column["bardydy"] = 4; // Same as BARDY, but lower and upper error bars are defined separately
	_type_to_n_column["xyhilo"] = 5; // Hi/Low/Open/Close plot
	_type_to_n_column["xyz"] = 3; // Same as XY; makes no sense unless the annotated value is Z
	_type_to_n_column["xyr"] = 3; // X, Y, Radius. Only allowed in Fixed graphs
	_type_to_n_column["xysize"] = 3; // Same as XY, but symbol size is variable
	_type_to_n_column["xycolor"] = 3; // X, Y, color index (of the symbol fill)
	_type_to_n_column["xycolpat"] = 4; // X, Y, color index, pattern index (currently used for Pie charts only)
	_type_to_n_column["xyvmap"] = 4; // Vector map
	_type_to_n_column["xyboxplot"] = 6; // Box plot (X, median, upper/lower limit, upper/lower whisker)

	_implemented_types << "xy";
}

Dataset::~Dataset() {

}

void Dataset::append_header_line(QString line) {
	_header_lines.push_back(line);

	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");
	QRegularExpressionMatch match = re_set_start.match(line);
	if(match.hasMatch()) _id_header = match.captured(1).toInt();
}

void Dataset::append_agr_line(QString line) {
	QRegularExpression re_graph_set_number("@target G(\\d+).S(\\d+)");
	QRegularExpression re_type("@type (\\w+)");

	QRegularExpressionMatch number_match = re_graph_set_number.match(line);
	QRegularExpressionMatch type_match = re_type.match(line);
	if(number_match.hasMatch()) {
		set_id(number_match.captured(2).toInt());

		if(_id_header != -1 && id() != _id_header) {
			qCritical() << "Dataset " << id() << "is being associated to the wrong set" << _id_header;
			// TODO: to be removed
			exit(1);
		}

		QString name = QString("S%1").arg(id());
		set_name(name);
	}
	else if(type_match.hasMatch()) set_type(type_match.captured(1));
	else {
		if(_type == "") {
			qCritical() << "Trying to write to an uninitialised dataset";
			// TODO: to be removed
			exit(1);
		}

		int n_columns = _type_to_n_column[_type];
		QStringList spl = line.split(QRegExp("\\s"));
		if(spl.size() == n_columns) {
			// TODO: this works only for xy sets
			x.push_back(spl[0].toDouble());
			y.push_back(spl[1].toDouble());
			commit_data_changes();
		}
	}
}

void Dataset::init_from_file(QFile &input, QString type) {
	set_type(type);

	int n_columns = -1;
	int n_lines = 0;
	while(!input.atEnd()) {
		QString line = QString(input.readLine()).trimmed();
		if(line[0] != '#' && line.size() > 0) {
			// the regexp makes it possible to split the line in the presence of *any* whitespace
			QStringList spl = line.split(QRegExp("\\s"));
			if(n_columns == -1) n_columns = spl.size();

			switch(n_columns) {
			case 1: {
				x.push_back(n_lines);
				double val = spl[0].toDouble();
				y.push_back(val);
				break;
			}
			case 2:
			default: {
				double val = spl[0].toDouble();
				x.push_back(val);
				val = spl[1].toDouble();
				y.push_back(val);
			}
			}

			n_lines++;
		}
		else if(!empty()) return;
	}

	set_name(input.fileName());
}

void Dataset::commit_data_changes() {
	emit data_changed(this);
}

void Dataset::set_type(QString type) {
	if(_type_to_n_column.contains(type)) {
		if(!_implemented_types.contains(type)) {
			qCritical() << "Type" << type << "is not supported yet";
			// TODO: to be removed
			exit(1);
		}
	}
	else {
		qCritical() << "Unknown type" << type;
		// TODO: to be removed
		exit(1);
	}
	_type = type;
}

void Dataset::set_name(QString name) {
	_name = name;
}

void Dataset::set_id(int n_id) {
	_id_dataset = n_id;
}

} /* namespace dg */
