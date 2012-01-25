#include <QFileDialog>
#include <QMessageBox>
#include <itkMinimumMaximumImageCalculator.h>
#include "itkImageFileReader.h"
#include "mriwatchergui.h"
// #include "ui_mriwatcherform.h"

#include "itkAnalyzeImageIO.h"
#include "itkImage.h"
#include "itkOrientImageFilter.h"

static const float MAX_PERCENTAGE = 1000;

MriWatcherGUI::MriWatcherGUI(QWidget *parent)
{
  setupUi(this);
  installEventFilter(this);
  setAcceptDrops(true);

  connect(g_loadimg, SIGNAL( clicked() ), this, SLOT( LoadImg() ) );
  connect(g_loadoverlay, SIGNAL( clicked() ), this, SLOT( LoadOverlay() ) );
  connect(g_loadimg2, SIGNAL( clicked() ), this, SLOT( LoadImg2() ) );
  connect(g_reset_view, SIGNAL( clicked() ), this, SLOT( ResetView() ) );
  connect(g_screenshot, SIGNAL( clicked() ), this, SLOT( ScreenShot() ) );
  connect(g_combine, SIGNAL( clicked() ), this, SLOT( Combine() ) );
  connect(g_help, SIGNAL( clicked() ), this, SLOT( Help() ) );
  connect(g_xview, SIGNAL( toggled(bool) ), this, SLOT( ChangeView() ) );
  connect(g_yview, SIGNAL( toggled(bool) ), this, SLOT( ChangeView() ) );
  connect(g_zview, SIGNAL( toggled(bool) ), this, SLOT( ChangeView() ) );
  connect(g_slice_slider, SIGNAL( sliderMoved(int) ), this, SLOT( ChangeSlice(int) ) );
//    connect(g_slice_slider, SIGNAL( valueChanged(int) ), this, SLOT( ChangeSlice(int) ));
  connect(g_overlay_alpha_slider, SIGNAL( valueChanged(int) ), this, SLOT( ChangeOverlayAlpha(int) ) );
  connect(g_draw_name, SIGNAL( stateChanged(int) ), this, SLOT( DrawImageName(int) ) );
  connect(g_draw_info, SIGNAL( stateChanged(int) ), this, SLOT( DrawImageInfo(int) ) );
  connect(g_draw_position, SIGNAL( stateChanged(int) ), this, SLOT( DrawImagePosition(int) ) );
  connect(g_draw_intensity, SIGNAL( stateChanged(int) ), this, SLOT( DrawImageIntensity(int) ) );
  connect(g_overlayzero, SIGNAL( stateChanged(int) ), this, SLOT( ChangeOverlayZero() ) );
  connect(g_intensity_min_slider, SIGNAL( valueChanged(int) ), this, SLOT( ChangeIntensityMin(int) ) );
  connect(g_intensity_max_slider, SIGNAL( valueChanged(int) ), this, SLOT( ChangeIntensityMax(int) ) );
  connect(g_blending_mode, SIGNAL( stateChanged(int) ), this, SLOT( SetBlendingMode() ) );
  connect(g_alpha, SIGNAL( valueChanged(int) ), this, SLOT( ChangeImageAlpha(int) ) );
  connect(g_viewall, SIGNAL( toggled(bool) ), this, SLOT( ViewOptions() ) );
  connect(g_viewcolumn, SIGNAL( toggled(bool) ), this, SLOT( ViewOptions() ) );
  connect(g_nbcolumn, SIGNAL( returnPressed() ), this, SLOT( ViewOptions() ) );
  connect(g_overlaymin, SIGNAL( returnPressed() ), this, SLOT( ChangeOverlayMinMax() ) );
  connect(g_overlaymax, SIGNAL( returnPressed() ), this, SLOT( ChangeOverlayMinMax() ) );
  connect(g_desired_orientation, SIGNAL( returnPressed() ), this, SLOT( ChangeDesiredOrientation() ));


  m_direction = 0;
  m_numberofimages = 0;
  m_maxsize[0] = 0;
  m_maxsize[1] = 0;
  m_maxsize[2] = 0;
  m_imagemin = 999999;
  m_imagemax = 0;
  m_keyalt = false;
  m_keyctrl = false;   // multiselection
  m_overlaymin = 0;
  m_overlaymax = 0;
  m_imagemanager.ChangeAlpha(100);

  g_slice_slider->setEnabled(false);
  g_overlay_alpha_slider->setEnabled(false);

  g_intensity_min_slider->setMinimum(0);
  g_intensity_max_slider->setMinimum(0);
  g_intensity_min_slider->setMaximum(static_cast<int>(MAX_PERCENTAGE) );
  g_intensity_max_slider->setMaximum(static_cast<int>(MAX_PERCENTAGE) );
  g_intensity_min_slider->setValue(0);
  g_intensity_max_slider->setValue(static_cast<int>(MAX_PERCENTAGE) );

  m_frame = new MriWatcherFrame(g_scrollArea);

  connect(m_frame, SIGNAL( UnSelectAll() ), this, SLOT( UnSelectAll() ) );
  connect(m_frame, SIGNAL( GetFiles(const QString &) ), this, SLOT( LoadFile(const QString &) ) );
  //   m_imageframelayout = new ImageFrameLayout();
  //   QBoxLayout* gm = new QVBoxLayout(m_frame);
  m_imageframelayout = new ImageFrameLayout();
//    g_scrollArea->setLayout(m_imageframelayout);
  g_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  g_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // g_scrollArea->verticalScrollBar()->setRange();
  g_scrollArea->setWidget(m_frame);
  m_frame->setLayout(m_imageframelayout);

  g_name_version->setText(QString("MriWatcher") + "  " + MriWatcher_VERSION);
  g_tips->setText(
    "\nQuick Tips:\n- Left mouse: Move\n- Right mouse: Zoom\n- Middle mouse: Select\n- Shift + mouse: Global");
/*
     imageLabel = new QLabel;
     imageLabel->setBackgroundRole(QPalette::Base);
     imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     imageLabel->setScaledContents(true);

     g_scrollArea->setBackgroundRole(QPalette::Dark);
     g_scrollArea->setWidget(imageLabel);
     */
}

