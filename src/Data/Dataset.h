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
#include "../qcustomplot/qcustomplot.h"

namespace dg {

struct SetAppearanceDetails;

class Dataset: public QObject {
Q_OBJECT

public:
	Dataset();
	virtual ~Dataset();

	QVector<double> x, y, z, dx, dy, dz;

	void set_type(QString type);
	void set_name(QString name);
	void set_id(int n_id);
	void set_appearance(SetAppearanceDetails &new_appearance);

	SetAppearanceDetails appearance();
	void create_plottable(QCPAxisRect *axis_rect);

	void append_header_line(QString line);
	void append_agr_line(QString line);
	void init_from_file(QFile &input, QString type);

	void write_headers(QTextStream &ts);
	void write_dataset(QTextStream &ts);

	QString name() { return _name; }
	bool empty() { return x.empty(); }
	int id() { return _id_dataset; }
	QCPAbstractPlottable *plottable() { return _plottable; }

signals:
	void changed(Dataset *);

private:
	QString _name;
	QString _type;
	/// id of the dataset. It is unique on a per-graph basis.
	int _id_dataset;
	/// id of the dataset, as found in the header lines of the agr file. If the dataset was not created from an agr then its value is -1.
	int _id_header;
	QVector<QString> _header_lines;
	/// associated QCustomPlot plottable
	QCPAbstractPlottable *_plottable;
	QPen _default_pen;

	// TODO: we should not keep a copy of these in each instance...
	QMap<QString, int> _type_to_n_column;
	QVector<QString> _implemented_types;

	QPen _pen();
	void _set_plottable_data();
	void _check_type(QString type);
};

} /* namespace dg */

#endif /* SRC_DATASET_H_ */
