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

#include "../AgrObjects/AgrFile.h"
#include "Dataset.h"
#include "../Commands/Set.h"
#include "../qcustomplot/qcustomplot.h"

namespace dg {

class DataManager: public QAbstractTableModel {
Q_OBJECT;

friend class SetAppearanceCommand;

public:
	/**
	 * Stores all the possible model fields (name, legend, line style, etc.).
	 *
	 * The last item of this iterator is there for convenience, since it is automatically associated to the number of fields.
	 * This procedure is a bit hacky but it works, as long as no direct initialisation is given (such as Name = 5) and the
	 * FieldNumber remains the last element of the enumerator.
	 */
	enum Fields {
		Name, Legend,
		LineStyle, LineWidth, LineColour,
		SymbolType, SymbolSize, SymbolColour,
		NumberOfFields
	};

	DataManager(QCustomPlot *plot, QObject *parent = 0);
	virtual ~DataManager();

	void add_datasets_from_agr(AgrFile &agr_file);
	void add_datasets_from_file(QString filename);
	void emit_dataChanged(Dataset *dataset);

	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

public slots:
	void update_graph_data(Dataset *);
	bool submit() Q_DECL_OVERRIDE;

signals:
	/** Signals that some data modification, encapsulated in a QUndoCommand, has been performed.
	 *
	 * @param nc the command that should be added to the undo stack
	 */
	void new_command(QUndoCommand *nc);

private:
	SetAppearanceDetails _current_appearance(Dataset *dataset);
	QPen _next_pen();

	/**
	 * Creates a new plottable and associates it to the dataset passed in.
	 *
	 * This methods also updates the relevant data structures.
	 *
	 * @param dataset
	 * @return A pointer to the newly created plottable
	 */
	QCPCurve *_add_plottable(Dataset *dataset);

	QCustomPlot *_plot;
	QMap<Dataset *, QCPAbstractPlottable *> _datasets;
	QVector<Dataset *> _sorted_datasets;

	QPen _default_pen;
	QList<QColor> _default_colors;
	QList<QColor>::iterator _curr_color;

	SetAppearanceDetails _old_appearance;
	SetAppearanceDetails _new_appearance;
};

} /* namespace dg */

#endif /* SRC_DATA_DATAMANAGER_H_ */
