/*
 * DataManager.cpp
 *
 *  Created on: 12 Oct 2016
 *      Author: rovigattil
 */

#include "DataManager.h"

namespace dg {

DataManager::DataManager(QCustomPlot *plot, QObject *parent):
		QAbstractTableModel(parent),
		_plot(plot) {
	_default_pen.setWidth(2);

	_default_colors.push_back(Qt::black);
	_default_colors.push_back(Qt::red);
	_default_colors.push_back(Qt::green);
	_default_colors.push_back(Qt::blue);
	_default_colors.push_back(QColor("orange"));
	_default_colors.push_back(QColor("brown"));
	_default_colors.push_back(QColor("magenta"));

	_curr_color = _default_colors.begin();
	_new_appearance.dataset = NULL;
}

DataManager::~DataManager() {
	QMap<Dataset *, QCPAbstractPlottable *>::iterator it = _datasets.begin();
	while(it != _datasets.end()) {
		delete it.key();
		++it;
	}
}

void DataManager::add_datasets_from_agr(AgrFile &agr_file) {
	foreach(Dataset *curr_dataset, agr_file.datasets()) {
		_add_plottable(curr_dataset);
	}
}

void DataManager::add_datasets_from_file(QString filename) {
	QFile input(filename);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not readable";
		return;
	}

	while(!input.atEnd()) {
		Dataset *curr_dataset = new Dataset();
		curr_dataset->init_from_file(input, "xy");
		_add_plottable(curr_dataset);
	}
}

void DataManager::update_graph_data(Dataset *ds) {
	if(_datasets.contains(ds)) {
		QCPCurve *associated_graph = static_cast<QCPCurve *>(_datasets.value(ds));
		if(!associated_graph) {
			qCritical() << "The QCPAbstractPlottable pointer associated to a dataset does not point to a QCPCurve object";
			return;
		}

		associated_graph->setData(ds->x, ds->y);
		_plot->replot();
	}
}

int DataManager::rowCount(const QModelIndex& parent) const {
	return _datasets.size();
}

int DataManager::columnCount(const QModelIndex& parent) const {
	return NumberOfFields;
}

Qt::ItemFlags DataManager::flags(const QModelIndex &index) const {
	if (!index.isValid()) return Qt::ItemIsEnabled;

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant DataManager::data(const QModelIndex& index, int role) const {
	if(!index.isValid()) return QVariant();
	if(index.row() >= _datasets.size() || index.row() < 0) return QVariant();

	int n_set = index.row();
	QVariant res;
	QCPCurve *graph = static_cast<QCPCurve *>(_datasets[_sorted_datasets[n_set]]);

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
			qCritical() << "The DataManager model has only" << columnCount() <<  "columns";
		}
	}

	return res;
}

bool DataManager::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(index.isValid() && role == Qt::EditRole) {
		if(index.row() >= _datasets.size() || index.row() < 0) return false;
		int n_set = index.row();
		Dataset *dataset = _sorted_datasets[n_set];
		QCPCurve *graph = static_cast<QCPCurve *>(_datasets[dataset]);

		if(dataset != _new_appearance.dataset) _old_appearance = _new_appearance = _current_appearance(dataset);

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
			qCritical() << "The DataManager model has only" << columnCount() <<  "columns";
		}

		return true;
	}

	return false;
}

void DataManager::emit_dataChanged(Dataset *dataset) {
	int n_set = _sorted_datasets.indexOf(dataset);
	if(n_set == -1) qCritical() << "The dataset involved in the change cannot be found in the list of datasets";
	else {
		QModelIndex index = createIndex(n_set, 0);
		emit dataChanged(index, index);
	}
}

bool DataManager::submit() {
	SetAppearanceCommand *nc = new SetAppearanceCommand(this, _old_appearance, _new_appearance);
	emit new_command(nc);

	return true;
}

SetAppearanceDetails DataManager::_current_appearance(Dataset *dataset) {
	QCPCurve *graph = static_cast<QCPCurve *>(_datasets[dataset]);

	SetAppearanceDetails res;
	res.dataset = dataset;
	res.legend = graph->name();
	res.line_pen = graph->pen();

	res.symbol_type = graph->scatterStyle().shape();
	res.symbol_size = graph->scatterStyle().size();
	res.symbol_pen = graph->scatterStyle().pen();

	return res;
}

QPen DataManager::_next_pen() {
	QPen new_pen(_default_pen);
	new_pen.setColor(*_curr_color);
	_curr_color++;
	if(_curr_color == _default_colors.end()) _curr_color = _default_colors.begin();

	return new_pen;
}

QCPCurve *DataManager::_add_plottable(Dataset *dataset) {
	QCPCurve *new_graph = new QCPCurve(_plot->xAxis, _plot->yAxis);
	QPen next_pen = _next_pen();
	new_graph->setPen(next_pen);
	new_graph->setData(dataset->x, dataset->y);
	new_graph->setName(dataset->name());
	new_graph->addToLegend();

	QCPScatterStyle ss = new_graph->scatterStyle();
	ss.setPen(next_pen);
	new_graph->setScatterStyle(ss);
	// TODO: this is needed because of a bug in qcustomplot (QCPCurve::QCPCurve() does not initialise the scatter skip)
	new_graph->setScatterSkip(0);

	QObject::connect(dataset, &Dataset::data_changed, this, &DataManager::update_graph_data);
	_datasets[dataset] = new_graph;
	_sorted_datasets.push_back(dataset);

	return new_graph;
}

} /* namespace dg */
