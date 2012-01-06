/****************************************************************************
** $Id: MriWatcherFlow.h,v 1.2 2007/08/28 22:57:42 gcasey Exp $
**
** Definition of MriWatcher flow layout for custom layout example
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FLOW_H
#define FLOW_H

#include <qlayout.h>
#include <qlist.h>

class MriWatcherFlow : public QLayout
{
public:
  MriWatcherFlow( QWidget *parent, int border=0, int space=-1,
                  const char *name=0 )
    : QLayout( parent, border, space, name ),
      cached_width(0),  m_nbcolumn(0), m_nbimage(1),
      m_oldx(0), m_oldy(0), m_oldwidth(0), m_oldheight(0) {}

  MriWatcherFlow( QLayout* parent, int space=-1, const char *name=0 )
    : QLayout( parent, space, name ),
      cached_width(0), m_nbcolumn(0), m_nbimage(1),
      m_oldx(0), m_oldy(0), m_oldwidth(0), m_oldheight(0) {}

  MriWatcherFlow( int space=-1, const char *name=0 )
    : QLayout( space, name ),
      cached_width(0), m_nbcolumn(0), m_nbimage(1),
      m_oldx(0), m_oldy(0), m_oldwidth(0), m_oldheight(0) {}

  ~MriWatcherFlow();

  void addItem( QLayoutItem *item);
  bool hasHeightForWidth() const;
  int heightForWidth( int ) const;
  QSize sizeHint() const;
  QSize minimumSize() const;
  QLayoutIterator iterator();
  QSizePolicy::ExpandData expanding() const;
  void test();
  void SetColumn(int nbcolumn);
  void SetNbImage(int nbimage);
  void Switch(int i,int j);
  void Delete(int i);
  int doLayout( QRect rec = QRect(-1,-1,-1,-1), bool testonly = false);
  void DeleteAll();

protected:
  void setGeometry( const QRect& );

private:
  int cached_width;
  int cached_hfw;
  int m_nbcolumn;
  int m_nbimage;
  int m_oldx;
  int m_oldy;
  int m_oldwidth;
  int m_oldheight; 

  QList<QLayoutItem> list;

};

#endif