MriWatcherGUI::~MriWatcherGUI()
{
//    delete ui;
}

void MriWatcherGUI::LoadImg(void)
{
  QStringList filenames = QFileDialog::getOpenFileNames(
      this,
      tr("Select one or more image files to open"),
      QDir::currentPath(),
      tr("*.jpg *.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*") );
  for( QStringList::Iterator it = filenames.begin(); it != filenames.end(); ++it )
    {
    LoadFile(*it);
    }
}

void MriWatcherGUI::LoadFile(const QString& filename)
{
  ReaderType::Pointer reader = ReaderType::New();
  ImageType::Pointer  image = reader->GetOutput();

  reader->SetFileName( filename.toLatin1().constData() );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    // std::cerr << "Exception in file reader " << std::endl;
    // std::cerr << e << std::endl;
    QMessageBox::information( this, "MriWatcher",
                              "Unable to load file:\n" + filename);

    return;
    }

    if (g_load_RAI->isChecked())
    {
        image = ChangeOrientation(image);
    }

  m_numberofimages++;

  ImageFrameGUI* m_imageframe = new ImageFrameGUI(m_frame);

  connect(m_imageframe->g_imageframe, SIGNAL( Clicked(int) ), this, SLOT( ImageFrameClicked() ) );
  connect(m_imageframe, SIGNAL( GetFiles(const QString &) ), this, SLOT( LoadOverlay(const QString &) ) );

  m_imageframe->g_imageframe->SetManager(&m_imagemanager);
  m_imageframe->g_imageframe->SetSlider(g_slice_slider);
  m_imageframe->g_imageframe->SetId(m_direction);
  QFileInfo info(filename);
  m_imageframe->g_imageframe->SetImageName(info.baseName() );
  m_imageframe->g_imageframe->DrawPosition(g_draw_position->isChecked() );
  m_imageframe->g_imageframe->DrawIntensity(g_draw_intensity->isChecked() );

  // This string should  only be used for display purposes it is not
  // safe to use this for comparison of types
  //  QString pixeltypestring(reader->GetImageIO()->GetComponentTypeInfo().name());
  QString pixeltypestring(QString::fromStdString(reader->GetImageIO()->GetComponentTypeAsString(
                                                   reader->GetImageIO()->GetComponentType() ) ) );
  m_imageframe->g_imageframe->DrawImageName(g_draw_name->isChecked() );

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

  m_imageframe->g_imageframe->SetImageInfo(m_cimagesize, m_cpixdim, pixeltypestring);
  m_imageframe->g_imageframe->DrawImageInfo(g_draw_info->isChecked() );

  typedef MinimumMaximumImageCalculator<ImageType> CalculatorType;
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage(image);
  calculator->Compute();
  ImageType::PixelType m_min = calculator->GetMinimum();
  ImageType::PixelType m_max = calculator->GetMaximum();
  if( m_max > m_imagemax )
    {
    m_imagemax = m_max;
    }
  if( m_min < m_imagemin )
    {
    m_imagemin = m_min;
    }

  m_imageframe->g_imageframe->SetImageMinMax(m_min, m_max);

  if( m_numberofimages == 1 )
    {
    m_imagemanager.SetSourceImage(image);
    }
  m_imageframe->g_imageframe->SetInputImage(image);
  m_imageframe->g_imageframe->ChangeAlpha(100);

  ReDraw();

  g_slice_slider->setEnabled(true);

  m_frame->setGeometry(QRect(m_frame->x(), m_frame->y(), m_frame->height(), m_frame->width() ) );

  // gridLayout_imageWindows->addWidget(m_imageframe);

  m_imageframelayout->addWidget(m_imageframe);
  m_imageframelist.push_back(m_imageframe);
  // m_imageframe->setLayout(m_imageframelayout);

  m_imageframe->show();
