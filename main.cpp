#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include "ScanControl.h"

void myMessageOutput(QtMsgType type, const char *msg) {
	switch (type) {
		case QtDebugMsg:
			fprintf(stdout, "%s\n", msg);
			break;
		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n", msg);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg);
			abort();
	}
}

int main(int argc, char **argv) {
	uint thread_count = 16;
	qInstallMsgHandler(myMessageOutput);
	QCoreApplication app(argc, argv);
	// usage
	if ( argc < 2 ) {
		qDebug() << "Build:" << __DATE__ << __TIME__;
		qDebug() << "Git revision: " << GREEDY_REVISION;
		qDebug() << "Usage: " << argv[0] << " [options] /path/to/mount_point";
		qDebug() << "Options:";
		qDebug() << "\t-threads"<<thread_count<<"";
		return(-1);
	}
	char* path = argv[(argc-1)];
	// test access to mount point
	if ( access (path, R_OK) != 0 ) {
		qCritical() << "Permisson denied from  " << path;
		return(-1);
	}
	// options
	for (int i = 1 ; i < (argc-1);i++) {
		char* option = argv[i];
		char* value = argv[(i+1)];
		if ( strcmp(option,"-threads") == 0  )  {
			QRegExp rx("(\\d+)");
			if ( rx.exactMatch(value) ) {
				thread_count = rx.cap(0).toInt();
			}
		}
	}
	// TODO: discover real directory path with QDir
	QString real_path = QString(path);
	// clear last "/" out
	real_path.remove(QRegExp("/$"));
	// main
	ScanControl main(QString(path),thread_count,real_path);
	QObject::connect(&main,SIGNAL(nothingRuning()),&app,SLOT(quit()));
	return(app.exec());
}
