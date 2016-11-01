/*
 * AgrFile.h
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#ifndef AGROBJECTS_AGRFILE_H_
#define AGROBJECTS_AGRFILE_H_

#include <QString>
#include <QVector>
#include <QMap>
#include <QRegularExpression>

#include "AgrDrawingObject.h"
#include "AgrGraph.h"
#include "AgrRegion.h"
#include "AgrSettings.h"
#include "Dataset.h"

namespace dg {

class AgrFile {
public:
	AgrFile(QCustomPlot *plot);
	virtual ~AgrFile();

	bool parse_agr(QString filename);
	void parse_text(QString filename, int graph_id = 0);
	QList<AgrGraph *> graphs() { return _graphs.values(); }
	QList<Dataset *> datasets(int graph_id = 0);
	void write_to(QString filename);

private:
	QVector<QString> _header_lines;
	QMap<int, AgrGraph *> _graphs;
	QVector<AgrRegion> _regions;
	QVector<AgrDrawingObject> _drawing_objects;
	QString _filename;
	QCustomPlot *_plot;

	// needed in the parsing stage
	QRegularExpressionMatch _last_match;
	AgrGraph *_curr_graph;
	Dataset *_curr_dataset;

	AgrSettings _settings;
	SettingsMap _settings_map;

	void _add_agr_graph(int graph_id, QString line);
	void _add_header_line(QString line);
	bool _has_match(QRegularExpression &re, QString &str);
	void _check_consistency();
	void _setup_colours();
};

} /* namespace dg */

#endif /* AGROBJECTS_AGRFILE_H_ */
