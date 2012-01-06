/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: MriWatcherGUIControls.cxx,v $
  Language:  C++
  Date:      $Date: 2009/04/21 13:20:19 $
  Version:   $Revision: 1.7 $
  Author:    Matthieu Jomier

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "MriWatcherGUIControls.h"

#include <qpushbutton.h>
#include <qslider.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qregexp.h> 
#include <qcheckbox.h>
#include <qscrollview.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qmessagebox.h> 

#include <itkMinimumMaximumImageCalculator.h>

#include "MriWatcherHelpGUI.h"
#include "MriWatcherConfig.h"

static const float MAX_PERCENTAGE=1000;

MriWatcherGUIControls::MriWatcherGUIControls( QWidget* parent,  const char* name, WFlags fl )
    : MriWatcherGUI( parent, name,  fl )
{

  g_version->setText(QString("MriWatcher ") + MriWatcher_VERSION );
  
  setAcceptDrops(TRUE);
  m_direction = 0;
  g_frame->setVScrollBarMode(QScrollView::AlwaysOn);
  g_frame->setHScrollBarMode(QScrollView::AlwaysOff);
  m_frame = new MriWatcherFrame(g_frame->viewport());
  connect (m_frame,SIGNAL(GetFiles(const QString&)),this,SLOT(LoadFile(const QString&)));
  connect (m_frame,SIGNAL(UnSelect()),this,SLOT(UnSelect()));

  g_frame->addChild(m_frame);

  QBoxLayout* gm = new QVBoxLayout(m_frame, 5 );
  m_watcher = new MriWatcherFlow( gm );
  m_frame->setMinimumWidth(g_frame->visibleWidth());
  m_NumberOfImages = 0;
  
  m_imagemanager.ChangeAlpha(100);
  g_SliderX->setEnabled(false);

  m_maxsize[0] = 0;
  m_maxsize[1] = 0;
  m_maxsize[2] = 0;
  installEventFilter(this);
  ViewAllImages();
  m_imagemin = 999999;
  m_imagemax = 0;
  m_keyalt = false;
  m_keyctrl = false; //multiselection
  m_overlaymin = 0;
  m_overlaymax = 0;

  g_minslider->setMaxValue(static_cast<int>(MAX_PERCENTAGE));
  g_maxslider->setMaxValue(static_cast<int>(MAX_PERCENTAGE));
  g_maxslider->setValue(0);

  g_minslider->setMinValue(0);
  g_maxslider->setMinValue(0);
  g_minslider->setValue(static_cast<int>(MAX_PERCENTAGE));

}

MriWatcherGUIControls::~MriWatcherGUIControls()
{
  
}


void MriWatcherGUIControls::dragEnterEvent( QDragEnterEvent *e )
{
    // Check if you want the drag...
    if ( QTextDrag::canDecode( e )
      || QImageDrag::canDecode( e )
      || QUrlDrag::canDecode( e ) )
    {
      e->accept();
    }
}


void MriWatcherGUIControls::dropEvent( QDropEvent * e )
{
  QStringList files;
  if ( QUrlDrag::decodeLocalFiles( e, files ) ) {
    for(QStringList::Iterator i=files.begin(); i!=files.end(); ++i)
      {
          LoadFile (*i);
      }
  }
}

