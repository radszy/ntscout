#include "mainwindow.h"
#include "settings.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QThread>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QStringList args;
	for (int i = 0; i < argc; ++i) {
		args.append(argv[i]);
	}

	QCommandLineParser parser;
	QCommandLineOption postUpdateOption("post-update");
	parser.addOption(postUpdateOption);
	parser.parse(args);
	bool isPostUpdate = parser.isSet("post-update");
	if (isPostUpdate) {
		QThread::msleep(100);
		QFile oldu("Updater.exe"), newu("Updater.exe-new");
		if (oldu.exists() && newu.exists()) {
			oldu.remove();
			newu.rename("Updater.exe");
		}
	}

	Settings::read();

	MainWindow w;
	w.show();

	return a.exec();
}
