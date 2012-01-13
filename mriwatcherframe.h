#ifndef MRIWATCHERFRAME_H
#define MRIWATCHERFRAME_H

#include <QFrame>
#include <QScrollArea>
#include <QtGui>
// #include <qdragobject.h>

class MriWatcherFrame : public QScrollArea
{
  Q_OBJECT
public:
  MriWatcherFrame( QWidget * parent = 0);
  ~MriWatcherFrame();
signals:
  void GetFiles(const QString &);

  void UnSelectAll();

protected:
  void dragEnterEvent( QDragEnterEvent * );

  void dropEvent( QDropEvent * );

  void mousePressEvent( QMouseEvent * );

};

#endif
