/*
 * AgrSettings.h
 *
 *  Created on: 25 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRSETTINGS_H_
#define AGROBJECTS_AGRSETTINGS_H_

#include <boost/property_tree/ptree.hpp>
#include <QColor>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QMap>
#include <string>

namespace pt = boost::property_tree;

namespace dg {

/**
 * @brief Thin wrapper around Boost's property tree.
 *
 * This class provides some convenience functions to manage the agr objects' configurations.
 */
class AgrSettings {
public:
	AgrSettings();
	virtual ~AgrSettings();

	void overwrite_settings_from(const AgrSettings &new_settings);
	void set_paths_to_be_quoted(QStringList path_list) { _paths_to_be_quoted = path_list; }
	void set_overlapping_keys(QStringList key_list) { _overlapping_keys = key_list; }
	/**
	 * @brief Put q_value as the value of the key pointed by q_path.
	 *
	 * @param q_path
	 * @param q_value
	 */
	void put(QString q_path, QString q_value);
	/**
	 * @brief Split line in a key/value pair and add them to the settings tree.
	 *
	 * @param line
	 */
	void put(QString line);
	/**
	 * @brief Put the boolean q_value as the value of the key pointed by q_path.
	 *
	 * This is a convenience function that translates a boolean into a string (either "true" or "false") and
	 * writes it into the the settings tree. A string different from "false" is always written as "true".
	 *
	 * @param q_path
	 * @param q_value
	 */
	void put_bool(QString q_path, bool q_value);

	template<typename T> T get(QString q_path);
	QString get_overlapping(QString q_path);
	QStringList as_string_list();

	void print_as_info();

private:
	pt::ptree _tree;
	QStringList _paths_to_be_quoted;
	/// List of keys that have more than one associated value, one of which should be either on or off
	QStringList _overlapping_keys;

	QStringList _as_string_list(pt::ptree &tree, QString tot_path);
	/**
	 * @brief Sanitises the given path.
	 *
	 * q_path is first split into substrings wherever whitespace occurs, and then reconstructed after having trimmed all the substrings.
	 *
	 * @param q_path
	 * @return
	 */
	std::string _translate_path(QString q_path);
};

/**
 * @brief Incapsulates "@map font/color ID to VALUE, "name"" lines.
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
	QMap<int, colour_pair> _colours;
	QMap<int, font_pair> _fonts;
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRSETTINGS_H_ */
