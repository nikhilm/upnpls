#ifndef UPNPLS_H
#define UPNPLS_H

#include <QMainWindow>
#include <QModelIndex>
#include <QStringList>
#include <QUuid>
#include <QUrl>

#include <HUdn>

class QStandardItemModel;
class QTreeView;

namespace Herqq
{
  namespace Upnp
  {
    class HControlPoint;
    class HDevice;
    class HService;
  }
}

class Upnpls : public QMainWindow
{
  Q_OBJECT
  public:
    Upnpls();
    ~Upnpls();

  private:
    enum Roles {
      ServiceRole = Qt::UserRole + 1
    };
    void setupGui();
    bool ready();
    void waitForDevice();
    bool initCDS( Herqq::Upnp::HDevice *dev );

    Herqq::Upnp::HControlPoint *m_cp;
    Herqq::Upnp::HService *m_cds;
    Herqq::Upnp::HUdn m_udn;

    QStandardItemModel *m_model;
    QTreeView *m_view;

    const QStringList m_args;

  signals:
    void done();

  private slots:
    void rootDeviceOnline( Herqq::Upnp::HDevice *device );
    void rootDeviceOffline( Herqq::Upnp::HDevice *device );

    void browse( const QModelIndex & );

};

#endif
