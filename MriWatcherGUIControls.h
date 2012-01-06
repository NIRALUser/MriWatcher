/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: MriWatcherGUIControls.h,v $
  Language:  C++
  Date:      $Date: 2009/01/09 19:19:37 $
  Version:   $Revision: 1.5 $
  Author:    Matthieu Jomier

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef MriWatcherGUIControls_h
#define MriWatcherGUIControls_h

#include "MriWatcherGUI.h"
#include "ImageManagerFloat.h"
#include "QtImageLoader.h"
#include "QtWindow2DFloat.h"
//#include "QtWindow2D.h"


//#include <itkImageSpatialObject.h>
#include <itkImageRegionIterator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "MriWatcherFlow.h"
#include "MriWatcherFrame.h"
#include "ImageFrameGUIControls.h"

class MriWatcherGUIControls : public MriWatcherGUI
{ 
public:
    
  MriWatcherGUIControls( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~MriWatcherGUIControls();

  typedef float PixelType;
  typedef float OverlayPixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<unsigned char,2> Image2DType;
  typedef itk::Image<OverlayPixelType,3> OverlayType;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  typedef itk::ImageRegionIterator<Image2DType> Iterator2DType;
  typedef itk::ImageFileWriter<ImageType>   WriterType;
  typedef itk::ImageFileReader<ImageType>   ReaderType;
 


  void LoadImg();
  void ChangeSliceX(int value);
  void ChangeSliceY(int value);
  void ChangeSliceZ(int value);
  void ChangeAlpha(int value);
  void ChangeImageAlpha(int value);
  void Update3DCursor(int x,int y,int z);
  void ChangeView();
  void resizeEvent(QResizeEvent *e);
  void DrawImageName();
  void DrawImageInfo();
  void DrawPosition();
  void DrawIntensity();
  void ViewSetAllImages();
  void ViewAllImages();
  void ScreenShot();
  void ScreenShot(QString filename);
  void ComputeMaxColumn(int& nb_column,int& height);
  void ResetView();
  void LoadOverlay();
  void Combine();
  ImageType::Pointer CreateNewImage(int sizex,int sizey,int sizez,float dimx,float dimy,float dimz);
  bool eventFilter(QObject* o, QEvent* e);
  void ReDraw();
  void Help();
  void UpdateLabel();
  void LoadImg2();
  void SetBlendingMode();
  void UpdateIntensitySlider();
  void ChangeOverlayMinMax();
  void ChangeOverlayZero();

public slots:
  void ChangeMin(int);
  void ChangeMax(int);
  void LoadFile(const QString& filename);
  void ImageFrameClicked(int button);
  void UnSelect();
  void SelectAll();
  void LoadOverlay(const QString& filename2);

protected:
    void dragEnterEvent( QDragEnterEvent * );
    void dropEvent( QDropEvent * );

public:
  // The number of images being viewed
  int m_NumberOfImages;
  // Selectes the plane of slicing i.e. (Axial, Saggital, Coronal)
  int m_direction;

  // The maximum image size over all images
  unsigned int m_maxsize[3];
  // The minimum intensity over all images
  float m_imagemin;
  // The maximum intensity over all images
  float m_imagemax;
  unsigned int m_overlaymin;
  unsigned int m_overlaymax;

  ImageManagerFloat<PixelType,OverlayPixelType> m_imagemanager;
  ImageType::SizeType m_imagesize;
  ImageType::SpacingType m_imagespacing;
  ImageType::Pointer m_originalimage;

  std::vector<ImageFrameGUIControls*> m_winlist;
  MriWatcherFrame *m_frame;
  MriWatcherFlow *m_watcher;


private:
  bool m_keyalt;
  bool m_keyctrl;
};

#endif
