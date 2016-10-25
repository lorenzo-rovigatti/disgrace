/*
 * Dataset.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "Dataset.h"

#include <iostream>
#include <QRegularExpression>
#include <QDebug>
#include "../Commands/defs.h"

namespace dg {

Dataset::Dataset(): _type(""), _id_dataset(-1), _id_header(-1), _plottable(NULL), _legend(NULL) {
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

	QStringList to_be_quoted;
	to_be_quoted << "legend"
			<< "comment"
			<< "avalue prepend"
			<< "avalue append";
	_settings.set_paths_to_be_quoted(to_be_quoted);

	_implemented_types << "xy";
	_default_pen.setWidth(2);
}

Dataset::~Dataset() {
	if(_plottable != NULL) delete _plottable;
}

void Dataset::set_appearance(SetAppearanceDetails &new_appearance) {
	if(_type == "xy") {
		QCPCurve *graph = static_cast<QCPCurve *>(_plottable);

		set_legend(new_appearance.legend);
		graph->setPen(new_appearance.line_pen);

		QCPScatterStyle ss((QCPScatterStyle::ScatterShape) new_appearance.symbol_type,
				new_appearance.symbol_pen.color(),
				new_appearance.symbol_size);
		graph->setScatterStyle(ss);
	}

	emit changed(this);
}

SetAppearanceDetails Dataset::appearance() {
	SetAppearanceDetails res;

	if(_type == "xy") {
		QCPCurve *graph = static_cast<QCPCurve *>(_plottable);

		res.dataset = this;
		res.legend = graph->name();
		res.line_pen = graph->pen();

		res.symbol_type = graph->scatterStyle().shape();
		res.symbol_size = graph->scatterStyle().size();
		res.symbol_pen = graph->scatterStyle().pen();
	}

	return res;
}

QPen Dataset::_pen() {
	QPen pen(_default_pen);
	// +1 because we don't want to have white pens
	int id_colour = (id() % (QColorDialog::customCount() - 1)) + 1;
	pen.setColor(QColorDialog::customColor(id_colour));

	return pen;
}

void Dataset::create_plottable(QCPAxisRect *axis_rect, QCPLegend *rect_legend) {
	_legend = rect_legend;

	if(_type == "xy") {
		_plottable = new QCPCurve(axis_rect->axis(QCPAxis::atBottom), axis_rect->axis(QCPAxis::atLeft));
		_plottable->setPen(_pen());
		_set_plottable_data();
	}

	set_legend(legend());
	set_visible(visible());
}

bool Dataset::visible() {
	return !_settings.get<bool>("hidden");
}

void Dataset::set_visible(bool is_visible) {
	if(is_visible) _plottable->addToLegend(_legend);
	else _plottable->removeFromLegend(_legend);
	_plottable->setVisible(is_visible);
}

QString Dataset::legend() {
	qDebug() << _settings.get<QString>("legend").endsWith('"');
	return _settings.get<QString>("legend");
}

void Dataset::set_legend(QString new_legend) {
	_settings.put("legend", new_legend);
	_plottable->setName(new_legend);
}

void Dataset::append_header_line(QString line) {
	_header_lines.push_back(line);

	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");
	QRegularExpressionMatch match = re_set_start.match(line);
	if(match.hasMatch()) _id_header = match.captured(1).toInt();

	// this RE matches lines like "@  s9 key1 ... value"
	QRegularExpression re_option("@\\s*s(\\d+) (.+)\\s+(.+)$");
	match = re_option.match(line);
	if(!match.hasMatch()) {
		qCritical() << "Set" << _id_header << "contains the line" << line << "which does not refer to itself";
	}
	else _settings.put(match.captured(2), match.captured(3));
}

void Dataset::append_agr_line(QString line) {
	QRegularExpression re_graph_set_number("@target G(\\d+).S(\\d+)");
	QRegularExpression re_type("@type (\\w+)");

	QRegularExpressionMatch number_match = re_graph_set_number.match(line);
	QRegularExpressionMatch type_match = re_type.match(line);
	if(number_match.hasMatch()) {
		_dataset_header_lines.push_back(line);
		set_id(number_match.captured(2).toInt());

		if(_id_header != -1 && id() != _id_header) {
			qCritical() << "Dataset " << id() << "is being associated to the wrong set" << _id_header;
			// TODO: to be removed
			exit(1);
		}
	}
	else if(type_match.hasMatch()) {
		set_type(type_match.captured(1));
		_dataset_header_lines.push_back(line);
	}
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
			QStringList spl = line.split(QRegExp("\\s+"));
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

	_settings.put("legend", input.fileName());
}

void Dataset::_set_plottable_data() {
	if(_type == "xy") {
		QCPCurve *curve = static_cast<QCPCurve *>(_plottable);
		curve->setData(x, y);
	}
}

void Dataset::_check_type(QString type) {
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
}

void Dataset::set_type(QString type) {
	_check_type(type);
	_type = type;
}

void Dataset::set_id(int n_id) {
	_id_dataset = n_id;
}

void Dataset::write_headers(QTextStream &ts) {
	QString prefix = QString("@    s%1 ").arg(id());
	foreach(QString line, _settings.as_string_list()) {
		ts << prefix << line << '\n';
	}
}

void Dataset::write_dataset(QTextStream &ts) {
	foreach(QString line, _dataset_header_lines) {
		ts << line << '\n';
	}

	for(int i = 0; i < x.size(); i++) {
		if(_type == "xy") ts << x.at(i) << " " << y.at(i) << '\n';
	}
	ts << "&" << '\n';
}

} /* namespace dg */
