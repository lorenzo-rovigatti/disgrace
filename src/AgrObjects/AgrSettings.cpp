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
#include <QDebug>

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

void AgrSettings::put(QString line) {
	QString key, value;

	line = line.trimmed();
	// if the line contains a double quote then that's where the value starts
	int idx = line.indexOf("\"");
	// if the line contains a number then what's left of the number is interpreted as the key, the rest is the value
	if(idx == -1) idx = line.indexOf(QRegExp("\\d"));
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

	QString mystr(pt::ptree::path_type(_translate_path(q_path), ' ').dump().c_str());

	_tree.put(pt::ptree::path_type(_translate_path(q_path), ' '), value);
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

	// check that the data has been set. This call will return true even if data() == ""
	if(!tree.data().empty()) {
		QString value = QString(tree.data().c_str());
		if(_paths_to_be_quoted.contains(tot_path)) value = "\"" + value + "\"";
		value = tot_path + " " + value;
		to_ret.push_back(value);
	}
	// loop over all the children
	for(pt::ptree::iterator it = tree.begin(); it != tree.end(); it++) {
		QString child_data(it->first.c_str());
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

} /* namespace dg */
