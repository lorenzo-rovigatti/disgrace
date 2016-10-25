/*
 * AgrOption.cpp
 *
 *  Created on: 24 ott 2016
 *      Author: lorenzo
 */

#include "AgrOption.h"

#include <QDebug>

namespace dg {

AgrOption::AgrOption(AgrOption *parent, QString value): _parent(parent) {
	_set_value(value);
}

AgrOption::AgrOption(AgrOption *parent, QString value, QStringList lst): _parent(parent) {
	_set_value(value);
	append_child_recursively(lst);
}

AgrOption::~AgrOption() {
	foreach(AgrOption *opt, _children) delete opt;
}

void AgrOption::_set_value(QString new_val) {
	new_val = new_val.trimmed();
	if(_parent != NULL) {
		_parent->_children_map.remove(value());
		_parent->_children_map[new_val] = this;
	}
	_value = new_val;
}

void AgrOption::set_descendant(QString key, QString value) {
	QStringList lst = key.split("\\s+");
	set_descendant(lst, value);
}

void AgrOption::set_descendant(QStringList lst, QString value) {
	if(lst.empty()) {
		qCritical() << "Tragedia";
		// TODO: to be removed
		exit(1);
	}
	QString key = lst.at(0).trimmed();
	lst.pop_front();

	if(_children_map.contains(key)) {
		if(lst.size() == 0) _children_map[key]->_set_value(value);
		else _children_map[key]->set_descendant(lst, value);
	}
}

QString AgrOption::value_recursive_as_string(QString key) {
	QStringList lst = value_recursive(key);
	return lst.join(' ');
}

QStringList AgrOption::value_recursive(QString key) {
	QStringList lst = key.split(QRegExp("\\s+"));
	return value_recursive(lst);
}

QStringList AgrOption::value_recursive(QStringList lst) {
	if(lst.empty()) {
		QStringList res = as_string_list("");
		// here we remove the value of the current option, as well as trailing spaces, from all the strings
		return res.replaceInStrings(QRegExp(value() + "\\s+"), "");
	}

	QString key = lst.at(0).trimmed();
	if(_children_map.contains(key)) {
		AgrOption *opt = _children_map[key];
		lst.pop_front();
		return opt->value_recursive(lst);
	}

	return QStringList();
}

void AgrOption::append_child_recursively(QStringList lst) {
	if(lst.empty()) return;

	QString key = lst.at(0).trimmed();
	lst.pop_front();
	if(_children_map.contains(key)) _children_map[key]->append_child_recursively(lst);
	else {
		AgrOption *new_opt = new AgrOption(this, key, lst);
		_children.push_back(new_opt);
	}
}

QStringList AgrOption::as_string_list(QString prefix) {
	QString new_prefix = (prefix.size() == 0) ? value() : prefix + " " + value();
	QStringList to_ret;
	foreach(AgrOption *opt, _children) to_ret.append(opt->as_string_list(new_prefix));
	if(to_ret.empty()) to_ret.push_back(new_prefix);

	return to_ret;
}

} /* namespace dg */
