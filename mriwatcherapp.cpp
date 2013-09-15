

#include "mriwatcherapp.h"

//
MriWatcherApp::MriWatcherApp( int & argc, char **argv ) 
	: QApplication(argc, argv)
{
	win = new MriWatcherGUI();
	win->setWindowTitle(QString("MriWatcher") + "   v " + MriWatcher_VERSION);
	win->show(); 
	
  if( argc > 1 )
    {
    for( int i = 1; i < argc; i++ )
      {
      if( (!strcmp(argv[i], "--overlay") ) || (!strcmp(argv[i], "-o") ) )
        {
        i++;
        win->LoadOverlay(argv[i]);
        }
      else if( !strcmp(argv[i], "--title") )
        {
        i++;
        win->setWindowTitle(QString(argv[i]) );
        }
      else if( (!strcmp(argv[i], "--viewAll") ) || (!strcmp(argv[i], "-A") ) )
        {
        win->SetViewAllImages();
        }
      else if( !strcmp(argv[i], "--quit") )
        {
        exit(0);
        }
      else if( (!strcmp(argv[i], "--snapshot") ) || (!strcmp(argv[i], "-s") ) )
        {
        i++;
       win->ScreenShot( QString(argv[i]) );
        }
      else if( !strcmp(argv[i], "--overlayAlpha") )
        {
        i++;
        int alpha;
        sscanf(argv[i], " %d ", &alpha);
        win->ChangeOverlayAlpha(alpha);
        }
      else if( (!strcmp(argv[i], "--intensityMin") ) || (!strcmp(argv[i], "-m") ) )
        {
        i++;
        int min;
        sscanf(argv[i], " %d ", &min);
        win->SelectAll();
        win->ChangeIntensityMin(min);
        }
      else if( (!strcmp(argv[i], "--intensityMax") ) || (!strcmp(argv[i], "-M") ) )
        {
        i++;
        int max;
        sscanf(argv[i], " %d ", &max);
        win->SelectAll();
        win->ChangeIntensityMax(max);
        }
      else
        {
        win->LoadFile(argv[i]);
        }
      }
    }
}
//
MriWatcherApp::~MriWatcherApp()
{
}
//
void 
MriWatcherApp::loadFile(const QString &fileName)
{
	win->LoadFile(fileName);

}
bool MriWatcherApp::event(QEvent *event)
{
        switch (event->type()) {
        case QEvent::FileOpen:
            loadFile(static_cast<QFileOpenEvent *>(
                     event)->file());        
            return true;
        default:
            return QApplication::event(event);
    }
}
