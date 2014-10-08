#ifndef IMAGEFRAMELAYOUT_H
#define IMAGEFRAMELAYOUT_H

#include <QLayout>
#include <QList>

class ImageFrameLayout : public QLayout
{

public:
  ImageFrameLayout(QWidget * parent = 0);
  ~ImageFrameLayout();

  QSize sizeHint() const;

  void addItem(QLayoutItem *);

  QLayoutItem * itemAt(int) const;

  QLayoutItem * takeAt(int);

  int count() const;

  QSize minimumSize() const;

//    QSize maximumSize() const;
  Qt::Orientations expandingDirections() const;

  void setGeometry(const QRect &);

//    QRect geometry() const;
//    bool isEmpty() const;

  bool hasHeightForWidth() const;

  int heightForWidth(int) const;

  void setColumn(int nbcolum);

  void swap(int i, int j);

  void deleteItem(int i);

  int doLayout(QRect rec = QRect(-1, -1, -1, -1), bool testmode = false);

private:
  QList<QLayoutItem *> list;
  int                  m_nbcolumn;
  int                  m_nbimage;
  int                  m_oldx;
  int                  m_oldy;
  int                  m_oldwidth;
  int                  m_oldheight;
  int                  cached_width;
  int                  cached_hfw;

};

#endif
