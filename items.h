#ifndef UPNPLS_ITEM_H
#define UPNPLS_ITEM_H

#include <QDebug>
#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QUuid>

#include <HDeviceInfo>

namespace Herqq
{
  namespace Upnp
  {
    class HService;
  }
}

class UpnpModel : public QStandardItemModel
{
  public:
    UpnpModel( QObject *parent=0 )
      : QStandardItemModel(parent)
    {};

    bool hasChildren( const QModelIndex &parent=QModelIndex() ) const;
};

class ContentDirectoryItem : public QObject, public QStandardItem
{
  Q_OBJECT
  public:
    enum Type {
      ContentDirectory = QStandardItem::UserType + 1,
      Container,
      Item
    };

    ContentDirectoryItem( Herqq::Upnp::HService *srv, const QString &text );
    inline int type() const { return ContentDirectory; }
    Herqq::Upnp::HDeviceInfo deviceInfo();
    virtual inline const QString objectID() const { return "0"; };
    virtual inline void setObjectID( const QString &id ) {  };
    virtual inline bool hasChildren() const { return true; };
    void browse();

  protected:
    inline Herqq::Upnp::HService *service() { return m_service; };

  private:
    void createListingFromDIDL( const QString &xml );
    Herqq::Upnp::HService *m_service;
    Herqq::Upnp::HAction *m_browseAct;

  private slots:
    void invokeComplete( QUuid );
};

class ContainerItem : public ContentDirectoryItem
{
  public:
    ContainerItem( Herqq::Upnp::HService *srv, const QString &objectID, const QString &text );

    inline const QString objectID() const { return m_objectID; };
    inline void setObjectID( const QString &id ) { m_objectID = id; };
    inline int type() const { return Container; };

  private:
    QString m_objectID;
};

class ItemItem : public ContentDirectoryItem
{
  public:
    ItemItem( Herqq::Upnp::HService *srv, const QString &objectID, const QString &text );
    inline const QString objectID() const { return m_objectID; };
    inline void setObjectID( const QString &id ) { m_objectID = id; };
    inline int type() const { return Item; };
    inline bool hasChildren() const { return false; };

  private:
    QString m_objectID;
};
#endif