void MriWatcherGUIControls::LoadFile(const QString& filename)
{
  ReaderType::Pointer reader = ReaderType::New();
  ImageType::Pointer image = reader->GetOutput();

  reader->SetFileName( filename.latin1() );
  try
  {
      reader->Update();
  }
  catch (itk::ExceptionObject & e)
  {
     //std::cerr << "Exception in file reader " << std::endl;
     //std::cerr << e << std::endl;
     QMessageBox::information( this, "MriWatcher",
                              "Unable to load file:\n"+filename);

     return;
  }

  m_NumberOfImages++;
  ImageFrameGUIControls* m_window = new ImageFrameGUIControls(m_frame);
  connect (m_window,SIGNAL(GetFiles(const QString&)),this,SLOT(LoadOverlay(const QString&)));
  connect (m_window->g_window,SIGNAL(Clicked(int)),this,SLOT(ImageFrameClicked(int)));
  m_watcher->add(m_window);
  m_winlist.push_back(m_window);  
  m_window->g_window->SetManager(&m_imagemanager);
  m_window->g_window->SetSlider(g_SliderX);
  //m_window->g_window->SetLabel(g_LabelX);
  m_window->g_window->SetId(m_direction);

  m_window->g_window->SetImageName(QFileInfo(filename).baseName(true));

  m_window->g_window->DrawPosition(g_drawposition->isChecked());
  m_window->g_window->DrawIntensity(g_drawintensity->isChecked());

  // This string should  only be used for display purposes it is not
  // safe to use this for comparison of types
  //  QString pixeltypestring(reader->GetImageIO()->GetComponentTypeInfo().name());
  QString pixeltypestring(reader->GetImageIO()->GetComponentTypeAsString(reader->GetImageIO()->GetComponentType()));
  m_window->g_window->DrawImageName(g_drawimagename->isChecked());

  m_imagesize =  image->GetLargestPossibleRegion().GetSize();
  m_imagespacing = image->GetSpacing();
   
  int m_cimagesize[3];
  m_cimagesize[0] = m_imagesize[0];
  m_cimagesize[1] = m_imagesize[1];
  m_cimagesize[2] = m_imagesize[2];
  float m_cpixdim[3];
  m_cpixdim[0] = m_imagespacing[0];
  m_cpixdim[1] = m_imagespacing[1];
  m_cpixdim[2] = m_imagespacing[2];

  m_window->g_window->SetImageInfo(m_cimagesize, m_cpixdim, pixeltypestring);
  m_window->g_window->DrawImageInfo(g_drawimageinfo->isChecked());

  typedef MinimumMaximumImageCalculator<ImageType> CalculatorType;
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage(image);
  calculator->Compute();     
  ImageType::PixelType m_min = calculator->GetMinimum();
  ImageType::PixelType m_max = calculator->GetMaximum();


  if (m_max > m_imagemax)
  {
    m_imagemax = m_max;
  }


  if (m_min < m_imagemin)
  {
    m_imagemin = m_min;

  }

  m_window->g_window->SetImageMinMax(m_min, m_max);

  if (m_NumberOfImages == 1) m_imagemanager.SetSourceImage(image);
  m_window->g_window->SetInputImage(image);
  m_window->g_window->ChangeAlpha(100);

  ReDraw();

  g_SliderX->setEnabled(true);

  m_frame->setGeometry(QRect(m_frame->x(),m_frame->y(),m_frame->height(),m_frame->width()));
  m_window->show();

  if (m_winlist.size() == 1)
    m_imagemanager.SetCrosshair(m_imagesize[0]/2,m_imagesize[1]/2,m_imagesize[2]/2);
  else
    m_imagemanager.UpdateCrosshair();

  if (m_imagesize[0] > m_maxsize[0]) m_maxsize[0] = m_imagesize[0];
  if (m_imagesize[1] > m_maxsize[1]) m_maxsize[1] = m_imagesize[1];
  if (m_imagesize[2] > m_maxsize[2]) m_maxsize[2] = m_imagesize[2];

  UpdateLabel();

}

void MriWatcherGUIControls::ChangeMax(int percentage)
{

  ImageType::PixelType max=0; 
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->IsSelected())
    {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max; 
      m_winlist[i]->g_window->GetImageMinMax(m_min,m_max);
      if (m_max>max)
        max=m_max;
    }
  }

  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->IsSelected())
    {
      m_winlist[i]->g_window->SetIntensityMax(max*percentage/MAX_PERCENTAGE);
    }
  }
}

void MriWatcherGUIControls::ChangeMin(int percentage)
{

  ImageType::PixelType max=0; 
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->IsSelected())
    {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max; 
      m_winlist[i]->g_window->GetImageMinMax(m_min,m_max);
      if (m_max>max)
        max=m_max;
    }
  }

  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->IsSelected())
    {
      m_winlist[i]->g_window->SetIntensityMin(max*percentage/MAX_PERCENTAGE);
    }
  }
}


void MriWatcherGUIControls::ResetView()
{
   for (unsigned int i=0;i<m_winlist.size();i++)
    m_winlist[i]->g_window->ResetView();
}

