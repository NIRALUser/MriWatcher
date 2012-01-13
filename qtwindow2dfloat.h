/*=========================================================================

 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: QtWindow2DFloat.h,v $
 Language:  C++
 Date:      $Date: 2007/09/21 20:45:31 $
 Version:   $Revision: 1.1 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

   This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef QtWindow2DFloat_H
#define QtWindow2DFloat_H

#include "itkNumericTraitsRGBAPixel.h"
#include "itkImage.h"
#include "itkColorTable.h"
#include "itkImageRegionIterator.h"
#include "itkRGBAPixel.h"
// #include "ImageManager.h"
#include "Palette.h"
#include "NeuroLibOpenGLSliceTexture.h"

#include <math.h>
#include <QtOpenGL/QtOpenGL>
// #include <QtOpenGL/qgl.h>
// #include <qgl.h>
#include <QSlider>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
// #include <qpixmap.h>

template <class TPixelImage, class TPixelOverlay>
class ImageManagerFloat;

using namespace itk;

/*! Clicking in a window will cause different events
*  NOP = nothing
*  SELECT = report pixel info
*/
const int NUM_ClickModeTypesFloat = 3;
typedef enum { CM_NOPFloat, CM_SELECTFloat, CM_BOXFloat } ClickModeTypeFloat;
const char ClickModeTypeNameFloat[3][7] =
        {{'N', 'O', 'P', '\0', ' ', ' ', ' '},
        {'S', 'e', 'l', 'e', 'c', 't', '\0'},
        {'B', 'o', 'x', '\0', ' ', ' ', ' '}};

/*! Handling of values outside intensity window range - values above
*    and below can be handled separately
*  IW_MIN = set values outside range to min value
*  IW_MAX = set values outside range to max value
*  IW_FLIP = rescale values to be within range by flipping
*/
const int NUM_ImageModeTypesFloat = 8;
typedef enum { IMG_VALFloat, IMG_INVFloat, IMG_LOGFloat, IMG_DXFloat, IMG_DYFloat, IMG_DZFloat,
               IMG_BLENDFloat, IMG_MIPFloat } ImageModeTypeFloat;
const char ImageModeTypeNameFloat[8][8] =
        {{'V', 'a', 'l', 'u', 'e', '\0', ' ', ' '},
        {'I', 'n', 'v', 'e', 'r', 's', 'e', '\0'},
        {'L', 'o', 'g', '\0', ' ', ' ', ' ', ' '},
        {'D', 'e', 'r', 'i', 'v', '-', 'X', '\0'},
        {'D', 'e', 'r', 'i', 'v', '-', 'Y', '\0'},
        {'D', 'e', 'r', 'i', 'v', '-', 'Z', '\0'},
        {'B', 'l', 'e', 'n', 'd', '\0', ' ', ' '},
        {'M', 'I', 'P', '\0', ' ', ' ', ' ', ' '}};

const int NUM_IWModeTypesFloat = 3;
typedef enum { IW_MINFloat, IW_MAXFloat, IW_FLIPFloat } IWModeTypeFloat;
const char IWModeTypeNameFloat[3][5] =
        {{'M', 'i', 'n', '\0', ' '},
        {'M', 'a', 'x', '\0', ' '},
        {'F', 'l', 'i', 'p', '\0'}};

/*! Structure clickPoint to store the x,y,z and intensity value of a
* point in the image
*/
struct ClickPointFloat
  {
  float x, y, z;
  double value;

  ClickPointFloat(float _x, float _y, float _z, double v)
    : x(_x), y(_y), z(_z), value(v)
  {
  }

  };

/**
* QtGlSliceView : Derived from abstract class SliceView and Fl_Gl_Window
* See SliceView.h for details...
  **/