//    g_scrollArea->setWidget(m_imageframe);

  if( m_imageframelist.size() == 1 )
    {
    m_imagemanager.SetCrosshair(m_imagesize[0] / 2, m_imagesize[1] / 2, m_imagesize[2] / 2);
    }
  else
    {
    m_imagemanager.UpdateCrosshair();
    }

  if( m_imagesize[0] > m_maxsize[0] )
    {
    m_maxsize[0] = m_imagesize[0];
    }
  if( m_imagesize[1] > m_maxsize[1] )
    {
    m_maxsize[1] = m_imagesize[1];
    }
  if( m_imagesize[2] > m_maxsize[2] )
    {
    m_maxsize[2] = m_imagesize[2];
    }

  UpdateLabel();

}

void MriWatcherGUI::LoadOverlay()
{
  QString filename = QFileDialog::getOpenFileName(
      this,
      tr("Select one image files to open"),
      QDir::currentPath(),
      tr("*.jpg *.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*") );

  if( filename != NULL )
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( filename.toLatin1().constData() );
    try
      {
      reader->Update();
      }
    catch( itk::ExceptionObject & e )
      {
      QMessageBox::information( this, "MriWatcher", "Unable to load file:\n" + filename.toLatin1() );
      return;
      }

    m_originalimage = reader->GetOutput();

        if (g_load_RAI->isChecked())
        {    
            m_originalimage = ChangeOrientation(m_originalimage);
        }

    if( m_originalimage.IsNotNull() )
      {
      m_imagemanager.SetSourceOverlay(m_originalimage);
      g_slice_slider->setEnabled(true);
      g_overlay_alpha_slider->setEnabled(true);
      }

    g_loadimg->setDown(false);
    g_loadoverlay->setDown(false);
    }
}

void MriWatcherGUI::LoadOverlay(const QString& filename2)
{
  ReaderType::Pointer reader = ReaderType::New();
  QString             filename = filename2;

  filename.replace( QRegExp("\\"), "/" );
  reader->SetFileName( filename.toLatin1().constData() );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    // std::cerr << "Exception in file reader " << std::endl;
    // std::cerr << e << std::endl;
    QMessageBox::information( this, "MriWatcher", "Unable to load file:\n" + filename);
    return;
    }
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->g_imageframe->SetInputOverlay(reader->GetOutput() );
    m_originalimage = reader->GetOutput();

    if (g_load_RAI->isChecked())
    {
        m_originalimage = ChangeOrientation(m_originalimage);
    }        
 
    if( m_originalimage.IsNotNull() )
      {
      m_imagemanager.SetSourceOverlay(m_originalimage);
      g_slice_slider->setEnabled(true);
      g_overlay_alpha_slider->setEnabled(true);
      }
    g_loadimg->setDown(false);
    g_loadoverlay->setDown(false);
    }
}

void MriWatcherGUI::LoadImg2()
{
  QString filename = QFileDialog::getOpenFileName(
      this,
      tr("Select one image files to open"),
      QDir::currentPath(),
      tr("*.jpg *.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*") );

  if( filename != NULL )
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( filename.toLatin1().constData() );
    try
      {
      reader->Update();
      }
    catch( itk::ExceptionObject & e )
      {
      QMessageBox::information( this, "MriWatcher", "Unable to load file:\n" + filename.toLatin1() );
      return;
      }

    m_originalimage = reader->GetOutput();

        if (g_load_RAI->isChecked())
        {
            m_originalimage = ChangeOrientation(m_originalimage);
        }

    if( m_originalimage.IsNotNull() )
      {
      m_imagemanager.SetTargetImage(m_originalimage);
      g_alpha->setValue(50);
//	    g_overlay_alpha_slider->setEnabled(true);
      }
    g_loadimg2->setDown(false);
    }
}

void MriWatcherGUI::UpdateLabel()
{
  if( m_numberofimages > 0 )
    {
    char text[20];
    if( m_direction == 0 )
      {
      g_slice_slider->setMaximum(m_maxsize[2]);
      sprintf(text, "%i/%i", g_slice_slider->value(), m_maxsize[2] - 1);
      }
    if( m_direction == 1 )
      {
      g_slice_slider->setMaximum(m_maxsize[1]);
      sprintf(text, "%i/%i", g_slice_slider->value(), m_maxsize[1] - 1);
      }

    if( m_direction == 2 )
      {
      g_slice_slider->setMaximum(m_maxsize[0]);
      sprintf(text, "%i/%i", g_slice_slider->value(), m_maxsize[0] - 1);
      }
    g_slice_label->setText(text);
    }
}

void MriWatcherGUI::ChangeSlice(int value)
{
  if( m_direction == 0 )
    {
    m_imagemanager.ChangeSliceX(value);
    }
  if( m_direction == 1 )
    {
    m_imagemanager.ChangeSliceY(value);
    }
  if( m_direction == 2 )
    {
    m_imagemanager.ChangeSliceZ(value);
    }

  UpdateLabel();
}

void MriWatcherGUI::ChangeView()
{
  if( g_xview->isChecked() )
    {
    m_direction = 0;
    }

  if( g_yview->isChecked() )
    {
    m_direction = 1;
    }

  if( g_zview->isChecked() )
    {
    m_direction = 2;
    }
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->g_imageframe->ChangeId(m_direction);
    }

  UpdateLabel();

}

