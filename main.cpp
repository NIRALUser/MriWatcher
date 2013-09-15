#include <QtGui/QApplication>
#include "mriwatcherapp.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  if( (argc == 2) && (!strcmp(argv[1], "--help") ) )
    {
    std::cout << std::endl << "Grey level images' viewer." << std::endl;
    std::cout << "Usage: MriWatcher [GreyScaleImages]... [OPTIONS]..." << std::endl << std::endl;
    std::cout << "OPTIONS" << std::endl;
    std::cout << "  -A, --viewAll \t\t set view options to view all images" << std::endl;
    std::cout << "  -m, --intensityMin <arg> \t set intensity minimum of all images" << std::endl;
    std::cout << "  -M, --intensityMax <arg> \t set intensity maximum of all images" << std::endl;
    std::cout << "  -o, --overlayImage <file> \t load overlay image" << std::endl;
    std::cout << "  --overlayAlpha <arg> \t\t set alpha of overlay image" << std::endl;
    std::cout << "  --quit \t\t\t just quit MriWatcher" << std::endl;
    std::cout << "  -s, --snapshot <file> \t set snapshot image name and save snapshot" << std::endl;
    std::cout << "  --title <arg> \t\t set MriWatcher main window's title" << std::endl;
    std::cout << "  --help \t\t\t display this help and exit" << std::endl;
    std::cout << "  --version \t\t\t output MriWatcher version information and exit" << std::endl << std::endl;
    return 0;
    }

  if( (argc == 2) && (!strcmp(argv[1], "--version") ) )
    {
    std::cout << "MriWatcher " << MriWatcher_VERSION << " - Compiled on: " << __DATE__ << " - " __TIME__  << std::endl;
    return 0;
    }

  MriWatcherApp app(argc, argv);

  // QApplication  app(argc, argv);
  // MriWatcherGUI gui;
  // gui.setWindowTitle(QString("MriWatcher") + "   v " + MriWatcher_VERSION);
  // gui.show();

  // if( argc > 1 )
  //   {
  //   for( int i = 1; i < argc; i++ )
  //     {
  //     if( (!strcmp(argv[i], "--overlay") ) || (!strcmp(argv[i], "-o") ) )
  //       {
  //       i++;
  //       gui.LoadOverlay(argv[i]);
  //       }
  //     else if( !strcmp(argv[i], "--title") )
  //       {
  //       i++;
  //       gui.setWindowTitle(QString(argv[i]) );
  //       }
  //     else if( (!strcmp(argv[i], "--viewAll") ) || (!strcmp(argv[i], "-A") ) )
  //       {
  //       gui.SetViewAllImages();
  //       }
  //     else if( !strcmp(argv[i], "--quit") )
  //       {
  //       exit(0);
  //       }
  //     else if( (!strcmp(argv[i], "--snapshot") ) || (!strcmp(argv[i], "-s") ) )
  //       {
  //       i++;
  //       gui.ScreenShot( QString(argv[i]) );
  //       }
  //     else if( !strcmp(argv[i], "--overlayAlpha") )
  //       {
  //       i++;
  //       int alpha;
  //       sscanf(argv[i], " %d ", &alpha);
  //       gui.ChangeOverlayAlpha(alpha);
  //       }
  //     else if( (!strcmp(argv[i], "--intensityMin") ) || (!strcmp(argv[i], "-m") ) )
  //       {
  //       i++;
  //       int min;
  //       sscanf(argv[i], " %d ", &min);
  //       gui.SelectAll();
  //       gui.ChangeIntensityMin(min);
  //       }
  //     else if( (!strcmp(argv[i], "--intensityMax") ) || (!strcmp(argv[i], "-M") ) )
  //       {
  //       i++;
  //       int max;
  //       sscanf(argv[i], " %d ", &max);
  //       gui.SelectAll();
  //       gui.ChangeIntensityMax(max);
  //       }
  //     else
  //       {
  //       gui.LoadFile(argv[i]);
  //       }
  //     }
  //   }

  return app.exec();
}