void MriWatcherGUIControls::LoadOverlay(const QString& filename2)
{
  
  //std::cout << "LoadOverlay: " << filename2.latin1() << std::endl;
  ReaderType::Pointer reader = ReaderType::New();
  QString filename = filename2;
  filename.replace( QRegExp("\\"), "/" );
  reader->SetFileName( filename.latin1());
  try
  {
      reader->Update();
  }
  catch (itk::ExceptionObject & e)
  {
     //std::cerr << "Exception in file reader " << std::endl;
     //std::cerr << e << std::endl;
         QMessageBox::information( this, "MriWatcher",
                              "Unable to load file:\n"+filename);
 
    return;
  }

  for (unsigned int i=0;i<m_winlist.size();i++) {
    m_winlist[i]->g_window->SetInputOverlay(reader->GetOutput());
    m_originalimage = reader->GetOutput(); 
    if (m_originalimage.IsNotNull())
      {
	m_imagemanager.SetSourceOverlay(m_originalimage);
	g_SliderX->setEnabled(true);
      }
    g_loadimg->setDown(false);
    g_loadoverlay->setDown(false);
  }
}

void MriWatcherGUIControls::LoadOverlay()
{
  QtImageLoader<PixelType> * m_loader = new QtImageLoader<PixelType>();  
  m_originalimage = m_loader->GetOutput(); 
  if (m_originalimage.IsNotNull())
  {
    //std::cout << "LoadOverlay(): " << m_loader->GetFilePath() << std::endl;
    m_imagemanager.SetSourceOverlay(m_originalimage);
    g_SliderX->setEnabled(true);
  }

  g_loadimg->setDown(false);
  delete m_loader;
  g_loadoverlay->setDown(false);

}

void MriWatcherGUIControls::LoadImg()
{
    QStringList m_filename = QFileDialog::getOpenFileNames("*.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*",QString::null,this); 

    for ( QStringList::Iterator it = m_filename.begin(); it != m_filename.end(); ++it )
         LoadFile(*it);

      g_loadimg->setDown(false);
}

void MriWatcherGUIControls::LoadImg2()
{
  QtImageLoader<PixelType> * m_loader = new QtImageLoader<PixelType>();  
  m_originalimage = m_loader->GetOutput(); 
  if (m_originalimage.IsNotNull())
  {
    m_imagemanager.SetTargetImage(m_originalimage);
    g_alpha->setValue(50);
  }
  g_loadimg2->setDown(false);
}

void MriWatcherGUIControls::DrawImageName()
{
   for (int i=0;i<m_NumberOfImages;i++)
     m_winlist[i]->g_window->DrawImageName(g_drawimagename->isChecked());
}

void MriWatcherGUIControls::DrawImageInfo()
{
   for (int i=0;i<m_NumberOfImages;i++)
     m_winlist[i]->g_window->DrawImageInfo(g_drawimageinfo->isChecked());
}


void MriWatcherGUIControls::DrawPosition()
{
  for (int i=0;i<m_NumberOfImages;i++)
     m_winlist[i]->g_window->DrawPosition(g_drawposition->isChecked());
}

void MriWatcherGUIControls::DrawIntensity()
{
  for (int i=0;i<m_NumberOfImages;i++)
     m_winlist[i]->g_window->DrawIntensity(g_drawintensity->isChecked());
}


void MriWatcherGUIControls::ChangeSliceX(int value)
{
  if (m_direction == 0)
    m_imagemanager.ChangeSliceX(value);
  if (m_direction == 1)
    m_imagemanager.ChangeSliceY(value);
  if (m_direction == 2)
    m_imagemanager.ChangeSliceZ(value);

  UpdateLabel();
}

void MriWatcherGUIControls::ChangeSliceY(int value)
{
  m_imagemanager.ChangeSliceY(value);
  int x,y,z;
  m_imagemanager.GetCrosshair(&x,&y,&z);
  Update3DCursor(x,y,z);
}

void MriWatcherGUIControls::ChangeSliceZ(int value)
{
  m_imagemanager.ChangeSliceZ(value);
  int x,y,z;
  m_imagemanager.GetCrosshair(&x,&y,&z);
  Update3DCursor(x,y,z);
}

void MriWatcherGUIControls::ChangeAlpha(int value)
{
  m_imagemanager.ChangeLabelOverlay(value);
}


void MriWatcherGUIControls::Update3DCursor(int x,int y,int z)
{
}


