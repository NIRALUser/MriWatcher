#ifndef IMAGEFRAMEGUI_H
#define IMAGEFRAMEGUI_H

#include <QtGui>
#include "ui_imageframeform.h"

class ImageFrameGUI : public QWidget, public Ui::ImageFrameForm
{
  Q_OBJECT
public:
  ImageFrameGUI(QWidget *parent = 0);
  ~ImageFrameGUI();

  void Select();

  void UnSelect();

  bool IsSelected();

  void dragEnterEvent(QDragEnterEvent *event);

  void dropEvent(QDropEvent *de);

signals:
  void GetFiles(const QString &);

protected:
  bool m_selected;

};

#endif