void MriWatcherGUI::ChangeOverlayMinMax()
{
  m_overlaymin = g_overlaymin->text().toInt();
  m_overlaymax = g_overlaymax->text().toInt();
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->g_imageframe->SetOverlayMinMax(m_overlaymin, m_overlaymax);
    m_imageframelist[i]->g_imageframe->update();
    }
}

void MriWatcherGUI::ChangeOverlayZero()
{
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->g_imageframe->SetDisplayOverlayZero(g_overlayzero->isChecked() );
    m_imageframelist[i]->g_imageframe->update();
    }
}

void MriWatcherGUI::ChangeOverlayAlpha(int value)
{
  m_imagemanager.ChangeLabelOverlay(value);
}

void MriWatcherGUI::DrawImageName(int state)
{
  for( int i = 0; i < m_numberofimages; i++ )
    {
    m_imageframelist[i]->g_imageframe->DrawImageName(state);
    }
}

void MriWatcherGUI::DrawImageInfo(int state)
{
  for( int i = 0; i < m_numberofimages; i++ )
    {
    m_imageframelist[i]->g_imageframe->DrawImageInfo(state);
    }
}

void MriWatcherGUI::DrawImagePosition(int state)
{
  for( int i = 0; i < m_numberofimages; i++ )
    {
    m_imageframelist[i]->g_imageframe->DrawPosition(state);
    }
}

void MriWatcherGUI::DrawImageIntensity(int state)
{
  for( int i = 0; i < m_numberofimages; i++ )
    {
    m_imageframelist[i]->g_imageframe->DrawIntensity(state);
    }
}

void MriWatcherGUI::ChangeIntensityMax(int percentage)
{
  ImageType::PixelType max = 0;
  int                  count = 0;
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->IsSelected() )
      {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max;
      m_imageframelist[i]->g_imageframe->GetImageMinMax(m_min, m_max);
      if( m_max > max )
        {
        max = m_max;
        }
      }
    else
      {
      count++;
      }
    }
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->IsSelected() )
      {
      m_imageframelist[i]->g_imageframe->SetIntensityMax(max * percentage / MAX_PERCENTAGE);
      }
    if( count == m_numberofimages )
      {
      m_imageframelist[i]->g_imageframe->SetIntensityMax(m_imagemax * percentage / MAX_PERCENTAGE);
      }
    }
  g_intensity_max_slider->setValue(percentage);
}

void MriWatcherGUI::ChangeIntensityMin(int percentage)
{
  ImageType::PixelType max = 0;
  int                  count = 0;
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->IsSelected() )
      {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max;
      m_imageframelist[i]->g_imageframe->GetImageMinMax(m_min, m_max);
      if( m_max > max )
        {
        max = m_max;
        }
      }
    else
      {
      count++;
      }
    }
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->IsSelected() )
      {
      m_imageframelist[i]->g_imageframe->SetIntensityMin(max * percentage / MAX_PERCENTAGE);
      }
    if( count == m_numberofimages )
      {
      m_imageframelist[i]->g_imageframe->SetIntensityMin(m_imagemax * percentage / MAX_PERCENTAGE);
      }
    }
  g_intensity_min_slider->setValue(percentage);
}

void MriWatcherGUI::ChangeImageAlpha(int value)
{
  m_imagemanager.ChangeAlpha(value);
}

void MriWatcherGUI::SetBlendingMode()
{
  if( g_blending_mode->isChecked() )
    {
    m_imagemanager.SetBlendingMode(1);
    }
  else
    {
    m_imagemanager.SetBlendingMode(0);
    }
}

void MriWatcherGUI::ComputeMaxColumn(int& nb_column, int& height)
{
  int max = 0;
  for( int i = 1; i < m_numberofimages + 1; i++ )
    {
    int j = (int)floor( ( (float)m_numberofimages / (float)i) + 0.99);
    int w = g_scrollArea->viewport()->size().width() / i;
    int h = g_scrollArea->viewport()->size().height() / j;
    if( w < h )
      {
      h = w;
      }

    if( h > max )
      {
      max = h;
      nb_column = i;
      height = j * h;
      }

    // printf("i=%d, j=%d, w=%d, h=%d, max=%d, nb_column=%d, height=%d\n", i,j,w,h,max,nb_column,height);

    // reverse
    w = g_scrollArea->viewport()->size().width() / j;
    h = g_scrollArea->viewport()->size().height() / i;

    if( w < h )
      {
      h = w;
      }

    if( h > max )
      {
      max = h;
      nb_column = j;
      height = i * h;
      }

    // printf("i=%d, j=%d, w=%d, h=%d, max=%d, nb_column=%d, height=%d\n", i,j,w,h,max,nb_column,height);
    }
}

void MriWatcherGUI::ReDraw()
{
  m_frame->setMinimumWidth(g_scrollArea->viewport()->size().width() );
  if( m_numberofimages == 0 )
    {
    m_frame->setMinimumHeight(0);
    }
  else
    {
    if( g_viewall->isChecked() )
      {
      int height;
      int nb_column;
      ComputeMaxColumn(nb_column, height);
      m_frame->setMinimumHeight(height);
      m_imageframelayout->setColumn(nb_column);
      }
    else
      {
      int nb_column = g_nbcolumn->text().toInt();
      m_frame->setMinimumHeight( ( ( (m_numberofimages
                                      - 1) / nb_column) + 1) * (g_scrollArea->viewport()->size().width() / nb_column) );
      m_imageframelayout->setColumn(nb_column);
      }
    }
}

