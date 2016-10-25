/*
 * AgrGraph.cpp
 *
 *  Created on: 19 ott 2016
 *      Author: lorenzo
 */

#include "AgrGraph.h"

#include "../Commands/Set.h"

#include <algorithm>
#include <QRegularExpression>
#include <QDebug>

namespace dg {

AgrGraph::AgrGraph(QCustomPlot *plot) {
	_initialise(plot);
	_state = "";
}

AgrGraph::AgrGraph(QCustomPlot *plot, QString &line) {
	_initialise(plot);
	_lines.push_back(line);
	_state = "in_properties";

	QRegularExpression re_graph_start("@g(\\d+) (on|off)");
	QRegularExpressionMatch match = re_graph_start.match(line);
	if(match.hasMatch()) _id_graph = match.captured(1).toInt();
	else {
		qCritical() << "The AgrGraph header line" << line << "does not contain a valid graph id";
		// TODO: to be removed
		exit(1);
	}
}

AgrGraph::~AgrGraph() {
	delete _legend;
}

void AgrGraph::_initialise_axis(QCPAxis *axis) {
	axis->grid()->setPen(Qt::NoPen);
	axis->grid()->setZeroLinePen(Qt::NoPen);

	QFont font("Times New Roman", 18);

	axis->setBasePen(QPen(Qt::black, 2));
	axis->setLabelFont(font);
	axis->setTickLabelFont(font);

	// ticks
	axis->setTickLengthIn(14);
	axis->setTickPen(QPen(Qt::black, 2));

	// subticks
	axis->setSubTickLengthIn(7);
}

void AgrGraph::_initialise(QCustomPlot *plot) {
	_curr_dataset = NULL;
	_id_graph = -1;

	_plot = plot;
	_legend = NULL;
	_axis_rect = new QCPAxisRect(_plot, true);

	// create a top and a right axes, set their visibility to true and then connects their ranges to the bottom and left axes, respectively
	_axis_rect->setupFullAxesBox(true);
	foreach(QCPAxis *axis, _axis_rect->axes()) {
		_initialise_axis(axis);
	}

	// add the plot to the main widget
	_plot->plotLayout()->addElement(0, 0, _axis_rect);

	// create the legend
	_legend = new QCPLegend();
	// add the legend to the layout
	_axis_rect->insetLayout()->addElement(_legend, Qt::AlignTop|Qt::AlignRight);
	// set the placement of the legend (index 0 in the axis rect's inset layout) to not be border-aligned (default), but free, so we can reposition it anywhere:
	_axis_rect->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
	// setup the legend
	QFont legend_font("Timer New Roman", 14);
	_legend->setFont(legend_font);
	_legend->setVisible(true);
	_legend->setLayer("legend");
}

void AgrGraph::plot() {
	foreach(Dataset *dataset, _datasets.values()) {
		dataset->create_plottable(_axis_rect);
		_legend->addItem(new QCPPlottableLegendItem(_legend, dataset->plottable()));
	}
}

void AgrGraph::replot() {
	_plot->replot();
}

void AgrGraph::add_datasets_from_file(QString filename) {
	QFile input(filename);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not readable";
		// TODO: to be removed
	}

	int max_idx = (_sorted_datasets.empty()) ? -1 : *std::max_element(_sorted_datasets.constBegin(), _sorted_datasets.constEnd());

	while(!input.atEnd()) {
		max_idx++;
		_new_dataset(max_idx);
		_curr_dataset->init_from_file(input, "xy");
	}
}

void AgrGraph::_new_dataset(int id) {
	_curr_dataset = new Dataset();
	_curr_dataset->set_id(id);
	_datasets[id] = _curr_dataset;
	_sorted_datasets.push_back(id);

	connect(_curr_dataset, &Dataset::changed, this, &AgrGraph::replot);
}

void AgrGraph::parse_line(QString &line) {
	QRegularExpression re_set_start("@\\s*s(\\d+) hidden");

	QRegularExpressionMatch match = re_set_start.match(line);
	if(_state == "in_properties") {
		if(match.hasMatch()) {
			int set_id = match.captured(1).toInt();
			_new_dataset(set_id);
			_curr_dataset->append_header_line(line);

			_state = "in_sets";
		}
		else _lines.push_back(line);
	}
	else if(_state == "in_sets") {
		if(match.hasMatch()) {
			int set_id = match.captured(1).toInt();
			_new_dataset(set_id);
		}
		_curr_dataset->append_header_line(line);
	}
	else {
		qCritical() << "Could not parse graph line";
		// TODO: to be removed
		exit(1);
	}
}

