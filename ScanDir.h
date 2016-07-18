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

#include <dirent.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/vfs.h>

struct gstat {
	qint64 count;
	qint64 size;
};

struct fileStatistics {
    qint64 file_count;
	qint64 dir_count;
	qint64 size;
	time_t last_modify;
	time_t last_create;
	time_t last_access;
};

class ScanDir : public QThread {
	Q_OBJECT

public:
	bool data_read;
	QHash<long int, fileStatistics> userFileStatistics;
	QHash<long int, fileStatistics> groupFileStatistics;
    QHash<int,quint64> yearFileSizes;
	bool permission;
    ScanDir() {
        permission = true;
        files = 0;
        dirs = 0;
        data_read = true;
 		// current time
		now = time(0);
        seconds_in_year = (double)(now - QDateTime::currentDateTime().addYears(-1).toTime_t());
        userFileStatistics.reserve(20000); // init size for 20k uids
        groupFileStatistics.reserve(5000); // init size for 5k gids
		
	}
	void setDir(QString dir) {
		this->directory=dir;
		this->directory.remove(QRegExp("/$"));
	}
	void run() {
		// clear init values
        data_read = false;
		directory_list.clear();
        permission = true;
		files=0;
        dirs=0;
        // clear year data
        yearFileSizes.clear();
		userFileStatistics.clear();
		groupFileStatistics.clear();
		if ( access (directory.toLatin1().data(), R_OK) != 0 ) {
            permission = false;
			return;
		}
		struct dirent *dp;	
		// get directory data
		DIR *dir = opendir(directory.toLatin1().data());
		while ((dp=readdir(dir)) != NULL) {
            // common filename skip
			if ( strcmp(dp->d_name,"..") == 0 || strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,".snapshot") == 0 ) 
				continue;
			char rfile[(directory.length()+strlen(dp->d_name)+2)]; // full path to file
			sprintf(rfile,"%s/%s",directory.toLatin1().data(),dp->d_name);
			struct stat st;
			if ( lstat(rfile, &st) == 0 ) {			
				long int uid=st.st_uid;
				long int gid=st.st_gid;

                if ( ! userFileStatistics.contains(uid) ) { // init uid values if not exists yet
					userFileStatistics[uid].file_count=0;
					userFileStatistics[uid].dir_count=0;
					userFileStatistics[uid].size=0;
					userFileStatistics[uid].last_modify=0;					
					userFileStatistics[uid].last_create=0;
                    userFileStatistics[uid].last_access=0;
                }

                if ( ! groupFileStatistics.contains(gid) ) { // init gid values if not exists yet
					groupFileStatistics[gid].file_count=0;
					groupFileStatistics[gid].dir_count=0;
					groupFileStatistics[gid].size=0;
					groupFileStatistics[gid].last_modify=0;					
					groupFileStatistics[gid].last_create=0;
					groupFileStatistics[gid].last_access=0;
				}

                // use structure pointers
				fileStatistics &ownerStats = userFileStatistics[ uid ];
				fileStatistics &groupStats = groupFileStatistics[ gid ];

                // calculate years (not exact)
                int year = floor((double)(now-st.st_mtime)/(seconds_in_year));
                if ( ! yearFileSizes.contains(year) ) {
                    yearFileSizes.insert(year,st.st_size);
                } else {
                    yearFileSizes[year] += st.st_size;
                }
	
                // ignore last modify date if date is in future
				if ( st.st_mtime > ownerStats.last_modify && st.st_mtime < now )
					ownerStats.last_modify=st.st_mtime;
				if ( st.st_mtime > groupStats.last_modify && st.st_mtime < now )
					groupStats.last_modify=st.st_mtime;

                // ignore last create date if date is in future
				if ( st.st_ctime > ownerStats.last_create && st.st_ctime < now )
					ownerStats.last_create=st.st_ctime;
				if ( st.st_ctime > groupStats.last_create && st.st_ctime < now )
					groupStats.last_create=st.st_ctime;
					
                // ignore last access date if date is in future
				if ( st.st_atime > ownerStats.last_access && st.st_atime < now )
					ownerStats.last_access=st.st_atime;
				if ( st.st_atime > groupStats.last_access && st.st_atime < now )
					groupStats.last_access=st.st_atime;					
					
				// add file size
				ownerStats.size += st.st_size;
				groupStats.size += st.st_size;				
				
				if ( S_ISDIR(st.st_mode) ) {
					directory_list	<< QString(rfile);
					dirs++;					
					++ownerStats.dir_count;
					++groupStats.dir_count;
				} else {
					files++;				
					++ownerStats.file_count;
					++groupStats.file_count;
				}
			}
		}
		closedir(dir);
	}
	int getFiles(void) {
		return(files);
	}	
	int getDirs(void) {
		return(dirs);
	}
	QStringList getNewDirs(void) {
		return(directory_list);
	}
private:
    double seconds_in_year;
	quint64 files;
	quint64 dirs;
	time_t now;
	QString directory;
	QStringList directory_list;
};

#endif // SCANDIR_H