void MriWatcherGUI::SetViewAllImages()
{
  g_viewall->setChecked(true);
  ViewOptions();
}

void MriWatcherGUI::ViewOptions()
{
// printf("enter viewoptions\n");
  if( g_viewall->isChecked() )
    {
    m_imageframelayout->setColumn(0);

    if( m_numberofimages == 0 )
      {
      m_frame->setMinimumHeight(0);
      }
    else
      {
      int height;
      int nb_column;
      ComputeMaxColumn(nb_column, height);

// printf("view all: height=%d, nb_column=%d\n", height, nb_column);

      m_frame->setMinimumHeight(height);
      m_imageframelayout->setColumn(nb_column);
      }
    }
  else
    {
    int nb_column = g_nbcolumn->text().toInt();
    // printf("nb_column=%d\n", nb_column);
    if( nb_column != 0 )
      {
      m_imageframelayout->setColumn(nb_column);
      m_frame->setMinimumWidth(g_scrollArea->viewport()->size().width() );
      if( m_numberofimages == 0 )
        {
        m_frame->setMinimumHeight(0);
        }
      else
        {
        m_frame->setMinimumHeight( ( ( (m_numberofimages
                                        - 1) / nb_column) + 1) * (g_scrollArea->viewport()->size().width() / nb_column) );
        }
      }
    }
  m_imageframelayout->doLayout();
}

void MriWatcherGUI::ResetView()
{
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->g_imageframe->ResetView();
    }
}

void MriWatcherGUI::ScreenShot()
{
  QString m_filename = QFileDialog::getSaveFileName(
      this,
      tr("Save As"),
      QDir::currentPath(),
      tr("*.bmp") );

  if( m_filename.isEmpty() )
    {
    g_screenshot->setDown(false);
    return;
    }
  ScreenShot(m_filename);
}

void MriWatcherGUI::ScreenShot(QString filename)
{
  QPixmap  pixmap = QPixmap::grabWidget( m_frame );
  QPixmap* m_pixmaplist = new QPixmap[m_imageframelist.size()];
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_pixmaplist[i] =  m_imageframelist[i]->g_imageframe->renderPixmap();
    }

  if( filename.lastIndexOf(".") == -1 )
    {
    filename = filename + ".bmp";
    }
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    QPoint m_point = m_imageframelist[i]->mapToParent(QPoint(0, 0) );
    int    xpos = m_point.x() + 2;
    int    ypos = m_point.y() + 2;
    // bitBlt(&pixmap,xpos,ypos,&m_pixmaplist[i],0,0,m_pixmaplist[i].width(),m_pixmaplist[i].height(),CopyROP);
    QPainter painter(&pixmap);
    painter.drawPixmap(xpos, ypos, m_pixmaplist[i], 0, 0, m_pixmaplist[i].width(), m_pixmaplist[i].height() );

    }

  filename = filename.mid(0, filename.lastIndexOf(".") ) + ".bmp";
  pixmap.save(filename, "BMP" );
  g_screenshot->setDown(false);
}

MriWatcherGUI::ImageType::Pointer MriWatcherGUI::CreateNewImage(int sizex, int sizey, int sizez, float dimx, float dimy,
                                                                float dimz)
{
  int imagesize[3];

  imagesize[0] = sizex;
  imagesize[1] = sizey;
  imagesize[2] = sizez;

  ImageType::Pointer m_outputimage = ImageType::New();
  float              values[3];

  values[0] = dimx;
  values[1] = dimy;
  values[2] = dimz;

  float origin_x = ( (imagesize[0] / 2) * values[0] * (-1) );
  float origin_y = ( (imagesize[1] / 2) * values[1] * (-1) );
  float origin_z = ( (imagesize[0] / 2) * values[2] * (-1) );
  float origin[3] = {origin_x, origin_y, origin_z};

  ImageType::RegionType region;
  ImageType::SizeType   size;
  size[0] = imagesize[0];
  size[1] = imagesize[1];
  size[2] = imagesize[2];
  region.SetSize( size );
  m_outputimage->SetRegions( region );
  m_outputimage->Allocate();
  m_outputimage->SetOrigin(origin);
  m_outputimage->SetSpacing(values);

  return m_outputimage;
}

