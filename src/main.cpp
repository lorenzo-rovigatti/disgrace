#include "MainWindow.h"
#include "AgrObjects/AgrDefaults.h"

#include <iostream>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>

void setup_parser(QCommandLineParser &parser) {
	parser.setApplicationDescription(QApplication::translate("main", "A qt5-based plotting tool inspired by xmgrace"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file(s)", QApplication::translate("main", "Text file(s) containing the data to be plotted"));
}

/**
 * Logs the message msg, also adding the message type, timestamp and the filename, function and line which called the logger.
 *
 * @param type
 * @param context
 * @param msg
 */
void message_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	static const char* typeStr[] = { "[   Debug]", "[ Warning]", "[Critical]", "[   Fatal]" };

	if(type <= QtFatalMsg) {
		QByteArray localMsg = msg.toLocal8Bit();
		QString filename = QFileInfo(context.file).fileName();

		QString contextString(QStringLiteral("(%1, %2, %3)").arg(filename)
		.arg(context.function)
		.arg(context.line));

		QString timeStr(QDateTime::currentDateTime().toString("dd-MM-yy HH:mm:ss:zzz"));

		std::cerr << timeStr.toLocal8Bit().constData() << " - " << typeStr[type] << " " << contextString.toLocal8Bit().constData() << " " << localMsg.constData() << std::endl;

		if(type == QtFatalMsg) abort();
	}
}

int main(int argc, char *argv[]) {
	qInstallMessageHandler(message_handler);

	QApplication app(argc, argv);
	QApplication::setOrganizationName("disgrace");
	QApplication::setApplicationName("disgrace");
	QApplication::setApplicationVersion("alpha");

	QCommandLineParser parser;
	setup_parser(parser);
	parser.process(app);

	dg::AgrDefaults::init_defaults();

	dg::MainWindow window(&parser);

	window.show();

	// this code gives an idea of what needs to be done to directly export to a file without showing the window (or showing the window for as little time as possible)
//	window.write_to_pdf("/home/rovigattil/prova.pdf");
//	window.close();
//	QWidgetList ql = app.topLevelWidgets();
//	foreach(QWidget *w, ql) {
//		qDebug() << w->metaObject()->className();
//		QMenu *menu = static_cast<QMenu *>(w);
//		if(menu) {
//			qDebug() << menu->title() << menu->isVisible();
//			menu->close();
//		}
//	}
//	app.quit();
//	printf("N %d\n", ql.size());

	return app.exec();
}
