#ifndef MRIWATCHERFRAME_H
#define MRIWATCHERFRAME_H

#include <qframe.h>
#include <qdragobject.h> 

class MriWatcherFrame : public QFrame
{ 
Q_OBJECT
public:
    MriWatcherFrame( QWidget * parent = 0, const char * name = 0 );
    ~MriWatcherFrame();

signals:
    void GetFiles(const QString&);
    void UnSelect();
    
protected:
  void dragEnterEvent( QDragEnterEvent * );
  void dropEvent( QDropEvent * );
  void mousePressEvent ( QMouseEvent * );
};

#endif
