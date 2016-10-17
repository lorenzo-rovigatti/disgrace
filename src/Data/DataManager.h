/*
 * DataManager.h
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#ifndef SRC_DATA_DATAMANAGER_H_
#define SRC_DATA_DATAMANAGER_H_

#include <QAbstractTableModel>
#include <QMap>

#include "Dataset.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

struct SetAppearanceDetails {
	int idx;
	QString legend;
};

}

// This is needed if we want to be able to store a SetAppearanceDetails object in a QVariant
Q_DECLARE_METATYPE(dg::SetAppearanceDetails)

namespace dg {

class DataManager: public QAbstractTableModel {
Q_OBJECT;

public:
	DataManager(QCustomPlot *plot, QObject *parent = 0);
	virtual ~DataManager();

	void add_datasets_from_file(QString filename, bool rescale_x, bool rescale_y);

	int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

public slots:
	void update_graph_data(Dataset *);

private:
	Dataset *_parse_next_dataset(QFile &input);
	QPen _get_next_pen();

	QCustomPlot *_plot;
	QMap<Dataset *, QCPAbstractPlottable *> _datasets;
	QVector<Dataset *> _sorted_datasets;

	QPen _default_pen;
	QList<QColor> _default_colors;
	QList<QColor>::iterator _curr_color;
};

} /* namespace dg */

#endif /* SRC_DATA_DATAMANAGER_H_ */
