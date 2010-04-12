#include "items.h"

#include <QMessageBox>
#include <QXmlStreamReader>

#include <HAction>
#include <HActionArguments>
#include <HDevice>
#include <HDeviceInfo>
#include <HService>

using namespace Herqq::Upnp;

bool UpnpModel::hasChildren( const QModelIndex &parent ) const
{
  if( parent.isValid() ) {
    QStandardItem *item = itemFromIndex( parent );
    if( item->type() == ContentDirectoryItem::Item )
      return false;
  }
  return true;
}

ContentDirectoryItem::ContentDirectoryItem( HService *srv, const QString &text ) 
  : QObject( 0 )
  , QStandardItem(text)
  , m_service(srv)
  , m_browseAct( NULL )
{
  setEditable(false);
}

HDeviceInfo ContentDirectoryItem::deviceInfo()
{
  return m_service->parentDevice()->deviceInfo();
}

void ContentDirectoryItem::browse()
{
  if( type() == Item )
    return;
  m_browseAct = service()->actionByName( "Browse" );
  if( m_browseAct == NULL ) {
    QMessageBox::warning( 0, tr("Not browsable"), tr("%1 does not support browsing").arg( deviceInfo().friendlyName() ) );
    return;
  }

  HActionArguments argsIn = m_browseAct->inputArguments();

  argsIn["ObjectID"]->setValue( objectID() );
  argsIn["BrowseFlag"]->setValue( "BrowseDirectChildren" );
  argsIn["Filter"]->setValue( "*" );
  argsIn["StartingIndex"]->setValue( 0 );
  argsIn["RequestedCount"]->setValue( 0 );
  argsIn["SortCriteria"]->setValue( "" );

  connect( m_browseAct, SIGNAL( invokeComplete( QUuid ) ), this, SLOT( invokeComplete( QUuid ) ) );
  m_browseAct->beginInvoke( argsIn );
}

void ContentDirectoryItem::invokeComplete( QUuid id )
{
  disconnect( m_browseAct, SIGNAL( invokeComplete( QUuid ) ), this, SLOT( invokeComplete( QUuid ) ) );
  HActionArguments outArgs;
  int ok;
  m_browseAct->waitForInvoke( id, &ok, &outArgs );

  if( ok != 0 ) {
    QMessageBox::warning( 0, tr("Error"), tr("Error invoking browse on %1").arg( objectID() ) );
  }

  if( outArgs.contains("Result") ) {
    createListingFromDIDL( outArgs["Result"]->value().toString() );
  }
}

void ContentDirectoryItem::createListingFromDIDL( const QString &xml )
{
  QXmlStreamReader reader( xml );

  ContentDirectoryItem *entry = NULL;
  while( !reader.atEnd() ) {
    reader.readNext();

    if( reader.isStartElement() ) {
      if( reader.name() == "item" ) {
        entry = new ItemItem( service(), 0, "" );
      }
      else if( reader.name() == "container" ) {
        entry = new ContainerItem( service(), 0, "" );
      }
      else {
        continue;
      }
      QXmlStreamAttributes attrs = reader.attributes();
      entry->setObjectID( attrs.value("id").toString() );

      while( reader.name() != "title" ) {
        reader.readNext();
      }

      reader.readNext();
      if( reader.isCharacters() ) {
        entry->setText( reader.text().toString() );
      }

      appendRow( entry );
    }
  }

  if( reader.hasError() ) {
    qDebug() << "ERROR:"<< reader.errorString();
  }

}

///////////// Container ////////////
ContainerItem::ContainerItem( HService *srv, const QString &objectID, const QString &text )
  : ContentDirectoryItem( srv, text )
  , m_objectID( objectID )
{
}

///////////// Item ///////////
ItemItem::ItemItem( HService *srv, const QString &objectID, const QString &text )
  : ContentDirectoryItem( srv, text )
  , m_objectID( objectID )
{
}