void MriWatcherGUIControls::ChangeView()
{
  if (g_xview->isChecked()) 
    m_direction = 0;

  if (g_yview->isChecked()) 
    m_direction = 1;

  if (g_zview->isChecked())
    m_direction = 2;

  for (unsigned int i=0;i<m_winlist.size();i++)
    m_winlist[i]->g_window->ChangeId(m_direction);

  UpdateLabel();
}


void MriWatcherGUIControls::ViewSetAllImages()
{
  g_viewall->setChecked(true);
  ViewAllImages();
}

void MriWatcherGUIControls::ViewAllImages()
{
  if (g_viewall->isChecked())
  {
      m_watcher->SetColumn(0);
      
      if (m_NumberOfImages == 0)
        m_frame->setMinimumHeight(0);
      else
      {
        int height;
        int nb_column;
        ComputeMaxColumn(nb_column,height);
        m_frame->setMinimumHeight(height);
        m_watcher->SetColumn(0);
      }
  }
  else
  {
      m_watcher->SetColumn(atoi(g_nbcolumn->text()));
      m_frame->setMinimumWidth(g_frame->visibleWidth());
      if (m_NumberOfImages == 0)
        m_frame->setMinimumHeight(0);
      else
        m_frame->setMinimumHeight((((m_NumberOfImages-1)/atoi(g_nbcolumn->text()))+1)*(g_frame->visibleWidth()/atoi(g_nbcolumn->text())));
  }
  m_watcher->doLayout();
}


void MriWatcherGUIControls::ComputeMaxColumn(int& nb_column,int& height)
{
  int max = 0;
  for (int i=1;i<m_NumberOfImages+1;i++)
  {
    int j = (int)floor(((float)m_NumberOfImages/(float)i)+0.99);
    int w= g_frame->visibleWidth()/i;
    int h= g_frame->visibleHeight()/j;
    if (w<h)
      h = w; 

    if (h>max)
    {
      max = h;
      nb_column = i;
      height = j*h;
    }

    //reverse
    w= g_frame->visibleWidth()/j;
    h= g_frame->visibleHeight()/i;

    if (w<h)
      h = w; 

    if (h>max)
    {
      max = h;
      nb_column = j;
      height = i*h;
    }
  }
}


void MriWatcherGUIControls::ReDraw()
{
 m_frame->setMinimumWidth(g_frame->visibleWidth());
  if (m_NumberOfImages == 0)
      m_frame->setMinimumHeight(0);
  else
  {
    if (g_viewall->isChecked())
    {
        int height;
        int nb_column;
        ComputeMaxColumn(nb_column,height);
        m_frame->setMinimumHeight(height);
        m_watcher->SetColumn(0);
    }
    else
      m_frame->setMinimumHeight((((m_NumberOfImages-1)/atoi(g_nbcolumn->text()))+1)*(g_frame->visibleWidth()/atoi(g_nbcolumn->text())));
  }
}


void MriWatcherGUIControls::resizeEvent(QResizeEvent *e)
{
    ReDraw();
}

void MriWatcherGUIControls::ScreenShot()
{
    //Profile(0);
    QString m_filename = QFileDialog::getSaveFileName(QString::null,"*.bmp",this); 
    if (m_filename.isEmpty())
    {
      g_screenshot->setDown(false);
      return;
    }
    ScreenShot(m_filename);
}

void MriWatcherGUIControls::ScreenShot(QString filename)
{
    QPixmap pixmap = QPixmap::grabWidget( m_frame );
    QPixmap* m_pixmaplist = new QPixmap[m_winlist.size()];
    for (unsigned int i=0;i<m_winlist.size();i++)
    {
      m_pixmaplist[i] =  m_winlist[i]->g_window->renderPixmap();
    }

     if (filename.findRev(".") == -1)
        filename = filename + ".bmp";

    for (unsigned int i=0;i<m_winlist.size();i++)
    {
      QPoint m_point = m_winlist[i]->mapToParent(QPoint(0,0));
      int xpos = m_point.x()+2;
      int ypos = m_point.y()+2;
      bitBlt(&pixmap,xpos,ypos,&m_pixmaplist[i],0,0,m_pixmaplist[i].width(),m_pixmaplist[i].height(),CopyROP);
    }

     filename = filename.mid(0,filename.findRev(".")) + ".bmp";
     pixmap.save(filename, "BMP" );
    g_screenshot->setDown(false);
}

