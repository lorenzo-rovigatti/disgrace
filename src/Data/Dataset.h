/*
 * Dataset.h
 *
 *  Created on: 06 ott 2016
 *      Author: lorenzo
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <QVector>
#include <QString>
#include <QObject>
#include <QMap>

namespace dg {

class Dataset: public QObject {
Q_OBJECT

private:
	QString _name;
	QString _type;
	// TODO: we should not keep a copy of these in each instance...
	QMap<QString, int> _type_to_n_column;
	QVector<QString> _implemented_types;

public:
	Dataset();
	virtual ~Dataset();

	QVector<double> x, y, z, dx, dy, dz;

	void set_name(QString name);
	void commit_data_changes();

	void append_agr_line(QString &line);

	QString name() { return _name; }
	bool empty() { return x.size() == 0; }

signals:
	void data_changed(Dataset *);
};

} /* namespace dg */

#endif /* SRC_DATASET_H_ */
