/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: MriWatcherFlow.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/28 22:57:42 $
  Version:   $Revision: 1.2 $
  Author:    Matthieu Jomier

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "MriWatcherFlow.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class MriWatcherFlowIterator :public QGLayoutIterator
{
public:
    MriWatcherFlowIterator( QList<QLayoutItem> *l ) :idx(0), list(l)  {}
    uint count() const;
    QLayoutItem *current();
    QLayoutItem *next();
    QLayoutItem *takeCurrent();

private:
    int idx;
    QList<QLayoutItem> *list;
};



uint MriWatcherFlowIterator::count() const
{
    return list->count();
}

QLayoutItem *MriWatcherFlowIterator::current()
{
    return idx < int(count()) ? list->at(idx) : 0;
}

QLayoutItem *MriWatcherFlowIterator::next()
{
    idx++; return current();
}

QLayoutItem *MriWatcherFlowIterator::takeCurrent()
{
    return idx < int(count()) ? list->take( idx ) : 0;
}

MriWatcherFlow::~MriWatcherFlow()
{

}

void MriWatcherFlow::DeleteAll()
{
  deleteAllItems();
}

int MriWatcherFlow::heightForWidth( int w ) const
{
    if ( cached_width != w ) {
  //Not all C++ compilers support "mutable" yet:
  MriWatcherFlow * mthis = (MriWatcherFlow*)this;
  int h = mthis->doLayout( QRect(0,0,w,0), TRUE );
  mthis->cached_hfw = h;
  return h;
    }
    return cached_hfw;
}

void MriWatcherFlow::addItem( QLayoutItem *item)
{
    list.append( item );
    
}

void MriWatcherFlow::Switch(int i,int j)
{
  list.insert(i,list.take(j));
}

void MriWatcherFlow::Delete(int i)
{
   delete list.take(i)->widget();
}

bool MriWatcherFlow::hasHeightForWidth() const
{
    return TRUE;
}

QSize MriWatcherFlow::sizeHint() const
{
    return minimumSize();
}

QSizePolicy::ExpandData MriWatcherFlow::expanding() const
{
    return QSizePolicy::NoDirection;
}

QLayoutIterator MriWatcherFlow::iterator()
{
    return QLayoutIterator( new MriWatcherFlowIterator( &list ) );
}

void MriWatcherFlow::setGeometry( const QRect &r )
{
    QLayout::setGeometry( r );
    doLayout( r );
}


void MriWatcherFlow::test()
{
    doLayout(QRect(0,0,800,800) );
}

void MriWatcherFlow::SetColumn(int nbcolumn)
{
  m_nbcolumn = nbcolumn;
}

void MriWatcherFlow::SetNbImage(int nbimage)
{
  m_nbimage = nbimage;
}


int MriWatcherFlow::doLayout( QRect rec, bool testonly)
{
    if ((rec.x() == -1) || (rec.height() == -1))
    {
      rec.setX(m_oldx);
      rec.setY(m_oldy);
      rec.setWidth(m_oldwidth);
      rec.setHeight(m_oldheight);
    }
    else
    {
      m_oldx = rec.x();
      m_oldy = rec.y();
      m_oldwidth = rec.width();
      m_oldheight = rec.height(); 
    }
        
    int x = rec.x();
    int y = rec.y();

    int h = 0;    //height of this line so far.
    if ((x == 0) && (y==0))
      return 0;
    //std::cout << "DoLayout: " <<  x << " | " << y << std::endl;

    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) 
    {
      ++it;

      if (m_nbcolumn != 0)
      {
        o->widget()->setMinimumWidth((rec.width()-spacing()*m_nbcolumn)/m_nbcolumn) ;
        o->widget()->setMinimumHeight((rec.width()-spacing()*m_nbcolumn)/m_nbcolumn);
      }
      else
      {
         int height = 0;
        for (unsigned int i=1;i<list.count()+1;i++)
        {
          int j = (int)(floor(((float)list.count()/(float)i)+0.99));
          int w= (rec.width()-2)/i;
          int h= (rec.height()-2)/j;

          if (w<h)
            h = w; 

         if (h>height)
            height = h;


         w= (rec.width()-2)/j;
         h= (rec.height()-2)/i;

         if (w<h)
            h = w; 

         if (h>height)
            height = h;

        }
      //  std::cout << "DoLayout Compute H: " <<  height << std::endl;
    
        if ((height != o->widget()->minimumHeight()) && (height != 0))
        {
          o->widget()->setMinimumWidth(height);
          o->widget()->setMinimumHeight(height);
        }
  }

  if (list.count() == 0)
  {
    heightForWidth(0);  
  }

  int nextX = x + o->sizeHint().width() + spacing();
  if ( nextX - spacing() > rec.right() && h > 0 ) {
      x = rec.x();
      y = y + h + spacing();
      nextX = x + o->sizeHint().width() + spacing();
      h = 0;
  }


  if ( !testonly )
      o->setGeometry( QRect( QPoint( x, y ), o->sizeHint() ) );
  x = nextX;
  h = QMAX( h, o->sizeHint().height() );
    }
    return y + h - rec.y();
}

QSize MriWatcherFlow::minimumSize() const
{
    QSize s(0,0);
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
  ++it;
    //s = s.expandedTo( o->minimumSize() ); //  o->minimumSize()  
    }
    return s;
}

