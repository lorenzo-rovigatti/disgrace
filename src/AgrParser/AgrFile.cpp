/*
 * AgrFile.cpp
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#include "AgrFile.h"

#include <QRegularExpression>
#include <QFile>
#include <QDebug>

namespace dg {

AgrFile::AgrFile() {

}

AgrFile::~AgrFile() {

}

bool AgrFile::parse(QString filename) {
	_filename = filename;

	QRegularExpression rx_header_start("# Grace project file");
	QRegularExpression rx_header_stop("@timestamp def");
	QRegularExpression rx_region_start("@r(\\d+) (on|off)");
	QRegularExpression rx_object_start("@with (\\w+)");
	QRegularExpression rx_graph_start("@g(\\d+) (on|off)");
	QRegularExpression rx_dataset_start("@target G(\\d+).S(\\d+)");
	QRegularExpression rx_set_label("G(\\d+)S(\\d+)", QRegularExpression::CaseInsensitiveOption);
	QRegularExpression rx_page_size("@page\\s+size\\s+(\\d+),\\s*(\\d+)");
	QRegularExpression rx_font_size("(^@ \\s* default \\s+ char \\s+ size \\s+)(?P<val>.*)");

	QString state("opened");

	QFile fh(filename);
	if(!fh.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "File" << filename << "is not readable";
		return false;
	}

	int line_nr = 0;
	while(!fh.atEnd()) {
		QString line = QString(fh.readLine()).trimmed();
		line_nr += 1;

		if(line.size() == 0) continue;
		if(state == "opened") {

		}
		else if(state == "in_header") {

		}
		else if(state == "in_drawing_objects") {

		}
		else if(state == "in_regions") {

		}
		else if(state == "in_graphs") {

		}
		else if(state == "in_datasets") {

		}
		else {
			QString msg = QString("Could not parse line %1 of %2").arg(line_nr).arg(filename);
			qWarning() << msg;
		}
	}

	if(state == "opened") {
		qCritical() << "The file is missing the 'Grace project file' header.";
		// TODO: to be removed
		exit(1);
	}
}

} /* namespace dg */
