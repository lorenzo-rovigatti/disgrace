/*
 * AgrSettings.h
 *
 *  Created on: 25 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRSETTINGS_H_
#define AGROBJECTS_AGRSETTINGS_H_

#include <boost/property_tree/ptree.hpp>
#include <QString>
#include <QStringList>
#include <string>

namespace pt = boost::property_tree;

namespace dg {

/**
 * @brief Thin wrapper around Boost's property tree.
 *
 * This class provides some convenice functions to manage the agr objects' configurations.
 */
class AgrSettings {
public:
	AgrSettings();
	virtual ~AgrSettings();

	void overwrite_settings_from(const AgrSettings &new_settings);
	void set_paths_to_be_quoted(QStringList path_list) { _paths_to_be_quoted = path_list; }
	/**
	 * @brief Put q_value as the value of the key pointed by q_path.
	 *
	 * @param q_path
	 * @param q_value
	 */
	void put(QString q_path, QString q_value);
	/**
	 * @brief Put the boolean q_value as the value of the key pointed by q_path.
	 *
	 * This is a convenience function that translates a boolean into a string (either "true" or "false") and
	 * writes it into the the settings tree.
	 *
	 * @param q_path
	 * @param q_value
	 */
	void put_bool(QString q_path, bool q_value);

	template<typename T> T get(QString q_path);
	QStringList as_string_list();

	void print_as_info();

private:
	pt::ptree _tree;
	QStringList _paths_to_be_quoted;

	QStringList _as_string_list(pt::ptree &tree, QString tot_path);
	std::string _translate_path(QString q_path);
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRSETTINGS_H_ */
