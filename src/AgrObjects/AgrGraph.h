/*
 * AgrGraph.h
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRGRAPH_H_
#define AGROBJECTS_AGRGRAPH_H_

#include <QString>
#include <QVector>
#include <QMap>
#include <QPair>
#include "../qcustomplot/qcustomplot.h"
#include "../Commands/defs.h"
#include "Dataset.h"
#include "AgrSettings.h"
#include "AgrDefaults.h"

namespace dg {

/**
 * Represents a plot in the xmgrace language.
 */
class AgrGraph: public QAbstractTableModel {
Q_OBJECT

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

	AgrGraph(QCustomPlot *plot);
	AgrGraph(QCustomPlot *plot, QString &line);
	virtual ~AgrGraph();

	void add_datasets_from_file(QString filename);
	void parse_agr_line(QString &line);
	void load_agr_settings();

	void write_headers(QTextStream &ts);
	void write_datasets(QTextStream &ts);
	/**
	 * @brief Removes this plot from the qcustomplot's layout
	 */
	void remove();

	void set_id(int n_id) { _id_graph = n_id; }

	int id() const { return _id_graph; }
	QList<Dataset *> datasets() const { return _datasets.values(); }
	bool empty() const { return _datasets.empty(); }
	Dataset *dataset(int d_id);
	void setup_new_datasets();

	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

	// getters and setters
	GraphRange graph_range() const;
	GraphRange current_graph_range() const;
	void set_graph_range(GraphRange &range);

	bool visible() const;
	void set_visible(bool is_visible);

	QString axis_label(QCPAxis *axis) const;
	void set_axis_label(QCPAxis *axis, QString new_label);

	bool axis_enabled(QCPAxis *axis) const;

public slots:
	bool submit() Q_DECL_OVERRIDE;
	void replot();

signals:
	/** Signals that some change, encapsulated in a QUndoCommand, has been performed.
	 *
	 * @param nc the command that should be added to the undo stack
	 */
	void new_command(QUndoCommand *nc);

protected:
	QCPAxisRect *_axis_rect;
	QMap<QCPAxis::AxisType, QString> _axis_names;
	QCustomPlot *_plot;
	QCPLegend *_legend;
	Dataset *_curr_dataset;

	QMap<int, Dataset *> _datasets;
	QVector<int> _sorted_datasets;
	QString _state;
	int _id_graph;

	SetAppearanceDetails _old_appearance;
	SetAppearanceDetails _new_appearance;

	AgrSettings _settings;

	void _new_dataset(int id);
	void _initialise_axis(QCPAxis *axis);
	void _initialise(QCustomPlot *plot);
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRGRAPH_H_ */
