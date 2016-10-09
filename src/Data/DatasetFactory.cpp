/*
 * DatasetFactory.cpp
 *
 *  Created on: 08 ott 2016
 *      Author: lorenzo
 */

#include "DatasetFactory.h"

#include <qt5/QtCore/qdebug.h>
#include <qt5/QtCore/qfile.h>

using namespace std;

namespace dg {

Dataset DatasetFactory::build_dataset(QString filename) {
	Dataset new_dataset;
	new_dataset.set_name(filename);

	QFile input(filename);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text)) qCritical() << "File" << filename << "is not readable";

	int n_columns = -1;
	int n_lines = 0;
	while(!input.atEnd()) {
		QString line = QString(input.readLine()).trimmed();
		if(line[0] != '#') {
			QStringList spl = line.split(' ');
			if(spl.size() > 0) {
				if(n_columns == -1) n_columns = spl.size();
				bool conv_ok = true;

				switch(n_columns) {
				case 1: {
					new_dataset.x.push_back(n_lines);
					double val = spl[0].toDouble(&conv_ok);
					new_dataset.y.push_back(val);
					break;
				}
				case 2:
				default: {
					double val = spl[0].toDouble(&conv_ok);
					new_dataset.x.push_back(val);
					val = spl[1].toDouble(&conv_ok);
					new_dataset.y.push_back(val);
				}
				}

				n_lines++;
			}
		}
	}

	input.close();

	return new_dataset;
}

DatasetFactory::~DatasetFactory() {

}
DatasetFactory::DatasetFactory() {

}

} /* namespace dg */