void AgrGraph::write_headers(QTextStream &ts) {
	foreach(QString line, _lines) {
		ts << line << "\n";
	}

	foreach(Dataset *dataset, _datasets.values()) {
		dataset->write_headers(ts);
	}
}

void AgrGraph::write_datasets(QTextStream &ts) {
	foreach(Dataset *dataset, _datasets.values()) {
		dataset->write_dataset(ts);
	}
}

Dataset *AgrGraph::dataset(int d_id) {
	if(!_datasets.contains(d_id)) {
		qCritical() << "Graph" << _id_graph;
		// TODO: to be removed
		exit(1);
	}

	return _datasets[d_id];
}

int AgrGraph::rowCount(const QModelIndex& parent) const {
	return _datasets.size();
}

int AgrGraph::columnCount(const QModelIndex& parent) const {
	return NumberOfFields;
}

Qt::ItemFlags AgrGraph::flags(const QModelIndex &index) const {
	if (!index.isValid()) return Qt::ItemIsEnabled;

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant AgrGraph::data(const QModelIndex& index, int role) const {
	if(!index.isValid()) return QVariant();
	if(index.row() >= _datasets.size() || index.row() < 0) return QVariant();

	int n_set = index.row();
	QVariant res;
	QCPCurve *graph = static_cast<QCPCurve *>(_datasets[_sorted_datasets[n_set]]->plottable());

	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		switch(index.column()) {
		case Name:
			res = tr("Set %1 [%2]").arg(n_set).arg(graph->dataCount());
			break;
		case Legend:
			res = graph->name();
			break;
		case LineStyle:
			res = QString::number(graph->pen().style());
			break;
		case LineWidth:
			res = graph->pen().width();
			break;
		case LineColour:
			res = graph->pen().color().name();
			break;
		case SymbolType:
			res = QString::number(graph->scatterStyle().shape());
			break;
		case SymbolSize:
			res = graph->scatterStyle().size();
			break;
		case SymbolColour:
			res = graph->scatterStyle().pen().color().name();
			break;
		default:
			qCritical() << "The AgrGraph model has only" << columnCount() <<  "columns";
		}
	}

	return res;
}

bool AgrGraph::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(index.isValid() && role == Qt::EditRole) {
		if(index.row() >= _datasets.size() || index.row() < 0) return false;
		int n_set = index.row();
		Dataset *dataset = _datasets[_sorted_datasets[n_set]];
		QCPCurve *graph = static_cast<QCPCurve *>(dataset->plottable());

		if(dataset != _new_appearance.dataset) _old_appearance = _new_appearance = dataset->appearance();

		switch(index.column()) {
		case Name:
			break;
		case Legend:
			_new_appearance.legend = value.toString();
			break;
		case LineStyle:
			// we need to explicitly set the old appearance's pen properties because of QPen implicit sharing facilities
			_old_appearance.line_pen.setStyle(graph->pen().style());
			_new_appearance.line_pen.setStyle((Qt::PenStyle) value.toString().toInt());
			break;
		case LineWidth:
			_old_appearance.line_pen.setWidth(graph->pen().width());
			_new_appearance.line_pen.setWidth(value.toInt());
			break;
		case LineColour:
			_old_appearance.line_pen.setColor(graph->pen().color());
			_new_appearance.line_pen.setColor(QColor(value.toString()));
			break;
		case SymbolType:
			_old_appearance.symbol_type = graph->scatterStyle().shape();
			_new_appearance.symbol_type = (QCPScatterStyle::ScatterShape) value.toString().toInt();
			break;
		case SymbolSize:
			_old_appearance.symbol_size = graph->scatterStyle().size();
			_new_appearance.symbol_size = value.toInt();
			break;
		case SymbolColour:
			_old_appearance.symbol_pen.setColor(graph->scatterStyle().pen().color());
			_new_appearance.symbol_pen.setColor(QColor(value.toString()));
			break;
		default:
			qCritical() << "The AgrGraph model has only" << columnCount() <<  "columns";
			return false;
		}

		return true;
	}

	return false;
}

bool AgrGraph::submit() {
	SetAppearanceCommand *nc = new SetAppearanceCommand(_old_appearance, _new_appearance);
	emit new_command(nc);

	return true;
}

} /* namespace dg */
