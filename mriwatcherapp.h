#ifndef MRIWATCHERAPP_H
#define MRIWATCHERAPP_H
//
#include <QtGui>
#include "mriwatchergui.h"
//
class MriWatcherApp : public QApplication
{
Q_OBJECT
public:
	MriWatcherApp(int & argc, char **argv);
	virtual ~MriWatcherApp();
protected:
        bool event(QEvent *);
private:
        void loadFile(const QString &fileName);

public:
MriWatcherGUI *win;

};
#endif
