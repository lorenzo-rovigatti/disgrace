#include <iostream>

#include <qt5/QtWidgets/qapplication.h>
#include <qt5/QtCore/qcommandlineparser.h>
#include <qt5/QtCore/qdebug.h>
#include <qt5/QtCore/qdatetime.h>
#include <qt5/QtCore/qfileinfo.h>

#include "MainWindow.h"
#include "Data/DatasetFactory.h"

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
	QApplication::setApplicationName("disgrace");
	QApplication::setApplicationVersion("0.1");

	QCommandLineParser parser;
	setup_parser(parser);
	parser.process(app);

	MainWindow window;

	const QStringList args = parser.positionalArguments();
	qDebug() << "Passed in" << args.size() << "file(s)";

	QString filename;
	foreach(filename, args) {
		dg::Dataset new_dataset = dg::DatasetFactory::build_dataset(filename);
		window.add_plot(new_dataset);
	}

	window.show();

	return app.exec();
}