void MriWatcherGUIControls::Combine()
{

  QString m_filename = QFileDialog::getSaveFileName("*.gipl *.hdr *.mha *.mhd",QString::null,this); 

  if (m_filename.isEmpty())
  {  
    g_combine->setDown(false);
    return;
  }

  if (m_filename.findRev(".") == -1)
    m_filename += ".gipl";

    int xc; int yc; int zc;
    m_imagemanager.GetCrosshair(&xc,&yc,&zc);
  
   // if (m_direction == 0)
   //      nbpoint = m_imagesize[0];

  ImageType::Pointer  m_newimage = CreateNewImage(m_imagesize[0],m_imagesize[1],m_NumberOfImages,m_imagespacing[0],m_imagespacing[1],m_imagespacing[2]);
 
  IteratorType m_itnewimage(m_newimage,m_newimage->GetLargestPossibleRegion());
  m_itnewimage.GoToBegin();

  for (int z=0;z<m_NumberOfImages;z++)
  {
    ImageType::Pointer m_image = m_winlist[z]->g_window->GetInputImage2();
    IteratorType m_itimage(m_image,m_image->GetLargestPossibleRegion());
     m_itimage.GoToBegin();
     for (unsigned int i=0;i<zc*m_imagesize[0]*m_imagesize[1];i++)
      ++m_itimage;

    for (unsigned int y=0;y<m_imagesize[0];y++)
      for (unsigned int x=0;x<m_imagesize[1];x++)
      {
        m_itnewimage.Set(m_itimage.Get());
        ++m_itimage;
        ++m_itnewimage;
      }
  }


  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_filename.latin1());
  writer->SetInput(m_newimage);
  writer->Update();

  g_combine->setDown(false);

}


MriWatcherGUIControls::ImageType::Pointer 
MriWatcherGUIControls::CreateNewImage(int sizex,int sizey,int sizez,float dimx,float dimy,float dimz)
{
  int imagesize[3];
  imagesize[0] = sizex;
  imagesize[1] = sizey;
  imagesize[2] = sizez;

  ImageType::Pointer m_outputimage = ImageType::New();
  float values[3];

  values[0]= dimx; 
  values[1]= dimy;
  values[2]= dimz;

  float origin_x= ((imagesize[0]/2)*values[0]*(-1));
  float origin_y=((imagesize[1]/2)*values[1]*(-1));
  float origin_z= ((imagesize[0]/2)*values[2]*(-1));
  float origin[3] = {origin_x, origin_y, origin_z};
 
  ImageType::RegionType region;
  ImageType::SizeType size;
  size[0]= imagesize[0];
  size[1]= imagesize[1];
  size[2]= imagesize[2];
  region.SetSize( size );
  m_outputimage->SetRegions( region );
  m_outputimage->Allocate();
  m_outputimage->SetOrigin(origin);
  m_outputimage->SetSpacing(values);

  return m_outputimage;
}


void MriWatcherGUIControls::UnSelect()
{
  for (unsigned int i=0;i<m_winlist.size();i++)
    m_winlist[i]->UnSelect();
}

void MriWatcherGUIControls::SelectAll()
{
  for (unsigned int i=0;i<m_winlist.size();i++)
    m_winlist[i]->Select();
}


void MriWatcherGUIControls::UpdateIntensitySlider()
{
 bool m_first=true;
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->IsSelected())
    {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max; 
      m_winlist[i]->g_window->GetImageMinMax(m_min,m_max);

      float m_imagemin = m_winlist[i]->g_window->GetIntensityMin();
      float m_imagemax = m_winlist[i]->g_window->GetIntensityMax();

      float m_imagemin_percent=(m_imagemin-m_min)*MAX_PERCENTAGE/(m_max-m_min);
      float m_imagemax_percent=(m_imagemax-m_min)*MAX_PERCENTAGE/(m_max-m_min);

      if (m_first)
      {
        g_maxslider->setValue(static_cast<int>(m_imagemax_percent));
        g_minslider->setValue(static_cast<int>(m_imagemin_percent));
        m_first = false;
      }
      else
      {
        if (m_max > static_cast<int> (g_maxslider->value()))
        {
          g_maxslider->setValue(static_cast<int>(m_imagemax_percent));
        }

        if (m_min < static_cast<int> (g_minslider->value()))
        {
          g_minslider->setValue(static_cast<int>(m_imagemin_percent));
        }
      }
    }
  }
}

