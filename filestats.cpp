#include "filestats.h"

FileStats::FileStats() {
	atime = 0;
	ctime = 0;
	mtime = 0;
	size = 0;
	dirs = 0;
	files = 0;
}

void FileStats::setDirs(quint64 _dirs) {
	dirs = _dirs;
}

quint64 FileStats::getDirs(void) {
	return dirs;
}

void FileStats::setFiles(quint64 _files) {
	files = _files;
}

quint64 FileStats::getFiles(void) {
	return files;
}

void FileStats::setSize(quint64 _size) {
	size = _size;
}

quint64 FileStats::getSize(void) {
	return size;
}

time_t FileStats::getModify() {
	return mtime;
}

time_t FileStats::getCreate() {
	return ctime;
}

time_t FileStats::getAccess() {
	return atime;
}


time_t FileStats::getModify(time_t _mtime) {
	if ( mtime > _mtime )
		return mtime;
	return _mtime;
}

time_t FileStats::getCreate(time_t _ctime) {
	if ( ctime > _ctime )
		return ctime;
	return _ctime;
}

time_t FileStats::getAccess(time_t _atime) {
	if ( atime > _atime )
		return atime;
	return _atime;
}




void FileStats::setStat(struct stat st,time_t now) {
	if ( st.st_ctime > this->ctime && st.st_ctime < now )
		ctime=st.st_ctime;
	if ( st.st_mtime > this->mtime && st.st_mtime < now )
		mtime=st.st_mtime;
	if ( st.st_atime > this->atime && st.st_atime < now )
		atime=st.st_atime;
	size += st.st_size;
	if ( S_ISDIR(st.st_mode) ) {
		dirs ++;
	} else {
		files ++;
	}
}
