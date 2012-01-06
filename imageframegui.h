#ifndef IMAGEFRAMEGUI_H
#define IMAGEFRAMEGUI_H

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
    

signals:
    void GetFiles(const QString &);

protected:
    bool m_selected;

};

#endif
