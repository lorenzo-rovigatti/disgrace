/*
 * Disgrace.h
 *
 *  Created on: 12 nov 2016
 *      Author: lorenzo
 */

#ifndef SRC_DISGRACE_H_
#define SRC_DISGRACE_H_

#include "MainWindow.h"

#include <QApplication>

namespace dg {

class Disgrace: public QApplication {
public:
	Disgrace(int &argc, char **argv);
	virtual ~Disgrace();

	bool notify(QObject *receiver, QEvent *event);

	void die_gracefully();

private:
	dg::MainWindow *_window;

	void _setup_parser(QCommandLineParser &parser);
};

} /* namespace dg */

#endif /* SRC_DISGRACE_H_ */
