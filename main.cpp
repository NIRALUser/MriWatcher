#include <QtGui/QApplication>
#include "mriwatchergui.h"
#include <GL/glut.h>

int main(int argc, char *argv[])
{
    glutInit(&argc,argv);

    QApplication app(argc, argv);
    MriWatcherGUI gui;
    gui.setWindowTitle(QString("MriWatcher"));
    gui.show();

    return app.exec();
}
