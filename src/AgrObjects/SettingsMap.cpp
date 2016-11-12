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
int SMap<T>::idx_by_pair(const T &first, const QString &second) {
	int res = -1;
	foreach(int idx, _map.keys()) {
		s_pair curr_pair = _map.value(idx);
		if(first == curr_pair.first) {
			res = idx;
			break;
		}
	}

	if(res == -1) {
		// find the next available index
		int res = 0;
		while(_map.contains(res)) res++;
		set(res, first, second);
	}

	return res;
}

template<typename T>
QPair<T, QString> SMap<T>::pair_by_idx(int idx) const {
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
		}
	}
	else if(type == "font") {
		QRegularExpression my_font("(\".+\"), (\".+\")$");
		QRegularExpressionMatch match = my_font.match(value);
		if(match.hasMatch()) {
			_fonts.set(idx, match.captured(1), match.captured(2));
		}
	}
	else {
		qCritical() << "Map type" << type << "unsupported";
		// TODO: to be removed
		exit(1);
	}
}

int SettingsMap::idx_by_colour(QColor colour) {
	return _colours.idx_by_pair(colour, colour.name());
}

QColor SettingsMap::colour_by_idx(int idx) const {
	return _colours.pair_by_idx(idx).first;
}

int SettingsMap::colour_idx_by_dataset_idx(int idx) {
	QList<QColor> all_colours = colours();
	// we remove the first colour (which, by default, is white)
	all_colours.takeFirst();
	qDebug() << idx;
	idx = idx % all_colours.size();
	QColor new_colour = all_colours[idx];
	return idx_by_colour(new_colour);
}

void SettingsMap::write_maps(QTextStream &ts) {
	_colours.write_to(ts);
	_fonts.write_to(ts);
}

QList<QColor> SettingsMap::colours() {
	return _colours.values();
}

} /* namespace dg */
