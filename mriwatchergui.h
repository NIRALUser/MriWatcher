#ifndef MRIWATCHERGUI_H
#define MRIWATCHERGUI_H

#include <QWidget>
#include <QLabel>

#include <itkImageRegionIterator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "ui_mriwatcherform.h"
// #include "mriwatcherhelp.h"
#include "imageframegui.h"
#include "mriwatcherframe.h"
#include "qtwindow2dfloat.h"
#include "imagemanagerfloat.h"
#include "imageframelayout.h"

/*
namespace Ui {
    class MriWatcherForm;
}
*/

class MriWatcherGUI : public QWidget, private Ui::MriWatcherForm
{
  Q_OBJECT
public:
  explicit MriWatcherGUI(QWidget *parent = 0);
  ~MriWatcherGUI();
  void ScreenShot(QString);
  void resizeEvent(QResizeEvent *);

  bool eventFilter(QObject *, QEvent *);

  void SetViewAllImages();
    
    typedef float PixelType;
    typedef float OverlayPixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef itk::Image<unsigned char,2> Image2DType;
    typedef itk::Image<OverlayPixelType,3> OverlayType;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    typedef itk::ImageRegionIterator<Image2DType> Iterator2DType;
    typedef itk::ImageFileWriter<ImageType>   WriterType;
    typedef itk::ImageFileReader<ImageType>   ReaderType;
public slots:
  void LoadFile(const QString &);

  void LoadImg();

  void LoadOverlay();

  void LoadOverlay(const QString &);

  void LoadImg2();

  void UpdateLabel();

  void ChangeView();

  void ChangeSlice(int);

  void ChangeOverlayAlpha(int);

  void DrawImageName(int);

  void DrawImageInfo(int);

  void DrawImagePosition(int);

  void DrawImageIntensity(int);

  void ChangeIntensityMin(int);

  void ChangeIntensityMax(int);

  void SetBlendingMode();

  void ChangeImageAlpha(int);

  void ViewOptions();

  void ChangeColumn();

  void ResetView();

  void ScreenShot();

  void Combine();

  void Help();

  void ChangeOverlayMinMax();

  void ChangeOverlayZero();

  void ImageFrameClicked();

  void SelectAll();

  void UnSelectAll();

private:
  void ComputeMaxColumn(int &, int &);
  void ComputeHeightWidth(int &, int &, int &);
  void ReDraw();

  void UpdateIntensitySlider();

  ImageType::Pointer CreateNewImage(int sizex, int sizey, int sizez, float dimx, float dimy, float dimz);

//    void UpdateLabel();
//    void ChangeView();
public:
  // The number of images being viewed
  int m_numberofimages;
  // Selectes the plane of slicing i.e. (Axial, Saggital, Coronal)
  int m_direction;
  // The maximum image size over all images
  unsigned int m_maxsize[3];
  // The minimum intensity over all images
  float m_imagemin;
  // The maximum intensity over all images
  float        m_imagemax;
  unsigned int m_overlaymin;
  unsigned int m_overlaymax;

  ImageManagerFloat<PixelType, OverlayPixelType> m_imagemanager;
  ImageType::SizeType                            m_imagesize;
  ImageType::SpacingType                         m_imagespacing;
  ImageType::Pointer                             m_originalimage;

  std::vector<ImageFrameGUI *> m_imageframelist;
  MriWatcherFrame *            m_frame;
  ImageFrameLayout *           m_imageframelayout;
protected:
  void dragEnterEvent(QDragEnterEvent *);

  void dropEvent(QDropEvent *);

private:
  bool m_keyalt;
  bool m_keyctrl;

};

#endif // MRIWATCHERGUI_H
