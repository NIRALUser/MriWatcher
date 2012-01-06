#include "imageframegui.h"

ImageFrameGUI::ImageFrameGUI(QWidget *parent)
{
    setupUi(this);
    m_selected = false;
}

ImageFrameGUI::~ImageFrameGUI()
{
}

void ImageFrameGUI::Select()
{
    m_selected =  true;
    QPalette pal = palette();
    pal.setColor(backgroundRole(), QColor(255,50,50));
    setPalette(pal);
//    printf("enter select\n");
    //setBackgroundColor(QColor(255,50,50));
}

void ImageFrameGUI::UnSelect()
{
    m_selected = false;
    QPalette pal = palette();
    pal.setColor(backgroundRole(), QColor(193,193,193));
    setPalette(pal);
//    printf("enter unselect\n");
    //setBackgroundColor(QColor(193,193,193));
}

bool ImageFrameGUI::IsSelected()
{
    return m_selected;
}
