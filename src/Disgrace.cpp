/*
 * Disgrace.cpp
 *
 *  Created on: 12 nov 2016
 *      Author: lorenzo
 */

#include "Disgrace.h"

#include <QCommandLineParser>
#include <QIcon>
#include <iostream>

namespace dg {

Disgrace::Disgrace(int &argc, char **argv): QApplication(argc, argv), _window(NULL) {
	setOrganizationName("disgrace");
	setApplicationName("disgrace");
	setApplicationVersion("alpha");
	setWindowIcon(QIcon(":/images/icons/disgrace_icon.png"));

	QCommandLineParser parser;
	_setup_parser(parser);
	parser.process(*this);

	dg::AgrDefaults::init_defaults();

	_window = new dg::MainWindow(&parser);
	_window->show();
}

Disgrace::~Disgrace() {
	if(_window != NULL) delete _window;
}

void Disgrace::_setup_parser(QCommandLineParser &parser) {
	parser.setApplicationDescription(QApplication::translate("main", "A qt5-based plotting tool inspired by xmgrace"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file(s)", QApplication::translate("main", "Text file(s) containing the data to be plotted"));
}

bool Disgrace::notify(QObject *receiver, QEvent *event) {
	try {
		return QApplication::notify(receiver, event);
	}
	catch(std::exception& e) {
		QMessageBox::warning(_window, tr("Warning"), e.what());
		return false;
	}
}

void Disgrace::die_gracefully() {
	QString filename = QString("disgrace_%1.agr").arg(applicationPid());
	std::cerr << "disgrace crashed, saving the plot in file '" << filename.toStdString() << "'" << std::endl;
	_window->write_to_agr(filename);
}

} /* namespace dg */
