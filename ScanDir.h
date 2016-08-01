#ifndef SCANDIR_H
#define SCANDIR_H

#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QFileInfo>
#include <QDir>

#include <dirent.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/vfs.h>

#include "filestats.h"

class ScanDir : public QThread {
	Q_OBJECT

public:
	bool data_read;
	QHash<__uid_t, FileStats*> userFileStatistics;
	QHash<__gid_t, FileStats*> groupFileStatistics;
	QHash<uint,quint64> yearFileSizes;
	bool permission;

	ScanDir() {
		data_read = true;
		now = time(0);
		seconds_in_year = (double)(now - QDateTime::currentDateTime().addYears(-1).toTime_t());
		userFileStatistics.reserve(20000); // init size for 20k uids
		groupFileStatistics.reserve(5000); // init size for 5k gids
	}

	void setDir(QDir dir) {
		this->directory = dir;
	}

	void run() {
		reset(); // values
		QByteArray cdir = directory.absolutePath().toLatin1();
		if ( access (cdir.constData(), R_OK) != 0 ) {
			permission = false;
			return;
		}
		struct dirent *dp;
		// get directory data
		DIR *dir = opendir(cdir.constData());
		while ((dp=readdir(dir)) != NULL) {
			// common filename skip
			if ( strcmp(dp->d_name,"..") == 0 || strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,".snapshot") == 0 ) {
				continue;
			}
			char rfile[(strlen(cdir.constData())+strlen(dp->d_name)+2)]; // full path to file
			sprintf(rfile,"%s/%s",cdir.constData(),dp->d_name);
			struct stat st;
			if ( lstat(rfile, &st) == 0 ) {
				if ( ! userFileStatistics.contains(st.st_uid) ) { // init if not exists yet
					userFileStatistics.insert(st.st_uid,new FileStats());
				}
				if ( ! groupFileStatistics.contains(st.st_gid) ) { // init if not exists yet
					groupFileStatistics.insert(st.st_gid,new FileStats());
				}
				userFileStatistics[ st.st_uid ]->setStat(st,now);
				groupFileStatistics[ st.st_gid ]->setStat(st,now);
				// calculate years (not exact)
				int year = floor((double)(now-st.st_mtime)/(seconds_in_year));
				if ( ! yearFileSizes.contains(year) ) {
					yearFileSizes.insert(year,st.st_size);
				} else {
					yearFileSizes[year] += st.st_size;
				}
				if ( S_ISDIR(st.st_mode) ) {
					directory_list	<< QDir(rfile);
				}
			}
		}
		closedir(dir);
	}
	QList<QDir> getNewDirs(void) {
		return(directory_list);
	}
private:
	double seconds_in_year;
	time_t now;
	QDir directory;
	QList<QDir> directory_list;

	void reset() { // to initial values
		now = time(0);
		data_read = false;
		directory_list.clear();
		permission = true;
		yearFileSizes.clear();
		userFileStatistics.clear();
		groupFileStatistics.clear();
	}
};

#endif // SCANDIR_H