void MriWatcherGUIControls::ImageFrameClicked(int button)
{
  setFocus();
  
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    if (m_winlist[i]->g_window == sender())
    {
      if (m_winlist[i]->IsSelected())
        m_winlist[i]->UnSelect();
      else
        m_winlist[i]->Select();
    }
    else
    {
      if (m_keyalt == false && m_keyctrl == false)
      {
        m_winlist[i]->UnSelect();
      }
    }
  }
  
  UpdateIntensitySlider();
}


bool MriWatcherGUIControls::eventFilter(QObject* o, QEvent* e)
{
    switch (e->type())
    {
      case QEvent::KeyPress: 
        if (static_cast<QKeyEvent *>( e )->key()== Key_Right )
        {
          for (int i=m_winlist.size()-2;i>=0 ;i--)
          {
            if (m_winlist[i]->IsSelected())
            { 
              ImageFrameGUIControls* m_image = m_winlist[i];
              m_winlist[i] = m_winlist[i+1];
              m_winlist[i+1] = m_image;
              m_watcher->Switch(i,i+1);
              m_watcher->doLayout();
            }
          }
        }


       if (static_cast<QKeyEvent *>( e )->key()== Key_Left )
        {
          for (unsigned int i=1;i<m_winlist.size();i++)
          {
            if (m_winlist[i]->IsSelected())
            { 
              ImageFrameGUIControls* m_image = m_winlist[i];
              m_winlist[i] = m_winlist[i-1];
              m_winlist[i-1] = m_image;
              m_watcher->Switch(i-1,i);
              m_watcher->doLayout();
            }
          }
        }

       if (static_cast<QKeyEvent *>( e )->key()== Key_Delete )
       { 
         for (unsigned int i=0;i<m_winlist.size();i++)
         {
          if (m_winlist[i]->IsSelected())
          { 
             std::vector<ImageFrameGUIControls*>::iterator it = m_winlist.begin();
             for (unsigned int k=0;k<i;k++)
             {
                ++it;
             }

        
             m_imagemanager.DelWindow2D(m_winlist[i]->g_window);
             m_winlist.erase(it);

             m_watcher->Delete(i);
  
             m_maxsize[0] = 0;
             m_maxsize[1] = 0;
             m_maxsize[2] = 0;

             for (unsigned int j=0;j<m_winlist.size() ;j++)
             {
               QtWindow2DFloat::SizeType m_size = m_winlist[j]->g_window->GetImageSize();
               if (m_size[0] > m_maxsize[0]) m_maxsize[0] = m_size[0];
               if (m_size[1] > m_maxsize[1]) m_maxsize[1] = m_size[1];
               if (m_size[2] > m_maxsize[2]) m_maxsize[2] = m_size[2];
             }

             i--;
             m_NumberOfImages--;
             ReDraw();
             m_watcher->doLayout();
           }
         }
       }

        if ( (static_cast<QKeyEvent *>( e )->key()== Key_A)  && ( static_cast<QKeyEvent *>( e )->state() & ControlButton ))
        { 
          for (unsigned int i=0;i<m_winlist.size();i++)
            m_winlist[i]->Select();
        }

        if (static_cast<QKeyEvent *>( e )->key()== Key_O )
        { 
          QtImageLoader<PixelType> * m_loader = new QtImageLoader<PixelType>();  
          m_originalimage = m_loader->GetOutput(); 
          if (m_originalimage.IsNotNull())
          {
            for (unsigned int i=0;i<m_winlist.size();i++)
              if (m_winlist[i]->IsSelected())
                m_winlist[i]->g_window->SetInputOverlay(m_originalimage);
          }
          delete m_loader;

        }

      if (static_cast<QKeyEvent *>( e )->key()== Key_H )
      { 
        for (unsigned int i=0;i<m_winlist.size();i++)
          if (m_winlist[i]->IsSelected())
          {
            if (m_winlist[i]->g_window->OverlayVisible())
              m_winlist[i]->g_window->HideOverlay();
            else
              m_winlist[i]->g_window->ShowOverlay();
          }
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_C )
      { 
        for (unsigned int i=0;i<m_winlist.size();i++)
          if (m_winlist[i]->IsSelected())
          {
            if (m_winlist[i]->g_window->IsCrosshairVisible())
              m_winlist[i]->g_window->SetCrosshairVisible(false);
            else
              m_winlist[i]->g_window->SetCrosshairVisible(true);
          }
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_S )
      { 
        QtImageLoader<PixelType> * m_loader = new QtImageLoader<PixelType>();  
        m_originalimage = m_loader->GetOutput(); 
        if (m_originalimage.IsNotNull())
        {
          for (unsigned int i=0;i<m_winlist.size();i++)
            if (m_winlist[i]->IsSelected())
              m_winlist[i]->g_window->SetInputImage2(m_originalimage);
        }
        delete m_loader;
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_Plus )
      { 
        for (unsigned int i=0;i<m_winlist.size();i++)
          if (m_winlist[i]->IsSelected())
          {
            if (m_winlist[i]->g_window->cOverlayOpacity*100 < 95)
              m_winlist[i]->g_window->ChangeLabelOverlay((int)(m_winlist[i]->g_window->cOverlayOpacity*100+5));
          }
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_Minus )
      { 
        for (unsigned int i=0;i<m_winlist.size();i++)
          if (m_winlist[i]->IsSelected())
          {
            if (m_winlist[i]->g_window->cOverlayOpacity*100  > 5)
              m_winlist[i]->g_window->ChangeLabelOverlay((int)(m_winlist[i]->g_window->cOverlayOpacity*100-5));
          }
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_Alt )
      {
        m_keyalt = true;
      }

      if (static_cast<QKeyEvent *>( e )->key()== Key_Control )
      {
        m_keyctrl = true;
      }

    break;

    case QEvent::KeyRelease: 
    
      if (static_cast<QKeyEvent *>( e )->key()== Key_Alt )
      {
        m_keyalt = false;
      }
      if (static_cast<QKeyEvent *>( e )->key()== Key_Control )
      {
        m_keyctrl = false;
      }


    break;

    default: break;
  }

  return false;
}

