/*
 * SettingsMap.cpp
 *
 *  Created on: 11 Nov 2016
 *      Author: rovigattil
 */

#include "SettingsMap.h"

#include <QRegularExpression>
#include <QDebug>

namespace dg {

template<typename T>
SMap<T>::SMap() {

}

template<typename T>
SMap<T>::~SMap() {

}

template<typename T>
void SMap<T>::set(int idx, const T &first, const QString &second) {
	if(_map.contains(idx)) qWarning() << "Overwriting map" << idx;

	s_pair np(first, second);
	_map[idx] = np;
}

template<typename T>
QPair<T, QString> SMap<T>::get_by_idx(int idx) {
	if(!_map.contains(idx)) {
		QString excp = QString("This map does not have the index %1").arg(idx);
		throw excp;
	}

	return _map.value(idx);
}

template<typename T>
QList<T> SMap<T>::values() {
	QList<int> keys = _map.keys();
	std::sort(keys.begin(), keys.end());

	QList<T> res;
	foreach(int key, keys) {
		res.push_back(_map.value(key).first);
	}

	return res;
}

template<>
void SMap<QColor>::write_to(QTextStream &ts) {
	foreach(int key, _map.keys()) {
		s_pair cp = _map.value(key);
		QString color = QString("(%1, %2, %3)").arg(cp.first.red()).arg(cp.first.green()).arg(cp.first.blue());
		QString line = QString("@map color %1 to %2, %3\n").arg(key).arg(color).arg(cp.second);
		ts << line;
	}
}

template<>
void SMap<QString>::write_to(QTextStream &ts) {
	foreach(int key, _map.keys()) {
		s_pair fp = _map.value(key);
		QString line = QString("@map font %1 to %2, %3\n").arg(key).arg(fp.first).arg(fp.second);
		ts << line;
	}
}

template class SMap<QString>;
template class SMap<QColor>;

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
			_colours.set(idx, QColor(r, g, b), name);
//			colour_pair new_colour(QColor(r, g, b), name);
//			if(_colours.contains(idx)) qWarning() << "Overwriting colour map" << idx;
//			_colours[idx] = new_colour;
		}
	}
	else if(type == "font") {
		QRegularExpression my_font("(\".+\"), (\".+\")$");
		QRegularExpressionMatch match = my_font.match(value);
		if(match.hasMatch()) {
			_fonts.set(idx, match.captured(1), match.captured(2));
//			font_pair new_font(match.captured(1), match.captured(2));
//			if(_fonts.contains(idx)) qWarning() << "Overwriting font map" << idx;
//			_fonts[idx] = new_font;
		}
	}
	else {
		qCritical() << "Map type" << type << "unsupported";
		// TODO: to be removed
		exit(1);
	}
}

void SettingsMap::write_maps(QTextStream &ts) {
	_colours.write_to(ts);
	_fonts.write_to(ts);
//	foreach(int key, _colours.keys()) {
//		colour_pair cp = _colours.value(key);
//		QString color = QString("(%1, %2, %3)").arg(cp.first.red()).arg(cp.first.green()).arg(cp.first.blue());
//		QString line = QString("@map color %1 to %2, %3\n").arg(key).arg(color).arg(cp.second);
//		ts << line;
//	}
//
//	foreach(int key, _fonts.keys()) {
//		font_pair fp = _fonts.value(key);
//		QString line = QString("@map font %1 to %2, %3\n").arg(key).arg(fp.first).arg(fp.second);
//		ts << line;
//	}
}

QList<QColor> SettingsMap::colours() {
	return _colours.values();
//	QList<int> keys = _colours.keys();
//	std::sort(keys.begin(), keys.end());
//
//	QList<QColor> res;
//	foreach(int key, keys) {
//		res.push_back(_colours.value(key).first);
//	}
//
//	return res;
}

} /* namespace dg */
