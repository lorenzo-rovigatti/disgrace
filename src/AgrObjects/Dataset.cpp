/*
 * Dataset.cpp
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#include "Dataset.h"
#include "AgrDefaults.h"

#include <iostream>
#include <QRegularExpression>
#include <QDebug>
#include "../Commands/defs.h"

namespace dg {

Dataset::Dataset(SettingsMap *settings_map): _type(""), _id_dataset(-1), _id_header(-1),
		_plottable(NULL), _legend(NULL), _settings_map(settings_map) {
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

	_settings.overwrite_settings_from(AgrDefaults::dataset());

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
		_settings.put("line linewidth", QString::number(new_appearance.line_pen.width()*SYMBOL_FACTOR));
		_settings.put("line type", QString::number(new_appearance.line_pen.style()));
		int colour_idx = _settings_map->idx_by_colour(new_appearance.line_pen.color());
		_settings.put("line color", QString::number(colour_idx));
		graph->setPen(new_appearance.line_pen);

		_settings.put("symbol", QString::number(new_appearance.symbol_type));
		_settings.put("symbol size", QString::number(new_appearance.symbol_size*SYMBOL_FACTOR));
		colour_idx = _settings_map->idx_by_colour(new_appearance.symbol_pen.color());
		_settings.put("symbol color", QString::number(colour_idx));

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
		res.dataset = this;
		res.legend = legend();
		res.line_pen.setWidth(_settings.get<float>("line linewidth")/LINE_FACTOR);
		res.line_pen.setStyle((Qt::PenStyle) _settings.get<int>("line type"));
		int color_idx = _settings.get<int>("line color");
		res.line_pen.setColor(_settings_map->colour_by_idx(color_idx));

		res.symbol_type = _settings.get<int>("symbol");
		res.symbol_size = (int)(_settings.get<float>("symbol size")/SYMBOL_FACTOR);
		color_idx = _settings.get<int>("symbol color");
		res.symbol_pen.setColor(_settings_map->colour_by_idx(color_idx));
	}

	return res;
}

void Dataset::create_plottable(QCPAxisRect *axis_rect, QCPLegend *rect_legend) {
	if(_plottable != NULL) return;

	_legend = rect_legend;

	if(_type == "xy") {
		QCPCurve *new_curve = new QCPCurve(axis_rect->axis(QCPAxis::atBottom), axis_rect->axis(QCPAxis::atLeft));
		new_curve->setScatterSkip(0);
		_plottable = new_curve;
		_set_plottable_data();
	}

	set_visible(visible());
	SetAppearanceDetails curr_appearance = appearance();
	set_appearance(curr_appearance);
}

bool Dataset::visible() {
	return !_settings.get<bool>("hidden");
}

void Dataset::set_visible(bool is_visible) {
	_settings.put_bool("hidden", !is_visible);

	if(_plottable) {
		if(is_visible) _plottable->addToLegend(_legend);
		else _plottable->removeFromLegend(_legend);
		_plottable->setVisible(is_visible);
	}
}

QString Dataset::legend() {
	return _settings.get<QString>("legend");
}

void Dataset::set_legend(QString new_legend) {
	_settings.put("legend", new_legend);

	if(_plottable != NULL) _plottable->setName(new_legend);
}

void Dataset::append_header_line(QString line) {
	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");
	QRegularExpressionMatch match = re_set_start.match(line);
	if(match.hasMatch()) _id_header = match.captured(1).toInt();

	// this RE matches lines like "@  s9 key/values"
	QRegularExpression re_option("@\\s*s(\\d+) (.+)");
	match = re_option.match(line);
	if(!match.hasMatch()) {
		qCritical() << "Set" << _id_header << "contains the line" << line << "which does not refer to itself";
	}
	else _settings.put(match.captured(2));
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
		}
	}
}

void Dataset::init_from_file(QFile &input, QString type) {
	set_type(type);
	set_visible(true);
	set_legend(input.fileName());
	_settings.put("comment", input.fileName());
	int colour_idx = _settings_map->colour_idx_by_dataset_idx(id());
	_settings.put("line color", QString::number(colour_idx));
	_settings.put("symbol color", QString::number(colour_idx));

	int n_columns = -1;
	int n_lines = 0;
	while(!input.atEnd()) {
		QString line = QString(input.readLine()).trimmed();
		if(line[0] != '#' && line.size() > 0) {
			// the regexp makes it possible to split the line in the presence of *any* whitespace
			QStringList spl = line.split(QRegExp("\\s+"));
			if(n_columns == -1) n_columns = spl.size();

			if(spl.size() >= n_columns) {
				switch(n_columns) {
				case 0:
					break;
				case 1: {
					bool cast_ok = false;
					x.push_back(n_lines);
					double val = spl[0].toDouble(&cast_ok);
					if(cast_ok)	y.push_back(val);
					break;
				}
				case 2:
				default: {
					bool cast_x_ok = false;
					bool cast_y_ok = false;
					double new_x = spl[0].toDouble(&cast_x_ok);
					double new_y = spl[1].toDouble(&cast_y_ok);
					if(cast_x_ok && cast_y_ok) {
						x.push_back(new_x);
						y.push_back(new_y);
					}
				}
				}
			}

			n_lines++;
		}
		else if(!empty()) return;
	}
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
	_settings.put("type", type);
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

void Dataset::write_dataset(QTextStream &ts, int graph_id) {
	ts << "@target G" << graph_id << ".S" << id() << '\n';
	ts << "@type " << _settings.get<QString>("type") << '\n';

	for(int i = 0; i < x.size(); i++) {
		if(_type == "xy") ts << x.at(i) << " " << y.at(i) << '\n';
	}
	ts << "&" << '\n';
}

} /* namespace dg */
