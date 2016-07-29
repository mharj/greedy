#ifndef FILESTATS_H
#define FILESTATS_H

#include <QDebug>
#include <QtGlobal>
#include <sys/types.h>
#include <sys/stat.h>

class FileStats
{
public:
	FileStats();
	void setDirs(quint64 dirs);
	quint64 getDirs(void);
	void setFiles(quint64 files);
	quint64 getFiles(void);
	void setSize(quint64 files);
	quint64 getSize(void);
	void setStat(struct stat st,time_t now);
	time_t getModify();
	time_t getCreate();
	time_t getAccess();
	time_t getModify(time_t mtime);
	time_t getCreate(time_t ctime);
	time_t getAccess(time_t atime);


	void add(FileStats* sub) {
		files += sub->getFiles();
		dirs += sub->getDirs();
		size += sub->getSize();
		mtime = sub->getModify(mtime);
		ctime = sub->getCreate(ctime);
		atime = sub->getAccess(atime);
	}

private:
	quint64 files;
	quint64 dirs;
	quint64 size;
	time_t ctime;
	time_t mtime;
	time_t atime;
};

#endif // FILESTATS_H
