/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: ImageFrameGUIControls.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/14 20:59:37 $
  Version:   $Revision: 1.1 $
  Author:    Matthieu Jomier

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "ImageFrameGUIControls.h"
#include <qdragobject.h> 
#include <qurl.h>

ImageFrameGUIControls::ImageFrameGUIControls( QWidget * parent, const char * name )
    : ImageFrameGUI( parent, name )
{
  setAcceptDrops(TRUE);
  installEventFilter(this);
  m_selected = false;
}


ImageFrameGUIControls::~ImageFrameGUIControls( )
{
}

void ImageFrameGUIControls::Select()
{
  m_selected = true;
  setBackgroundColor(QColor(255,50,50));
}

void ImageFrameGUIControls::UnSelect()
{
  m_selected = false;
  setBackgroundColor(QColor(193,193,193));
}

bool ImageFrameGUIControls::IsSelected()
{
  return m_selected ;
}

void ImageFrameGUIControls::dragEnterEvent( QDragEnterEvent *e )
{
    if ( QTextDrag::canDecode( e ))
    {
        e->accept();
    }
}

void ImageFrameGUIControls::dropEvent( QDropEvent * e )
{
  QStringList files;
  if ( QUrlDrag::decodeLocalFiles( e, files ) ) {
      for (QStringList::Iterator i=files.begin(); i!=files.end(); ++i)
      {
          emit GetFiles( (*i) );
      }
  }
}
