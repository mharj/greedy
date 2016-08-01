#include <QObject>
#include <QDebug>
#include <QList>
#include <QMutex>
#include <QStringList>

#include <unistd.h>

#include "ScanDir.h"
#include "filestats.h"

class ScanControl : public QObject {
	Q_OBJECT
public:
	ScanControl(QDir directory,int thread_count) {
		userFileStatistics.reserve(20000);
		groupFileStatistics.reserve(5000);
		all = new FileStats();
		all->setDirs(1); // add base dir
		permission = true;
		dirs << directory;
		this->base = directory;
		for ( int i = 0 ; i < thread_count ; i++ ) { // init threads and connect signals
			thread.insert(i,new ScanDir());
			connect(thread[i],SIGNAL(finished()),this,SLOT(finishedThread()));
		}
		connect(this,SIGNAL(dirChanged()),this,SLOT(checkThreads()));
		scan_time.start();
		emit dirChanged(); // initial start signal
	}

private:
	QTime scan_time;
	QList<ScanDir*> thread;
	QList<QDir> dirs;
	QDir base;
	FileStats* all;
	QHash<__uid_t, FileStats*> userFileStatistics;
	QHash<__gid_t, FileStats*> groupFileStatistics;
	QMap<int,quint64> yearFileSizes;
	bool permission;
	QMutex mutex;

signals:
	void dirChanged();
	void nothingRuning();

public slots:
	void checkThreads() {
		if ( ! dirs.empty() ) {
			bool change = false;
			QListIterator<ScanDir*> t(thread);
			while ( ! dirs.empty() && t.hasNext() ) {
				ScanDir* c = t.next();
				if ( ! c->isRunning() && c->data_read ) {
					c->setDir(dirs.takeFirst());
					c->start();
					change = true;
				}
			}
			if ( change == true ) {
				emit dirChanged();
			}
		} else {
			// test what we have something running
			bool running=false;
			QListIterator<ScanDir*> t(thread);
			while ( t.hasNext() ) {
				ScanDir* c = t.next();
				if ( ! c->data_read || c->isRunning() ) {
					running=true;
				}
			}
			if ( ! running ) { // not threads or data left
				// uid data
				QStringList uidList;
				QHashIterator<__uid_t, FileStats*> ue(userFileStatistics);
				while (ue.hasNext()) {
					ue.next();
					uidList << QString("{\"uid\":%1,\"files\":%2,\"dirs\":%3,\"size\":%4,\"mtime\":%5,\"ctime\":%6,\"atime\":%7}")
						.arg(ue.key())
						.arg(ue.value()->getFiles())
						.arg(ue.value()->getDirs())
						.arg(ue.value()->getSize())
						.arg(ue.value()->getModify())
						.arg(ue.value()->getCreate())
						.arg(ue.value()->getAccess());
				}
				// gid data
				QStringList gidList;
				QHashIterator<__gid_t, FileStats*> ge(groupFileStatistics);
				while (ge.hasNext()) {
					ge.next();
					gidList << QString("{\"gid\":%1,\"files\":%2,\"dirs\":%3,\"size\":%4,\"mtime\":%5,\"ctime\":%6,\"atime\":%7}")
						.arg(ge.key())
						.arg(ge.value()->getFiles())
						.arg(ge.value()->getDirs())
						.arg(ge.value()->getSize())
						.arg(ge.value()->getModify())
						.arg(ge.value()->getCreate())
						.arg(ge.value()->getAccess());
				}
				// age data
				QStringList yearDataList;
				QMapIterator<int, quint64> yd(yearFileSizes);
				while ( yd.hasNext() ) {
					yd.next();
					yearDataList << QString("\"%1\":%2").arg(yd.key()).arg(yd.value());
				}

				// Error string
				QString error = QString("");
				if (! permission ) {
					QString errorString = QString("%1 have some permission problems, check root access").arg(base.absolutePath());
					error = QString(",\"error\":\"%1\"").arg(QString(errorString) );
				}
				// Scan stats
				QString stats = QString("{\"elapsed\":%1,\"fps\":%2}").arg(scan_time.elapsed()).arg(((all->getFiles()+all->getDirs())/((float)scan_time.elapsed()/1000)), 0, 'f', 0);
				// all data
				QString allData = QString("{\"files\":%1,\"dirs\":%2,\"size\":%3,\"mtime\":%4,\"ctime\":%5,\"atime\":%6}").arg(all->getFiles()).arg(all->getDirs()).arg(all->getSize()).arg(all->getModify()).arg(all->getCreate()).arg(all->getAccess());
				// output
				QTextStream out(stdout);
				out << QString("{\"all\":%1,\"uids\":[%2],\"gids\":[%3],\"year\":{%4},\"stats\":%5%6}").arg(allData).arg(uidList.join(",")).arg(gidList.join(",")).arg(yearDataList.join(",")).arg(stats).arg(error)<< endl;
				// release threads
				QListIterator<ScanDir*> t(thread);
				while ( t.hasNext() ) {
					ScanDir* c = t.next();
					disconnect(c,SIGNAL(finished()),this,SLOT(finishedThread()));
					delete c;
				}
				emit nothingRuning(); // we are done, signal to main app!
			}
		}
	}

	void finishedThread(){
		__uid_t uid;
		__gid_t gid;
		uint years;
		mutex.lock();	// lock for write
		ScanDir* cthread = (ScanDir*)this->sender();
		dirs.append( cthread->getNewDirs() );
		// collect thread user data and insert to global
		QHashIterator<__uid_t, FileStats*> e(cthread->userFileStatistics);
		while (e.hasNext()) {
			e.next();
			uid = e.key();
			// init values
			if ( ! userFileStatistics.contains(uid) ) {
				userFileStatistics[uid] = new FileStats();
			}
			userFileStatistics[uid]->add(e.value());
			all->add(e.value());
		}
		// collect thread group data and insert to global
		QHashIterator<__gid_t, FileStats*> g(cthread->groupFileStatistics);
		while (g.hasNext()) {
			g.next();
			gid = g.key();
			// init values
			if ( ! groupFileStatistics.contains(gid) ) {
				groupFileStatistics[gid]= new FileStats();
			}
			groupFileStatistics[gid]->add(g.value());
		}
		// year age data collection
		QHashIterator<uint, quint64> yfs(cthread->yearFileSizes);
		while ( yfs.hasNext() ) {
			yfs.next();
			years = yfs.key();
			if ( yearFileSizes.contains( years ) ) {
				yearFileSizes[ years ] += yfs.value();
			} else {
				yearFileSizes.insert( years ,yfs.value());
			}
		}

		if ( cthread->permission == false ) {
			permission=false;
		}
		// done, set thread free to use and unlock mutex
		cthread->data_read = true;
		mutex.unlock(); // unlock
		emit dirChanged();
	}
};