void MriWatcherGUI::Combine()
{
  QString m_filename = QFileDialog::getSaveFileName(
      this,
      tr("Save As"),
      QDir::currentPath(),
      tr("*.gipl *.hdr *.mha *.mhd") );

  if( m_filename.isEmpty() )
    {
    g_combine->setDown(false);
    return;
    }

  if( m_filename.lastIndexOf(".") == -1 )
    {
    m_filename += ".gipl";
    }

  int xc; int yc; int zc;
  m_imagemanager.GetCrosshair(&xc, &yc, &zc);

  ImageType::Pointer m_newimage =
    CreateNewImage(m_imagesize[0], m_imagesize[1], m_numberofimages, m_imagespacing[0], m_imagespacing[1],
                   m_imagespacing[2]);
  IteratorType m_itnewimage(m_newimage, m_newimage->GetLargestPossibleRegion() );
  m_itnewimage.GoToBegin();
  for( int z = 0; z < m_numberofimages; z++ )
    {
    ImageType::Pointer m_image = m_imageframelist[z]->g_imageframe->GetInputImage2();
    IteratorType       m_itimage(m_image, m_image->GetLargestPossibleRegion() );
    m_itimage.GoToBegin();
    for( unsigned int i = 0; i < zc * m_imagesize[0] * m_imagesize[1]; i++ )
      {
      ++m_itimage;
      }
    for( unsigned int y = 0; y < m_imagesize[0]; y++ )
      {
      for( unsigned int x = 0; x < m_imagesize[1]; x++ )
        {
        m_itnewimage.Set(m_itimage.Get() );
        ++m_itimage;
        ++m_itnewimage;
        }
      }
    }

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(m_filename.toLatin1().constData() );
  writer->SetInput(m_newimage);
  writer->Update();

  g_combine->setDown(false);
}

void MriWatcherGUI::Help()
{
// printf("help\n");
  QMessageBox helpMsgBox;

  helpMsgBox.setText(
    "Command Key:\n\n- O:\tOpen an overlay image for the selected images\n- H:\tHide/ Show the overlay for the selected images\n- Del:\tRemove the selected images\n- C:\tHide/ Show crosshair for the selected images\n- S:\tLoad second images for comparison\n- +:\tIncrease overlay labels opacity\n- -:\tDecrease overlay labels opacity\n- Shift + mouse:\tGlobal zoom and position");
//    helpMsgBox.setInformativeText("Do you want to save your changes?");
//     helpMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
//      helpMsgBox.setDefaultButton(QMessageBox::Save);
  helpMsgBox.exec();
//    MriWatcherHelp * m_help = new MriWatcherHelp();
//    m_help->show();
//  m_help->g_help->setDown(false);
}

void MriWatcherGUI::UpdateIntensitySlider()
{
  bool m_first = true;
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->IsSelected() )
      {
      ImageType::PixelType m_min;
      ImageType::PixelType m_max;
      m_imageframelist[i]->g_imageframe->GetImageMinMax(m_min, m_max);

      float m_imagemin = m_imageframelist[i]->g_imageframe->GetIntensityMin();
      float m_imagemax = m_imageframelist[i]->g_imageframe->GetIntensityMax();

      float m_imagemin_percent = (m_imagemin - m_min) * MAX_PERCENTAGE / (m_max - m_min);
      float m_imagemax_percent = (m_imagemax - m_min) * MAX_PERCENTAGE / (m_max - m_min);

      if( m_first )
        {
        g_intensity_max_slider->setValue(static_cast<int>(m_imagemax_percent) );
        g_intensity_min_slider->setValue(static_cast<int>(m_imagemin_percent) );
        m_first = false;
        }
      else
        {
        if( m_max > static_cast<int>(g_intensity_max_slider->value() ) )
          {
          g_intensity_max_slider->setValue(static_cast<int>(m_imagemax_percent) );
          }
        if( m_min < static_cast<int>(g_intensity_min_slider->value() ) )
          {
          g_intensity_min_slider->setValue(static_cast<int>(m_imagemin_percent) );
          }
        }
      }
    }
}

void MriWatcherGUI::ImageFrameClicked()
{
// printf("enter ImageFrameClicked\n");
  setFocus();
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    if( m_imageframelist[i]->g_imageframe == sender() )
      {
      // printf("enter sender\n");
      if( m_imageframelist[i]->IsSelected() )
        {
        // printf("enter isslected\n");
        m_imageframelist[i]->UnSelect();
        }
      else
        {
        // printf("enter not selected\n");
        m_imageframelist[i]->Select();
        }
      }
    else
      {
      if( m_keyalt == false && m_keyctrl == false )
        {
        m_imageframelist[i]->UnSelect();
        }
      }
    }

  UpdateIntensitySlider();
}

void MriWatcherGUI::UnSelectAll()
{
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->UnSelect();
    }
}

void MriWatcherGUI::SelectAll()
{
  for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
    {
    m_imageframelist[i]->Select();
    }
}

void MriWatcherGUI::resizeEvent(QResizeEvent *e)
{
  ReDraw();
}

