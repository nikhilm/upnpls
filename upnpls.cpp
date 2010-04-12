#include "upnpls.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QStandardItemModel>
#include <QTreeView>

#include <HControlPoint>
#include <HDevice>
#include <HDeviceInfo>
#include <HService>
#include <HServiceId>
#include <HUpnp>

#include "items.h"

using namespace Herqq::Upnp;

Upnpls::Upnpls()
  : QMainWindow(0)
  , m_cp( new HControlPoint( 0, this ) )
  , m_cds( 0 )
{
  setupGui();

  connect( m_cp, SIGNAL( rootDeviceOnline( Herqq::Upnp::HDevice * ) ),
      this, SLOT( rootDeviceOnline( Herqq::Upnp::HDevice * ) ) );
  connect( m_cp, SIGNAL( rootDeviceOffline( Herqq::Upnp::HDevice * ) ),
      this, SLOT( rootDeviceOffline( Herqq::Upnp::HDevice * ) ) );
  QString err;
  if( m_cp->init(&err) != HControlPoint::Success )
  {
    qDebug() << err;
    QCoreApplication::exit( -1 );
  }

}

void Upnpls::setupGui()
{
  setWindowTitle( "UPnP ContentDirectory browser" );

  m_model = new UpnpModel( this );
  m_view = new QTreeView( this );
  m_view->setModel( m_model );
  connect( m_view, SIGNAL(expanded( const QModelIndex & )), this, SLOT(browse( const QModelIndex & )) );
  setCentralWidget( m_view );
}

Upnpls::~Upnpls()
{
  if( m_cp != NULL )
  {
    m_cp->quit();
    delete m_cp;
    m_cp = NULL;
  }
}

bool Upnpls::ready()
{
  return m_cds != NULL;
}

void Upnpls::waitForDevice()
{
  QEventLoop loop;
  connect( this, SIGNAL( done() ), &loop, SLOT( quit() ) );
  loop.exec( QEventLoop::ExcludeUserInputEvents );
}

bool Upnpls::initCDS( HDevice *dev )
{
  foreach( HService *serv, dev->services() ) {
    qDebug() << serv->serviceId().toString();
  }
  HService *serv = dev->serviceById( HServiceId( "urn:upnp-org:serviceId:ContentDirectory" ) );
  if( serv == NULL ) {
    serv = dev->serviceById( HServiceId( "urn:schemas-upnp-org:serviceId:ContentDirectory" ) );
  }
  if( serv == NULL ) {
    qDebug() << "No service";
    return false;
  }

  QStandardItem *item = new ContentDirectoryItem( serv, dev->deviceInfo().friendlyName() );
  m_model->appendRow( item );

  qDebug() << "Adding row " ;
  return true;
}

void Upnpls::rootDeviceOnline( HDevice *dev )
{
  qDebug() << dev->deviceInfo().udn().toString() << dev->deviceInfo().friendlyName();
  // we found the device we're looking for
  if( !initCDS( dev ) ) {
    qDebug() << "Does not support ContentDirectory";
  }
}

void Upnpls::rootDeviceOffline( HDevice *dev )
{
  for( int i = 0; i < m_model->rowCount(); i++ ) {
    QStandardItem *item = m_model->item(i, 0);
    if( item->type() == ContentDirectoryItem::ContentDirectory ) {
      if( dev->deviceInfo().udn() == ((ContentDirectoryItem*)item)->deviceInfo().udn() ) {
        m_model->removeRow( i );
      }
    }
    emit done();
    //QCoreApplication::exit( 2 );
  }
}

void Upnpls::browse( const QModelIndex &index )
{
  ContentDirectoryItem *item = static_cast<ContentDirectoryItem*>(m_model->itemFromIndex( index ));
  item->browse();
}
