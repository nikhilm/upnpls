SOURCES = upnpls.cpp main.cpp items.cpp
HEADERS = upnpls.h items.h

INCLUDEPATH += /home/nikhil/workspace/hupnp-git-svn/herqq/hupnp/include
LIBS += -lQtSolutions_SOAP-2.7 -L/home/nikhil/workspace/hupnp-git-svn/herqq/hupnp/bin -L/home/nikhil/workspace/hupnp-git-svn/herqq/hupnp/lib -lHUpnp

CONFIG += qt

QT += xml
