/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: MriWatcherFrame.cxx,v $
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
#include "mriwatcherframe.h"
#include <iostream>
#include <qurl.h>

MriWatcherFrame::MriWatcherFrame( QWidget * parent)
  : QScrollArea(parent)
{
  setAcceptDrops(TRUE);
  installEventFilter(this);
}

MriWatcherFrame::~MriWatcherFrame()
{
}

/*
void MriWatcherFrame::dragEnterEvent( QDragEnterEvent *e )
{
    // Check if you want the drag...
    if ( QTextDrag::canDecode( e ))
    {
        e->accept();
    }
}

void MriWatcherFrame::dropEvent( QDropEvent * e )
{
  QStringList files;
  if ( QUrlDrag::decodeLocalFiles( e, files ) ) {
      for (QStringList::Iterator i=files.begin(); i!=files.end(); ++i)
      {
          emit GetFiles( (*i) );
      }
  }
}
*/

void MriWatcherFrame::dragEnterEvent(QDragEnterEvent *event)
{
  //   if (QTextDrag::canDecode(event))
    {
    event->accept();
    }
}

void MriWatcherFrame::dropEvent(QDropEvent *de)
{
  QList<QUrl> urls;
  urls = de->mimeData()->urls();
  for( int i = 0; i < urls.size(); ++i )
    {
		emit GetFiles(urls.at(i).toLocalFile() );
    }
}

void MriWatcherFrame::mousePressEvent( QMouseEvent * )
{
  emit UnSelectAll();
}
