#include <QObject>
#include <QDebug>
#include <QList>
#include <QMutex>
#include <QStringList>

#include <unistd.h>

#include "ScanDir.h"

class ScanControl : public QObject {
	Q_OBJECT
public:
	int r_exit;
    ScanControl(QString base,int max,QString real_path) {
        userFileStatistics.reserve(20000);
		groupFileStatistics.reserve(5000);	
		scan_time.start();
		r_exit=0;
        this->real_path = real_path;
        total_files = 0;
        total_dirs = 1; // as we have root path already
        permission = true;
        if ( access(base.toLatin1().data(), R_OK) != 0 ) {
			return;
		}
        this->max = max;
		dirs << base;
        this->base = base;
        // init threads and connect signals
        for ( int i = 0 ; i < max ; i++ ) {
            thread.insert(i,new ScanDir());
            thread[i]->setObjectName(QString("%1").arg(i)); // use thread index as name so we can eaily read those
			connect(thread[i],SIGNAL(finished()),this,SLOT(finishedThread()));
		}
		connect(this,SIGNAL(dirChanged()),this,SLOT(checkThreads()));
        emit dirChanged(); // initial start signal
	}

private:
	QTime scan_time;
	QList<ScanDir*> thread;
	int max;
	QStringList dirs;
	QString base;
	quint64 total_files;
	quint64 total_dirs;	
	QHash<long int, fileStatistics> userFileStatistics;
	QHash<long int, fileStatistics> groupFileStatistics;
    QMap<int,quint64> yearFileSizes;
    bool permission;
    QMutex mutex;
    QString real_path;


signals:
     void dirChanged();
	 void nothingRuning();
public slots:
     void checkThreads() {
		if ( ! dirs.empty() ) {
			for ( int i=0;i<max;i++) {
				// not give thread if it's not yet read
				if ( ! thread[i]->isRunning() && thread[i]->data_read ) {
					thread[i]->setDir(dirs.takeFirst());
					thread[i]->start();
					i=max;
					emit dirChanged();
				}
			}
		} else {
			// test what we have something running
			bool running=false;
			for ( int i=0;i<max;i++) {
				if ( ! thread.at(i)->data_read || thread.at(i)->isRunning() ) 
					running=true;
			}
			if ( ! running ) { // not threads or data left
                // uid data
                QStringList uidList;
				QHashIterator<long int, fileStatistics> ue(userFileStatistics);
				while (ue.hasNext()) {
					ue.next();
                    uidList << QString("{\"uid\":%1,\"files\":%2,\"dirs\":%3,\"size\":%4,\"mtime\":%5,\"ctime\":%6,\"atime\":%7}")
                        .arg(ue.key())
                        .arg(ue.value().file_count)
                        .arg(ue.value().dir_count)
                        .arg(ue.value().size)
                        .arg(ue.value().last_modify)
                        .arg(ue.value().last_create)
                        .arg(ue.value().last_access);
				}
                // gid data
                QStringList gidList;
				QHashIterator<long int, fileStatistics> ge(groupFileStatistics);
				while (ge.hasNext()) {
					ge.next();
                    gidList << QString("{\"gid\":%1,\"files\":%2,\"dirs\":%3,\"size\":%4,\"mtime\":%5,\"ctime\":%6,\"atime\":%7}")
                        .arg(ge.key())
                        .arg(ge.value().file_count)
                        .arg(ge.value().dir_count)
                        .arg(ge.value().size)
                        .arg(ge.value().last_modify)
                        .arg(ge.value().last_create)
                        .arg(ge.value().last_access);
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
                    QString errorString = QString("%1 have some permission problems, check root access").arg(real_path);
                    error = QString(",\"error\":\"%1\"").arg(QString(errorString) );
				}
                // Scan stats
                QString stats = QString("{\"elapsed\":%1,\"fps\":%2}").arg(scan_time.elapsed()).arg(((total_files+total_dirs)/((float)scan_time.elapsed()/1000)), 0, 'f', 0);
                // output
                QTextStream out(stdout);
                out<< QString("{\"uids\":[%1],\"gids\":[%2],\"year\":{%3},\"stats\":%4%5}").arg(uidList.join(",")).arg(gidList.join(",")).arg(yearDataList.join(",")).arg(stats).arg(error)<< endl;
				// release threads
				for ( int i=0;i<max;i++) {
					disconnect(thread[i],SIGNAL(finished()),this,SLOT(finishedThread()));
					delete thread[i];
				}
				emit nothingRuning(); // we are done, signal to main app!
			}
		}
	}

