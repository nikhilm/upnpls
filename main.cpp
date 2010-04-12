#include "upnpls.h"

#include <iostream>

#include <QApplication>
#include <QStringList>

// simple program to list a upnp mediaserver's contents
int main( int argc, char **argv )
{
  QApplication app( argc, argv );

  Upnpls up;
  up.show();
  app.exec();
  return 0;
}