void MriWatcherGUIControls::Help()
{
  MriWatcherHelpGUI* m_help = new MriWatcherHelpGUI(0,0,true);

  m_help->g_version->setText( QString("MriWatcher ") + MriWatcher_VERSION );
  m_help->show();
  g_help->setDown(false);
}

void MriWatcherGUIControls::UpdateLabel()
{
  if (m_direction == 0) 
  {
    g_SliderX->setMaxValue(m_maxsize[2]);
    char text[20];
    sprintf(text,"%i/%i",g_SliderX->value(),m_maxsize[2]-1);
    g_LabelX->setText(text);
  }
    
  if (m_direction == 1)
  {
    g_SliderX->setMaxValue(m_maxsize[1]);
    char text[20];
    sprintf(text,"%i/%i",g_SliderX->value(),m_maxsize[1]-1);
    g_LabelX->setText(text);
  }

  if (m_direction == 2)
  {
       g_SliderX->setMaxValue(m_maxsize[0]);
    char text[20];
    sprintf(text,"%i/%i",g_SliderX->value(),m_maxsize[0]-1);
    g_LabelX->setText(text);
  }
}


void MriWatcherGUIControls::ChangeImageAlpha(int value)
{
  m_imagemanager.ChangeAlpha(value);
}

void MriWatcherGUIControls::SetBlendingMode()
{
  if (g_blendingmode->isChecked())
    m_imagemanager.SetBlendingMode(0);
  else
    m_imagemanager.SetBlendingMode(1);
}


void MriWatcherGUIControls::ChangeOverlayMinMax()
{
  m_overlaymin = atoi(g_overlaymin->text());
  m_overlaymax = atoi(g_overlaymax->text());
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    m_winlist[i]->g_window->SetOverlayMinMax(m_overlaymin,m_overlaymax);
    m_winlist[i]->g_window->update();
  }
}

void MriWatcherGUIControls::ChangeOverlayZero()
{
  for (unsigned int i=0;i<m_winlist.size();i++)
  {
    m_winlist[i]->g_window->SetDisplayOverlayZero(g_overlayzero->isChecked());
    m_winlist[i]->g_window->update();
  }
}