bool MriWatcherGUI::eventFilter(QObject* o, QEvent* e)
{
  switch( e->type() )
    {
    case QEvent::KeyPress:
      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Right )
        {
        // printf("enter key right\n");
        for( int i = m_imageframelist.size() - 2; i >= 0; i-- )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            ImageFrameGUI* m_image = m_imageframelist[i];
            m_imageframelist[i] = m_imageframelist[i + 1];
            m_imageframelist[i + 1] = m_image;
            m_imageframelayout->swap(i, i + 1);
            m_imageframelayout->doLayout();
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Left )
        {
        // printf("enter key left\n");
        for( unsigned int i = 1; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            ImageFrameGUI* m_image = m_imageframelist[i];
            m_imageframelist[i] = m_imageframelist[i - 1];
            m_imageframelist[i - 1] = m_image;
            m_imageframelayout->swap(i - 1, i);
            m_imageframelayout->doLayout();
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Delete )
        {
        // printf("enter key delete\n");
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            std::vector<ImageFrameGUI *>::iterator it = m_imageframelist.begin();
            for( unsigned int k = 0; k < i; k++ )
              {
              ++it;
              }

            m_imagemanager.DelWindow2D(m_imageframelist[i]->g_imageframe);
            m_imageframelist.erase(it);

            m_imageframelayout->deleteItem(i);

            m_maxsize[0] = 0;
            m_maxsize[1] = 0;
            m_maxsize[2] = 0;
            for( unsigned int j = 0; j < m_imageframelist.size(); j++ )
              {
              QtWindow2DFloat::SizeType m_size = m_imageframelist[j]->g_imageframe->GetImageSize();
              if( m_size[0] > m_maxsize[0] )
                {
                m_maxsize[0] = m_size[0];
                }
              if( m_size[1] > m_maxsize[1] )
                {
                m_maxsize[1] = m_size[1];
                }
              if( m_size[2] > m_maxsize[2] )
                {
                m_maxsize[2] = m_size[2];
                }
              }

            i--;
            m_numberofimages--;
            ReDraw();
            m_imageframelayout->doLayout();
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->matches(QKeySequence::SelectAll) )
        {
        // printf("enter key select all\n");
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          m_imageframelist[i]->Select();
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_O )
        {
        QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Select one image files to open"),
            QDir::currentPath(),
            tr("*.jpg *.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*") );
        if( filename != NULL )
          {
          ReaderType::Pointer reader = ReaderType::New();
          reader->SetFileName( filename.toLatin1().constData() );
          try
            {
            reader->Update();
            }
          catch( itk::ExceptionObject & e )
            {
            QMessageBox::information( this, "MriWatcher", "Unable to load file:\n" + filename.toLatin1() );
            return false;
            }

          m_originalimage = reader->GetOutput();

          if (g_load_RAI->isChecked())
          {
              m_originalimage = ChangeOrientation(m_originalimage);
          }

          if( m_originalimage.IsNotNull() )
            {
            for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
              {
              if( m_imageframelist[i]->IsSelected() )
                {
                m_imageframelist[i]->g_imageframe->SetInputOverlay(m_originalimage);
                }
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_H )
        {
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            if( m_imageframelist[i]->g_imageframe->OverlayVisible() )
              {
              m_imageframelist[i]->g_imageframe->HideOverlay();
              }
            else
              {
              m_imageframelist[i]->g_imageframe->ShowOverlay();
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_C )
        {
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            if( m_imageframelist[i]->g_imageframe->IsCrosshairVisible() )
              {
              m_imageframelist[i]->g_imageframe->SetCrosshairVisible(false);
              }
            else
              {
              m_imageframelist[i]->g_imageframe->SetCrosshairVisible(true);
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_S )
        {
        QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Select one image files to open"),
            QDir::currentPath(),
            tr("*.jpg *.gipl* *.raw* *.hdr *.mha *.mhd* *.nhdr* *nrrd* *nii*") );
        if( filename != NULL )
          {
          ReaderType::Pointer reader = ReaderType::New();
          reader->SetFileName( filename.toLatin1().constData() );
          try
            {
            reader->Update();
            }
          catch( itk::ExceptionObject & e )
            {
            QMessageBox::information( this, "MriWatcher", "Unable to load file:\n" + filename.toLatin1() );
            return false;
            }

          m_originalimage = reader->GetOutput();

          if (g_load_RAI->isChecked())
          {
              m_originalimage = ChangeOrientation(m_originalimage);
          }

          if( m_originalimage.IsNotNull() )
            {
            for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
              {
              if( m_imageframelist[i]->IsSelected() )
                {
                m_imageframelist[i]->g_imageframe->SetInputImage2(m_originalimage);
                }
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Plus )
        {
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            if( m_imageframelist[i]->g_imageframe->cOverlayOpacity * 100 < 95 )
              {
              m_imageframelist[i]->g_imageframe->ChangeLabelOverlay( (int)(m_imageframelist[i]->g_imageframe->
                                                                           cOverlayOpacity * 100 + 5) );
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Minus )
        {
        for( unsigned int i = 0; i < m_imageframelist.size(); i++ )
          {
          if( m_imageframelist[i]->IsSelected() )
            {
            if( m_imageframelist[i]->g_imageframe->cOverlayOpacity * 100  > 5 )
              {
              m_imageframelist[i]->g_imageframe->ChangeLabelOverlay( (int)(m_imageframelist[i]->g_imageframe->
                                                                           cOverlayOpacity * 100 - 5) );
              }
            }
          }
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Alt )
        {
        m_keyalt = true;
        }

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Control )
        {
        m_keyctrl = true;
        }

      break;

    case QEvent::KeyRelease:

      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Alt )
        {
        m_keyalt = false;
        }
      if( static_cast<QKeyEvent *>( e )->key() == Qt::Key_Control )
        {
        m_keyctrl = false;
        }

      break;

    default: break;
    }

  return false;
}

void MriWatcherGUI::dragEnterEvent(QDragEnterEvent *event)
{
  event->accept();
}

void MriWatcherGUI::dropEvent(QDropEvent *de)
{
  QList<QUrl> urls;
  urls = de->mimeData()->urls();
  for( int i = 0; i < urls.size(); ++i )
    {
    LoadFile(urls.at(i).path() );
    }
}

QString MriWatcherGUI::OrientationToString(OrientationType in)
{
  switch(in)
    {
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP:
      return QString("RIP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIP:
      return QString("LIP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP:
      return QString("RSP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSP:
      return QString("LSP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIA:
      return QString("RIA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIA:
      return QString("LIA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA:
      return QString("RSA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSA:
      return QString("LSA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRP:
      return QString("IRP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILP:
      return QString("ILP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRP:
      return QString("SRP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLP:
      return QString("SLP");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRA:
      return QString("IRA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILA:
      return QString("ILA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRA:
      return QString("SRA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLA:
      return QString("SLA");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI:
      return QString("RPI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI:
      return QString("LPI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI:
      return QString("RAI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAI:
      return QString("LAI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS:
      return QString("RPS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS:
      return QString("LPS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS:
      return QString("RAS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAS:
      return QString("LAS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRI:
      return QString("PRI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLI:
      return QString("PLI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARI:
      return QString("ARI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALI:
      return QString("ALI");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRS:
      return QString("PRS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLS:
      return QString("PLS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARS:
      return QString("ARS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS:
      return QString("ALS");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPR:
      return QString("IPR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPR:
      return QString("SPR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAR:
      return QString("IAR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAR:
      return QString("SAR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPL:
      return QString("IPL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPL:
      return QString("SPL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAL:
      return QString("IAL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAL:
      return QString("SAL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR:
      return QString("PIR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSR:
      return QString("PSR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIR:
      return QString("AIR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASR:
      return QString("ASR");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL:
      return QString("PIL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSL:
      return QString("PSL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIL:
      return QString("AIL");
    case itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASL:
      return QString("ASL");
    default:
      return QString("N/A");
    }
}

itk::SpatialOrientation::ValidCoordinateOrientationFlags MriWatcherGUI::StringToOrientation(QString in)
{
    if (in == "RIP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP;
    if (in == "LIP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIP;
    if (in == "RSP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP;
    if (in == "LSP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSP;
    if (in == "RIA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIA;
    if (in == "LIA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIA;
    if (in == "RSA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA;
    if (in == "LSA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSA;
    if (in == "IRP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRP;
    if (in == "ILP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILP;
    if (in == "SRP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRP;
    if (in == "SLP")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLP;
    if (in == "IRA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRA;
    if (in == "ILA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILA;
    if (in == "SRA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRA;
    if (in == "SLA")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLA;
    if (in == "RPI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI;
    if (in == "LPI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI;
    if (in == "RAI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI;
    if (in == "LAI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAI;
    if (in == "RPS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS;
    if (in == "LPS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS;
    if (in == "RAS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS;
    if (in == "LAS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAS;
    if (in == "PRI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRI;
    if (in == "PLI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLI;
    if (in == "ARI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARI;
    if (in == "ALI")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALI;
    if (in == "PRS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRS;
    if (in == "PLS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLS;
    if (in == "ARS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARS;
    if (in == "ALS")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS;
    if (in == "IPR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPR;
    if (in == "SPR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPR;
    if (in == "IAR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAR;
    if (in == "SAR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAR;
    if (in == "IPL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPL;
    if (in == "SPL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPL;
    if (in == "IAL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAL;
    if (in == "SAL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAL;
    if (in == "PIR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR;
    if (in == "PSR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSR;
    if (in == "AIR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIR;
    if (in == "ASR")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASR;
    if (in == "PIL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL;
    if (in == "PSL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSL;
    if (in == "AIL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIL;
    if (in == "ASL")
        return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASL;
    
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_INVALID;
}

MriWatcherGUI::ImageType::Pointer MriWatcherGUI::ChangeOrientation(ImageType::Pointer image, QString orientation)
{
    itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter = itk::OrientImageFilter<ImageType,ImageType>::New();
    orienter->UseImageDirectionOn();
    orienter->SetDesiredCoordinateOrientation(StringToOrientation(orientation));
    orienter->SetInput(image);
    orienter->Update();
    image = orienter->GetOutput();
    return image;
}

void MriWatcherGUI::ChangeDesiredOrientation()
{
    QString orientation = g_desired_orientation->text();
    ImageType::Pointer image;
    for (unsigned int i=0; i<m_imageframelist.size(); i++)
    {
       image = m_imageframelist[i]->g_imageframe->GetInputImage2();
       image = ChangeOrientation(image, orientation);
       m_imageframelist[i]->g_imageframe->SetInputImage(image);
    }
    UpdateLabel();

}
