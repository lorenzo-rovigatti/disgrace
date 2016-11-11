/*
 * SettingsMap.h
 *
 *  Created on: 11 Nov 2016
 *      Author: rovigattil
 */

#ifndef SRC_AGROBJECTS_SETTINGSMAP_H_
#define SRC_AGROBJECTS_SETTINGSMAP_H_

#include <QMap>
#include <QList>
#include <QColor>
#include <QTextStream>

namespace dg {

template<typename T>
class SMap {
	typedef QPair<T, QString> s_pair;

public:
	SMap();
	~SMap();

	void set(int idx, const T &first, const QString &second);
	s_pair get_by_idx(int idx);
	QList<T> values();

	void write_to(QTextStream &ts);

private:
	QMap<int, s_pair> _map;
};

/**
 * @brief Encapsulates "@map font/color ID to VALUE, "name"" lines.
 */
class SettingsMap {
	typedef QPair<QColor, QString> colour_pair;
	typedef QPair<QString, QString> font_pair;

public:
	SettingsMap();
	~SettingsMap();

	void add_line(QString line);
	void write_maps(QTextStream &ts);

	QList<QColor> colours();
private:
	SMap<QColor> _colours;
	SMap<QString> _fonts;
//	QMap<int, colour_pair> _colours;
//	QMap<int, font_pair> _fonts;
};

} /* namespace dg */

#endif /* SRC_AGROBJECTS_SETTINGSMAP_H_ */
