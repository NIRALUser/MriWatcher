#include "imageframelayout.h"
#include <QWidget>
#include <QtGlobal>
#include <math.h>
#include <stdio.h>

ImageFrameLayout::ImageFrameLayout(QWidget * parent)
  : QLayout(parent)
{
  cached_width = 0;
  m_nbcolumn = 0;
  m_nbimage = 1;
  m_oldx = 0;
  m_oldy = 0;
  m_oldwidth = 0;
  m_oldheight = 0;
  setSpacing(1);
}

ImageFrameLayout::~ImageFrameLayout()
{
}

QSize ImageFrameLayout::sizeHint() const
{
  return minimumSize();
}

void ImageFrameLayout::addItem(QLayoutItem *item)
{
  list.append(item);
}

QLayoutItem * ImageFrameLayout::itemAt(int index) const
{
  return list.value(index);
}

QLayoutItem * ImageFrameLayout::takeAt(int index)
{
  if( index >= 0 && index < list.size() )
    {
    return list.takeAt(index);
    }
  else
    {
    return 0;
    }
}

int ImageFrameLayout::count() const
{
  return list.size();
}

QSize ImageFrameLayout::minimumSize() const
{
  QSize s(0, 0);

  return s;
}

/*
QSize ImageFrameLayout::maximumSize() const
{
}
*/
Qt::Orientations ImageFrameLayout::expandingDirections() const
{
  return 0;
}

void ImageFrameLayout::setGeometry(const QRect & rect)
{
  QLayout::setGeometry(rect);
  doLayout(rect, false);
}

/*
QRect ImageFrameLayout::geometry() const
{
}

bool ImageFrameLayout::isEmpty() const
{
}
*/
bool ImageFrameLayout::hasHeightForWidth() const
{
  return true;
}

int ImageFrameLayout::heightForWidth(int w) const
{
  if( cached_width != w )
    {
    // Not all C++ compilers support "mutable" yet:
    ImageFrameLayout * mthis = (ImageFrameLayout *)this;
    int                h = mthis->doLayout( QRect(0, 0, w, 0), TRUE );
    mthis->cached_hfw = h;
    return h;
    }
  return cached_hfw;
}

void ImageFrameLayout::setColumn(int nbcolumn)
{
  m_nbcolumn = nbcolumn;
}

void ImageFrameLayout::swap(int i, int j)
{
  if( (i >= 0) && (j >= 0) && (i < list.size() ) && (j < list.size() ) )
    {
    list.swap(i, j);
    }
}

void ImageFrameLayout::deleteItem(int i)
{
  delete list.takeAt(i)->widget();
}

int ImageFrameLayout::doLayout(QRect rec, bool testmode)
{

// printf("framelayout: x=%d, y=%d, width=%d, height=%d, m_oldx=%d, m_oldy=%d, m_oldwidth=%d, m_oldheight=%d\n",
// rec.x(), rec.y(), rec.width(), rec.height(),m_oldx, m_oldy, m_oldwidth, m_oldheight);
  if( (rec.x() == -1) || (rec.height() == -1) )
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

// printf("framelayout: after set x and y, x=%d, y=%d, list.size=%d\n",x,y,list.size());

  int h = 0;      // height of this line so far.
  // if( (x == 0) && (y == 0) )
  //   {
  //   return 0;
  //   }
  // std::cout << "DoLayout: " <<  x << " | " << y << std::endl;

  // printf("framelayout: m_nbcolumn=%d\n", m_nbcolumn);

  QLayoutItem *o;
  int          idx = 0;
  while( idx < list.size() )
    {
    o = list.at(idx);
    ++idx;

    if( m_nbcolumn != 0 )
      {
      o->widget()->setMinimumWidth( (rec.width() - spacing() * m_nbcolumn) / m_nbcolumn);
      o->widget()->setMinimumHeight( (rec.width() - spacing() * m_nbcolumn) / m_nbcolumn);
      // printf("framelayout: minWidth=%d, minHeight=%d\n",(rec.width()-spacing()*m_nbcolumn)/m_nbcolumn,
      // (rec.width()-spacing()*m_nbcolumn)/m_nbcolumn);
      }
    else
      {
      int height = 0;
      for( int i = 1; i < list.count() + 1; i++ )
        {
        int j = (int)(floor( ( (float)list.count() / (float)i) + 0.99) );
        int w = (rec.width() - 2) / i;
        int h = (rec.height() - 2) / j;

        if( w < h )
          {
          h = w;
          }

        if( h > height )
          {
          height = h;
          }

        w = (rec.width() - 2) / j;
        h = (rec.height() - 2) / i;

        if( w < h )
          {
          h = w;
          }

        if( h > height )
          {
          height = h;
          }

        }
      //  std::cout << "DoLayout Compute H: " <<  height << std::endl;

      if( (height != o->widget()->minimumHeight() ) && (height != 0) )
        {
        o->widget()->setMinimumWidth(height);
        o->widget()->setMinimumHeight(height);
        }
      }

    // if( list.count() == 0 )
    //   {
    //   // printf("framelayout: list.count()==0, call heightForWidth\n");
    //   heightForWidth(0);
    //   }

    int nextX = x + o->sizeHint().width() + spacing();
    if( nextX - spacing() > rec.right() && h > 0 )
      {
      x = rec.x();
      y = y + h + spacing();
      nextX = x + o->sizeHint().width() + spacing();
      h = 0;
      }

    if( !testmode )
      {
      o->setGeometry( QRect( QPoint( x, y ), o->sizeHint() ) );
      }
    x = nextX;
    h = qMax( h, o->sizeHint().height() );
    }

  return y + h - rec.y();
}
