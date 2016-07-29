TEMPLATE = app
TARGET = greedy.$$system(uname -i)
QT += core
QT -= gui

# build GREEDY_REVISION macro
GIT_HEAD = $$cat(.git/HEAD)
contains(GIT_HEAD, "ref:") {
  GIT_HEAD = .git/$$member(GIT_HEAD, 1)
  exists($$GIT_HEAD) {
    GIT_HEAD = $$cat($$GIT_HEAD)
  } else {
    clear(GIT_HEAD)
  }
}
count(GIT_HEAD, 1) {
  GIT_HEAD ~= s/^(.......).*/\\1/
  DEFINES += GREEDY_REVISION=\\\"$$GIT_HEAD\\\"
}

SOURCES += main.cpp \
    filestats.cpp
HEADERS += ScanDir.h ScanControl.h \
    filestats.h
CONFIG += qt warn_on release console
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
