#ifndef IMAGEFRAMEGUICONTROLS_H
#define IMAGEFRAMEGUICONTROLS_H

#include "ImageFrameGUI.h"

class ImageFrameGUIControls : public ImageFrameGUI
{ 
Q_OBJECT
public:
    ImageFrameGUIControls( QWidget * parent = 0, const char * name = 0 );
    ~ImageFrameGUIControls();
  
    void Select();
    void UnSelect();
    bool IsSelected();
    void dragEnterEvent( QDragEnterEvent *e );
    void dropEvent( QDropEvent * e );

signals:
    void GetFiles(const QString&);

protected:
  bool m_selected;
};

#endif
