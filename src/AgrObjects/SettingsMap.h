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
	int idx_by_pair(const T &first, const QString &second);
	s_pair pair_by_idx(int idx) const;
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

	/**
	 * @brief Given the colour, returns the associated index (after having added the colour to the map, if not previously present)
	 *
	 * @param colour
	 * @return the map index associated to the given colour
	 */
	int idx_by_colour(QColor colour);
	/**
	 * @brief Given an index, returns the associated color
	 *
	 * @param idx
	 * @return the colour associated to the given index
	 */
	QColor colour_by_idx(int idx) const;
	/**
	 * @brief Returns a colour index according to the given dataset index.
	 * @param idx dataset index
	 * @return colour index
	 */
	int colour_idx_by_dataset_idx(int idx);

	QList<QColor> colours();
private:
	SMap<QColor> _colours;
	SMap<QString> _fonts;
};

} /* namespace dg */

#endif /* SRC_AGROBJECTS_SETTINGSMAP_H_ */