//
class QtWindow2DFloat :
  public QGLWidget
{
  Q_OBJECT
public:

  typedef float                         ImagePixelType;
  typedef float                         OverlayPixelType;
  typedef itk::Image<ImagePixelType, 3> ImageType;
  typedef itk::Image<unsigned char, 2>  Image2DType;
  typedef itk::RGBAPixel<unsigned char> LabelPixelType;
  typedef itk::Image<LabelPixelType, 2> Image2DLabelType;

  typedef itk::Image<OverlayPixelType, 3> OverlayType;
  typedef itk::Image<OverlayPixelType, 2> Overlay2DType;

  typedef itk::ImageRegionIterator<ImageType>        IteratorType;
  typedef itk::ImageRegionIterator<Image2DType>      Iterator2DType;
  typedef itk::ImageRegionIterator<Image2DLabelType> Iterator2DLabelType;

  typedef ImageType::Pointer   ImagePointer;
  typedef Image2DType::Pointer Image2DPointer;

  typedef OverlayType::Pointer   OverlayPointer;
  typedef Overlay2DType::Pointer Overlay2DPointer;

  typedef ImageType::RegionType   RegionType;
  typedef Image2DType::RegionType Region2DType;

  typedef ImageType::SizeType   SizeType;
  typedef Image2DType::SizeType Size2DType;

  typedef ImageType::IndexType   IndexType;
  typedef Image2DType::IndexType Index2DType;

  typedef ImageManagerFloat<ImagePixelType, OverlayPixelType> ManagerType;
  void SetManager(ManagerType* manager);

  void SetSlider(QSlider* slider);

  void SetSliderValue(int value);

  void  SetLabel(QLabel* label);

  void SetId(int id);

  int GetId();

  void SetCrosshair(int x, int y);

  void GetCrosshair(int& x, int& y);

  void ChangeAlpha(int value);

  void ChangeLabelOverlay(int value);

  void ChangeId(int id);

  void SetRotation(float angle);

  void ShowCrosshair(bool flag);

  void DrawString();

  void OutputString( float x, float y, const char *string );

  void SetImageName(QString imagename);

  void DrawPosition(bool flag);

  void DrawIntensity(bool flag);

  void DrawImageInfo(bool flag);

  void DrawImageName(bool flag);

  void SetImageMinMax(float min, float max);

  void GetImageMinMax(float& min, float& max);

  void SetOverlayMinMax(float min, float max);

  void SetOverlayMinMax2(float min, float max);

  void SetImageInfo(int* size, float* pixdim, QString type);

  bool OverlayVisible();

  void HideOverlay();

  void ShowOverlay();

  void SetZoomFactor(float zoom);

  void SetViewPosition(float posx, float posy);

  QPixmap RenderPixmap();

  void SetDisplayOverlayZero(bool flag);

  float BilinearFunc(int f, float x);

  float CubicFunc(int f, float x);

  float TriangleFunction(float x);

  float SquareFunction(float x);

  float CubicInterpolation(float x);

  float CubicBSpline(float x);

  int Bicubic_Interpol(unsigned char *m_image, int newx, int newy, int xsize, int ysize, float a, float b, int f);

public:
  bool  cValidOverlayData;
  float cOverlayOpacity;
  bool  cShowCrosshair;

  OverlayPointer cOverlayData;
  OverlayPointer cOverlayData2;
  void           (* cViewOverlayCallBack)(void);

  unsigned char * cWinOverlayData;

  typedef itk::ColorTable<float>  ColorTableType;
  typedef ColorTableType::Pointer ColorTablePointer;

  ColorTablePointer cColorTable;

  void initializeGL();

  void resizeGL( int w, int h);

  void paintGL();

public:
/*! FLTK required constructor - must use imData() to complete
  definition */
// QtGlSliceView(int x, int y, int w, int h, const char *l);

  QtWindow2DFloat( QWidget *parent = 0, const char *name = 0 );
  QtWindow2DFloat( QGLFormat glf, QWidget *parent = 0, const char *name = 0 );

  /*! Specify the 3D image to view slice by slice */
  virtual void SetInputImage(ImageType * newImData);

  virtual void SetInputImage2(ImageType * newImData);

  virtual void UnSetInputImage();

  virtual void UnSetInputImage2();

  virtual void UnSetInputOverlay();

  virtual void UnSetInputOverlay2();

  virtual const ImagePointer & GetInputImage2(void) const;

  /*! Specify the 3D image to view as an overlay */
  void SetInputOverlay(OverlayType * newOverlayData);

  void SetInputOverlay2(OverlayType * newOverlayData);

  /*! Return a pointer to the overlay data */
  const OverlayPointer & GetInputOverlay(void) const;

  /*! Turn on/off the viewing of the overlay */
  void  ViewOverlayData(bool newViewOverlayData);

  /*! Status of the overlay - viewed /not viewed */
  bool  ViewOverlayData(void);

  /*! Specify the opacity of the overlay */
  void  OverlayOpacity(float newOverlayOpacity);

  /*! Get the opacity of the overlay */
  float OverlayOpacity(void);

  /*! Called when overlay is toggled or opacity is changed */
  void  ViewOverlayCallBack(void (* newOverlayCallBack)(void) );

  void SetBlendingMode(int mode);

  ColorTableType * GetColorTable(void);

  virtual void size(int w, int h);

  virtual void update();

  /*! Specify the slice to view */
  void      sliceNum(unsigned int newSliceNum);

  /*! What slice is being viewed */
  unsigned int    sliceNum(void);

  void mousePressEvent( QMouseEvent *event );

  void mouseMoveEvent( QMouseEvent *event );

  void SetManualIntensityOn();

  float GetIntensityMin()
  {
    return cIWMin;
  }

  float GetIntensityMax()
  {
    return cIWMax;
  }

  SizeType GetImageSize();

  bool IsCrosshairVisible();

  void SetCrosshairVisible(bool flag);

public slots:

  void ChangeSlice(unsigned int value);

  void SetIntensityMax(float value);

  void SetIntensityMin(float value);

  void ZoomIn();

  void ZoomOut();

  void ResetView();

signals:
  void Clicked(int);

  void Position(int, int, int, float);

protected:

  void   (* cSliceNumCallBack)(void);
  void * cSliceNumArg;
  void   (* cSliceNumArgCallBack)(void * sliceNumArg);

  bool         cValidImData;
  bool         cValidImData2;
  bool         cValidOverlayData2;
  bool         cViewImData;
  ImagePointer cImData;
  ImagePointer cImData2;

  unsigned long cDimSize[3];
  float         cSpacing[3];
  void          (* cViewImDataCallBack)(void);
  void *        cViewImDataArg;
  void          (* cViewImDataArgCallBack)(void *viewImDataArg);

  ClickModeTypeFloat cClickMode;
  float              cClickSelect[3];
  float              cClickSelectV;
  void               (* cClickSelectCallBack)(float x, float y, float z, float v);
  void *             cClickSelectArg;
  void               (* cClickSelectArgCallBack)(float x, float y, float z, float v, void *clickSelectArg);

  float  cBoxMin[3];
  float  cBoxMax[3];
  void   (* cClickBoxCallBack)(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
  void * cClickBoxArg;
  void   (* cClickBoxArgCallBack)(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
                                  void * clickBoxArg);

  float           cIWMin;
  float           cIWMax;
  float           cIWMin2;
  float           cIWMax2;
  IWModeTypeFloat cIWModeMin;
  IWModeTypeFloat cIWModeMax;
  void            (* cIWCallBack)(void);
  void *          cIWArg;
  void            (* cIWArgCallBack)(void * iwArg);

  ImageModeTypeFloat cImageMode;

  bool cFlipX[3];
  bool cFlipY[3];
  bool cFlipZ[3];
  bool cTranspose[3];

  float        cWinZoom;
  unsigned int cWinOrder[3];
  unsigned int cWinOrientation;
  void         (* cWinOrientationCallBack)(void);
  void *       cWinOrientationArg;
  void         (* cWinOrientationArgCallBack)(void *
                                              winOrientationArg);

  int    cWinCenter[3];
  void   (* cWinCenterCallBack)(void);
  void * cWinCenterArg;
  void   (* cWinCenterArgCallBack)(void * winCenterArg);

  bool cViewAxisLabel;
  char cAxisLabelX[3][80];
  char cAxisLabelY[3][80];

  bool cViewOverlayData;
  bool cViewCrosshairs;
  bool cViewValue;
  bool cViewDetails;

  int          cWinMinX;
  int          cWinMaxX;
  int          cWinMaxZ;
  unsigned int cWinSizeX;
  unsigned int cWinOffset;
  int          cWinMinY;
  int          cWinMaxY;
  unsigned int cWinSizeY;
  int          cWinDataSizeX;
  int          cWinDataSizeY;
  unsigned int inDataSizeX;
  unsigned int inDataSizeY;
  // unsigned char  *cWinImData;
  //  unsigned char  *cWinRotData;
  // unsigned short *cWinZBuffer;

  double cDataMax, cDataMin;

  /* list of points clicked and maximum no. of points to be stored*/
  std::list<ClickPointFloat *> cClickedPoints;
  unsigned int                 maxClickPoints;
  int                          cX, cY, cW, cH;

  void clickSelect(float newX, float newY, float newZ);

  void makeRasterFont(void);

  void printString(char *s);

private:
  typedef NeuroLibOpenGLSliceTexture<unsigned char> GreyTextureType;
  GreyTextureType* m_GreyTexture;
  typedef NeuroLibOpenGLSliceTexture<LabelPixelType> LabelTextureType;
  LabelTextureType* m_LabelTexture;

  ManagerType* m_manager;
  QSlider*     m_slicer;
  QLabel*      m_label;

  bool m_isslicer;
  bool m_islabel;
  int  m_id;

  float        view_pos_x;
  float        view_pos_y;
  float        shift_x;
  float        shift_y;
  float        start_x;
  float        start_y;
  unsigned int image_x;
  unsigned int image_y;

  int m_alpha;

  int   m_currentbutton;
  float view_zoom;
  float m_view_zoom;
  int   numslice;

  GLuint                fontOffset;
  Palette::colorstruct* color;

  bool  m_isImage1;
  bool  m_isImage2;
  bool  m_isOverlay1;
  bool  m_isOverlay2;
  float m_rotation;

  bool    m_drawposition;
  bool    m_drawintensity;
  bool    m_drawimagename;
  bool    m_drawimageinfo;
  QString m_imagename;
  QString imagetype;
  float   m_imagemin;
  float   m_imagemax;
  float   m_overlaymin;
  float   m_overlaymax;
  float   m_overlaymin2;
  float   m_overlaymax2;

  int*                 m_imageinfosize;
  float*               m_imageinfopixdim;
  QString              m_imageinfotype;
  bool                 m_overlayvisible;
  Image2DType::Pointer m_2Dimage;
  bool                 m_manualintensity;
  int                  m_x;
  int                  m_y;
  int                  m_blendingmode;
  bool                 m_crosshairvisible;
  bool                 m_displayoverlayzero;

};

#endif
