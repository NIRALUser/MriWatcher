/*=========================================================================

  Program:   MriWtacher
  Module:    $RCSfile: MriWatcher.cxx,v $
  Language:  C++
  Date:      $Date: 2007/08/28 22:57:42 $
  Version:   $Revision: 1.4 $
  Author:    Matthieu Jomier

  Copyright (c) 2004 NeuroImaging Lab @ UNC. All rights reserved.
  See NeuroLibCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "ImageManager.h"
#include "QtImageLoader.h"
#include "MriWatcherGUIControls.h"
#include <iostream>
#include <qstring.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qplatinumstyle.h>
#include "MriWatcherConfig.h"

#include <GL/glut.h>

int main( int argc, char* argv[] ) 
{
  glutInit(&argc,argv);

  if ((argc==2) && (!strcmp(argv[1],"-version")))
  {
    std::cout << "MriWatcher " << MriWatcher_VERSION << " - Compiled on: " << __DATE__ << " - " __TIME__  << std::endl;
    return 0;
  }
  if ((argc==2) && (!strcmp(argv[1],"-h")))
  {
    std::cout << "MriWatcher " << MriWatcher_VERSION << " - Compiled on: " << __DATE__ << " - " __TIME__  << std::endl;
    std::cout << "Usage: [{grayscaleImages}} [-overlay overlayImage] [-Title windowsTitle] [-viewAll] [-overlayAlpha alpha] [-intMin min] [-intMax max] [-snap image.bmp] [-quit]" << std::endl;
    return 0;
  }
  QApplication myApp( argc, argv );


  MriWatcherGUIControls m_GUI( 0, 0, TRUE );
      myApp.setMainWidget(&m_GUI);
  
  
  m_GUI.setCaption( QString("MriWatcher ") + MriWatcher_VERSION + " (Qt)" );

  myApp.setStyle( new QPlatinumStyle );
  QPalette p( QColor( 239, 239, 239 ) );
  myApp.setPalette( p, TRUE );

  m_GUI.show();
  if (argc >1)
  {
    for (int i=1;i<argc;i++)
    {
      //std::cout << argc << "/" << i << ": " << argv[i] << std::endl;
      if (!strcmp(argv[i],"-overlay")) 
      {
	i++;  
	m_GUI.LoadOverlay(argv[i]);
      } else if (!strcmp(argv[i],"-Title")) {
	i++;
	//std::cout << "setting title " << argv[i] << std::endl;
	m_GUI.setCaption( QString(argv[i]) );
      } else if (!strcmp(argv[i],"-viewAll")) {
	m_GUI.ViewSetAllImages();
      } else if (!strcmp(argv[i],"-quit")) {
	exit(0);
      } else if (!strcmp(argv[i],"-snap")) {
	i++;
	std::cout << "snap " << argv[i] << std::endl;
	m_GUI.ScreenShot( QString(argv[i]) );
      } else if (!strcmp(argv[i],"-overlayAlpha")) {
	i++;
        int alpha;
	sscanf(argv[i]," %d ", &alpha);
	//std::cout << "setting alpha " << alpha << std::endl;
	m_GUI.ChangeAlpha(alpha);
      } else if (!strcmp(argv[i],"-intMin")) {
	i++;
        int min;
	sscanf(argv[i]," %d ", &min);
	m_GUI.SelectAll();
	m_GUI.ChangeMin(min);
      } else if (!strcmp(argv[i],"-intMax")) {
	i++;
        int max;
	sscanf(argv[i]," %d ", &max);
	m_GUI.SelectAll();
	m_GUI.ChangeMax(max);
      } else {
	m_GUI.LoadFile(argv[i]);
      }
    }
  }


  myApp.exec();
  return 0;
}
