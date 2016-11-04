/*
 * AgrSettings.cpp
 *
 *  Created on: 25 ott 2016
 *      Author: lorenzo
 */

#include "AgrSettings.h"

#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include <QRegExp>
#include <QRegularExpression>
#include <QDebug>
#include <QRect>

using std::string;

namespace dg {

AgrSettings::AgrSettings() {

}

AgrSettings::~AgrSettings() {

}

void AgrSettings::overwrite_settings_from(const AgrSettings &new_settings) {
	_tree = new_settings._tree;
}

template<typename T>
T AgrSettings::get(QString q_path) {
	return _tree.get<T>(pt::ptree::path_type(_translate_path(q_path), ' '));
}

template<>
bool AgrSettings::get(QString q_path) {
	QString res(get<string>(q_path).c_str());

	if(res == "true") return true;
	if(res == "false") return false;
	return true;
}

template<>
QString AgrSettings::get(QString q_path) {
	return QString(get<string>(q_path).c_str());
}

template<>
QVector<float> AgrSettings::get(QString q_path) {
	QString res_str(get<string>(q_path).c_str());

	QStringList numbers = res_str.split(',');
	QVector<float> res;
	foreach(QString s, numbers) {
		res << s.toFloat();
	}

	return res;
}

template<>
QRectF AgrSettings::get(QString q_path) {
	QString res_str(get<string>(q_path).c_str());

	QStringList numbers = res_str.split(',');
	if(numbers.size() != 4) {
		qCritical() << "Key" << q_path << "has an associated value" << res_str << "which can't be casted to a QRectF";
		// TODO: to be removed
		exit(1);
	}
	QRectF res;
	res.setLeft(numbers.at(0).toFloat());
	res.setBottom(numbers.at(1).toFloat());
	res.setRight(numbers.at(2).toFloat());
	res.setTop(numbers.at(3).toFloat());

	return res;
}

QString AgrSettings::get_overlapping(QString q_path) {
	if(!_overlapping_keys.contains(q_path)) {
		qCritical() << q_path << "is not in the list of overlapping keys";
		// TODO: to be removed
		exit(1);
	}
	q_path += " state";
	return get<QString>(q_path);
}

void AgrSettings::put(QString line) {
	QString key, value;

	line = line.trimmed();
	// if the line contains a double quote then that's where the value starts
	int idx = line.indexOf("\"");
	// if the line contains a number then whatever is at the left of that number is interpreted as the key, the rest is the value
	if(idx == -1) idx = line.indexOf(QRegExp("-?\\d+(\\.\\d+)?"));
	// otherwise we split the string in the presence of the last whitespace
	if(idx == -1) idx = line.lastIndexOf(QRegExp("\\s"));
	key = line.left(idx).trimmed();
	value = line.right(line.size() - idx).trimmed();

	put(key, value);
}

void AgrSettings::put(QString q_path, QString q_value) {
	q_value = q_value.trimmed();
	// we remove trailing and leading double quotes, if present
	if(q_value.startsWith('"') && q_value.endsWith('"')) q_value = q_value.mid(1, q_value.size() - 2);
	string value = q_value.toStdString();

	string translated_path = _translate_path(q_path);
	if(_overlapping_keys.contains(QString(translated_path.c_str()))) {
		if(q_value == "on" || q_value == "off") translated_path += " state";
	}

	_tree.put(pt::ptree::path_type(translated_path, ' '), value);
}

void AgrSettings::put_bool(QString q_path, bool q_value) {
	QString value("true");
	if(!q_value) value = "false";

	put(q_path, value);
}

void AgrSettings::print_as_info() {
	pt::info_parser::write_info(std::cout, _tree);
}

QStringList AgrSettings::as_string_list() {
	return _as_string_list(_tree, "");
}

QStringList AgrSettings::_as_string_list(pt::ptree &tree, QString tot_path) {
	QStringList to_ret;
	bool is_overlapping = _overlapping_keys.contains(tot_path);

	// check that the data has been set or that the path should be quoted. In the latter case, we have to
	// return the string even when this is empty
	bool to_be_quoted = _paths_to_be_quoted.contains(tot_path);
	if(!tree.data().empty() || to_be_quoted) {
		if(is_overlapping) {
			QString value = tot_path + " " + get_overlapping(tot_path);
			to_ret.push_back(value);
		}

		QString value = QString(tree.data().c_str());
		if(to_be_quoted) value = "\"" + value + "\"";
		value = tot_path + " " + value;
		to_ret.push_back(value);
	}
	// loop over all the children
	for(pt::ptree::iterator it = tree.begin(); it != tree.end(); it++) {
		QString child_data(it->first.c_str());
		// if the current node is overlapping and the current child is "state" then it is a fake
		// key used internally to cope with the duplicate issue
		if(is_overlapping && child_data == "state") continue;
		QString new_tot_path = (tot_path.size() == 0) ? child_data : tot_path + " " + child_data;
		to_ret.append(_as_string_list(it->second, new_tot_path));
	}

	return to_ret;
}

string AgrSettings::_translate_path(QString q_path) {
	q_path = q_path.trimmed();
	QStringList lst = q_path.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	// trim all the strings
	QStringList trimmed_list;
	foreach(QString s, lst) trimmed_list << s.trimmed();

	return trimmed_list.join(' ').toStdString();
}

template float AgrSettings::get<float>(QString);
template double AgrSettings::get<double>(QString);
template int AgrSettings::get<int>(QString);

SettingsMap::SettingsMap() {

}

SettingsMap::~SettingsMap() {

}

void SettingsMap::add_line(QString line) {
	line = line.trimmed();
	QString key = line.section("to", 0, 0);
	QString value = line.section("to", 1);

	QStringList key_lst = key.trimmed().split(QRegExp("\\s+"));
	QString type = key_lst[1].trimmed();
	int idx = key_lst[2].toInt();
	if(type == "color") {
		QRegularExpression re_colour("\\((\\d+), (\\d+), (\\d+)\\), (\".+\")$");
		QRegularExpressionMatch match = re_colour.match(value);
		if(match.hasMatch()) {
			int r = match.captured(1).toInt();
			int g = match.captured(2).toInt();
			int b = match.captured(3).toInt();
			QString name = match.captured(4);
			colour_pair new_colour(QColor(r, g, b), name);
			if(_colours.contains(idx)) qWarning() << "Overwriting colour map" << idx;
			_colours[idx] = new_colour;
		}
	}
	else if(type == "font") {
		QRegularExpression my_font("(\".+\"), (\".+\")$");
		QRegularExpressionMatch match = my_font.match(value);
		if(match.hasMatch()) {
			font_pair new_font(match.captured(1), match.captured(2));
			if(_fonts.contains(idx)) qWarning() << "Overwriting font map" << idx;
			_fonts[idx] = new_font;
		}
	}
	else {
		qCritical() << "Map type" << type << "unsupported";
		// TODO: to be removed
		exit(1);
	}
}

void SettingsMap::write_maps(QTextStream &ts) {
	foreach(int key, _colours.keys()) {
		colour_pair cp = _colours.value(key);
		QString color = QString("(%1, %2, %3)").arg(cp.first.red()).arg(cp.first.green()).arg(cp.first.blue());
		QString line = QString("@map color %1 to %2, %3\n").arg(key).arg(color).arg(cp.second);
		ts << line;
	}

	foreach(int key, _fonts.keys()) {
		font_pair fp = _fonts.value(key);
		QString line = QString("@map font %1 to %2, %3\n").arg(key).arg(fp.first).arg(fp.second);
		ts << line;
	}
}

QList<QColor> SettingsMap::colours() {
	QList<int> keys = _colours.keys();
	std::sort(keys.begin(), keys.end());

	QList<QColor> res;
	foreach(int key, keys) {
		res.push_back(_colours.value(key).first);
	}

	return res;
}

} /* namespace dg */