	void finishedThread(){	
	 	QObject *pSender;
        pSender = this->sender();
        int i = pSender->objectName().toInt(); // we know which thread this is from name
        mutex.lock();	// lock for write
        ScanDir* cthread = thread[i];
        total_files += cthread->getFiles();
        total_dirs += cthread->getDirs();
        QStringList newlist = cthread->getNewDirs();
		if ( ! newlist.empty() ) {
			for (int l = 0; l < newlist.size(); ++l)
				dirs << newlist.at(l);
		}
		// collect thread user data and insert to global
        QHashIterator<long int, fileStatistics> e(cthread->userFileStatistics);
		while (e.hasNext()) {
			e.next();
			long int uid=e.key();
			// init values
			if ( ! userFileStatistics.contains(uid) ) {
				userFileStatistics[uid].file_count=0;
				userFileStatistics[uid].dir_count=0;
				userFileStatistics[uid].size=0;
				userFileStatistics[uid].last_modify=0;					
				userFileStatistics[uid].last_create=0;
				userFileStatistics[uid].last_access=0;				
			}

			userFileStatistics[uid].file_count+=e.value().file_count;
			userFileStatistics[uid].dir_count+=e.value().dir_count;
			userFileStatistics[uid].size+=e.value().size;					
			if ( e.value().last_modify > userFileStatistics[uid].last_modify  )
				userFileStatistics[uid].last_modify=e.value().last_modify;
			if ( e.value().last_create > userFileStatistics[uid].last_create  )
				userFileStatistics[uid].last_create=e.value().last_create;
			if ( e.value().last_access > userFileStatistics[uid].last_access  )
				userFileStatistics[uid].last_access=e.value().last_access;
    	}		
		// collect thread group data and insert to global
        QHashIterator<long int, fileStatistics> g(cthread->groupFileStatistics);
		while (g.hasNext()) {
			g.next();
			long int gid=g.key();
			// init values
			if ( ! groupFileStatistics.contains(gid) ) {
				groupFileStatistics[gid].file_count=0;
				groupFileStatistics[gid].dir_count=0;
				groupFileStatistics[gid].size=0;
				groupFileStatistics[gid].last_modify=0;					
				groupFileStatistics[gid].last_create=0;
				groupFileStatistics[gid].last_access=0;				
			}

			groupFileStatistics[gid].file_count+=g.value().file_count;
			groupFileStatistics[gid].dir_count+=g.value().dir_count;
			groupFileStatistics[gid].size+=g.value().size;			
			if ( g.value().last_modify > groupFileStatistics[gid].last_modify  )
				groupFileStatistics[gid].last_modify=g.value().last_modify;			
			if ( g.value().last_create > groupFileStatistics[gid].last_create  )
				groupFileStatistics[gid].last_create=g.value().last_create;
			if ( g.value().last_access > groupFileStatistics[gid].last_access  )
				groupFileStatistics[gid].last_access=g.value().last_access;
    	}
        // year age data collection
        QHashIterator<int, quint64> yfs(cthread->yearFileSizes);
        while (yfs.hasNext()) {
            yfs.next();
            if (yearFileSizes.key(yfs.key() ) ) {
                yearFileSizes[yfs.key()] += yfs.value();
            } else {
                yearFileSizes.insert(yfs.key(),yfs.value());
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
