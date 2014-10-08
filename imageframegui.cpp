#include "imageframegui.h"

ImageFrameGUI::ImageFrameGUI(QWidget *parent)
{
  setupUi(this);
  setAcceptDrops(true);
  m_selected = false;
}

ImageFrameGUI::~ImageFrameGUI()
{
}

void ImageFrameGUI::Select()
{
  m_selected =  true;
  QPalette pal = palette();
  pal.setColor(backgroundRole(), QColor(255, 50, 50) );
  setPalette(pal);
//    printf("enter select\n");
// setBackgroundColor(QColor(255,50,50));
}

void ImageFrameGUI::UnSelect()
{
  m_selected = false;
  QPalette pal = palette();
  pal.setColor(backgroundRole(), QColor(193, 193, 193) );
  setPalette(pal);
//    printf("enter unselect\n");
// setBackgroundColor(QColor(193,193,193));
}

bool ImageFrameGUI::IsSelected()
{
  return m_selected;
}

void ImageFrameGUI::dragEnterEvent(QDragEnterEvent *event)
{
  //   if (QTextDrag::canDecode(event))
    {
    event->accept();
    }
}

void ImageFrameGUI::dropEvent(QDropEvent *de)
{
  QList<QUrl> urls;
  urls = de->mimeData()->urls();
  for( int i = 0; i < urls.size(); ++i )
    {
    emit GetFiles(urls.at(i).toLocalFile() );
    }
}
