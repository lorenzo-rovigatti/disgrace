/*
 * AgrOption.h
 *
 *  Created on: 24 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGROPTION_H_
#define AGROBJECTS_AGROPTION_H_

#include <QString>
#include <QVector>
#include <QMap>

namespace dg {

/**
 * A horribly inefficient tree container, written to manage the settings associated with agr objects.
 */
class AgrOption {
public:
	AgrOption(AgrOption *parent, QString value);
	AgrOption(AgrOption *parent, QString value, QStringList lst);
	virtual ~AgrOption();

	void append_child_recursively(QStringList lst);
	
	void set_descendant(QString key, QString value);
	void set_descendant(QStringList lst, QString value);

	QString value_recursive_as_string(QString key);
	QStringList value_recursive(QString key);
	QStringList value_recursive(QStringList lst);
	QString value() { return _value; }
	QVector<AgrOption *> &children() { return _children; }
	QStringList as_string_list(QString prefix = "");

private:
	AgrOption *_parent;
	QString _value;
	QMap<QString, AgrOption *> _children_map;
	QVector<AgrOption *> _children;

	void _set_value(QString new_val);
};

} /* namespace dg */

#endif /* AGROBJECTS_AGROPTION_H_ */
