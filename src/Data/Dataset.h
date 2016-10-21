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
#include <QFile>
#include <QPair>

namespace dg {

class Dataset: public QObject {
Q_OBJECT

private:
	QString _name;
	QString _type;
	QPair<int, int> _g_s;
	// TODO: we should not keep a copy of these in each instance...
	QMap<QString, int> _type_to_n_column;
	QVector<QString> _implemented_types;

public:
	Dataset();
	virtual ~Dataset();

	QVector<double> x, y, z, dx, dy, dz;

	void set_type(QString type);
	void set_name(QString name);
	void set_g_s(int g, int s);
	void commit_data_changes();

	void append_agr_line(QString line);
	void init_from_file(QFile &input, QString type);

	QString name() { return _name; }
	bool empty() { return x.size() == 0; }
	QPair<int, int> g_s() { return _g_s; }

signals:
	void data_changed(Dataset *);
};

} /* namespace dg */

#endif /* SRC_DATASET_H_ */
